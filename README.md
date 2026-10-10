# MDDS30_Packetized
A library to use Packetized mode in CYTRON MDDS30 
MDDS30_Packetized

A lightweight Arduino library for controlling Cytron MDDS30 motor drivers in Serial Packetized mode using an ESP32 and a "HardwareSerial" UART.

This library provides functions to set a driver's address, control the speed and direction of either motor channel, stop an individual motor, and stop both motor channels on a driver.

Features

- Supports ESP32 "HardwareSerial", such as "Serial2".
- Supports packetized driver addresses from "0" to "7".
- Controls motor speed and direction using signed speed values.
- Stops individual motor channels.
- Stops both motor channels on one driver.
- Supports multiple driver objects using the same UART, provided each driver has a unique address and the library's shared-UART initialization limitation is addressed.
- Uses packetized serial commands.

Requirements

- ESP32 development board.
- Arduino IDE or another compatible Arduino development environment.
- Cytron MDDS30 motor driver.
- Driver configured for Serial Packetized mode.
- Suitable motor power supply and serial wiring.

Note: This implementation uses ESP32-specific "HardwareSerial" functionality. It is not currently designed for Arduino Uno or other platforms without compatible "HardwareSerial" support.

Installation

1. Create a folder named "MDDS30_Packetized".
2. Place these files inside it:
   - "MDDS30_Packetized.h"
   - "MDDS30_Packetized.cpp"
3. Place the folder in your Arduino libraries directory.
4. Restart Arduino IDE if necessary.
5. Include the library in your sketch:

#include <MDDS30_Packetized.h>

Make sure the source and header files use "#include <Arduino.h>", not "#include <Ardiuno.h>".

Wiring and Configuration

1. Configure the MDDS30 for Serial Packetized mode according to the manufacturer's instructions.
2. Set the driver's address using its DIP switches.
3. Connect the ESP32 UART TX pin to the appropriate serial input of the MDDS30.
4. Connect signal ground as required by the hardware documentation.
5. Select an available ESP32 UART and TX pin.
6. Ensure the configured address in your code matches the physical driver's address.

Important: Verify the driver's serial wiring, voltage levels, baud rate, DIP-switch settings, and packetized-mode configuration using the official MDDS30 manual.

API Reference

1. Constructor

MDDS30_Packetized(
    HardwareSerial &serialPort = Serial2,
    int address = 0
);

Creates an object representing an MDDS30 driver.

Parameters:

- "serialPort": UART used to communicate with the driver.
- "address": Packetized address of the driver, from "0" to "7".

Example:

MDDS30_Packetized driver(Serial2, 0);

This creates a driver object using "Serial2" with address "0".

2. sync()

void sync(int txPin, uint32_t baudRate = 115200);

Initializes the UART and sends the "0x80" auto-baud synchronization byte after a one-second delay.

Parameters:

- "txPin": ESP32 GPIO used as the UART TX pin.
- "baudRate": UART baud rate, defaulting to "115200".

The implementation uses:

_serial.begin(baudRate, SERIAL_8N1, -1, txPin);

Here, "-1" indicates that no RX pin is assigned.

Important: Call this function during initialization, not repeatedly inside a control loop.

When multiple driver objects share the same UART, synchronization should be performed once for the shared bus. The current implementation maintains "_started" separately for every object, so additional objects need a proper shared-UART initialization solution before they can send commands.

3. setAddress()

void setAddress(int address);

Sets the packetized address for the driver object.

The current implementation clamps addresses to the range "0" through "7".

Example:

driver.setAddress(1);

The physical driver's DIP-switch address must match the address used by the software.

4. motor()

void motor(int channel, int speed);

Controls the speed and direction of one motor channel.

Parameters:

- "channel": Motor channel identifier, "0" or "1".
- "speed": Signed speed command from "-100" to "100".

Speed| Command
"-100"| Maximum reverse command
"-50"| Approximately half reverse command
"0"| Stop command
"50"| Approximately half forward command
"100"| Maximum forward command

Values outside the permitted speed range are clamped. Invalid channel values are ignored.

Examples:

driver.motor(0, 50);   // Channel 0, forward command
driver.motor(1, -30);  // Channel 1, reverse command
driver.motor(0, 0);    // Stop channel 0

The channel numbering and speed encoding should be verified against the official protocol specification for your MDDS30.

5. stopMotor()

void stopMotor(int channel);

Stops one motor channel by sending a zero-speed command.

Example:

driver.stopMotor(0);

This stops channel "0" on the selected driver object.

6. stopAll()

void stopAll();

Stops both motor channels belonging to the selected driver object.

Example:

driver.stopAll();

If multiple driver objects are being used, call "stopAll()" on each driver that should stop.

This function is a software stop command and is not a substitute for a physical emergency-stop circuit.

Example 1: Controlling One MDDS30 Driver

