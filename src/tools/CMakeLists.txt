set(SRCS
BDMatrix3x3.cc
BDVector.cc
)

set(HEADERS
BDMatrix3x3.hpp
BDVector.hpp
)

add_library(tools ${SRCS} ${HEADERS})

