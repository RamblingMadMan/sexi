#ifndef SEXI_SEXI_H
#define SEXI_SEXI_H 1

#include "sexi/Expr.hpp"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SexiParseResultT *SexiParseResult;

SexiParseResult sexiParse(size_t len, const char *ptr);

void sexiDestroyParseResult(SexiParseResult res);

bool sexiParseResultHasError(SexiParseResult res);
SexiStr sexiParseResultError(SexiParseResult res);

size_t sexiParseResultNumExprs(SexiParseResult res);
const SexiExprConst *sexiParseResultExprs(SexiParseResult res);

#ifdef __cplusplus
}

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

			friend ParseResult parse(std::string_view);
	};

	inline ParseResult parse(std::string_view src){
		auto res = sexiParse(src.size(), src.data());
		return ParseResult(res);
	}
}
#endif // __cplusplus

#endif // !SEXI_SEXI_H
