#ifndef HTMLGENERATOR_H
#define HTMLGENERATOR_H

#include "OutputGenerator.h"

class HTMLGenerator : public OutputGenerator
{
	void writeFile(const char* path, const char* data);
	void setupHtmlHeader(std::stringstream& ss, const char* title, bool includeJS = false);

	std::string generateLink(const std::string text, const std::string ref);

public:
	HTMLGenerator();
	std::string generateOutput(LuaSource& src);
	void generatePages(LuaSource& index, std::vector<LuaSource>& sources, const char* outpath, const char* resources = "./");

	static std::string generateComment(LuaSource::Element& e);
	static std::string generateFunction(LuaSource::Element& e);
	static std::string generateVariable(LuaSource::Element& e);
	static std::string generateCodeblock(LuaSource::Element& e);
};

#endif // HTMLGENERATOR_H
