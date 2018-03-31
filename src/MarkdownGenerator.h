#ifndef MDGENERATOR_H
#define MDGENERATOR_H

#include "OutputGenerator.h"

class MarkdownGenerator : public OutputGenerator
{
	void writeFile(const char* path, const char* data);

public:
	MarkdownGenerator();
	
	std::string generateOutput(LuaSource& src);
	void generatePages(LuaSource& index, std::vector<LuaSource>& sources, const char* outpath, const char* resources = "./");

	static std::string generateComment(LuaSource::Element& e);
	static std::string generateFunction(LuaSource::Element& e);
	static std::string generateVariable(LuaSource::Element& e);
	static std::string generateCodeblock(LuaSource::Element& e);
};

#endif // MDGENERATOR_H
