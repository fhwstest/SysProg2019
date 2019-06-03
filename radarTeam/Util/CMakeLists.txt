add_library(util STATIC UART.h UART.cpp
                        Port.h
                        Pins.h
                        InterruptHandler.h
                        OneWire.h
                        array/Array.h
                        array/DynamicArray.h
                        array/ArrayIterator.h
                        TempSensor.h)

target_include_directories(util INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})