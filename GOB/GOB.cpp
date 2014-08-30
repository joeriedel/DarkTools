// GOB.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


void PrintUsage() {
	std::cout << "Usage: GOB [-e gobfile.gob directory]" << std::endl;
	std::cout << "\t-e: Extract gob context to directory." << std::endl << std::endl;
}

bool Extract(const CmdLine& cmdLine) {
	if (!cmdLine.HasArg("-e")) {
		return false;
	}

	const char *gobFileName = cmdLine.GetArgArg("-e", 0);
	const char *dirName = cmdLine.GetArgArg("-e", 1);

	if (gobFileName && dirName) {
		GOBFile gobFile;
		if (gobFile.Open(gobFileName)) {
			gobFile.Extract(dirName);
		}
	} else {
		PrintUsage();
	}

	return true;
}

int main(int argc, const char **argv) {
	CmdLine cmdLine(argc, argv);

	std::cout << "-- GOB utility program --" << std::endl;
	
	if ((cmdLine.GetNumArgs() < 1) || cmdLine.HasArg("-?") || cmdLine.HasArg("-help") || cmdLine.HasArg("-h")) {
		PrintUsage();
		return 0;
	}

	if (Extract(cmdLine)) {
		return 0;
	}

	std::cout << "Unrecognized arguments!" << std::endl;
	PrintUsage();
	return 0;
}
