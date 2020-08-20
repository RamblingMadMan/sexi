set(
	SEXI_SOURCES
	parse.cpp
)

add_library(sexi SHARED ${SEXI_HEADERS} ${SEXI_SOURCES})
