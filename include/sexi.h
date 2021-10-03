#ifndef SEXI_SEXI_H
#define SEXI_SEXI_H 1

#include "sexi/Expr.h"

/**
 * @defgroup Parsing Parsing
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque type representing result of parsing.
 */
typedef struct SexiParseResultT *SexiParseResult;

/**
 * @brief Parse s-expressions from a string.
 * @param len length of the string
 * @param ptr pointer to the string
 * @param copyStrs whether to make copies of refed strings
 * @returns newly created parse result
 * @see sexiDestroyParseResult
 */
SexiParseResult sexiParse(size_t len, const char *ptr, bool copyStrs);

/**
 * @brief Destroy a parse result created by \ref sexiParse .
 * @param res result to destroy
 */
void sexiDestroyParseResult(SexiParseResult res);

/**
 * @brief Check if a parse result contains an error
 * @param res result to check
 * @returns whether the result contains an error
 */
bool sexiParseResultHasError(SexiParseResult res);

/**
 * @brief Get the error string from a parse result.
 * @param res result to query
 * @returns error string or a `NULL` string of 0 length
 */
SexiStr sexiParseResultError(SexiParseResult res);

/**
 * @brief Get the number of expressions in a parse result.
 * @param res result to check
 * @returns number of expressions in the result
 */
size_t sexiParseResultNumExprs(SexiParseResult res);

/**
 * @brief Get the expressions from a parse result.
 * @param res result to query
 * @returns pointer to the expressions contained in result or `NULL`
 */
const SexiExprConst *sexiParseResultExprs(SexiParseResult res);

#ifdef __cplusplus
}

#include <vector>
#include <string_view>

namespace sexi{
	class ParseResult{
		public:
			~ParseResult(){
				sexiDestroyParseResult(m_res);
			}

			bool hasError() noexcept{ return sexiParseResultHasError(m_res); }

			std::string_view error() noexcept{
				auto str = sexiParseResultError(m_res);
				return { str.ptr, str.len };
			}

			std::size_t size() const noexcept{ return m_exprs.size(); }
			const Expr &operator[](std::size_t idx) const noexcept{ return m_exprs[idx]; }

			SexiExprConst operator[](std::size_t idx) noexcept{ return sexiParseResultExprs(m_res)[idx]; }

			auto begin() const noexcept{ return cbegin(m_exprs); }
			auto end() const noexcept{ return cend(m_exprs); }

			const std::vector<Expr> &exprs() const noexcept{ return m_exprs; }

		private:
			explicit ParseResult(SexiParseResult res_) noexcept
				: m_res(res_)
			{
				auto n = sexiParseResultNumExprs(m_res);
				auto p = sexiParseResultExprs(m_res);

				m_exprs.reserve(n);
				for(size_t i = 0; i < n; i++){
					m_exprs.emplace_back(p[i]);
				}
			}

			SexiParseResult m_res;
			std::vector<Expr> m_exprs;

			friend ParseResult parse(std::string_view, bool);
	};

	inline ParseResult parse(std::string_view src, bool copyStrs = true){
		auto res = sexiParse(src.size(), src.data(), copyStrs);
		return ParseResult(res);
	}
}
#endif // __cplusplus

/**
 * @}
 */

#endif // !SEXI_SEXI_H
