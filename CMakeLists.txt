cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR) 

project(sexi VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(SEXI_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

set(
	SEXI_HEADERS
	${SEXI_INCLUDE_DIR}/sexi.h
	${SEXI_INCLUDE_DIR}/sexi/Expr.hpp
)

add_subdirectory(lib)
add_subdirectory(testing)
