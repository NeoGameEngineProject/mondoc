#include "SphinxGenerator.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

static void replaceString(string &src, const char* s1, const char* s2)
{
	size_t s1Size = strlen(s1);
	size_t s2Size = strlen(s2);

	if(s1Size == 0)
		return;

	size_t idx = 0;
	while((idx = src.find(s1, idx)) != -1)
	{
		src.replace(idx, s1Size, s2);
		idx += s2Size;
	}
}

static void replaceString(string &src, string &s1, string &s2)
{
	replaceString(src, s1.c_str(), s2.c_str());
}

static void copyFile(string src, string dest)
{
	ifstream in;
	ofstream out;

	in.open(src.c_str(), ios::in);
	if (!in)
	{
		cout << "Can't open input file: " << src << endl;
		return;
	}

	out.open(dest.c_str(), ios::out);
	if (!in)
	{
		cout << "Can't open output file: " << dest  << endl;
		in.close();
		return;
	}

	out << in.rdbuf();
	in.close();
	out.close();
}

SphinxGenerator::SphinxGenerator()
{

}

string SphinxGenerator::generateOutput(LuaSource& src)
{
	stringstream ss;
	vector<LuaSource::Element>* elements = src.getElements();

	src.compress(SphinxGenerator::generateCodeblock);
	//setupSphinxHeader(ss, src.getModuleName());

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
			ss << "<div class='codeblock'>" << generateCodeblock(e) << "</div>" << endl;
		break;

		case LuaSource::ELEMENT_FUNCTION:
			ss << generateFunction(e) << endl;
		break;
		}
	}

	//ss << "</body></Sphinx>" << endl;

	return ss.str();
}

std::string SphinxGenerator::generateLink(const string text, const string ref)
{
	return "<a href='" + ref + "'>" + text + "</a>";
}

void SphinxGenerator::generatePages(LuaSource& index, std::vector<LuaSource>& sources, const char* outpath, const char* resources)
{
	string basePath = outpath;

	stringstream sidepanel;
	vector<string> pages;
	string page;
	string modname;

	for(LuaSource src : sources)
	{
		modname = src.getModuleName();

		const std::string outfile(basePath + PATH_SEP + modname + ".rst");
		ofstream ss(outfile);
		if(!ss)
			throw FileNotFoundException(outfile.c_str());

		ss << modname << std::endl;
		ss << std::string(modname.size(), '=') << std::endl;

		ss << generateOutput(src);
	}
}

void SphinxGenerator::writeFile(const char* path, const char* data)
{
	ofstream out(path);
	if(!out) throw FileNotFoundException(path);
	out << data;
	out.close();
}

string SphinxGenerator::generateComment(LuaSource::Element& e)
{
	stringstream ss;
	if(!e.title.empty())
		ss << e.title << "\n" << std::string(e.title.size(), '-') << endl;

	if(!e.content.empty())
		ss << e.content << endl << endl;
	return ss.str();
}

string SphinxGenerator::generateFunction(LuaSource::Element& e)
{
	stringstream ss;
	std::string contentWithIndent = e.content;
	replaceString(contentWithIndent, "\n", "\n    ");
	
	ss << ".. py:function:: " << e.title << endl << endl << "    " << contentWithIndent << endl;

	if(e.parameters.size())
	{
		for(LuaSource::Parameter p : e.parameters)
			ss << "    :param " << p.name << ": " << p.description << endl;
	}

	if(!e.ret.empty())
		ss << "    :return: " << e.ret << endl;

	ss << std::endl;
	return ss.str();
}

string SphinxGenerator::generateVariable(LuaSource::Element& e)
{
	stringstream ss;
	ss << ".. py:data:: " << e.content << ": " << e.title << endl << endl;
	return ss.str();
}

string SphinxGenerator::generateCodeblock(LuaSource::Element& e)
{
	std::string contentWithIndent = e.content;
	replaceString(contentWithIndent, "\n", "\n        ");

	stringstream ss;
	ss	<< "\n.. code-block:: lua\n\n    "
   		<< "    " << contentWithIndent << "\n\n";
	return ss.str();
}

