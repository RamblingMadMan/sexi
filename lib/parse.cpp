#include <cstdlib>
#include <cctype>

#include <memory>
#include <vector>

#include "sexi.h"

struct SexiParseResultT{
	bool hasError;
	std::string_view err;
	std::vector<SexiExpr> exprs;
	std::vector<std::vector<SexiExpr>> storage;
};

void sexiDestroyParseResult(SexiParseResult res){
	for(auto expr : res->exprs){
		sexiDestroyExpr(expr);
	}

	for(auto &&exprs : res->storage){
		for(auto expr : exprs){
			sexiDestroyExpr(expr);
		}
	}

	std::destroy_at(res);
	std::free(res);
}

bool sexiParseResultHasError(SexiParseResult res){ return res->hasError; }
SexiStr sexiParseResultError(SexiParseResult res){ return { .len = res->err.size(), .ptr = res->err.data() }; }

size_t sexiParseResultNumExprs(SexiParseResult res){ return res->exprs.size(); }
const SexiExprConst *sexiParseResultExprs(SexiParseResult res){ return res->exprs.data(); }

using ParseInnerResult = std::tuple<const char*, SexiExpr>;

inline ParseInnerResult sexiParseError(SexiParseResult res, std::string_view msg){
	res->hasError = true;
	res->err = msg;
	return std::make_tuple(nullptr, nullptr);
}

inline ParseInnerResult sexiParseId(SexiParseResult res, const char *beg, const char *end, bool copyStrs){
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
			return sexiParseError(res, "unexpected character in identifier");
		}

		++it;
	}

	if(delimIt == end){
		return sexiParseError(res, "unexpected end of source in id");
	}

	SexiStr str = {
		.len = uintptr_t(delimIt) - uintptr_t(beg),
		.ptr = beg
	};

	auto idExpr = sexiCreateId(str);
	if(copyStrs) sexiExprOwnString(idExpr);

	return std::make_tuple(it, idExpr);
}

inline ParseInnerResult sexiParseStr(SexiParseResult res, const char *beg, const char *end, bool copyStrs){
	auto it = beg + 1;
	auto delimIt = end;

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
		return sexiParseError(res, "unexpected end of source in string");
	}

	auto str = SexiStr{
		.len = uintptr_t(it) - uintptr_t(beg),
		.ptr = beg
	};

	// check delimiter

	if(*it == ')'){
		delimIt = it;
	}
	else if(std::isspace(*it)){
		delimIt = it;
		++it;
	}
	else{
		return sexiParseError(res, "unexpected character in string");
	}

	auto strExpr = sexiCreateStr(str);
	if(copyStrs) sexiExprOwnString(strExpr);

	return std::make_tuple(it, strExpr);
}

inline ParseInnerResult sexiParseNum(SexiParseResult res, const char *beg, const char *end, bool copyStrs){
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
				return sexiParseError(res, "multiple decimal points in number");
			}
		}
		else if(!std::isalnum(*it)){
			return sexiParseError(res, "unexpected character in number");
		}

		++it;
	}

	if(delimIt == end){
		return sexiParseError(res, "unexpected end of source in number");
	}

	auto str = SexiStr{
		.len = uintptr_t(delimIt) - uintptr_t(beg),
		.ptr = beg
	};

	auto numExpr = sexiCreateNum(str);
	if(copyStrs) sexiExprOwnString(numExpr);

	return std::make_tuple(it, numExpr);
}

inline ParseInnerResult sexiParseList(SexiParseResult res, const char *beg, const char *end, bool copyStrs){
	auto it = beg + 1;
	auto delimIt = end;

	std::vector<SexiExpr> elems;

	// most possible elements is single-character expressions separated by spaces till end of source
	elems.reserve((std::uintptr_t(end) - std::uintptr_t(beg)) / 2);

	SexiExpr expr = nullptr;

	while(it != end){
		if(std::isspace(*it)){
			++it;
			continue;
		}
		else if(std::isdigit(*it)){
			std::tie(it, expr) = sexiParseNum(res, it, end, copyStrs);
			if(!it) return std::make_tuple(it, expr);
		}
		else if(*it == '"'){
			std::tie(it, expr) = sexiParseStr(res, it, end, copyStrs);
			if(!it) return std::make_tuple(it, expr);
		}
		else if(*it == '('){
			std::tie(it, expr) = sexiParseList(res, it, end, copyStrs);
			if(!it) return std::make_tuple(it, expr);
		}
		else if(*it == ')'){
			delimIt = it;
			++it;
			break;
		}
		else if(std::ispunct(*it) || std::isalpha(*it)){
			std::tie(it, expr) = sexiParseId(res, it, end, copyStrs);
			if(!it) return std::make_tuple(it, expr);
		}
		else{
			for(auto elem : elems){ sexiDestroyExpr(elem); }
			return sexiParseError(res, "unexpected token in list");
		}

		elems.emplace_back(expr);
	}

	if(delimIt == end){
		for(auto elem : elems){ sexiDestroyExpr(elem); }
		return sexiParseError(res, "unexpected end of source in list");
	}

	auto &&elemVec = res->storage.emplace_back(std::move(elems));

	auto listExpr = sexiCreateList(elemVec.size(), elemVec.data());

	return std::make_tuple(it, listExpr);
}

SexiParseResult sexiParse(size_t len, const char *ptr, bool copyStrs){
	auto mem = std::malloc(sizeof(SexiParseResultT));
	if(!mem) return nullptr;

	auto ret = new(mem) SexiParseResultT;
	ret->hasError = false;

	const char *it = ptr;
	const char *end = ptr + len;

	SexiExpr expr = nullptr;

	while(it != end){
		if(*it == '('){
			// parse a list
			std::tie(it, expr) = sexiParseList(ret, it, end, copyStrs);
			if(!it) return ret;
		}
		else if(std::isspace(*it)){
			++it;
			continue;
		}
		else{
			ret->hasError = true;
			ret->err = "unexpected token at top level";
			return ret;
		}

		ret->exprs.emplace_back(expr);
	}

	return ret;
}
