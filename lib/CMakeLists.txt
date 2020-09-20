set(
	SEXI_SOURCES
	parse.cpp
	Expr.cpp
)

add_library(sexi SHARED ${SEXI_HEADERS} ${SEXI_SOURCES})

target_include_directories(sexi PUBLIC ${SEXI_INCLUDE_DIR})
