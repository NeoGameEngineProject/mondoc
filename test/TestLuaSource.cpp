#include <gtest/gtest.h>
#include <vector>
#include "../src/LuaSource.h"
#include "../src/HTMLGenerator.h"

using namespace std;

TEST(LuaSource, HeaderComment)
{
	LuaSource src;
	src.parseString(
			"--- File header comment\n"
			"-- Some other line\n"
			"--- Some other header!"
			/*"--- This is a local variable\n"
			"local var = 234\n\n"
			"--- This is a function!!!\n"
			"-- @param a Parameter!\n"
			"-- @return A value!"
			"local function f(a) return true end"*/
	);

	vector<LuaSource::Element>* actual = src.getElements();

	// src.printTree();

	ASSERT_NE(0, (*actual).size());
	EXPECT_EQ("File header comment", (*actual)[0].title);
	EXPECT_EQ("Some other line\n", (*actual)[0].content);

	EXPECT_EQ(LuaSource::ELEMENT_COMMENT, (*actual)[0].type);
	EXPECT_EQ(LuaSource::ELEMENT_COMMENT, (*actual)[1].type);

	EXPECT_EQ("Some other header!", (*actual)[1].title);
}

TEST(LuaSource, StandaloneComment)
{
	LuaSource src;
	src.parseString(
			"--- File header comment\n"
			"-- Some other line\n\n"
	);

	vector<LuaSource::Element>* actual = src.getElements();

	// src.printTree();

	EXPECT_EQ(LuaSource::ELEMENT_STANDALONE_COMMENT, (*actual)[0].type);
}

TEST(LuaSource, FunctionComment)
{
	LuaSource src;
	src.parseString(
			//"--- This is a local variable\n"
			//"local var = 234\n\n"
			"--- This is a function!!!\n"
			"-- @param a A Parameter!\n"
			"-- @return A value!\n"
			"local function f(a)\nreturn true end"
	);

	vector<LuaSource::Element>* actual = src.getElements();

	// src.printTree();

	ASSERT_NE(0, (*actual).size());
	EXPECT_EQ("local function f(a)", (*actual)[0].title);
	EXPECT_EQ("This is a function!!!\n", (*actual)[0].content);

	EXPECT_EQ(1, (*actual)[0].parameters.size());
	EXPECT_EQ("a", (*actual)[0].parameters[0].name);
	EXPECT_EQ("A Parameter!", (*actual)[0].parameters[0].description);

	EXPECT_EQ(LuaSource::ELEMENT_FUNCTION, (*actual)[0].type);
}

TEST(LuaSource, VariableComment)
{
	LuaSource src;
	src.parseString(
			"--- This is a local variable\n"
			"local var = 234\n\n"
	);

	vector<LuaSource::Element>* actual = src.getElements();

	// src.printTree();

	ASSERT_NE(0, (*actual).size());
	EXPECT_EQ("This is a local variable", (*actual)[0].title);
	EXPECT_EQ("local var = 234", (*actual)[0].content);
	EXPECT_EQ(LuaSource::ELEMENT_VARIABLE, (*actual)[0].type);
}

TEST(LuaSource, isVariableTest)
{
	LuaSource src;
	EXPECT_EQ(true, src.isVariable("local var = 123"));
	EXPECT_EQ(true, src.isVariable("var = 123"));
	EXPECT_EQ(false, src.isVariable("local function(lalalala) end"));
}

TEST(LuaSource, CodeBlockTest)
{
	LuaSource src;
	src.parseString("--- Some comment!\n"
					"-- @code\n"
					"-- local code = 123\n"
					"-- @endcode\n"
					"-- More comment!\n"
					"-- LALALA\n"
					"-- @code\n"
					"-- -- More code\n"
					"-- @endcode"
					);

	// src.printTree();

	vector<LuaSource::Element>* actual = src.getElements();

	ASSERT_EQ(5, actual->size());

	LuaSource::Element e0 = (*actual)[0];
	LuaSource::Element e1 = (*actual)[1];
	LuaSource::Element e2 = (*actual)[2];
	LuaSource::Element e3 = (*actual)[3];

	EXPECT_EQ(LuaSource::ELEMENT_COMMENT, e0.type);
	EXPECT_EQ(LuaSource::ELEMENT_CODE, e1.type);
	EXPECT_EQ(LuaSource::ELEMENT_COMMENT, e2.type);
	EXPECT_EQ(LuaSource::ELEMENT_CODE, e3.type);

	EXPECT_EQ("Some comment!", e0.title);
	EXPECT_EQ(" local code = 123\n", e1.content);
	EXPECT_EQ("More comment!\nLALALA\n", e2.content);
}

TEST(LuaSource, ExceptionTest)
{
	LuaSource src;
	EXPECT_THROW(src.parseFile("LALALALALA"), FileNotFoundException);
}

