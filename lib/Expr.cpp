#include <cassert>
#include <cstdlib>
#include <cstring>

#include <string>
#include <memory>

#include "sexi/Expr.hpp"

struct SexiExprT{
	SexiExprType type;
	union {
		SexiStr str;
		struct {
			size_t n;
			const SexiExprConst *exprs;
		} list;
	};
	SexiOwnedStr ownedStr;
};

void sexiDestroyExpr(SexiExpr expr){
	if(expr->ownedStr.ptr){
		std::free(expr->ownedStr.ptr);
	}

	std::destroy_at(expr);
	std::free(expr);
}

static inline SexiExpr allocExpr(SexiExprType type){
	auto mem = std::malloc(sizeof(SexiExprT));
	if(!mem) return nullptr;
	auto ret = new(mem) SexiExprT;
	ret->type = type;
	ret->ownedStr.len = 0;
	ret->ownedStr.ptr = nullptr;
	return ret;
}

SexiExpr sexiCreateEmpty(){
	auto ret = allocExpr(SEXI_EMPTY);
	ret->str = { .len = 2, .ptr = "()" };
	return ret;
}

SexiExpr sexiCreateList(size_t n, const SexiExprConst *exprs){
	if(n == 0) return sexiCreateEmpty();

	auto ret = allocExpr(SEXI_LIST);
	ret->list = { .n = n, .exprs = exprs };
	return ret;
}

SexiExpr sexiCreateId(SexiStr str){
	auto ret = allocExpr(SEXI_ID);
	ret->str = str;
	return ret;
}

SexiExpr sexiCreateStr(SexiStr str){
	auto ret = allocExpr(SEXI_STR);
	ret->str = str;
	return ret;
}

SexiExpr sexiCreateNum(SexiStr str){
	auto ret = allocExpr(SEXI_NUM);
	ret->str = str;
	return ret;
}

SexiExprType sexiExprType(SexiExprConst expr){ return expr->type; }

bool sexiExprIsEmpty(SexiExprConst expr){ return expr->type == SEXI_EMPTY; }
bool sexiExprIsList(SexiExprConst expr){ return expr->type == SEXI_LIST; }
bool sexiExprIsId(SexiExprConst expr){ return expr->type == SEXI_ID; }
bool sexiExprIsStr(SexiExprConst expr){ return expr->type == SEXI_STR; }
bool sexiExprIsNum(SexiExprConst expr){ return expr->type == SEXI_NUM; }

static inline void sexiAllocListStr(SexiExpr list){
	auto str = sexiExprToStr(list->list.exprs[0]);
	std::string ret = "(" + std::string(str.ptr, str.len);

	for(size_t i = 1; i < list->list.n; i++){
		auto expr = list->list.exprs[i];
		auto str = sexiExprToStr(expr);
		ret += " " + std::string(str.ptr, str.len);
	}

	ret += ")";

	auto chars = new char[ret.size()];
	std::memcpy(chars, ret.data(), ret.size());

	list->ownedStr.len = ret.size();
	list->ownedStr.ptr = chars;
}

SexiStr sexiExprToStr(SexiExprConst expr){
	switch(expr->type){
		case SEXI_ID:
		case SEXI_STR:
		case SEXI_NUM:
			return expr->str;

		case SEXI_LIST:{
			if(!expr->ownedStr.ptr){
				// TODO: create work-around const_cast
				// possibly allocate string on creation
				sexiAllocListStr(const_cast<SexiExpr>(expr)); // ewww const_cast
			}

			return sexiRefOwnedStr(expr->ownedStr);
		}

		case SEXI_EMPTY:{
			static constexpr char emptyStr[] = "()";
			return { .len = sizeof(emptyStr)-1, .ptr = emptyStr };
		}

		default:{
			return { .len = 0, .ptr = nullptr };
		}
	}
}

size_t sexiExprLength(SexiExprConst expr){
	switch(expr->type){
		case SEXI_LIST: return expr->list.n;
		case SEXI_EMPTY: return 0;
		default: return 1;
	}
}

SexiExprConst sexiExprAt(SexiExprConst list, size_t idx){
	switch(list->type){
		case SEXI_LIST: return list->list.exprs[idx];
		default: return nullptr;
	}
}
