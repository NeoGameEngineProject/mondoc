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
	//setupHtmlHeader(ss, src.getModuleName());

	for(LuaSource::Element e : (*elements))
	{
		switch(e.type)
		{
		case LuaSource::ELEMENT_STANDALONE_COMMENT:
		case LuaSource::ELEMENT_COMMENT:
			ss << "<div class='comment'>" << generateComment(e) << "</div>" << endl;
		break;

		case LuaSource::ELEMENT_VARIABLE:
			ss << "<div class='variable'>" << generateVariable(e) << "</div>" << endl;
		break;

		case LuaSource::ELEMENT_CODE:
			ss << "<div class='codeblock'>" << generateCodeblock(e) << "</div>" << endl;
		break;

		case LuaSource::ELEMENT_FUNCTION:
			ss << "<div class='function'>" << generateFunction(e) << "</div>" << endl;
		break;
		}
	}

	//ss << "</body></html>" << endl;

	return ss.str();
}

void HTMLGenerator::setupHtmlHeader(stringstream& ss, const char* title, bool includeJS)
{
	ss << "<!DOCTYPE html>" << endl;
	ss << "<html>" << endl << "<head><meta charset='UTF-8'>" << endl;
	ss << "<title>" << title << "</title>" << endl;

	ss << "<link rel='stylesheet' type='text/css' href='style.css'/>" << endl;
	ss << "</head>" << endl;

	ss << "<script src='highlight.pack.js'></script>" << endl;
	ss << "<script>hljs.initHighlightingOnLoad();</script>" << endl;

	ss << "<body>" << endl;
}

/*void HTMLGenerator::generatePages(LuaSource& index, std::vector<LuaSource>& sources, const char* outpath, const char* resources)
{
	string basePath = outpath;
	string page = generateOutput(index);
	writeFile((basePath + PATH_SEP + index.getModuleName() + ".html").c_str(), page.c_str());

	stringstream sidepanel;
	string path;

	//setupHtmlHeader(sidepanel, "");
	//sidepanel << "<a onclick=\"loadPage('content', '" << index.getModuleName() << ".html');\"" << " href='" << index.getModuleName() << ".html' <<  target='content'> Index </a>" << endl;
	sidepanel << "<br><a onclick=\"loadPage('content', '" << index.getModuleName() << ".html');\"" << " href='#'>Index</a>" << endl;

	for(LuaSource s : sources)
	{
		path = basePath + PATH_SEP + s.getModuleName() + ".html";
		//sidepanel << "<br><a onclick=\"loadPage('content', '" << s.getModuleName() << ".html');\"" << " href='" << s.getModuleName() << ".html' target='content'>" << s.getModuleName() << "</a>" << endl;
		sidepanel << "<br><a onclick=\"loadPage('content', '" << s.getModuleName() << ".html');\"" << " href='#'>" << s.getModuleName() << "</a>" << endl;
		writeFile(path.c_str(), generateOutput(s).c_str());
	}

	//sidepanel << "</body></html>" << endl;

	// Finalize sidepanel
	writeFile((basePath + PATH_SEP + "sidepanel.html").c_str(), sidepanel.str().c_str());

	// Create index page
	stringstream ss;
	setupHtmlHeader(ss, "Reference", true);

	ss << "<div id='sidepanel' style=\"position: fixed; float: left; width: 20%; height: 100%;\">" << sidepanel.str() << "</div>" << endl;
	//ss << "<iframe id='contentIframe' onload='setIframeHeight(this.id)' src='" << index.getModuleName() << ".html' height='100%' align='middle' name='content' style=\"float: right; width: 78%; height: 100%;\"></iframe>" << endl;
	ss << "<div id='content' src='" << index.getModuleName() << ".html' name='content' style=\"margin-left: 20%; position: fixed; float: right; width: 80%; height: 100%;\"></div>" << endl;

	ss << "</body></html>" << endl;
	writeFile((basePath + PATH_SEP + "index.html").c_str(), ss.str().c_str());

	// Copy some resources
	string resPath = resources;
	copyFile(resPath + PATH_SEP + "style.css", basePath + PATH_SEP + "style.css");
	copyFile(resPath + PATH_SEP + "highlight.pack.js", basePath + PATH_SEP + "highlight.pack.js");
	copyFile(resPath + PATH_SEP + "navigation.js", basePath + PATH_SEP + "navigation.js");
}*/

std::string HTMLGenerator::generateLink(const string text, const string ref)
{
	return "<a href='" + ref + "'>" + text + "</a>";
}

void HTMLGenerator::generatePages(LuaSource& index, std::vector<LuaSource>& sources, const char* outpath, const char* resources)
{
	string basePath = outpath;

	stringstream indexStream;
	setupHtmlHeader(indexStream, "Index");
	indexStream << "<div id='content' src='" << index.getModuleName() << ".html' name='content' style=\"float: right; width: 80%;\">";
	indexStream << generateOutput(index);
	indexStream << "</div>" << endl;

	//writeFile((basePath + PATH_SEP + index.getModuleName() + ".html").c_str(), page.c_str());

	stringstream sidepanel;
	vector<string> pages;
	string page;
	string modname;

	page.resize(512);

	sidepanel << "<div id='sidepanel' style=\"position: fixed; float: left; width: 20%; height: 100%;\">" << endl;

	modname = index.getModuleName();
	sidepanel << generateLink("Index", "index.html") << "<br>" << endl;

	for(LuaSource src : sources)
	{
		stringstream ss;
		modname = src.getModuleName();
		sidepanel << generateLink(modname, modname + ".html") << "<br>" << endl;

		setupHtmlHeader(ss, src.getModuleName());
		ss << "<div id='content' src='" << index.getModuleName() << ".html' name='content' style=\"float: right; width: 80%; height: 100%;\">";
		ss << generateOutput(src);
		ss << "</div>" << endl;

		pages.push_back(ss.str());
	}
	sidepanel << "</div>" << endl;

	for(int i = 0; i < pages.size(); i++)
	{
		string* str = &pages[i];
		str->append(sidepanel.str());
		str->append("</body></html>");
		//cout << *str << endl << "**********************************************************" << endl;
		writeFile((basePath + PATH_SEP + sources[i].getModuleName() + ".html").c_str(), str->c_str());
	}

	indexStream << sidepanel.rdbuf() << endl << "</body></html>" << endl;
	writeFile((basePath + PATH_SEP + "index.html").c_str(), indexStream.str().c_str());

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

	if(!e.title.empty())
		ss << "<h1>" << e.title << "</h1>" << endl;

	if(!e.content.empty())
		ss << "<p>" << e.content << "</p>" << endl << endl;
	return ss.str();
}

string HTMLGenerator::generateFunction(LuaSource::Element& e)
{
	stringstream ss;
	replaceString(e.content, "\n", "<br>");
	ss << "<h2>" << e.title << "</h2>" << endl << "<p>" << e.content << endl;

	if(e.parameters.size())
	{
		ss << "<p class='arguments'>" << endl;
		for(LuaSource::Parameter p : e.parameters)
			ss << "<strong>" << p.name << ":</strong> " << p.description << "<br>" << endl;
		ss << "</p>" << endl;
	}

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

