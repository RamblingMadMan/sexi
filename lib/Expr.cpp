#include <cassert>
#include <cstdlib>
#include <cstring>

#include <string>
#include <memory>

#include "sexi/Expr.h"

using namespace sexi;

struct SexiExprT{
	SexiExprType type;
	union {
		SexiStr str;
		struct {
			size_t n;
			SexiExpr *exprs;
		} list;
	};
	SexiOwnedStr ownedStr;
};

std::vector<Expr> Expr::toList() const noexcept{
	if(!isList()) return { *this };

	std::vector<Expr> ret;
	ret.reserve(m_expr->list.n);

	for(std::size_t i = 0; i < m_expr->list.n; i++){
		ret.emplace_back(m_expr->list.exprs[i]);
	}

	return ret;
}

void sexiDestroyExpr(SexiExpr expr){
	if(sexiExprIsList(expr)){
		std::free(expr->list.exprs);
	}
	else if(expr->ownedStr.ptr){
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
	ret->list.n = 0;
	ret->list.exprs = nullptr;
	return ret;
}

SexiExpr sexiCreateEmpty(){
	auto ret = allocExpr(SEXI_EMPTY);
	ret->str = { .len = 2, .ptr = "()" };
	return ret;
}

SexiExpr sexiCloneExpr(SexiExprConst expr){
	if(!expr) return nullptr;

	if(sexiExprIsList(expr)){
		return sexiCreateList(expr->list.n, expr->list.exprs);
	}

	auto ret = allocExpr(expr->type);
	ret->str = expr->str;

	sexiExprOwnString(ret);

	return ret;
}

SexiExpr sexiCreateList(size_t n, const SexiExprConst *exprs){
	if(n == 0) return sexiCreateEmpty();

	auto ret = allocExpr(SEXI_LIST);

	SexiExpr *newList = (SexiExpr*)std::malloc(sizeof(SexiExpr) * n);

	for(std::size_t i = 0; i < n; i++){
		newList[i] = sexiCloneExpr(exprs[i]);

		if(!sexiExprIsList(newList[i])){
			sexiExprOwnString(newList[i]);
		}
	}

	ret->list = { .n = n, .exprs = newList };
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

	auto strView = std::string_view(str.ptr, str.len);

	if(strView.find('.') != std::string_view::npos){
		auto strEnd = strView.find_last_not_of("0");
		auto numStr = strView.substr(0, strEnd + 1);

		str.len = numStr.size();
	}

	ret->str = { .len = str.len, .ptr = str.ptr };
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

	auto chars = (char*)std::malloc(ret.size() + 1);
	std::memcpy(chars, ret.data(), ret.size());
	chars[ret.size()] = '\0';

	list->ownedStr.len = ret.size();
	list->ownedStr.ptr = chars;
}

SexiStr sexiExprToStr(SexiExprConst expr){
	if(expr->ownedStr.ptr) return sexiRefStr(expr->ownedStr);

	switch(expr->type){
		case SEXI_ID:
		case SEXI_STR:
		case SEXI_NUM:
			return expr->str;

		case SEXI_LIST:{
			// TODO: create work-around const_cast
			// possibly allocate string on creation
			sexiAllocListStr(const_cast<SexiExpr>(expr)); // ewww const_cast

			return sexiRefStr(expr->ownedStr);
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

void sexiExprOwnString(SexiExpr expr){
	if(sexiExprIsEmpty(expr) || expr->ownedStr.ptr) return;

	expr->ownedStr.len = expr->str.len;
	expr->ownedStr.ptr = (char*)std::malloc(expr->str.len + 1);

	std::memcpy(expr->ownedStr.ptr, expr->str.ptr, expr->str.len);
	expr->ownedStr.ptr[expr->str.len] = '\0'; // null terminate string

	expr->str.ptr = expr->ownedStr.ptr;
}

SexiExprConst sexiExprAt(SexiExprConst list, size_t idx){
	switch(list->type){
		case SEXI_LIST: return list->list.exprs[idx];
		default: return nullptr;
	}
}
