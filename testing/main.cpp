#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "sexi.hpp"

std::vector<std::unique_ptr<sexi::Value>> parseFile(const std::filesystem::path &p){
	if(!std::filesystem::exists(p)){
		std::cerr << "File '" << p << "' does not exist\n";
		return {};
	}
	else if(!std::filesystem::is_regular_file(p)){
		std::cerr << "File '" << p << "' is not a regular file\n";
		return {};
	}

	std::ifstream file(p);
	if(!file){
		std::cerr << "Error opening file\n";
		return {};
	}

	std::string src;
	{
		std::string tmp;
		while(std::getline(file, tmp)){
			src += tmp + '\n';
		}
	}

	return sexi::parse(src);
}

void testEmpty(sexi::ValuePtr v){
	assert(v->isList());
	assert(v->isEmpty());
	assert(v->size() == 0);
}

void testArray(sexi::ValuePtr v){
	assert(v->isList());
	assert(!v->isEmpty());
	assert(v->size() == 4);
	assert(v->at(0)->toStr() == "1");
	assert(v->at(1)->toStr() == "2");
	assert(v->at(2)->toStr() == "3");
	assert(v->at(3)->toStr() == "4");
}

void testList(sexi::ValuePtr v){
	assert(v->isList());
	assert(!v->isEmpty());
	assert(v->size() == 2);

	std::vector<std::string> parsedElements;
	parsedElements.reserve(4);

	const sexi::Value *element = v;

	while(1){
		assert(element->at(0)->isNum());

		parsedElements.emplace_back(element->at(0)->toStr());

		element = element->at(1);

		if(element->isEmpty()){
			break;
		}
	}

	assert(parsedElements.size() == 4);
	assert(parsedElements[0] == "1");
	assert(parsedElements[1] == "2");
	assert(parsedElements[2] == "3");
	assert(parsedElements[3] == "4");
}

void testText(sexi::ValuePtr v){
	assert(v->isList());
	assert(!v->isEmpty());
	assert(v->size() == 3);
	assert(v->at(0)->isId());
	assert(v->at(0)->toStr() == "bold");
	assert(v->at(1)->isId());
	assert(v->at(1)->toStr() == "italic");
	assert(v->at(2)->isStr());
	assert(v->at(2)->toStr() == "\"Hello\"");
}

void testMath(sexi::ValuePtr v){
	assert(v->isList());
	assert(!v->isEmpty());
	assert(v->size() == 3);
	assert(v->at(0)->isId());
	assert(v->at(0)->toStr() == "+");

	auto head = v->at(1);
	assert(head->isList());
	assert(!head->isEmpty());
	assert(head->size() == 3);
	assert(head->at(0)->isId());
	assert(head->at(0)->toStr() == "/");

	assert(head->at(1)->isNum());
	assert(head->at(1)->toStr() == "1.3");
	assert(head->at(2)->isNum());
	assert(head->at(2)->toStr() == "2.6");

	auto last = v->at(2);
	assert(last->isList());
	assert(!last->isEmpty());
	assert(last->size() == 3);
	assert(last->at(0)->isId());
	assert(last->at(0)->toStr() == "*");

	assert(last->at(1)->isNum());
	assert(last->at(1)->toStr() == "0.0162");
	assert(last->at(2)->isNum());
	assert(last->at(2)->toStr() == "569.27");
}

int main(int argc, char *argv[]){
	(void)argc;
	(void)argv;

	auto exprs = parseFile("test.se");

	std::cout << "Parsed " << exprs.size() << " test expressions\n";
	
	for(auto &&expr : exprs){
		assert(expr->isList() && expr->size() == 2);

		auto head = expr->at(0);

		assert(head->isId());

		auto testId = head->toStr();

		void(*testFn)(sexi::ValuePtr) = nullptr;

		if(testId == "empty") testFn = testEmpty;
		else if(testId == "array") testFn = testArray;
		else if(testId == "list") testFn = testList;
		else if(testId == "text") testFn = testText;
		else if(testId == "math") testFn = testMath;
		else{
			std::cerr << "unexpected test type '" << testId << "'\n";
			std::exit(EXIT_FAILURE);
		}

		auto testExpr = expr->at(1);

		std::cout << "Testing: " << testExpr->toStr() << '\n';

		testFn(testExpr);
	}

	std::cout << "All tests passed\n";

	return 0;
}
