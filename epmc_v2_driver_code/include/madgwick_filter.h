#ifndef MADGWICK_FILTER_H
#define MADGWICK_FILTER_H

#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#include <math.h>


class MadgwickFilter
{
  public:
    MadgwickFilter();

  private:
    // **** paramaters
    double gain_ = 1.0;                         // algorithm gain
    double zeta_ = 0.0;                         // gyro drift bias gain
    const String world_frame[3] = {"NWU", "ENU", "NED"};  // NWU, ENU, NED
    int world_frame_id; // 0 - NWU, 1 - ENU, 2 - NED

    float roll;
    float pitch;
    float yaw;

    // **** state variables
    double q0, q1, q2, q3;      // quaternion
    float w_bx_, w_by_, w_bz_;  //

    long last_time_;

  public:
    void setAlgorithmGain(double gain)
    {
        /*Gain of the filter.
         Higher values lead to faster convergence but more noise.
         Lower values lead to slower convergence but smoother signal.*/
        gain_ = gain;
    }

    void setDriftBiasGain(double zeta)
    {
      /*zeta
      Gyro drift gain (approx. rad/s).*/
        zeta_ = zeta;
    }

    void setWorldFrameId(int id)
    {
        world_frame_id = id;
    }

    void computeRPY(){
      float qw = this->q0;
      float qx = this->q1;
      float qy = this->q2;
      float qz = this->q3;

      //------ CALC RPY from QUAT -----------------------//
      float t0 = 2.0 * (qw * qx + qy * qz);
      float t1 = 1.0 - 2.0 * (qx * qx + qy * qy);
      roll = atan2(t0, t1);

      float t2 = 2.0 * (qw * qy - qz * qx);
      if(t2 > 1.0){
        t2 = 1.0;
      }
      else if (t2 < -1.0){
        t2 = -1.0;
      }
      pitch = asin(t2);

      float t3 = 2.0 * (qw * qz + qx * qy);
      float t4 = 1.0 - 2.0 * (qy * qy + qz * qz);
      yaw = atan2(t3, t4);
      //----------------------------------------------------//
    }

    void getOrientationQuat(float& q0, float& q1, float& q2, float& q3)
    {
        q0 = (float)this->q0;
        q1 = (float)this->q1;
        q2 = (float)this->q2;
        q3 = (float)this->q3;
    }

    void getOrientationRPY(float& roll, float& pitch, float& yaw)
    {
        roll = this->roll;
        pitch = this->pitch;
        yaw = this->yaw;
    }

    void madgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay,
                               float az);

};

#endif // IMU_MADGWICK_FILTER_H





//------------------------------------------------------------------------------------------------------------//

// Fast inverse square-root
// See:
// http://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Reciprocal_of_the_square_root
float invSqrt(float x)
{
  float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	y = y * (1.5f - (halfx * y * y));
  y = y * (1.5f - (halfx * y * y));
	return y;
}

template <typename T>
void normalizeVector(T& vx, T& vy, T& vz)
{
    T recipNorm = invSqrt(vx * vx + vy * vy + vz * vz);
    vx *= recipNorm;
    vy *= recipNorm;
    vz *= recipNorm;
}

template <typename T>
void normalizeQuaternion(T& q0, T& q1, T& q2, T& q3)
{
    T recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;
}

void rotateAndScaleVector(float q0, float q1, float q2, float q3,
                          float _2dx, float _2dy, float _2dz,
                          float& rx, float& ry, float& rz)
{
    // result is half as long as input
    rx = _2dx * (0.5f - q2 * q2 - q3 * q3) + _2dy * (q0 * q3 + q1 * q2) +
         _2dz * (q1 * q3 - q0 * q2);
    ry = _2dx * (q1 * q2 - q0 * q3) + _2dy * (0.5f - q1 * q1 - q3 * q3) +
         _2dz * (q0 * q1 + q2 * q3);
    rz = _2dx * (q0 * q2 + q1 * q3) + _2dy * (q2 * q3 - q0 * q1) +
         _2dz * (0.5f - q1 * q1 - q2 * q2);
}

void compensateGyroDrift(float q0, float q1, float q2, float q3,
                          float s0, float s1, float s2, float s3,
                          float dt, float zeta, float& w_bx,
                          float& w_by, float& w_bz, float& gx,
                          float& gy, float& gz)
{
    // w_err = 2 q x s
    float w_err_x =
        2.0f * q0 * s1 - 2.0f * q1 * s0 - 2.0f * q2 * s3 + 2.0f * q3 * s2;
    float w_err_y =
        2.0f * q0 * s2 + 2.0f * q1 * s3 - 2.0f * q2 * s0 - 2.0f * q3 * s1;
    float w_err_z =
        2.0f * q0 * s3 - 2.0f * q1 * s2 + 2.0f * q2 * s1 - 2.0f * q3 * s0;

    w_bx += w_err_x * dt * zeta;
    w_by += w_err_y * dt * zeta;
    w_bz += w_err_z * dt * zeta;

    gx -= w_bx;
    gy -= w_by;
    gz -= w_bz;
}

