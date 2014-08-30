#pragma once

#include <string.h>

class CmdLine {
public:

	explicit CmdLine(int argc, const char **argv) : m_argc(argc), m_argv(argv) {}

	int GetNumArgs() const { return m_argc - 1; }
	const char *GetArg(int i) const { return ((i+1) < m_argc) ? m_argv[i+1] : nullptr; }

	bool HasArg(const char *arg) const {
		return FindArgIndex(arg) >= 0;
	}

	int FindArgIndex(const char *arg) const {
		for (int i = 1; i < m_argc; ++i) {
			if (!stricmp(m_argv[i], arg)) {
				return i-1;
			}
		}
		return -1;
	}

	const char *GetArgArg(const char *arg, int i = 0) const {
		int argI = FindArgIndex(arg);
		if (argI < 0) {
			return nullptr;
		}
		return GetArg(argI + i + 1);
	}

private:

	int m_argc;
	const char **m_argv;
};
