/*
   MPU6050 Raw Reading Without External Libraries
   Accelerometer: m/s²
   Gyroscope: rad/s
*/
#include <Arduino.h>
#include <Wire.h>

// MPU6050 Registers
#define MPU6050_ADDR     0x68
#define SMPLRT_DIV       0x19
#define CONFIG           0x1A
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define INT_ENABLE       0x38
#define PWR_MGMT_1       0x6B
#define ACCEL_XOUT_H     0x3B
#define GYRO_XOUT_H      0x43

// Sensitivity scale factors
// ±2g range => 16384 LSB/g, g = 9.80665 m/s²
#define ACCEL_SENS       16384.0
#define G_TO_MS2         9.80665
// ±250°/s range => 131 LSB/(°/s), deg/s to rad/s = π/180
#define GYRO_SENS        131.0


// Write a single byte to MPU6050
void writeMPU6050(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

// Read two bytes from MPU6050 (big-endian) and combine into signed int16
int16_t readMPU6050Word(uint8_t reg) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU6050_ADDR, 2);
  int16_t high = Wire.read();
  int16_t low = Wire.read();
  int16_t value = (high << 8) | low;
  if (value > 32767) value -= 65536; // convert to signed
  return value;
}


void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Wake up MPU6050
  writeMPU6050(PWR_MGMT_1, 0x00); // Clear sleep bit

  // Set sample rate = Gyroscope output rate / (1 + SMPLRT_DIV)
  writeMPU6050(SMPLRT_DIV, 0x07); // 1kHz / (1+7) = 125Hz

  writeMPU6050(CONFIG, 0x00); // No DLPF
  writeMPU6050(GYRO_CONFIG, 0x00); // ±250°/s
  writeMPU6050(ACCEL_CONFIG, 0x00); // ±2g
  writeMPU6050(INT_ENABLE, 0x01); // Enable data ready interrupt

  delay(100);
}

void loop() {
  int16_t accX_raw, accY_raw, accZ_raw;
  int16_t gyroX_raw, gyroY_raw, gyroZ_raw;

  // Read accelerometer raw data
  accX_raw = readMPU6050Word(ACCEL_XOUT_H);
  accY_raw = readMPU6050Word(ACCEL_XOUT_H + 2);
  accZ_raw = readMPU6050Word(ACCEL_XOUT_H + 4);

  // Read gyroscope raw data
  gyroX_raw = readMPU6050Word(GYRO_XOUT_H);
  gyroY_raw = readMPU6050Word(GYRO_XOUT_H + 2);
  gyroZ_raw = readMPU6050Word(GYRO_XOUT_H + 4);

  // Convert to physical units
  float accX = (accX_raw / ACCEL_SENS) * G_TO_MS2; // m/s²
  float accY = (accY_raw / ACCEL_SENS) * G_TO_MS2; // m/s²
  float accZ = (accZ_raw / ACCEL_SENS) * G_TO_MS2; // m/s²

  float gyroX = (gyroX_raw / GYRO_SENS) * DEG_TO_RAD; // rad/s
  float gyroY = (gyroY_raw / GYRO_SENS) * DEG_TO_RAD; // rad/s
  float gyroZ = (gyroZ_raw / GYRO_SENS) * DEG_TO_RAD; // rad/s

  // Print results
  Serial.print("Acc [m/s²]: ");
  Serial.print(accX); Serial.print("\t");
  Serial.print(accY); Serial.print("\t");
  Serial.print(accZ); Serial.print("\t");

  Serial.print("Gyro [rad/s]: ");
  Serial.print(gyroX); Serial.print("\t");
  Serial.print(gyroY); Serial.print("\t");
  Serial.println(gyroZ);

  delay(100); // 10Hz output
}