void orientationChangeFromGyro(float q0, float q1, float q2,
                              float q3, float gx, float gy,
                              float gz, float& qDot1,
                              float& qDot2, float& qDot3,
                              float& qDot4)
{
    // Rate of change of quaternion from gyroscope
    // See EQ 12
    qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
    qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
    qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
    qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);
}

void addGradientDescentStep(float q0, float q1, float q2,
                            float q3, float _2dx, float _2dy,
                            float _2dz, float mx, float my,
                            float mz, float& s0, float& s1,
                            float& s2, float& s3)
{
    float f0, f1, f2;

    // Gradient decent algorithm corrective step
    // EQ 15, 21
    rotateAndScaleVector(q0, q1, q2, q3, _2dx, _2dy, _2dz, f0, f1, f2);

    f0 -= mx;
    f1 -= my;
    f2 -= mz;

    // EQ 22, 34
    // Jt * f
    s0 += (_2dy * q3 - _2dz * q2) * f0 + (-_2dx * q3 + _2dz * q1) * f1 +
          (_2dx * q2 - _2dy * q1) * f2;
    s1 += (_2dy * q2 + _2dz * q3) * f0 +
          (_2dx * q2 - 2.0f * _2dy * q1 + _2dz * q0) * f1 +
          (_2dx * q3 - _2dy * q0 - 2.0f * _2dz * q1) * f2;
    s2 += (-2.0f * _2dx * q2 + _2dy * q1 - _2dz * q0) * f0 +
          (_2dx * q1 + _2dz * q3) * f1 +
          (_2dx * q0 + _2dy * q3 - 2.0f * _2dz * q2) * f2;
    s3 += (-2.0f * _2dx * q3 + _2dy * q0 + _2dz * q1) * f0 +
          (-_2dx * q0 - 2.0f * _2dy * q3 + _2dz * q2) * f1 +
          (_2dx * q1 + _2dy * q2) * f2;
}

//-----------------------------------------------------------------------------------------------//




MadgwickFilter::MadgwickFilter()
{
  gain_ = 0.0;
  zeta_ = 0.0;
  world_frame_id = 0;

  q0 = 1.0;
  q1 = 0.0;
  q2 = 0.0;
  q3 = 0.0;

  w_bx_ = 0.0;
  w_by_ = 0.0;
  w_bz_ = 0.0;

  last_time_ = micros();
}

void MadgwickFilter::madgwickAHRSupdateIMU(float gx, float gy, float gz, float ax,
                                      float ay, float az)
{
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;

    float dt = (float)(micros() - last_time_)/ 1.0e6;

    // // Rate of change of quaternion from gyroscope
    // orientationChangeFromGyro(q0, q1, q2, q3, gx, gy, gz, qDot1, qDot2, qDot3,
    //                           qDot4);

    // Compute feedback only if accelerometer measurement valid (avoids NaN in
    // accelerometer normalisation)
    if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
    {
        // Normalise accelerometer measurement
        normalizeVector(ax, ay, az);

        // Gradient decent algorithm corrective step
        s0 = 0.0;
        s1 = 0.0;
        s2 = 0.0;
        s3 = 0.0;

        switch (world_frame_id)
        {
            case 0: //0 - NWU
                // Gravity: [0, 0, 1]
                addGradientDescentStep(q0, q1, q2, q3, 0.0, 0.0, 2.0, ax, ay,
                                       az, s0, s1, s2, s3);
                break;
            case 1: // 1 - ENU
                // Gravity: [0, 0, 1]
                addGradientDescentStep(q0, q1, q2, q3, 0.0, 0.0, 2.0, ax, ay,
                                       az, s0, s1, s2, s3);
                break;
            case 2: //2 - NED
                // Gravity: [0, 0, -1]
                addGradientDescentStep(q0, q1, q2, q3, 0.0, 0.0, -2.0, ax, ay,
                                       az, s0, s1, s2, s3);
                break;
        }
        normalizeQuaternion(s0, s1, s2, s3);

        // compute gyro drift bias
        compensateGyroDrift(q0, q1, q2, q3, s0, s1, s2, s3, dt, zeta_, w_bx_,
                            w_by_, w_bz_, gx, gy, gz);

        orientationChangeFromGyro(q0, q1, q2, q3, gx, gy, gz, qDot1, qDot2,
                                  qDot3, qDot4);

        // Apply feedback step
        qDot1 -= gain_ * s0;
        qDot2 -= gain_ * s1;
        qDot3 -= gain_ * s2;
        qDot4 -= gain_ * s3;
    }else
    {
        orientationChangeFromGyro(q0, q1, q2, q3, gx, gy, gz, qDot1, qDot2,
                                  qDot3, qDot4);
    }

    // Integrate rate of change of quaternion to yield quaternion
    q0 += qDot1 * dt;
    q1 += qDot2 * dt;
    q2 += qDot3 * dt;
    q3 += qDot4 * dt;

    // Normalise quaternion
    normalizeQuaternion(q0, q1, q2, q3);

    computeRPY();

    last_time_ = micros();
}