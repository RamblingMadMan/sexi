#include <cassert>

#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "sexi.h"
#include "sexi/literals.hpp"

using namespace sexi;
using namespace sexi::literals;

template<typename T, typename U>
static inline void expect(T &&value, U &&expected){
	if(value != expected){
		std::ostringstream stream;
		stream << "expected (" << expected << ") got (" << value << ")\n";
		throw std::runtime_error(stream.str());
	}
}

// empty expression:
// ()
void testEmpty(const sexi::Expr &v){
	assert(v.isEmpty());
}

// array expression:
// (1 2 3 4)
void testArray(const sexi::Expr &v){
	assert(v.isList());
	assert(v.length() == 4);
	assert(v[0].toStr() == "1");
	assert(v[1].toStr() == "2");
	assert(v[2].toStr() == "3");
	assert(v[3].toStr() == "4");
}

// list expression:
// (1 (2 (3 (4 ()))))
void testList(const sexi::Expr &v){
	std::vector<std::string> parsedElements;
	parsedElements.reserve(4);

	sexi::Expr element = v;

	while(1){
		assert(element.isList());
		assert(element.length() == 2);
		assert(element[0].isNum());

		parsedElements.emplace_back(element[0].toStr());

		element = element[1];

		if(element.isEmpty()){
			break;
		}
	}

	assert(parsedElements.size() == 4);
	assert(parsedElements[0] == "1");
	assert(parsedElements[1] == "2");
	assert(parsedElements[2] == "3");
	assert(parsedElements[3] == "4");
}

// text expression:
// (bold italic "Hello")
void testText(const sexi::Expr &v){
	assert(v.isList());
	assert(v.length() == 3);
	assert(v[0].isId());
	assert(v[0].toStr() == "bold");
	assert(v[1].isId());
	assert(v[1].toStr() == "italic");
	assert(v[2].isStr());
	assert(v[2].toStr() == "\"Hello\"");
}

// math expression:
// (+ (/ 1.3 2.6) (* 0.0162 569.27))
void testMath(const sexi::Expr &v){
	assert(v.isList());
	assert(v.length() == 3);
	assert(v[0].isId());
	assert(v[0].toStr() == "+");

	auto head = v[1];
	assert(head.isList());
	assert(head.length() == 3);
	assert(head[0].isId());
	assert(head[0].toStr() == "/");

	assert(head[1].isNum());
	expect(head[1].toStr(), "1.3");
	//assert(head[1].toStr() == "1.3");
	assert(head[2].isNum());
	assert(head[2].toStr() == "2.6");

	auto last = v[2];
	assert(last.isList());
	assert(last.length() == 3);

	assert(last[0].isId());
	assert(last[0].toStr() == "*");

	assert(last[1].isNum());
	assert(last[1].toStr() == "0.0162");
	assert(last[2].isNum());
	assert(last[2].toStr() == "569.27");
}

// set expression:
// (= %0 (alloc n32))
void testSet(const sexi::Expr &v){
	assert(v.isList());
	assert(v.length() == 3);
	assert(v[0].isId());
	assert(v[0].toStr() == "=");

	auto head = v[1];
	assert(head.isId());
	assert(head.toStr() == "%0");

	auto last = v[2];
	assert(last.isList());
	assert(last.length() == 2);
	assert(last[0].isId());
	assert(last[0].toStr() == "alloc");
	assert(last[1].isId());
	assert(last[1].toStr() == "n32");
}

void testOperators(){
	auto emptyExpr = ""_se;

	testEmpty(emptyExpr);

	auto arrayExpr = (1_se << 2 << 3 << 4);

	testArray(arrayExpr);

	auto listExpr =
		(1_se << (2_se << (3_se << (4_se << (""_se)))));

	testList(listExpr);

	auto textExpr =
		("bold"_se << "italic" << "\"Hello\"");

	testText(textExpr);

	auto mathExpr =
		("+"_se <<
			("/"_se << 1.3 << 2.6) <<
			("*"_se << 0.0162 << 569.27));

	testMath(mathExpr);

	auto setExpr =
		("="_se << "%0" << ("alloc"_se << "n32"));

	testSet(setExpr);
}

int main(int argc, char *argv[]){
	(void)argc;
	(void)argv;

	if(!std::filesystem::exists("test.se")){
		throw std::runtime_error("file does not exist");
	}
	else if(!std::filesystem::is_regular_file("test.se")){
		throw std::runtime_error("path points to non-regular file");
	}

	std::ifstream file("test.se");
	if(!file){
		throw std::runtime_error("error in std::ifstream::ifstream");
	}

	std::string src;
	{
		std::string tmp;
		while(std::getline(file, tmp)) src += tmp + '\n';
	}

	auto result = sexi::parse(src);

	if(result.hasError()){
		std::cerr << "Error parsing test file: " << result.error() << '\n';
		return 1;
	}

	std::cout << "Parsed " << result.size() << " test expressions\n";

	for(auto &&expr : result){
		assert(expr.isList() && expr.length() == 2);

		auto head = expr[0];

		assert(head.isId());

		auto testId = head.toStr();

		void(*testFn)(const sexi::Expr&) = nullptr;

		if(testId == "empty") testFn = testEmpty;
		else if(testId == "array") testFn = testArray;
		else if(testId == "list") testFn = testList;
		else if(testId == "text") testFn = testText;
		else if(testId == "math") testFn = testMath;
		else if(testId == "set") testFn = testSet;
		else{
			std::cerr << "unexpected test type '" << testId << "'\n";
			std::exit(EXIT_FAILURE);
		}

		auto testExpr = expr[1];

		std::cout << "Testing: " << testExpr.toStr() << '\n';

		testFn(testExpr);
	}

	testOperators();

	std::cout << "All tests passed\n";

	return 0;
}
