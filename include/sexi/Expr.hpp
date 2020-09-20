#ifndef SEXI_EXPR_HPP
#define SEXI_EXPR_HPP 1

/**
 * @defgroup Exprs Expressions
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Enumeration of possible types of expression.
 */
typedef enum {
	SEXI_EMPTY, SEXI_ID, SEXI_LIST, SEXI_STR, SEXI_NUM,
	SEXI_EXPRTYPE_COUNT
} SexiExprType;

/**
 * @brief Type representing a view of a string.
 */
typedef struct {
	size_t len;
	const char *ptr;
} SexiStr;

/**
 * @brief Type representing an owned string.
 */
typedef struct {
	size_t len;
	char *ptr;
} SexiOwnedStr;

/**
 * @brief Get a non-owning handle to an owned string.
 * @param owned owned string to reference
 * @returns view of owned string
 */
inline SexiStr sexiRefOwnedStr(SexiOwnedStr owned){
	return { .len = owned.len, .ptr = owned.ptr };
}

/**
 * @brief Opaque type representing an s-expression.
 */
typedef struct SexiExprT *SexiExpr;

/**
 * @brief Opaque handle to a immutable s-expression.
 */
typedef const struct SexiExprT *SexiExprConst;

/**
 * @brief Destroy an expression.
 * @param expr
 */
void sexiDestroyExpr(SexiExpr expr);

/**
 * @brief Create an empty expression.
 * @returns newly created expression
 */
SexiExpr sexiCreateEmpty();

/**
 * @brief Create a list expression.
 * @param n number of elements in the list
 * @param exprs pointer to the elements of the list
 * @returns newly created list expression
 */
SexiExpr sexiCreateList(size_t n, const SexiExprConst *exprs);

/**
 * @brief Create an ID expression.
 * @param str the identifier
 * @returns newly created expression
 */
SexiExpr sexiCreateId(SexiStr str);

/**
 * @brief Create a string expression.
 * @param str the string
 * @returns newly created expression
 */
SexiExpr sexiCreateStr(SexiStr str);

/**
 * @brief Create a number expression.
 * @param str the string
 * @returns newly created expression
 */
SexiExpr sexiCreateNum(SexiStr str);

/**
 * @brief Get the type of an expression.
 * @param expr the expression to query
 * @returns type of the expression
 */
SexiExprType sexiExprType(SexiExprConst expr);

/**
 * @brief Check if an expression is empty.
 * @param expr expression to check
 * @returns whether the expression is empty
 */
bool sexiExprIsEmpty(SexiExprConst expr);

/**
 * @brief Check if an expression is a list.
 * @param expr expression to check
 * @returns whether the expression is a list
 */
bool sexiExprIsList(SexiExprConst expr);

/**
 * @brief Check if an expression is an ID.
 * @param expr expression to check
 * @returns whether the expression is an ID
 */
bool sexiExprIsId(SexiExprConst expr);

/**
 * @brief Check if an expression is a string.
 * @param expr expression to check
 * @returns whether the expression is a string
 */
bool sexiExprIsStr(SexiExprConst expr);

/**
 * @brief Check if an expression is a number.
 * @param expr expression to check
 * @returns whether the expression is a number
 */
bool sexiExprIsNum(SexiExprConst expr);

/**
 * @brief Get the string representation of an expression.
 * @param expr expression to query
 * @returns the expression represented as a string
 */
SexiStr sexiExprToStr(SexiExprConst expr);

/**
 * @brief Get the length of an expression.
 * Empty expression always return 0, and non-list expressions always return 1.
 * @param expr expression to query
 * @returns number of elements in the expression
 */
size_t sexiExprLength(SexiExprConst expr);

/**
 * @brief Get an element of a list expression
 * @param list list expression to query
 * @param idx index of the list element
 * @returns element at \p idx of \p list .
 */
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

/**
 * @}
 */

#endif // !SEXI_EXPR_HPP
