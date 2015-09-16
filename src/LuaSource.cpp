#include "LuaSource.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <regex>
#include <exception>

using namespace std;

void trim(string &str, char c)
{
	string::size_type pos = str.find_last_not_of(c);

	if (pos != string::npos)
	{
		str.erase(pos + 1);
		pos = str.find_first_not_of(c);
		if (pos != string::npos)
			str.erase(0, pos);
	}
	else
		str.erase(str.begin(), str.end());
}

void trimWhitespace(string &str)
{
	trim(str, ' ');
	trim(str, '\t');
}

string getFilename(string filename)
{
	int idx = filename.find_last_of(PATH_SEP);
	if (idx != -1)
		return filename.substr(idx + 1);

	return filename;
}

LuaSource::LuaSource()
{

}

void LuaSource::parseFile(const char* filename)
{
	ifstream in(filename);
	if(!in)
	{
		throw FileNotFoundException(filename);
	}

	m_moduleName = getFilename(filename);

	stringstream ss;
	ss << in.rdbuf();
	parse(&ss);
}

void LuaSource::parseString(const char* src)
{
	stringstream ss(src);
	parse(&ss);
}

void LuaSource::parse(stringstream* ss)
{
	PARSER_STATES state = PARSE_OTHER;
	string line;

	Element e;
	while(getline(*ss, line))
	{
		trimWhitespace(line);

		switch(state)
		{
		// Search for the title line of a comment
		case PARSE_TITLE:
		case PARSE_OTHER:

			if(line.find("---") == 0)
			{
				e.type = ELEMENT_COMMENT;
				e.title = line.substr(3);
				trimWhitespace(e.title);
				state = PARSE_COMMENT;
			}

			break;

		// Parse the actual comment
		case PARSE_COMMENT:

			if(line.find("---") == 0)
			{
				e.type = ELEMENT_COMMENT;
				m_elements.push_back(e);

				e.title = line.substr(3);
				e.content.clear();

				trimWhitespace(e.title);
				break;
			}
			else if(line.find("--") == 0)
			{
				line = line.substr(2);
				trimWhitespace(line);

				if(line.find("@param") == 0)
				{
					line = line.substr(6);
					Parameter param;

					trimWhitespace(line);

					int idx = line.find(" ");
					if(idx == -1) break;

					param.name = line;
					param.name.erase(idx);

					param.description = line.substr(idx);

					trimWhitespace(param.name);
					trimWhitespace(param.description);

					e.parameters.push_back(param);
				}
				else if(line.find("@return") == 0)
				{
					e.ret = line.substr(7);
					trimWhitespace(e.ret);
				}
				else if(line.find("@code") == 0 || line.find("/code") == 0 || line.find("\\code") == 0)
				{
					line = line.substr(5);
					e.type = ELEMENT_COMMENT;
					m_elements.push_back(e);
					e.clear();

					state = PARSE_CODEBLOCK;
					e.content = line;
				}
				else if(line.find("@module") == 0)
				{
					m_moduleName = line.substr(7);
					trimWhitespace(m_moduleName);
				}
				else
				{
					e.content += line + "\n";
				}
				break;
			}
			else
			{
				if(line.find("function") == 0 || line.find("local function") == 0)
				{
					e.content = e.title + "\n" + e.content;
					e.title = line;
					e.type = ELEMENT_FUNCTION;

					m_elements.push_back(e);
					state = PARSE_OTHER;
					e.clear();
				}
				else if(isVariable(line.c_str()))
				{
					e.content = line;
					e.type = ELEMENT_VARIABLE;

					m_elements.push_back(e);
					state = PARSE_OTHER;
					e.clear();
				}
				else
				{					
					e.type = ELEMENT_STANDALONE_COMMENT;
					m_elements.push_back(e);
					state = PARSE_OTHER;

					e.clear();
				}
			}

			break;

		case PARSE_CODEBLOCK:
			if(line.find("--") == 0 && (line.find("@endcode") == -1 && line.find("/endcode") == -1 && line.find("\\endcode") == -1))
			{
				e.content += line.substr(2) + "\n";
			}
			else
			{
				e.type = ELEMENT_CODE;
				m_elements.push_back(e);
				e.clear();

				state = PARSE_COMMENT;
			}
			break;
		}
	}

	// If we are still parsing a comment, finalize it
	if(state != PARSE_OTHER)
	{
		e.type = ELEMENT_COMMENT;
		m_elements.push_back(e);
	}
}

bool LuaSource::isVariable(const char* str)
{
	static regex r("(\\w+\\s*=\\s*.+)|(local\\s+\\w+\\s*=\\s*.+)");
	return regex_match(str, r);
}

void LuaSource::compress(std::function<std::string(Element&)> generateCodeblock)
{
	bool handlingFunction = false;
	LuaSource::Element currentFunction;
	vector<LuaSource::Element> newElements;

	for(int i = m_elements.size() - 1; i >= 0; i--)
	{
		LuaSource::Element e = m_elements[i];

		// If we found a code block, integrate it by generating the
		// markup for it.
		if(handlingFunction && e.type == LuaSource::ELEMENT_CODE)
		{
			currentFunction.content = generateCodeblock(e) + currentFunction.content;
			continue;
		}
		// Standalone comments do not belong into a function
		// and stand by themselves
		else if(handlingFunction && e.type == LuaSource::ELEMENT_STANDALONE_COMMENT)
		{
			handlingFunction = false;
			newElements.push_back(currentFunction);
			newElements.push_back(e);

			currentFunction.clear();
			continue;
		}
		else if(handlingFunction && e.type == LuaSource::ELEMENT_COMMENT && !e.title.empty())
		{
			handlingFunction = false;
			currentFunction.content = e.title + "\n" + e.content + currentFunction.content;

			newElements.push_back(currentFunction);
			currentFunction.clear();

			continue;
		}
		// Comments without titles reside between code blocks
		// if multiple code blocks exist in one function definition.
		else if(handlingFunction && e.type == LuaSource::ELEMENT_COMMENT)
		{
			currentFunction.content = e.title + "\n" + e.content + currentFunction.content;
			continue;
		}

		if(!handlingFunction && e.type == LuaSource::ELEMENT_FUNCTION)
		{
			currentFunction = e;
			handlingFunction = true;
			continue;
		}

		newElements.push_back(e);
	}

	m_elements.clear();
	for(int i = newElements.size() - 1; i >= 0; i--)
	{
		m_elements.push_back(newElements[i]);
	}
}

void LuaSource::printList()
{
	for(Element e: m_elements)
	{
		cout << "*************************************************************" << endl;
		cout << "title = " << e.title << endl << "content = \"" << e.content << "\"" << endl << "type = " << e.type << endl << endl;
	}
}
