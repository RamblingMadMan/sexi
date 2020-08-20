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

int main(int argc, char *argv[]){
	(void)argc;
	(void)argv;

	auto exprs = parseFile("test.se");

	std::cout << "Parsed " << exprs.size() << " s-expression(s)\n";
	
	for(auto &&val : exprs){
		std::cout << "    " << val->toStr() << '\n';
	}

	return 0;
}
