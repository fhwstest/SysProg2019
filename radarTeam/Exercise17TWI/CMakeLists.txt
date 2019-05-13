add_flashable_executable(e17TWI main.cpp)
target_link_libraries(e17TWI util)

add_flashable_executable(e17TWIDietrich mainDietrich.cpp)
target_link_libraries(e17TWIDietrich util)