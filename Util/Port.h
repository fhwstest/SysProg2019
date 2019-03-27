#ifndef AVR_PORTS_H
#define AVR_PORTS_H

enum class Port {
    D = 0x10,
    C = 0x13,
    B = 0x16,
    A = 0x19,
};

enum class PortType {
    Pin,
    Port
};

enum class IOType {
    Input,
    Output
};


#endif //AVR_PORTS_H
