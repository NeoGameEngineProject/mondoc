#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <algorithm>

#include "HTMLGenerator.h"
#include "LuaSource.h"

#define VERSION_STRING "0.3"

#ifndef WIN32
#define SEPERATOR "/"
#else
#define SEPERATOR "\\"
#endif

using namespace std;

struct Settings
{
	vector<string> inputFiles;
	string indexFile;
	string outputDirectory;
	bool showHelp;
};

Settings parseCommandLine(int argc, char* argv[])
{
	Settings s;
	s.showHelp = false;
	s.outputDirectory = ".";

	if (argc <= 1)
	{
		cout << "Not enough arguments given!" << endl;
		s.showHelp = true;
		return s;
	}

	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-o"))
		{
			s.outputDirectory = argv[++i];
		}
		else if (!strcmp("-i", argv[i]))
		{
			s.indexFile = argv[++i];
		}
		else if (!strcmp(argv[i], "-h"))
		{
			s.showHelp = true;
			return s;
		}
		else
		{
			s.inputFiles.push_back(argv[i]);
		}
	}

	return s;
}

void displayHelp()
{
	cout << "This program is a generator for generating html documentation" << endl;
	cout << "from Lua source files. The syntax is similar to doxygen." << endl;
	cout << endl;
	cout << "Usage: mondoc -o <output> <input files>" << endl;
	cout << "\t-i: The file used to produce the index.html" << endl;
	cout << "\t-o: Selects the output directory" << endl;
	cout << endl;
	cout << "\"Der Mond, das blasse Auge der Nacht.\" - Heinrich Heine" << endl; 
}

string getPath(string filename)
{
	int idx = filename.find_last_of(SEPERATOR);
	if (idx != -1)
	{
		return filename.erase(idx + 1);
	}

	return filename;
}

int main(int argc, char* argv[])
{
	cout << "MonDoc v" << VERSION_STRING << endl;
	Settings s = parseCommandLine(argc, argv);

	if (s.showHelp)
	{
		displayHelp();
		return 0;
	}

#ifndef RESOURCE_DIR
	string execPath = getPath(argv[0]);
#else
	string execPath = RESOURCE_DIR;
#endif

	cout << "Processing " << s.inputFiles.size() + ((s.indexFile.empty()) ? 0 : 1) << " file(s)" << endl;

	HTMLGenerator generator;
	vector<LuaSource> sources;
	LuaSource indexSource;
	indexSource.parseFile(s.indexFile.c_str());

	for(string str : s.inputFiles)
	{
		LuaSource src;

		cout << "Processing " << str << endl;
		src.parseFile(str.c_str());
		sources.push_back(src);
	}

	try
	{
		generator.generatePages(indexSource, sources, s.outputDirectory.c_str(), execPath.c_str());
	}
	catch(std::exception* e)
	{
		cerr << "Could not generate documentation: " << e->what() << endl;
		return 1;
	}

	return 0;
}
