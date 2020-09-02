#include <cstdlib>
#include <cctype>

#include <iostream>
#include <fstream>

#include "sexi.hpp"

using namespace sexi;

using StrIt = std::string_view::const_iterator;

static inline std::pair<StrIt, std::unique_ptr<Value>> parseId(const StrIt beg, const StrIt end){
	auto it = beg + 1;
	auto delimIt = end;

	while(it != end){
		if(std::isspace(*it)){
			delimIt = it;
			++it;
			break;
		}
		else if(*it == ')'){
			delimIt = it;
			break;
		}
		else if(!std::ispunct(*it) && !std::isalnum(*it)){
			throw std::runtime_error("unexpected character in identifier");
		}

		++it;
	}

	if(delimIt == end){
		throw std::runtime_error("unexpected end of source in id");
	}

	auto str = std::string(beg, delimIt);
	return std::make_pair(it, std::make_unique<Value>(tags::id, std::move(str)));
}

static inline std::pair<StrIt, std::unique_ptr<Value>> parseNum(const StrIt beg, const StrIt end){
	auto it = beg + 1;
	auto delimIt = end;

	bool hasDecimal = false;

	while(it != end){
		if(std::isspace(*it)){
			delimIt = it;
			++it;
			break;
		}
		else if(*it == ')'){
			delimIt = it;
			break;
		}
		else if(*it == '.'){
			if(!hasDecimal){
				hasDecimal = true;
			}
			else{
				throw std::runtime_error("multiple decimal points in number");
			}
		}
		else if(!std::isalnum(*it)){
			throw std::runtime_error("unexpected character in number");
		}

		++it;
	}

	if(delimIt == end){
		throw std::runtime_error("unexpected end of source in number");
	}

	auto str = std::string(beg, delimIt);
	return std::make_pair(it, std::make_unique<Value>(tags::num, std::move(str)));
}

static inline std::pair<StrIt, std::unique_ptr<Value>> parseStr(const StrIt beg, const StrIt end){
	auto it = beg + 1;
	auto delimIt = end;

	// inner string parsing
	while(it != end){
		if(*it == '"'){
			delimIt = it;
			++it;
			break;
		}
		else if(*it == '\\'){
			++it;
		}

		++it;
	}

	if(delimIt == end){
		throw std::runtime_error("unexpected end of source in string");
	}

	auto str = std::string(beg, it);

	// check delimiter

	if(*it == ')'){
		delimIt = it;
	}
	else if(std::isspace(*it)){
		delimIt = it;
		++it;
	}
	else{
		throw std::runtime_error("unexpected character  in string");
	}

	return std::make_pair(it, std::make_unique<Value>(tags::str, std::move(str)));
}

std::pair<StrIt, std::unique_ptr<Value>> parseList(const StrIt beg, const StrIt end){
	auto it = beg + 1;
	auto delimIt = end;

	std::vector<std::unique_ptr<Value>> elems;

	elems.reserve((std::uintptr_t(end) - std::uintptr_t(beg)) / 2);

	while(it != end){
		std::unique_ptr<Value> val;

		if(std::isspace(*it)){
			++it;
			continue;
		}
		else if(std::isdigit(*it)){
			std::tie(it, val) = parseNum(it, end);
		}
		else if(*it == '"'){
			std::tie(it, val) = parseStr(it, end);
		}
		else if(*it == '('){
			std::tie(it, val) = parseList(it, end);
		}
		else if(*it == ')'){
			delimIt = it;
			++it;
			break;
		}
		else if(std::ispunct(*it) || std::isalpha(*it)){
			std::tie(it, val) = parseId(it, end);
		}
		else{
			throw std::runtime_error("unexpected token in list");
		}

		elems.emplace_back(std::move(val));
	}

	if(delimIt == end){
		throw std::runtime_error("unexpected end of source in list");
	}

	return std::make_pair(it, std::make_unique<Value>(tags::list, std::move(elems)));
}

std::vector<std::unique_ptr<sexi::Value>> sexi::parse(std::string_view src){
	std::vector<std::unique_ptr<Value>> ret;

	auto beg = cbegin(src);
	auto end = cend(src);
	auto it = beg;

	while(it != end){
		std::unique_ptr<Value> val;

		if(*it == '('){
			// parse a list
			std::tie(it, val) = parseList(it, end);
		}
		else if(std::isspace(*it)){
			++it;
			continue;
		}
		else{
			std::cerr << "Unexpected token at top level\n";
			return ret;
		}

		ret.emplace_back(std::move(val));
	}

	return ret;
}

std::vector<std::unique_ptr<Value>> sexi::parseFile(const std::filesystem::path &p){
	if(!std::filesystem::exists(p)) throw std::runtime_error("path does not exist");
	else if(!std::filesystem::is_regular_file(p)) throw std::runtime_error("path is not to a regular file");

	std::string src;

	{
		auto file = std::ifstream(p);

		std::string tmp;

		while(std::getline(file, tmp)){
			src += tmp + '\n';
		}
	}

	return parse(src);
}
