#ifndef LUASOURCE_H
#define LUASOURCE_H

#include <string>
#include <vector>
#include <exception>
#include <functional>

#ifndef WIN32
#define PATH_SEP "/"
#else
#define PATH_SEP "\\"
#endif


std::string generateHtml(const char* file);

class FileNotFoundException: public std::exception
{
	std::string filename;

public:
	FileNotFoundException(const char* fname) : filename(fname) {}
	virtual const char* what() const throw()
	{
		return ("Could not open file '" + filename + "'").c_str();
	}
};

class LuaSource
{
	std::string m_moduleName;
public:
	enum PARSER_STATES
	{
		PARSE_TITLE,
		PARSE_OTHER,
		PARSE_COMMENT,
		PARSE_CODEBLOCK,
	};

	enum ELEMENT_TYPES
	{
		ELEMENT_COMMENT,
		ELEMENT_STANDALONE_COMMENT,
		ELEMENT_VARIABLE,
		ELEMENT_FUNCTION,
		ELEMENT_CODE,
		ELEMENT_NONE
	};

	/**
	 * @brief The Parameter struct contains the name and description
	 * of a function parameter.
	 */
	struct Parameter
	{
		std::string name;
		std::string description;
	};

	/**
	 * @brief The Element class encapsulates the data needed to generate one block
	 * of documentation.
	 */
	class Element
	{
	public:
		/// The content of the block
		std::string content;

		/// The title of the block (function name for ELEMENT_FUNCTION)
		std::string title;

		/// Description of return value for ELEMENT_FUNCTION
		std::string ret;

		/// vector of parameters of ELEMENT_FUNCTION
		std::vector<Parameter> parameters;

		/// The type of the block
		ELEMENT_TYPES type;

		/**
		 * @brief Clears all strings and parameters in this block.
		 */
		void clear()
		{
			content = "";
			title = "";
			ret = "";
			parameters.clear();
		}
	};

private:
	std::vector<Element> m_elements;

public:
	LuaSource();

	/**
	 * @brief Reads a file into memory and parses its content.
	 * @param filename The file to load.
	 *
	 * Throws FileNotFoundException if the file could not be found.
	 */
	void parseFile(const char* filename);

	/**
	 * @brief Parses a given string.
	 * @param src The string to parse.
	 */
	void parseString(const char* src);

	/**
	 * @brief Parses a given stringstream.
	 * @param stream The stream to parse.
	 */
	void parse(std::stringstream* stream);

	/**
	 * @brief Changes the module name.
	 * @param name The new module name.
	 */
	void setModuleName(const char* name) { m_moduleName = name; }

	/**
	 * @brief Returns the module name.
	 * @return The module name.
	 */
	const char* getModuleName() { return m_moduleName.c_str(); }

	/**
	 * @brief Prints the internal list of Elements to stdout.
	 */
	void printList();

	/**
	 * @brief Checks if input is a valid Lua variable.
	 * @param str The line to check.
	 * @return A boolean value.
	 */
	static bool isVariable(const char *str);

	/**
	 * @brief Compresses the given Element list to unify codeblocks that are
	 * embedded in function comments with the surrounding comment blocks.
	 *
	 * @param generateCodeblock The function used to generate the codeblock
	 * string from the given element.
	 */
	void compress(std::function<std::string(Element&)> generateCodeblock);

	/**
	 * @brief Returns a pointer to the list of elements.
	 * @return The list of elements.
	 */
	std::vector<Element>* getElements() { return &m_elements; }
};

#endif
