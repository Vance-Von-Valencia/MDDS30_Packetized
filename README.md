MDDS30_Packetized

An Arduino library for controlling the Cytron MDDS30 motor driver using Serial Packetized communication.

The library supports motor speed and direction control, individual motor stopping, driver addressing, and UART initialization on ESP32.

Features

- Serial Packetized communication
- Compatible with ESP32 "HardwareSerial"
- Configurable driver address
- Independent control of left and right motors
- Forward and reverse motor control
- Individual motor stop
- Stop both motors
- Automatic baud-rate detection initialization using the dummy byte

Requirements

- ESP32 development board
- Cytron MDDS30 motor driver
- Arduino IDE
- Cytron MDDS30 configured for Serial Packetized mode

Installation

1. Download or clone this repository.
2. Place the "MDDS30_Packetized" folder inside your Arduino "libraries" directory.
3. Restart the Arduino IDE.
4. Include the library in your sketch.

#include "MDDS30_Packetized.h"

Wiring

Connect the ESP32 UART TX pin to the MDDS30 serial input and connect the grounds together.

ESP32| Cytron MDDS30
UART TX| Serial input (RX)
GND| GND

Configure the driver's DIP switches for the required address and select Serial Packetized mode according to the MDDS30 user manual.

Basic Usage

1. Create the driver object

#include "MDDS30_Packetized.h"

MDDS30_Packetized driver(Serial2, 1);

- "Serial2" is the ESP32 hardware UART used for communication.
- "1" is the MDDS30 address configured using its DIP switches.

2. Initialize communication

void setup()
{
    driver.sync(17, 115200);
}

- "17" is the ESP32 TX pin.
- "115200" is the baud rate.
- "sync()" initializes the UART and sends the dummy byte required for auto-baud detection.

3. Control the motors

void loop()
{
    driver.motor(0, 50);    // Left motor, forward
    delay(2000);

    driver.motor(1, 50);    // Right motor, forward
    delay(2000);

    driver.stopAll();
    delay(1000);

    driver.motor(0, -50);   // Left motor, reverse
    delay(2000);

    driver.stopAll();
    delay(1000);
}

API Reference

"MDDS30_Packetized(HardwareSerial &serialPort, int address)"

Creates a driver object using the specified hardware UART and driver address.

"sync(int txPin, uint32_t baudRate = 115200)"

Initializes UART communication and sends the dummy byte for auto-baud detection.

Parameters

- "txPin": ESP32 UART transmit pin.
- "baudRate": Serial communication speed.

"motor(int channel, int speed)"

Controls the selected motor.

Parameter| Description
"channel = 0"| Left motor
"channel = 1"| Right motor
"speed = -100"| Full reverse
"speed = 0"| Stop
"speed = 100"| Full forward

Speed values between "-100" and "100" set intermediate motor commands.

"stopMotor(int channel)"

Stops the selected motor.

driver.stopMotor(0);  // Stop left motor
driver.stopMotor(1);  // Stop right motor

"stopAll()"

Stops both motors of the selected MDDS30 driver.

driver.stopAll();

"setAddress(int address)"

Sets the MDDS30 driver address.

The supported address range is "0" to "7", corresponding to the address configured by the driver's DIP switches.

driver.setAddress(1);

Serial Packet Format

The library sends a four-byte packet for each motor command.

Byte| Description
1| Header: "0x55"
2| Motor channel and driver address
3| Motor speed and direction command
4| Checksum

The checksum is calculated by adding the first three bytes and retaining the lowest eight bits.

Driver Addressing

Each MDDS30 driver can be assigned an address from "0" to "7" using its DIP switches. The library uses the configured address to direct motor commands to the intended driver.

License

See the repository's license file for licensing and usage terms.