#include <cstdlib>
#include <cctype>

#include <iostream>

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
		else if(!std::isalnum(*it) && (*it != '_') && (*it != '-')){
			std::cerr << "Unexpected character '" << *it << "' in identifier\n";
			std::exit(EXIT_FAILURE);
		}

		++it;
	}

	if(delimIt == end){
		std::cerr << "Unexpected end of source in id\n";
		std::exit(EXIT_FAILURE);
	}

	auto str = std::string(beg, delimIt);
	return std::make_pair(it, std::make_unique<Value>(tags::id, std::move(str)));
}

static inline std::pair<StrIt, std::unique_ptr<Value>> parseNum(const StrIt beg, const StrIt end){
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
		else if(!std::isxdigit(*it)){
			std::cerr << "Unexpected character '" << *it << "' in number\n";
			std::exit(EXIT_FAILURE);
		}

		++it;
	}

	if(delimIt == end){
		std::cerr << "Unexpected end of source in number\n";
		std::exit(EXIT_FAILURE);
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
		std::cerr << "Unexpected end of source in string\n";
		std::exit(EXIT_FAILURE);
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
		std::cerr << "Unexpected character '" << *it << "' in string\n";
		std::exit(EXIT_FAILURE);
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
		else if(std::isalpha(*it) || (*it == '_')){
			std::tie(it, val) = parseId(it, end);
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
		else{
			std::cerr << "Unexpected token '" << *it << "' in list\n";
			std::exit(EXIT_FAILURE);
		}

		elems.emplace_back(std::move(val));
	}

	if(delimIt == end){
		std::cerr << "Unexpected end of source in list\n";
		std::exit(EXIT_FAILURE);
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

}
