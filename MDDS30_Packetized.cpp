
#include "MDDS30_Packetized.h"
#include <Arduino.h>

MDDS30_Packetized::MDDS30_Packetized(
    HardwareSerial &serialPort,
    int address
)
    : _serial2(serialPort),
      _address(0),
      _started(false),
      _valid(false)
{
    _valid = setAddress(address);
}


bool MDDS30_Packetized::sync(int txPin, uint32_t baudRate)
{
    if (!_valid)
    return false;

    _serial2.begin(baudRate, SERIAL_8N1, -1, txPin);

    delay(1000);
    _serial2.write(0x80);
    _serial2.flush();

    _started = true;
    return true;
}


bool MDDS30_Packetized::setAddress(int address)
{
    if (address < 0 || address > 7)
        return false;

    _address = address;
    _valid = true;

    return true;
}


void MDDS30_Packetized::stopMotor(int channel)
{
    motor(channel, 0);
}

void MDDS30_Packetized::stopAll()
{
    motor(0, 0);
    motor(1, 0);
}


void MDDS30_Packetized::motor(int channel, int speed)
{
    if (!_started || !_valid)
        return;

    if (channel < 0 || channel > 1)
        return;

    if (speed > 100)
        speed = 100;

    if (speed < -100)
        speed = -100;

    int value;

    if (speed < 0)
        value = 127 + speed * 127 / 100;
    else
        value = 127 + speed * 128 / 100;

    int byte1 = (channel << 3) | _address;

    uint8_t checksum = (0x55 + byte1 + value) & 0xFF;

    _serial2.write(0x55);
    _serial2.write((uint8_t)byte1);
    _serial2.write((uint8_t)value);
    _serial2.write(checksum);
}

