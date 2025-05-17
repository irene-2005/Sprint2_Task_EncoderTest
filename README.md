# Rover Serial Servo Control 

This Arduino project (`EncoderAngleTestRover.ino`) is designed to test and display the angle, total accumulated angle, raw position, and number of full rotations for four wheel encoders connected to a rover or robotic platform. It utilizes Paul Stoffregen's popular `Encoder` library and is configured for encoders with **20 Pulses Per Revolution (PPR)**.

## Table of Contents

1.  [Features](#features)
2.  [About Encoders](#about-encoders)
    *   [What is PPR (Pulses Per Revolution)?](#what-is-ppr-pulses-per-revolution)
    *   [Quadrature Encoding and Resolution](#quadrature-encoding-and-resolution)
3.  [Hardware Requirements](#hardware-requirements)
4.  [Software & Libraries](#software--libraries)
5.  [Pinout](#pinout)
6.  [Installation & Setup](#installation--setup)
7.  [Code Explanation](#code-explanation)
    *   [Includes & Libraries](#includes--libraries)
    *   [Pin Assignments](#pin-assignments)
    *   [Encoder Objects](#encoder-objects)
    *   [Constants (PPR & Degree Calculation)](#constants-ppr--degree-calculation)
    *   [Data Structure (`EncoderData`)](#data-structure-encoderdata)
    *   [Global Variables](#global-variables)
    *   [`setup()` Function](#setup-function)
    *   [`updateEncoder()` Function](#updateencoder-function)
    *   [`loop()` Function](#loop-function)
8.  [Key Equations](#key-equations)
9.  [Usage](#usage)
10. [Output Example](#output-example)
11. [Customization](#customization)

## Features

*   Monitors up to 4 quadrature encoders simultaneously.
*   Calculates and displays:
    *   Current raw encoder position (counts).
    *   Current angle normalized between 0° and 360° (or -360° to +360° depending on interpretation, here it's cumulative then normalized).
    *   Total accumulated angle since startup.
    *   Number of full rotations (positive for one direction, negative for the other).
*   Uses Paul Stoffregen's `Encoder` library for efficient quadrature decoding.
*   Configured for 20 PPR encoders.
*   Outputs data to the Arduino Serial Monitor.

## About Encoders

Rotary encoders are electromechanical devices that convert the angular position or motion of a shaft or axle to an analog or digital signal. This project uses **quadrature encoders**.

### What is PPR (Pulses Per Revolution)?

PPR stands for **Pulses Per Revolution**. It specifies the number of pulses the encoder generates on *each* of its output channels (typically Channel A and Channel B) for one complete mechanical revolution of the encoder's shaft.

For this project, we are using **20 PPR encoders**. This means for every 360° turn of the wheel/motor shaft, Channel A will produce 20 pulses, and Channel B will also produce 20 pulses.

### Quadrature Encoding and Resolution

Quadrature encoders have two output channels, A and B, that produce square wave pulses. These signals are 90 degrees out of phase with each other. This phase difference allows us to:

1.  **Determine Direction:** By checking which signal leads the other, we can determine the direction of rotation.
2.  **Increase Resolution:** We can count every rising and falling edge of both Channel A and Channel B. This effectively quadruples the number of countable events per pulse cycle.

So, for a 20 PPR encoder:
*   Pulses per revolution (PPR) = 20
*   Counts per revolution (CPR) with quadrature decoding = PPR * 4 = 20 * 4 = **80 counts per revolution**.

The `Encoder` library by Paul Stoffregen handles this quadrature decoding automatically.

## Hardware Requirements

*   Arduino board (e.g., Arduino Mega, ESP32, or any board compatible with the `Encoder` library and sufficient interrupt pins).
*   4 x Quadrature Rotary Encoders (specified as 20 PPR).
*   Connecting wires.
*   (Optional) Rover chassis with motors and wheels to attach the encoders.

## Software & Libraries

*   **Arduino IDE** (Version 2.3.7 or compatible).
*   **Encoder Library by Paul Stoffregen**: This library needs to be installed in your Arduino IDE.
    *   You can install it via the Arduino Library Manager: `Sketch -> Include Library -> Manage Libraries...` then search for "Encoder" and install the one by Paul Stoffregen.
*   **math.h**: Standard C math library, included for `floor()` if it were used (though the code uses casting for `rotations` which behaves like `trunc` for positive numbers). It's good practice for angle calculations.

## Pinout

The code defines the following pin assignments for the encoders. Adjust these in the `encoderPins` array if your wiring differs. Each encoder requires two digital pins, preferably interrupt-capable for best performance, though the `Encoder` library can work with non-interrupt pins on many architectures.

| Encoder       | Channel A Pin | Channel B Pin |
| :------------ | :------------ | :------------ |
| Front-Left    | 33            | 31            |
| Front-Right   | 27            | 29            |
| Rear-Left     | 23            | 25            |
| Rear-Right    | 45            | 43            |

## Installation & Setup

1.  **Connect Hardware:**
    *   Wire each encoder's Channel A and Channel B outputs to the corresponding pins defined in the `encoderPins` array on your Arduino.
    *   Connect the encoder's VCC and GND pins to your Arduino's 5V/3.3V (as per encoder spec) and GND respectively.
2.  **Install Library:**
    *   Open the Arduino IDE.
    *   Go to `Sketch -> Include Library -> Manage Libraries...`.
    *   Search for "Encoder" and install the library by Paul Stoffregen.
3.  **Load Sketch:**
    *   Open the `EncoderAngleTestRover.ino` sketch in the Arduino IDE.
    *   Select your board and port from the `Tools` menu.
    *   Upload the sketch to your Arduino.
4.  **Monitor Output:**
    *   Open the Serial Monitor (`Tools -> Serial Monitor` or Ctrl+Shift+M).
    *   Set the baud rate to `9600`.

## Code Explanation

### Includes & Libraries

```cpp
#include <Encoder.h> // Paul Stoffregen's Encoder library
#include <math.h>    // Standard math library (not strictly used for complex functions here but good practice)
