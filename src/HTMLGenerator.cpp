#include "HTMLGenerator.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

void replaceString(string &src, const char* s1, const char* s2)
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

void replaceString(string &src, string &s1, string &s2)
{
	replaceString(src, s1.c_str(), s2.c_str());
}

void copyFile(string src, string dest)
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

HTMLGenerator::HTMLGenerator()
{

}

string HTMLGenerator::generateOutput(LuaSource& src)
{
	stringstream ss;
	vector<LuaSource::Element>* elements = src.getElements();

	src.compress(HTMLGenerator::generateCodeblock);
	setupHtmlHeader(ss, src.getModuleName());

	for(LuaSource::Element e : (*elements))
	{
		switch(e.type)
		{
		case LuaSource::ELEMENT_STANDALONE_COMMENT:
		case LuaSource::ELEMENT_COMMENT:
			ss << "<p class='comment'>" << generateComment(e) << "</p>" << endl;
		break;

		case LuaSource::ELEMENT_VARIABLE:
			ss << "<p class='variable'>" << generateVariable(e) << "</p>" << endl;
		break;

		case LuaSource::ELEMENT_CODE:
			ss << "<p class='codeblock'>" << generateCodeblock(e) << "</p>" << endl;
		break;

		case LuaSource::ELEMENT_FUNCTION:
			ss << "<p class='function'>" << generateFunction(e) << "</p>" << endl;
		break;
		}
	}

	ss << "</body></html>" << endl;

	return ss.str();
}

void HTMLGenerator::setupHtmlHeader(stringstream& ss, const char* title, bool includeJS)
{
	ss << "<!DOCTYPE html>" << endl;
	ss << "<html>" << endl << "<head><meta charset='UTF-8'>" << endl;
	ss << "<title>" << title << "</title>" << endl;

	ss << "<link rel='stylesheet' type='text/css' href='style.css'/>" << endl;
	ss << "</head>" << endl;

	if(includeJS)
	{
		ss << "<script type='text/javascript' src=\"navigation.js\"></script>" << endl;
	}

	// Switch?
	ss << "<script src='highlight.pack.js'></script>" << endl;
	ss << "<script>hljs.initHighlightingOnLoad();</script>" << endl;

	ss << "<body>" << endl;
}

void HTMLGenerator::generatePages(LuaSource& index, std::vector<LuaSource>& sources, const char* outpath, const char* resources)
{
	string basePath = outpath;
	string page = generateOutput(index);
	writeFile((basePath + PATH_SEP + index.getModuleName() + ".html").c_str(), page.c_str());

	stringstream sidepanel;
	string path;

	setupHtmlHeader(sidepanel, "");
	sidepanel << "<a href='" << index.getModuleName() << ".html' target='content'> Index </a>" << endl;

	for(LuaSource s : sources)
	{
		path = basePath + PATH_SEP + s.getModuleName() + ".html";
		sidepanel << "<br><a href='" << s.getModuleName() << ".html' target='content'>" << s.getModuleName() << "</a>" << endl;
		writeFile(path.c_str(), generateOutput(s).c_str());
	}

	sidepanel << "</body></html>" << endl;

	// Finalize sidepanel
	writeFile((basePath + PATH_SEP + "sidepanel.html").c_str(), sidepanel.str().c_str());

	// Create index page
	stringstream ss;
	setupHtmlHeader(ss, "Reference", true);

	ss << "<iframe src='sidepanel.html' align='left' name='sidepanel' style=\"position: fixed; float: left; width: 20%; height: 100%;\"></iframe>" << endl;
	ss << "<iframe id='contentIframe' onload='setIframeHeight(this.id)' src='" << index.getModuleName() << ".html' height='100%' align='middle' name='content' style=\"float: right; width: 78%; height: 100%;\"></iframe>" << endl;

	ss << "</body></html>" << endl;
	writeFile((basePath + PATH_SEP + "index.html").c_str(), ss.str().c_str());

	// Copy some resources
	string resPath = resources;
	copyFile(resPath + PATH_SEP + "style.css", basePath + PATH_SEP + "style.css");
	copyFile(resPath + PATH_SEP + "highlight.pack.js", basePath + PATH_SEP + "highlight.pack.js");
	copyFile(resPath + PATH_SEP + "navigation.js", basePath + PATH_SEP + "navigation.js");
}

void HTMLGenerator::writeFile(const char* path, const char* data)
{
	ofstream out(path);
	if(!out) throw FileNotFoundException(path);

	out << data;
	out.close();
}

string HTMLGenerator::generateComment(LuaSource::Element& e)
{
	stringstream ss;
	replaceString(e.content, "\n", "<br>");
	ss << "<h1>" << e.title << "</h1>" << endl << "<p>" << e.content << "</p>" << endl << endl;
	return ss.str();
}

string HTMLGenerator::generateFunction(LuaSource::Element& e)
{
	stringstream ss;
	replaceString(e.content, "\n", "<br>");
	ss << "<h2>" << e.title << "</h2>" << endl << "<p>" << e.content << endl;

	for(LuaSource::Parameter p : e.parameters)
		ss << "<strong>" << p.name << ":</strong> " << p.description << "<br>" << endl;

	if(!e.ret.empty())
		ss << "<br><strong>Returns: </strong>" << e.ret << endl;

	ss << "</p>" << endl << endl;
	return ss.str();
}

string HTMLGenerator::generateVariable(LuaSource::Element& e)
{
	stringstream ss;
	ss << "<strong>" << e.content << ":</strong> " << e.title << "<br>" << endl;
	return ss.str();
}

string HTMLGenerator::generateCodeblock(LuaSource::Element& e)
{
	stringstream ss;
	ss << "<pre><code>" << e.content << "</code></pre>" << endl;
	return ss.str();
}

