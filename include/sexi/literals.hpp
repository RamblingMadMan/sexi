#include "../sexi.h"
#include "Expr.h"

namespace sexi::literals{
	inline Expr operator ""_se(const char *cstr, std::size_t len){
		if(len == 0) return Expr(empty);
		else return Expr(std::string_view(cstr, len));
	}

	inline Expr operator ""_se(unsigned long long val){ return Expr(num, val); }
	inline Expr operator ""_se(long double val){ return Expr(num, val); }
}
