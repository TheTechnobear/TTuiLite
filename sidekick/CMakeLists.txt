project(sidekick)

include_directories(
        "${PROJECT_SOURCE_DIR}/../ttuilite"
)


set(SK_SRC 
    sidekick.cpp 
    SKApp.cpp SKApp.h 
    SKPrefs.cpp SKPrefs.h
    SKHardwareCallback.cpp SKHardwareCallback.h 
    SKOscCallback.cpp SKOscCallback.h
    )
add_executable(sidekick ${SK_SRC})
target_link_libraries(sidekick ttuilite cjson pthread oscpack moodycamel wiringPi)

