#ifndef SEXI_VALUE_HPP
#define SEXI_VALUE_HPP 1

#include <vector>
#include <memory>
#include <string>
#include <variant>

namespace sexi{
	namespace tags{
		struct List{};
		struct Id{};
		struct Str{};
		struct Num{};

		inline constexpr List list;
		inline constexpr Id id;
		inline constexpr Str str;
		inline constexpr Num num;
	}

	class Value;

	using ValuePtr = std::unique_ptr<Value>;

	class Value{
		public:
			Value(tags::List, std::vector<ValuePtr> elems) noexcept
				: m_val(std::make_pair(tags::list, std::move(elems))){}

			Value(tags::Id, std::string id) noexcept
				: m_val(std::make_pair(tags::id, std::move(id))){}

			Value(tags::Str, std::string str) noexcept
				: m_val(std::make_pair(tags::str, std::move(str))){}

			Value(tags::Num, std::string num) noexcept
				: m_val(std::make_pair(tags::num, std::move(num))){}

			std::size_t size() const noexcept{
				auto getSize = [](auto &&tagged) -> std::size_t{
					using Tag = std::remove_cv_t<std::decay_t<decltype(tagged.first)>>;

					if constexpr(std::is_same_v<Tag, tags::List>){
						return tagged.second.size();
					}
					else{
						return 0;
					}
				};

				return std::visit(getSize, m_val);
			}

			const Value *operator[](const std::size_t idx) const noexcept{
				auto getElem = [idx](auto &&tagged) -> Value*{
					using Tag = std::remove_cv_t<std::decay_t<decltype(tagged.first)>>;

					if constexpr(std::is_same_v<Tag, tags::List>){
						return tagged.second[idx].get();
					}
					else{
						return nullptr;
					}
				};

				return std::visit(getElem, m_val);
			}

			const Value *at(const std::size_t idx) const noexcept{
				return (*this)[idx];
			}

			std::string toStr() const noexcept{
				auto getStr = [](auto &&tagged) -> std::string{
					using Tag = std::remove_cv_t<std::decay_t<decltype(tagged.first)>>;

					if constexpr(std::is_same_v<Tag, tags::List>){
						std::string ret = "(";
						if(!tagged.second.empty()){
							ret += tagged.second[0]->toStr();

							for(std::size_t i = 1; i < tagged.second.size(); i++){
								ret += " " + tagged.second[i]->toStr();
							}
						}

						ret += ')';
						return ret;
					}
					else{
						return tagged.second;
					}
				};

				return std::visit(getStr, m_val);
			}

			bool isList() const noexcept{ return hasTag<tags::List>(); }
			bool isId() const noexcept{ return hasTag<tags::Id>(); }
			bool isStr() const noexcept{ return hasTag<tags::Str>(); }
			bool isNum() const noexcept{ return hasTag<tags::Num>(); }

		private:
			template<typename Tag>
			bool hasTag() const noexcept{
				auto doHasTag = [](auto &&tagged){
					using ValueTag = std::remove_cv_t<std::decay_t<decltype(tagged.first)>>;
					return std::is_same_v<Tag, ValueTag>;
				};

				return std::visit(doHasTag, m_val);
			}

			std::variant<
				std::pair<tags::List, std::vector<ValuePtr>>,
				std::pair<tags::Id, std::string>,
				std::pair<tags::Str, std::string>,
				std::pair<tags::Num, std::string>
			> m_val;
	};
}

#endif // !SEXI_VALUE_HPP
