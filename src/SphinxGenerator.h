#ifndef SPHINXGENERATOR_H
#define SPHINXGENERATOR_H

#include "OutputGenerator.h"

class SphinxGenerator : public OutputGenerator
{
	void writeFile(const char* path, const char* data);
	std::string generateLink(const std::string text, const std::string ref);

public:
	SphinxGenerator();
	std::string generateOutput(LuaSource& src);
	void generatePages(LuaSource& index, std::vector<LuaSource>& sources, const char* outpath, const char* resources = "./");

	static std::string generateComment(LuaSource::Element& e);
	static std::string generateFunction(LuaSource::Element& e);
	static std::string generateVariable(LuaSource::Element& e);
	static std::string generateCodeblock(LuaSource::Element& e);
};

#endif // SPHINXGENERATOR_H
