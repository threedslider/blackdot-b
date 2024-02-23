include_directories(..)

set(LIBS
tools
)


add_executable(Blackdot Main.cc)

target_link_libraries(Blackdot ${LIBS})