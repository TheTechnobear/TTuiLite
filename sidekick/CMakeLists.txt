project(sidekick)

include_directories(
        "${PROJECT_SOURCE_DIR}/../ttuilite"
        ${LIBCAIRO_INC}

)


set(SK_SRC 
    sidekick.cpp 
    SKApp.cpp SKApp.h 
    SKPrefs.cpp SKPrefs.h
    SKHardwareCallback.cpp SKHardwareCallback.h 
    SKOscCallback.cpp SKOscCallback.h
    )
add_executable(sidekick ${SK_SRC})
target_link_libraries(sidekick ttuilite ${LIBCAIRO} cjson pthread oscpack moodycamel)

