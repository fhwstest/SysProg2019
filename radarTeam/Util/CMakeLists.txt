add_library(util STATIC UART.h UART.cpp
                        Port.h
                        Pins.h
                        InterruptHandler.h
                        OneWire.h
                        Array.h
                        TempSensor.h
                        SString.h
                        Queue.h)

target_include_directories(util INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})