#ifndef SEXI_SEXI_HPP
#define SEXI_SEXI_HPP 1

#include <string_view>
#include <vector>
#include <memory>
#include <filesystem>

#include "sexi/Value.hpp"

namespace sexi{
	std::vector<std::unique_ptr<Value>> parse(std::string_view src);
	std::vector<std::unique_ptr<Value>> parseFile(const std::filesystem::path &p);
}

#endif // !SEXI_SEXI_HPP
