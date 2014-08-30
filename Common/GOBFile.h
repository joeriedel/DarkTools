#pragma once

#pragma warning(disable:4996)

#include <stdio.h>
#include <map>
#include <string>
#include <iostream>

class GOBFile {
public:
#pragma pack (push, 1)
	struct Entry {
		unsigned int ofs;
		unsigned int length;
		char name[13];
	};
#pragma pack (pop)

	typedef std::map<std::string, Entry*> Map;

	GOBFile() : m_fp(nullptr), m_dirBlock(nullptr), m_numFiles(0) {}
	~GOBFile() {
		Close();
	}

	bool Open(const char *path) {
		if (m_fp) {
			return false;
		}

		m_fp = fopen(path, "rb");
		if (!m_fp) {
			std::cout << "Unable to open " << path << " for read." << std::endl;
			return false;
		}

		Header header;
		if (!fread(&header, sizeof(header), 1, m_fp)) {
			Close();
			return false;
		}

		if (header.id[0] != 'G' || header.id[1] != 'O' || header.id[2] != 'B' || header.id[3] != 0xA) {
			Close();
			std::cout << path << " does not appear to be a GOB file." << std::endl;
			return false;
		}

		fseek(m_fp, header.dirOfs, SEEK_SET);

		if (!fread(&m_numFiles, sizeof(m_numFiles), 1, m_fp)) {
			Close();
			std::cout << path << " premature end of file." << std::endl;
			return false;
		}

		if (m_numFiles < 1) {
			return true;
		}

		m_dirBlock = new Entry[m_numFiles];
		
		if (fread(m_dirBlock, sizeof(Entry), m_numFiles, m_fp) != m_numFiles) {
			Close();
			std::cout << path << " premature end of file." << std::endl;
			return false;
		}

		for (int i = 0; i < m_numFiles; ++i) {
			m_dirBlock[i].name[12] = 0; // ensure null terminated.
			m_dir.insert(Map::value_type(std::string(m_dirBlock[i].name), &m_dirBlock[i]));
		}

		return true;
	}

	bool Extract(const char *dstDir) {
		if (!m_fp) {
			return false;
		}

		for (int i = 0; i < m_numFiles; ++i) {
			const Entry &entry = m_dirBlock[i];

			fseek(m_fp, entry.ofs, SEEK_SET);

			void* block = malloc(entry.length);
			if (!fread(block, entry.length, 1, m_fp)) {
				free(block);
				std::cout << "Premature end of file." << std::endl;
				return false;
			}

			char filename[1024];
			strcpy(filename, dstDir);
			strcat(filename, "/");
			strcat(filename, entry.name);

			FILE *ofp = fopen(filename, "wb");
			if (!ofp) {
				free(block);
				std::cout << "Unable to open '" << filename << "' for write." << std::endl;
				return false;
			}

			if (!fwrite(block, entry.length, 1, ofp)) {
				free(block);
				std::cout << "Write failure " << entry.name << "." << std::endl;
				return false;
			}

			fclose(ofp);
			free(block);

			std::cout << "Extracted " << entry.name << std::endl;
		}

		return true;
	}

	void Close() {
		if (m_fp) {
			fclose(m_fp);
			m_fp = nullptr;
		}

		if (m_dirBlock) {
			delete[] m_dirBlock;
			m_dirBlock = nullptr;
		}

		m_dir.clear();
		m_numFiles = 0;
	}

	const Map& GetDir() const { return m_dir; }

private:

#pragma pack (push, 1)
	struct Header {
		char id[4];
		unsigned int dirOfs;
	};
#pragma pack (pop)

	FILE* m_fp;
	Entry* m_dirBlock;
	Map m_dir;
	int m_numFiles;
};

