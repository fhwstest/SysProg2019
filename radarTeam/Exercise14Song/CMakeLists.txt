add_flashable_executable(e14Keyboard keyboard.cpp)
target_link_libraries(e14Keyboard util)

add_flashable_executable(e14Player player.cpp Note.h Song.h)
target_link_libraries(e14Player util)
