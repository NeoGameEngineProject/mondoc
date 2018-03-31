#include "MarkdownGenerator.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

MarkdownGenerator::MarkdownGenerator()
{

}

string MarkdownGenerator::generateOutput(LuaSource& src)
{
	stringstream ss;
	vector<LuaSource::Element>* elements = src.getElements();
	src.compress(MarkdownGenerator::generateCodeblock);
	
	ss << "# " << src.getModuleName() << "\n\n";

	for(LuaSource::Element e : (*elements))
	{
		switch(e.type)
		{
		case LuaSource::ELEMENT_STANDALONE_COMMENT:
		case LuaSource::ELEMENT_COMMENT:
			ss << generateComment(e) << endl;
		break;

		case LuaSource::ELEMENT_VARIABLE:
			ss << generateVariable(e) << endl;
		break;

		case LuaSource::ELEMENT_CODE:
			ss << generateCodeblock(e) << endl;
		break;

		case LuaSource::ELEMENT_FUNCTION:
			ss << generateFunction(e) << endl;
		break;
		}
	}

	return ss.str();
}


void MarkdownGenerator::generatePages(LuaSource& index, std::vector<LuaSource>& sources, const char* outpath, const char* resources)
{
	string basePath = outpath;
	stringstream ss;
	for(LuaSource src : sources)
	{
		ss << generateOutput(src) << endl;
	}
	
	string modname = strlen(index.getModuleName()) == 0 ? "Documentation" : index.getModuleName();
	writeFile((basePath + PATH_SEP + modname + ".md").c_str(), ss.str().c_str());
}

void MarkdownGenerator::writeFile(const char* path, const char* data)
{
	ofstream out(path);
	if(!out) throw FileNotFoundException(path);
	out << data;
	out.close();
}

string MarkdownGenerator::generateComment(LuaSource::Element& e)
{
	stringstream ss;

	if(!e.title.empty())
		ss << "## " << e.title << "\n\n";

	if(!e.content.empty())
		ss << e.content << "\n";
	return ss.str();
}

string MarkdownGenerator::generateFunction(LuaSource::Element& e)
{
	stringstream ss;
	ss << "### " << e.title << "\n\n" << e.content << "\n";

	if(e.parameters.size())
	{
		ss << "#### Arguments\n\n";
		for(LuaSource::Parameter p : e.parameters)
			ss << "- " << p.name << ": " << p.description << "\n";
		ss << "\n";
	}

	if(!e.ret.empty())
		ss << "\nReturns: " << e.ret << endl;

	ss << "\n";
	return ss.str();
}

string MarkdownGenerator::generateVariable(LuaSource::Element& e)
{
	stringstream ss;
	ss << "### " << e.content << ": " << e.title << "\n";
	return ss.str();
}

string MarkdownGenerator::generateCodeblock(LuaSource::Element& e)
{
	stringstream ss;
	ss << "```\n" << e.content << "```" << endl;
	return ss.str();
}

