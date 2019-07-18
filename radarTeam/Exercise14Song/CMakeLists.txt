add_flashable_executable(e14Keyboard keyboard.cpp)
target_link_libraries(e14Keyboard util)

add_flashable_executable(e14Playerplayer player.cpp Note.h Song.h)
target_link_libraries(e14Playerplayer util)
