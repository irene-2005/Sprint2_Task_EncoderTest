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
12. [Includes & Libraries](#includes-libraries)
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

This section breaks down the `EncoderAngleTestRover.ino` sketch file, explaining each major part.

```cp
#include <Encoder.h>
````
This line includes Paul Stoffregen's Encoder library. This library is essential for efficiently reading the signals from quadrature encoders. It handles the complexities of detecting pulse changes on two channels (A and B) and determining the direction of rotation, as well as keeping an accumulated count.
```cp
#include <math.h>
```
This line includes the standard C math library. While this specific sketch doesn't use complex math functions from it (like floor() or fmod() which could be alternatives for angle normalization), it's good practice to include it when dealing with floating-point calculations for angles, as it provides a wide range of useful mathematical operations.

* Pin Assignments
```cp
//--- Pin assignments (adjust to your wiring) ---
const uint8_t encoderPins[4][2] = {
  {33, 31},  // Front-Left: A, B
  {27, 29},  // Front-Right: A, B
  {23, 25},  // Rear-Left: A, B
  {45, 43}   // Rear-Right: A, B
};
````

• This 2D array, encoderPins, defines which Arduino pins are connected to the A and B channels of each of the four encoders.
•encoderPins[0][0] refers to Channel A of the first encoder (Front-Left), encoderPins[0][1] to Channel B of the first encoder, and so on.
• You must adjust these pin numbers if your physical wiring is different.

* Encoder Objects
```cp
//--- Encoder objects array ---
Encoder encoders[4] = {
  Encoder(encoderPins[0][0], encoderPins[0][1]),
  Encoder(encoderPins[1][0], encoderPins[1][1]),
  Encoder(encoderPins[2][0], encoderPins[2][1]),
  Encoder(encoderPins[3][0], encoderPins[3][1])
};
````
• This creates an array named encoders containing four Encoder objects.
• Each Encoder object is an instance of the class provided by the Encoder.h library.
• During initialization, each Encoder object is passed the pin numbers for its A and B channels, as defined in the encoderPins array. For example, Encoder(encoderPins[0][0], encoderPins[0][1]) creates an encoder object for the Front-Left wheel, using pins 33 and 31.

* Constants (PPR & Degree Calculation)
 ```cp
// Pulses Per Revolution (PPR) — set to your encoder spec
const long PULSES_PER_REV = 20;
// Degrees per pulse
const float DEG_PER_PULSE = 360.0 / (PULSES_PER_REV * 4);
````

```cp
PULSES_PER_REV          //This constant defines the Pulses Per Revolution of your encoders. For this specific project, it's set to 20. This means that for one full mechanical rotation of the encoder shaft, each channel (A or B) will generate 20 pulses. This is a critical value that must match your encoder's specification.
DEG_PER_PULSE          // This constant calculates the angular resolution, i.e., how many degrees of rotation correspond to a single "count" detected by the Encoder library.
360.0: Represents the total degrees in a full circle.
PULSES_PER_REV * 4   //This is the total Counts Per Revolution (CPR). The Encoder library, when used with quadrature encoders, can detect four distinct events for each pulse cycle (rising edge of A, rising edge of B, falling edge of A, falling edge of B). This quadruples the resolution. So, for a 20 PPR encoder, the CPR is 20 * 4 = 80 counts.

//Therefore, DEG_PER_PULSE = 360.0 / 80 = 4.5 degrees per count.
````
* Data Structure (EncoderData)
```cp
// Encoder data structure
struct EncoderData {
  volatile float currentAngle = 0.0;   // -360 to +360° normalized
  volatile float totalAngle = 0.0;     // cumulative angle
  long lastPosition = 0;               // previous raw count
  long currentPosition = 0;            // current raw count
  int16_t rotations = 0;               // full rotations (positive or negative)
};
````
• This struct defines a custom data type to conveniently store all relevant information for a single encoder.
```cp
volatile float currentAngle       // Stores the current angle of the encoder shaft, normalized to be within a single rotation (e.g., 0° to 359.9° or 0° to -359.9°). The volatile keyword suggests that this variable might be changed by an interrupt, which is good practice when dealing with hardware interfaces, although the Encoder library handles its own interrupt logic internally if pins support it.

volatile float totalAngle        // Stores the total accumulated angle since the program started. This value can grow indefinitely, positive or negative, based on the direction and amount of rotation.

long lastPosition               // Stores the raw encoder count read during the previous update cycle. This is used to check if the encoder has moved.

long currentPosition            // Stores the current raw encoder count read from the Encoder library.

int16_t rotations               // Stores the number of full 360° rotations the encoder has completed. This can be positive or negative.
```
* Global Variables
// Instantiate data for all four encoders
```
EncoderData encoderData[4];
```
This line declares an array named encoderData of type EncoderData.
It creates four instances of the EncoderData structure, one for each of the four encoders being monitored. encoderData[0] will hold data for the Front-Left encoder, encoderData[1] for Front-Right, and so on.
```
setup() Function
void setup() {
  Serial.begin(9600);
  while (!Serial) { /* wait */ }
  Serial.println("---- Rover Encoder Angle & Position Monitor ----");
  delay(100);
}
````
This function runs once when the Arduino board powers up or is reset.
```
Serial.begin(9600);                // Initializes serial communication at a baud rate of 9600 bits per second. This is used to send data to the Serial Monitor.

while (!Serial) { /* wait */ }    // This loop is particularly important for boards like the Arduino Leonardo, Micro, or ESP32-based boards where the USB serial connection might take a moment to establish. It pauses the program execution until the serial port is ready.

Serial.println("---- Rover Encoder Angle & Position Monitor ----");  // Prints a header message to the Serial Monitor.

delay(100);                        // A small delay to ensure the startup message is sent and visible before the main loop starts rapidly printing data.

updateEncoder() Function
// Update function: reads encoder, calculates angles and rotations
float updateEncoder(int id) {
  // Read raw position
  encoderData[id].currentPosition = encoders[id].read();

  // If count changed
  if (encoderData[id].currentPosition != encoderData[id].lastPosition) {
    // Update cumulative angle
    encoderData[id].totalAngle = encoderData[id].currentPosition * DEG_PER_PULSE;

    // Compute rotations and normalize currentAngle
    encoderData[id].rotations = (int16_t)(encoderData[id].totalAngle / 360.0);
    encoderData[id].currentAngle = encoderData[id].totalAngle - (encoderData[id].rotations * 360.0);

    // Update lastPosition for next delta
    encoderData[id].lastPosition = encoderData[id].currentPosition;
  }

  return encoderData[id].currentAngle;
}
```

This function is responsible for reading data from a specific encoder (identified by id) and calculating its angular information.
```
encoderData[id].currentPosition = encoders[id].read();   // Reads the current accumulated pulse count from the specified Encoder object (e.g., encoders[0] for the first encoder). The read() method returns a long integer that increases with clockwise rotation and decreases with counter-clockwise rotation (or vice-versa, depending on wiring).

if (encoderData[id].currentPosition != encoderData[id].lastPosition):    // This check optimizes the calculations by only performing them if the encoder's position has actually changed since the last time this function was called for this specific encoder.

encoderData[id].totalAngle = encoderData[id].currentPosition * DEG_PER_PULSE;    // If the position has changed, the total accumulated angle is calculated by multiplying the currentPosition (total counts) by the DEG_PER_PULSE (degrees per count).

encoderData[id].rotations = (int16_t)(encoderData[id].totalAngle / 360.0);   // The number of full rotations is calculated by dividing the totalAngle by 360. The result is cast to an int16_t, which truncates the decimal part, effectively giving the whole number of rotations.

encoderData[id].currentAngle = encoderData[id].totalAngle - (encoderData[id].rotations * 360.0);     //The currentAngle (normalized angle within one rotation) is calculated by subtracting the angle accounted for by the full rotations from the totalAngle. This gives the remainder angle.

encoderData[id].lastPosition = encoderData[id].currentPosition;      //The lastPosition is updated to the currentPosition to be used for comparison in the next call.

return encoderData[id].currentAngle;     // The function returns the calculated currentAngle.
```
```
loop() Function
void loop() {
  // Update all encoders
  for (int i = 0; i < 4; i++) {
    updateEncoder(i);
  }

  // Print out positions and angles
  Serial.print("FL (pos: "); Serial.print(encoderData[0].currentPosition);
  Serial.print(", ang: "); Serial.print(encoderData[0].currentAngle);
  Serial.print("°, rot: "); Serial.print(encoderData[0].rotations);
  Serial.print(")  | ");

  Serial.print("FR (pos: "); Serial.print(encoderData[1].currentPosition);
  // ... (similar print statements for FR, RL, RR encoders) ...
  Serial.println(")");

  delay(100); // Adjust update rate as needed
}

````
This function runs repeatedly after setup() completes.
```cp
for (int i = 0; i < 4; i++) { updateEncoder(i); }       //  This loop iterates through each of the four encoders (from id = 0 to id = 3) and calls the updateEncoder() function for each one. This ensures all encoders are read and their data is processed.
````
Serial Printing Block:

A series of Serial.print() and Serial.println() statements are used to format and send the data for all four encoders to the Serial Monitor.

For each encoder, it prints:

• pos: The raw currentPosition (encoder counts).

• ang: The normalized currentAngle in degrees.

• rot: The number of full rotations.

The output is formatted to be a single line for all four encoders, separated by " | ".
```cp
delay(100);      // Pauses the execution of the loop() for 100 milliseconds. This controls the rate at which encoder data is updated and printed to the Serial Monitor. Reducing this value will increase the update frequency, while increasing it will slow it down.
````
## Key Equations 

* Counts Per Revolution (CPR):
```cp
CPR = PULSES_PER_REV * 4
For a 20 PPR encoder: CPR = 20 * 4 = 80 counts.
#Degrees Per Count (or Degrees Per Pulse from the library's perspective):
DEG_PER_PULSE = 360.0 / CPR
DEG_PER_PULSE = 360.0 / (PULSES_PER_REV * 4)
For a 20 PPR encoder: DEG_PER_PULSE = 360.0 / 80 = 4.5 degrees per count.
```
* Total Angle:
```cp
Total_Angle = Current_Position_Count * DEG_PER_PULSE
````
* Number of Rotations:
```cp
Rotations = floor(Total_Angle / 360.0)
````
(The code uses (int16_t)(totalAngle / 360.0), which truncates towards zero. For positive totalAngle, this is equivalent to floor. For negative, it's equivalent to ceil.)
#Normalized Current Angle:
```cp
Current_Angle = Total_Angle - (Rotations * 360.0)
````
Alternatively, 
```cp
Current_Angle = fmod(Total_Angle, 360.0)     // If Total_Angle can be negative, care must be taken as fmod can return negative results. The current implementation handles this by ensuring Current_Angle has the same sign as Total_Angle if it's not zero.
```

## Usage
* Ensure hardware is connected and the sketch is uploaded as per Installation & Setup.
* Open the Arduino Serial Monitor (baud rate 9600).
* Manually rotate the shafts connected to the encoders.
* Observe the output in the Serial Monitor. You should see the pos (raw counts), ang (current angle), and rot (full  rotations) update for each encoder as it moves.
  • pos: Will increment or decrement based on rotation direction.
  • ang: Will cycle from ~0° to ~359.9° (or ~0° to ~-359.9° if rotating backward from zero) and then reset as a full              rotation is completed.
  • rot: Will increment or decrement each time a full 360° rotation is completed.

## Output Example
```cp
---- Rover Encoder Angle & Position Monitor ----
FL (pos: 0, ang: 0.00°, rot: 0)  | FR (pos: 0, ang: 0.00°, rot: 0)  | RL (pos: 0, ang: 0.00°, rot: 0)  | RR (pos: 0, ang: 0.00°, rot: 0)
FL (pos: 10, ang: 45.00°, rot: 0)  | FR (pos: 5, ang: 22.50°, rot: 0)  | RL (pos: 0, ang: 0.00°, rot: 0)  | RR (pos: -8, ang: -36.00°, rot: -1)
FL (pos: 80, ang: 0.00°, rot: 1)  | FR (pos: 40, ang: 180.00°, rot: 0)  | RL (pos: 160, ang: 0.00°, rot: 2)  | RR (pos: -80, ang: 0.00°, rot: -1)
Note: ang will be close to 0.00 or 360.00 at the turnover point for rotations due to floating-point precision. The example shows 0.00 after a full rotation.
```

## Customization
* Encoder Pins:
If your wiring is different, modify the encoderPins array at the top of the sketch.
* PPR Value:
If you use encoders with a different PPR, change the PULSES_PER_REV constant. The DEG_PER_PULSE will      automatically  recalculate.
```cp
const long PULSES_PER_REV = YOUR_ENCODER_PPR; // e.g., 600 for a 600 PPR encoder
```
* Serial Baud Rate:
If you need a different baud rate, change it in Serial.begin(9600); and in your Serial Monitor settings.
* Update Rate:
Adjust the delay(100); value in the loop() function to change how frequently data is printed. A smaller value means faster updates.
* Number of Encoders:
If you have fewer or more encoders (up to the limit of available pins and Encoder library  capabilities), you'll need to adjust:
  • The size of the encoderPins array.
  • The size of the encoders object array.
  • The size of the encoderData array.
  • The loop limits in loop() for updating and printing.

## Includes & Libraries

```cp
#include <Encoder.h> // Paul Stoffregen's Encoder library
#include <math.h>    // Standard math library (not strictly used for complex functions here but good practice)
````
