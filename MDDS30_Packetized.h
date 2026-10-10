#ifndef MDDS30_PACKETIZED_H
#define MDDS30_PACKETIZED_H

#include <Arduino.h>

class MDDS30_Packetized{
public:

    MDDS30_Packetized(
        HardwareSerial &serialPort = Serial2,
        int address = 0
    );
// to define the pin and baud rate
    void sync(int txPin, uint32_t baudRate = 115200);
// To set the address
    void setAddress(int address);
// To specify channel we are using
    void motor(int channel, int speed);
    void stopMotor(int channel);
    void stopAll();

private:
    HardwareSerial &_serial;
    int _address;
    bool _started;
};
#endif
