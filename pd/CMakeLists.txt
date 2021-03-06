###############################
# Pure Data external for TTuiLite
project(TTuiPd)

set(PD_CMAKE_PATH C:/Program\ Files\ \(x86\)/Pd/bin)


set(TTUIPD_SRC
        TTuiPd.cpp
        TTuiPd.h)


include_directories(
        "${PROJECT_SOURCE_DIR}/../ttuilite"
)

add_library(TTuiPd SHARED ${TTUIPD_SRC})

target_link_libraries(TTuiPd ttuilite)


### setup for pure data
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
if (${APPLE})
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_darwin")
elseif (${UNIX})
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_linux")
elseif (${WIN32})
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dll")
    find_library(PD_LIBRARY NAMES pd HINTS ${PD_CMAKE_PATH})
    target_link_libraries(${PROJECT_NAME} ${PD_LIBRARY})
endif ()

# Removes some warning for Microsoft Visual C.
if (${MSVC})
    target_compile_definitions(${PROJECT_NAME} PRIVATE PD_INTERNAL)
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS "/D_CRT_SECURE_NO_WARNINGS /wd4091 /wd4996")
endif ()
