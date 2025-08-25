#ifndef MPU6050_H
#define MPU6050_H
#include <Arduino.h>
#include <Wire.h>


class MPU6050
{
  private:
    // MPU6050 Registers
    const int MPU6050_ADDR = 0x68;
    const uint8_t SMPLRT_DIV   = 0x19;
    const uint8_t CONFIG       = 0x1A;
    const uint8_t GYRO_CONFIG  = 0x1B;
    const uint8_t ACCEL_CONFIG = 0x1C;
    const uint8_t INT_ENABLE   = 0x38;
    const uint8_t PWR_MGMT_1   = 0x6B;
    const uint8_t ACCEL_XOUT_H = 0x3B;
    const uint8_t GYRO_XOUT_H  = 0x43;
    // Sensitivity scale factors
    // ±2g range => 16384 LSB/g, g = 9.80665 m/s²
    const double ACCEL_SENS   = 16384.0;
    const double G_TO_MS2     = 9.80665;
    // ±250°/s range => 131 LSB/(°/s), deg/s to rad/s = π/180
    const double GYRO_SENS    = 131.0;

    // Write a single byte to MPU6050
    void writeMPU6050(uint8_t reg, uint8_t data);
    // Read two bytes from MPU6050 (big-endian) and combine into signed int16
    int readMPU6050Word(uint8_t reg);

    int readAccX_raw();
    int readAccY_raw();
    int readAccZ_raw();

    int readGyroX_raw();
    int readGyroY_raw();
    int readGyroZ_raw();

  public:
    void begin();

    double readAccX_mps2();
    double readAccY_mps2();
    double readAccZ_mps2();

    double readGyroX_rps();
    double readGyroY_rps();
    double readGyroZ_rps();
    
};




// Write a single byte to MPU6050
void MPU6050::writeMPU6050(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

// Read two bytes from MPU6050 (big-endian) and combine into signed int16
int MPU6050::readMPU6050Word(uint8_t reg) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU6050_ADDR, 2);
  uint16_t value = Wire.read() << 8 | Wire.read();
  if (value > 32768)
      return value - 65536;
  return value;
}

void MPU6050::begin(){
  // Wake up MPU6050
  writeMPU6050(PWR_MGMT_1, 0x00); // Clear sleep bit

  // Set sample rate = Gyroscope output rate / (1 + SMPLRT_DIV)
  writeMPU6050(SMPLRT_DIV, 0x07); // 1kHz / (1+7) = 125Hz

  writeMPU6050(CONFIG, 0x00); // No DLPF
  writeMPU6050(GYRO_CONFIG, 0x00); // ±250°/s
  writeMPU6050(ACCEL_CONFIG, 0x00); // ±2g
  writeMPU6050(INT_ENABLE, 0x01); // Enable data ready interrupt
}

int MPU6050::readAccX_raw(){
  return readMPU6050Word(ACCEL_XOUT_H);
}

int MPU6050::readAccY_raw(){
  return readMPU6050Word(ACCEL_XOUT_H + 2);
}

int MPU6050::readAccZ_raw(){
  return readMPU6050Word(ACCEL_XOUT_H + 4);
}


int MPU6050::readGyroX_raw(){
  return readMPU6050Word(GYRO_XOUT_H);
}

int MPU6050::readGyroY_raw(){
  return readMPU6050Word(GYRO_XOUT_H + 2);
}

int MPU6050::readGyroZ_raw(){
  return readMPU6050Word(GYRO_XOUT_H + 4);
}


double MPU6050::readAccX_mps2(){
  int accX_raw = readAccX_raw();
  return ((double)accX_raw / ACCEL_SENS) * G_TO_MS2; // m/s²
}

double MPU6050::readAccY_mps2(){
  int accY_raw = readAccY_raw();
  return ((double)accY_raw / ACCEL_SENS) * G_TO_MS2; // m/s²
}

double MPU6050::readAccZ_mps2(){
  int accZ_raw = readAccZ_raw();
  return ((double)accZ_raw / ACCEL_SENS) * G_TO_MS2; // m/s²
}


double MPU6050::readGyroX_rps(){
  int gyroX_raw = readGyroX_raw();
  return ((double)gyroX_raw / GYRO_SENS) * DEG_TO_RAD; // rad/s
}

double MPU6050::readGyroY_rps(){
  int gyroY_raw = readGyroY_raw();
  return ((double)gyroY_raw / GYRO_SENS) * DEG_TO_RAD; // rad/s
}

double MPU6050::readGyroZ_rps(){
  int gyroZ_raw = readGyroZ_raw();
  return ((double)gyroZ_raw / GYRO_SENS) * DEG_TO_RAD; // rad/s
}


#endif

