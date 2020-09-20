#ifndef SEXI_EXPR_HPP
#define SEXI_EXPR_HPP 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

typedef enum {
	SEXI_EMPTY, SEXI_ID, SEXI_LIST, SEXI_STR, SEXI_NUM,
	SEXI_EXPRTYPE_COUNT
} SexiExprType;

typedef struct {
	size_t len;
	const char *ptr;
} SexiStr;

typedef struct {
	size_t len;
	char *ptr;
} SexiOwnedStr;

inline SexiStr sexiRefOwnedStr(SexiOwnedStr owned){
	return { .len = owned.len, .ptr = owned.ptr };
}

typedef struct SexiExprT *SexiExpr;
typedef const struct SexiExprT *SexiExprConst;

void sexiDestroyExpr(SexiExpr expr);

SexiExpr sexiCreateEmpty();
SexiExpr sexiCreateList(size_t n, const SexiExprConst *exprs);
SexiExpr sexiCreateId(SexiStr str);
SexiExpr sexiCreateStr(SexiStr str);
SexiExpr sexiCreateNum(SexiStr str);

SexiExprType sexiExprType(SexiExprConst expr);

bool sexiExprIsEmpty(SexiExprConst expr);
bool sexiExprIsList(SexiExprConst expr);
bool sexiExprIsId(SexiExprConst expr);
bool sexiExprIsStr(SexiExprConst expr);
bool sexiExprIsNum(SexiExprConst expr);

SexiStr sexiExprToStr(SexiExprConst expr);
size_t sexiExprLength(SexiExprConst expr);
SexiExprConst sexiExprAt(SexiExprConst list, size_t idx);

#ifdef __cplusplus
}

#include <string_view>

inline bool operator==(const std::string_view &lhs, const SexiStr &rhs) noexcept{
	return lhs == std::string_view(rhs.ptr, rhs.len);
}

inline bool operator==(const SexiStr &lhs, const std::string_view &rhs) noexcept{
	return std::string_view(lhs.ptr, lhs.len) == rhs;
}

namespace sexi{
	template<SexiExprType type> struct TypeTag{};

	inline constexpr TypeTag<SEXI_EMPTY> empty;
	inline constexpr TypeTag<SEXI_LIST> list;
	inline constexpr TypeTag<SEXI_ID> id;
	inline constexpr TypeTag<SEXI_STR> str;
	inline constexpr TypeTag<SEXI_NUM> num;

	class Expr{
		public:
			Expr(SexiExprConst expr_)
				: m_ownsExpr(false), m_expr(expr_){}

			explicit Expr(TypeTag<SEXI_EMPTY>) noexcept
				: m_ownsExpr(true), m_owned(sexiCreateEmpty()){}

			Expr(TypeTag<SEXI_LIST>, size_t n, SexiExprConst *exprs) noexcept
				: m_ownsExpr(true), m_owned(sexiCreateList(n, exprs)){}

			Expr(TypeTag<SEXI_ID>, std::string_view str) noexcept
				: m_ownsExpr(true), m_owned(sexiCreateId({ .len = str.size(), .ptr = str.data() })){}

			Expr(TypeTag<SEXI_STR>, std::string_view str) noexcept
				: m_ownsExpr(true), m_owned(sexiCreateStr({ .len = str.size(), .ptr = str.data() })){}

			Expr(TypeTag<SEXI_NUM>, std::string_view str) noexcept
				: m_ownsExpr(true), m_owned(sexiCreateNum({ .len = str.size(), .ptr = str.data() })){}

			Expr(const Expr &other) noexcept
				: m_ownsExpr(false), m_expr(other.m_expr){}

			Expr(Expr &&other) noexcept
				: m_ownsExpr(other.m_ownsExpr), m_expr(other.m_expr)
			{
				other.m_expr = nullptr;
				other.m_ownsExpr = false;
			}

			Expr &operator=(const Expr &other) noexcept{
				if(&other != this){
					if(m_ownsExpr){
						sexiDestroyExpr(m_owned);
					}

					m_expr = other.m_expr;
					m_ownsExpr = false;
				}

				return *this;
			}

			Expr &operator=(Expr &&other) noexcept{
				if(&other != this){
					if(m_ownsExpr){
						sexiDestroyExpr(m_owned);
					}

					m_expr = other.m_expr;
					m_ownsExpr = other.m_ownsExpr;

					other.m_expr = nullptr;
					other.m_ownsExpr = false;
				}

				return *this;
			}

			~Expr(){
				if(m_ownsExpr){
					sexiDestroyExpr(m_owned);
				}
			}

			operator SexiExprConst() const noexcept{ return m_expr; }

			SexiExprType type() const noexcept{ return sexiExprType(m_expr); }

			std::size_t length() const noexcept{ return sexiExprLength(m_expr); }

			Expr operator[](std::size_t idx) const noexcept{
				return sexiExprAt(m_expr, idx);
			}

			std::string_view toStr() const noexcept{
				auto str = sexiExprToStr(m_expr);
				return std::string_view(str.ptr, str.len);
			}

			bool isEmpty() const noexcept{ return sexiExprIsEmpty(m_expr); }
			bool isList() const noexcept{ return sexiExprIsList(m_expr); }
			bool isId() const noexcept{ return sexiExprIsId(m_expr); }
			bool isStr() const noexcept{ return sexiExprIsStr(m_expr); }
			bool isNum() const noexcept{ return sexiExprIsNum(m_expr); }

		private:
			bool m_ownsExpr;
			union{
				SexiExprConst m_expr;
				SexiExpr m_owned;
			};
	};
}
#endif // __cplusplus

#endif // !SEXI_EXPR_HPP
