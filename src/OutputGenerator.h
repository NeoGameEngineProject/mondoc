#ifndef OUTPUTGENERATOR_H
#define OUTPUTGENERATOR_H

#include "LuaSource.h"
#include <vector>

class OutputGenerator
{
public:
	/**
	 * @brief Generates a string from a given LuaSource.
	 *
	 * The output is the complete documentation that is derivable from
	 * the LuaSource::Element list in LuaSource.
	 *
	 * @param source The source to process.
	 * @return A complete documentation page.
	 */
	virtual std::string generateOutput(LuaSource& source) = 0;

	/**
	 * @brief Generates a whole documentation document from the given parameters.
	 *
	 * Generates a complete document for distribution.
	 *
	 * @param index The index page (or title page for non-html output)
	 * @param sources All LuaSources to include
	 * @param outpath The output directory to write to (has to exist!)
	 * @param resources The resources directory to read from
	 *
	 * @throws FileNotFoundException in case the outpath or resources do not exist.
	 */
	virtual void generatePages(LuaSource& index, std::vector<LuaSource>& sources, const char* outpath, const char* resources) = 0;

	/**
	 * @brief Generates a string from a LuaSources::ELEMENT_COMMENT Element.
	 * @param e The Element to convert.
	 * @return A string.
	 */
	static std::string generateComment(LuaSource::Element& e);

	/**
	 * @brief Generates a string from a LuaSources::ELEMENT_FUNCTION Element.
	 * @param e The Element to convert.
	 * @return A string.
	 */
	static std::string generateFunction(LuaSource::Element& e);

	/**
	 * @brief Generates a string from a LuaSources::ELEMENT_VARIABLE Element.
	 * @param e The Element to convert.
	 * @return A string.
	 */
	static std::string generateVariable(LuaSource::Element& e);

	/**
	 * @brief Generates a string from a LuaSources::ELEMENT_CODEBLOCK Element.
	 * @param e The Element to convert.
	 * @return A string.
	 */
	static std::string generateCodeblock(LuaSource::Element& e);
};

#endif // OUTPUTGENERATOR_H