TEST(HTMLGenerator, CommentTest)
{
	LuaSource src;
	HTMLGenerator gen;

	src.parseString("--- A comment!\n"
					"-- Description\n");

	string output = gen.generateComment((*src.getElements())[0]);
	string expected = "<h1>A comment!</h1>\n"
			"<p>Description<br>"
			"</p>\n\n";

	//cout << output << endl;
	EXPECT_EQ(expected, output);
}

TEST(HTMLGenerator, FunctionTest)
{
	LuaSource src;
	HTMLGenerator gen;

	src.parseString("--- A function!\n"
					"-- Description\n"
					"-- @param p Parameter!\n"
					"-- @return Value!\n"
					);

	string output = gen.generateFunction((*src.getElements())[0]);
	string expected = "<h2>A function!</h2>\n"
			"<p>Description<br>\n"
			"<strong>p:</strong> Parameter!<br>\n"
			"<br><strong>Returns: </strong>Value!\n"
			"</p>\n\n";

	// cout << output << endl;
	EXPECT_EQ(expected, output);
}

TEST(HTMLGenerator, VariableTest)
{
	LuaSource src;
	HTMLGenerator gen;

	src.parseString("--- A Variable!\n"
					"local var = 123\n"
					);

	string output = gen.generateVariable((*src.getElements())[0]);
	string expected = "<strong>local var = 123:</strong> A Variable!<br>\n";

	// cout << output << endl;
	EXPECT_EQ(expected, output);
}

TEST(HTMLGenerator, CodeblockTest)
{
	LuaSource src;
	HTMLGenerator gen;

	src.parseString("--- A codeblock\n"
					"-- @code\n"
					"-- -- somecode\n"
					"-- @endcode"
					"-- After comment!\n"
					);

	vector<LuaSource::Element>* v = src.getElements();
	string output = gen.generateCodeblock((*v)[1]);
	string expected = "<pre><code> -- somecode\n</code></pre>\n";

	// cout << output << endl;
	EXPECT_EQ(expected, output);
}

TEST(HTMLGenerator, SourcefileTest)
{
	LuaSource src;
	HTMLGenerator gen;

	src.parseString("--- This is a file!\n"
					"-- @module file\n"
					"somecode\n"
					"--- A codeblock\n"
					"-- @code\n"
					"-- -- somecode\n"
					"-- @endcode"
					);

	string output = gen.generateOutput(src);
	//string expected = "<p class='code'> -- somecode\n</p>\n";

	//cout << output << endl;
	//EXPECT_EQ(expected, output);
}

TEST(HTMLGenerator, GeneratePagesTest)
{
	LuaSource src;
	HTMLGenerator gen;

	src.parseString("--- This is a file!\n"
					"-- @module file\n"
					"somecode\n"
					"--- A variable!\n"
					"local var = 23\n"
					"--- A codeblock\n"
					"-- @code\n"
					"-- -- somecode\n"
					"-- @endcode\n"
					"-- COMMENT\n"
					"function func()\n"
					);

	vector<LuaSource> sources;

	LuaSource src1;
	src1.parseString("--- This file contains some stuff!\n"
					 "-- @module StuffModule\n"
					 "\n"
					 "--- A variable!\n"
					 "local variable = \"BANANA\"\n"
					 "\n"
					 "--- Another comment block!\n"
					 "-- Comment!\n"
					 "-- LALALALA\n"
					 "\n"
					 "--- Some function with params and return value!\n"
					 "-- @code\n"
					 "-- local var = 123\n"
					 "-- function f() end\n"
					 "-- @endcode\n"
					 "-- @param a Some value!\n"
					 "-- @param b Some other value!\n"
					 "-- @return A return value!\n"
					 "function f(a, b)\n"
					 );

	sources.push_back(src1);

	gen.generatePages(src, sources, "./");
	//string expected = "<p class='code'> -- somecode\n</p>\n";

	//cout << output << endl;
	//EXPECT_EQ(expected, output);
}

TEST(HTMLGenerator, CompressTest)
{
	LuaSource src;
	HTMLGenerator gen;

	src.parseString("--- This is a preceeding comment!\n\n"
					"--- Some comment!\n"
					"-- Comment!\n"
					"-- @code\n"
					"-- local code = 123\n"
					"-- @endcode\n"
					"-- LULZ!\n"
					"-- COMMENT!\n"
					"-- @code\n"
					"-- Somecode\n"
					"-- @endcode\n"
					"-- More comment!\n"
					"function a()\n"
					);

	//src.printTree();
	src.compress(HTMLGenerator::generateCodeblock);
	EXPECT_EQ(2, src.getElements()->size());
	//src.printTree();
}

TEST(HTMLGenerator, MoreCompressTest)
{
	LuaSource src;
	HTMLGenerator gen;

	src.parseString("--- test.lua - This is a test for mondoc!\n"
					"-- Some more test stuff\n"
					"-- LALALALALA\n"
					"-- TEST\n\n"

					"--- This is a function! WTF!\n"
					"function f(a,b) end\n"
					);

	//src.printTree();
	src.compress(HTMLGenerator::generateCodeblock);
	EXPECT_EQ(2, src.getElements()->size());
	//src.printTree();
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