This example demonstrates basic motor control using one driver object.

#include <MDDS30_Packetized.h>

MDDS30_Packetized driver(Serial2, 0);

const int MDDS30_TX_PIN = 17;

void setup() {
    driver.sync(MDDS30_TX_PIN, 115200);
}

void loop() {
    driver.motor(0, 40);
    delay(2000);

    driver.stopMotor(0);
    delay(1000);

    driver.motor(1, -40);
    delay(2000);

    driver.stopAll();
    delay(2000);
}

Operation:

1. Initializes the serial connection and performs synchronization.
2. Commands channel "0" to run forward at a speed command of "40".
3. Stops channel "0".
4. Commands channel "1" to run in reverse at a speed command of "40".
5. Stops both channels.

The example's TX pin is illustrative. Select a GPIO that is available on your ESP32 and connected correctly to the driver.

Example 2: Two MDDS30 Drivers on One UART

Two objects can share the same UART while using different driver addresses.

#include <MDDS30_Packetized.h>

MDDS30_Packetized driver1(Serial2, 0);
MDDS30_Packetized driver2(Serial2, 1);

const int MDDS30_TX_PIN = 17;

void setup() {
    driver1.sync(MDDS30_TX_PIN, 115200);
}

void loop() {
    driver1.motor(0, 25);
    driver1.motor(1, 25);

    delay(1000);

    driver1.stopAll();

    delay(1000);
}

This example initializes and controls only "driver1".

Current implementation limitation: Calling "driver1.sync()" sets "driver1._started" to "true", but "driver2._started" remains "false". Consequently, "driver2.motor()" and "driver2.stopAll()" will not send commands until the library is updated to support shared-UART initialization correctly.

The library should separate UART initialization and synchronization from the per-driver address state, or provide another safe mechanism for marking additional driver objects as ready.

Each physical driver must have a unique address matching its software object.

Packet Format

The current implementation sends a four-byte packet.

Byte| Description
Byte 1| Header: "0x55"
Byte 2| Channel and driver address
Byte 3| Encoded speed command
Byte 4| Checksum

The channel/address byte is calculated as:

int byte1 = (channel << 3) | _address;

The checksum is calculated as:

uint8_t checksum =
    (0x55 + byte1 + value) & 0xFF;

The speed conversion maps signed speed values to command bytes from "0" to "255", with "127" representing zero speed in this implementation.

Protocol verification is essential: Confirm the packet header, address/channel bit layout, speed encoding, checksum calculation, and channel numbering against the official Cytron MDDS30 protocol documentation before using the library on hardware.

Important Implementation Notes

- Correct the spelling of "Arduino.h" in both source files.
- The "_started" variable belongs to each individual driver object.
- The current shared-UART initialization design must be fixed before controlling two drivers through separate objects.
- "stopAll()" stops both channels on one driver object, not every driver on the UART.
- The library does not provide motor-speed feedback or encoder feedback.
- The library does not implement acceleration ramps, current sensing, or an emergency-stop circuit.
- Successful transmission through "write()" does not confirm that the driver received or executed the command.
- Address and speed inputs are clamped, but invalid channel inputs are ignored without an error message.

Troubleshooting

The code does not compile

- Confirm that the filenames are "MDDS30_Packetized.h" and "MDDS30_Packetized.cpp".
- Replace "Ardiuno.h" with "Arduino.h".
- Confirm that the ESP32 board package is installed and the correct board is selected.
- Ensure the function declarations in the header match the definitions in the source file.

The motor does not respond

- Check that the MDDS30 is configured for Serial Packetized mode.
- Verify the driver address and DIP-switch settings.
- Check the TX connection, signal ground, baud rate, and UART pin configuration.
- Ensure that "sync()" has run and the relevant object's "_started" flag is "true".
- Check the motor supply, driver status, and protection conditions.

The first driver works but the second does not

This is a known limitation of the current implementation. Each object maintains its own "_started" flag, but the current "sync()" method initializes only the object on which it is called.

Update the library to support shared-UART initialization before using multiple driver objects.

The wrong motor channel responds

Verify that software channel identifiers "0" and "1" correspond to the intended motor outputs according to the official protocol specification.

Safety

- Secure the robot before testing.
- Keep hands, clothing, and wires away from moving components.
- Start with low speed commands.
- Test one motor at a time.
- Use suitable fusing, wiring, and power supplies.
- Provide a physical emergency-stop or power-disconnect arrangement appropriate for your system.
- Never rely on a software stop command as a safety-rated emergency stop.

Author and Contributions

Maintainer: Shaurya Chauhan

Bug reports, improvements, and protocol corrections are welcome. When reporting an issue, include:

- ESP32 board and Arduino core version.
- MDDS30 operating mode and address configuration.
- Wiring details.
- A minimal code example.
- Compiler errors or observed behavior.