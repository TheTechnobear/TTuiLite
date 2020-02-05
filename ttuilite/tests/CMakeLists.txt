# basic testapp

#find_path(LIBCAIRO_INC NAME cairo.h PATHS /usr/include/cairo /usr/local/include/cairo)
#find_library(LIBCAIRO NAME cairo PATHS /usr/local/lib /usr/lib)

include_directories (
        "${PROJECT_SOURCE_DIR}/.."
)


set(TEST_SRC "ttuitest.cpp")
add_executable(ttuitest ${TEST_SRC})
target_link_libraries (ttuitest ttuilite pthread )
#target_link_libraries (ttuitest uilite ${LIBCAIRO} pthread )

#if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
#  target_link_libraries(fatestest atomic)
#endif()
#
#target_link_libraries(fatestest pthread)

