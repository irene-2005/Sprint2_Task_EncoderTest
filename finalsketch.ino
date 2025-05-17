// EncoderAngleTestRover.ino
// Test and display angles & positions for all four wheel encoders
// Using Paul Stoffregen's Encoder library in Arduino IDE 2.3.7

#include <Encoder.h>
#include <math.h>

//--- Pin assignments (adjust to your wiring) ---
const uint8_t encoderPins[4][2] = {
  {33, 31},  // Front-Left: A, B
  {27, 29},  // Front-Right: A, B
  {23, 25},  // Rear-Left: A, B
  {45, 43}   // Rear-Right: A, B
};

//--- Encoder objects array ---
Encoder encoders[4] = {
  Encoder(encoderPins[0][0], encoderPins[0][1]),
  Encoder(encoderPins[1][0], encoderPins[1][1]),
  Encoder(encoderPins[2][0], encoderPins[2][1]),
  Encoder(encoderPins[3][0], encoderPins[3][1])
};

// Pulses Per Revolution (PPR) — set to your encoder spec
const long PULSES_PER_REV = 20;
// Degrees per pulse
const float DEG_PER_PULSE = 360.0 / (PULSES_PER_REV*4);

// Encoder data structure
struct EncoderData {
  volatile float currentAngle = 0.0;   // -360 to +360° normalized
  volatile float totalAngle = 0.0;     // cumulative angle
  long lastPosition = 0;               // previous raw count
  long currentPosition = 0;            // current raw count
  int16_t rotations = 0;               // full rotations (positive or negative)
};

// Instantiate data for all four encoders
EncoderData encoderData[4];

void setup() {
  Serial.begin(9600);
  while (!Serial) { /* wait */ }
  Serial.println("---- Rover Encoder Angle & Position Monitor ----");
  delay(100);
}

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
  Serial.print(", ang: "); Serial.print(encoderData[1].currentAngle);
  Serial.print("°, rot: "); Serial.print(encoderData[1].rotations);
  Serial.print(")  | ");

  Serial.print("RL (pos: "); Serial.print(encoderData[2].currentPosition);
  Serial.print(", ang: "); Serial.print(encoderData[2].currentAngle);
  Serial.print("°, rot: "); Serial.print(encoderData[2].rotations);
  Serial.print(")  | ");

  Serial.print("RR (pos: "); Serial.print(encoderData[3].currentPosition);
  Serial.print(", ang: "); Serial.print(encoderData[3].currentAngle);
  Serial.print("°, rot: "); Serial.print(encoderData[3].rotations);
  Serial.println(")");

  delay(100); // Adjust update rate as needed
}
