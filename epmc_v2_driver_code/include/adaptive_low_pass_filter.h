#ifndef ADAPTIVE_LOW_PASS_FILTER_H
#define ADAPTIVE_LOW_PASS_FILTER_H
#include <Arduino.h>


class AdaptiveLowPassFilter
{
  private:
    int order; // 1 or 2
    float a[2];
    float b[3];
    float omega0;
    float dt;
    float tn1 = 0;
    float x[3]; // Raw values
    float y[3]; // Filtered values

  public:  
    AdaptiveLowPassFilter(int, float);

    void setCutOffFreq(float);
    void setOrder(int);
    void setCoef();
    float filter(float);
};




AdaptiveLowPassFilter::AdaptiveLowPassFilter(int orderVal, float f0)
{
  // f0: cutoff frequency (Hz)
  order = orderVal;
  if (order < 1)
    order = 1;
  else if (order > 2)
    order = 2;

  omega0 = 6.28318530718 * f0;

  for (int k = 0; k < order + 1; k++)
  {
    x[k] = 0;
    y[k] = 0;
  }
  setCoef();
}

void AdaptiveLowPassFilter::setCutOffFreq(float f0)
{
  // f0: cutoff frequency (Hz)
  omega0 = 6.28318530718 * f0;

  for (int k = 0; k < order + 1; k++)
  {
    x[k] = 0;
    y[k] = 0;
  }
  setCoef();
}

void AdaptiveLowPassFilter::setOrder(int orderVal)
{
  // f0: cutoff frequency (Hz)
  order = orderVal;
  if (order < 1)
    order = 1;
  else if (order > 2)
    order = 2;

  for (int k = 0; k < order + 1; k++)
  {
    x[k] = 0;
    y[k] = 0;
  }
  setCoef();
}

void AdaptiveLowPassFilter::setCoef()
{
  float t = micros() / 1.0e6;
  dt = t - tn1;
  tn1 = t;

  float alpha = omega0 * dt;

  if (order == 1)
  {
    a[0] = -(alpha - 2.0) / (alpha + 2.0);
    b[0] = alpha / (alpha + 2.0);
    b[1] = alpha / (alpha + 2.0);
  }

  if (order == 2)
  {
    float alphaSq = alpha * alpha;
    float beta[] = {1, sqrt(2), 1};
    float D = alphaSq * beta[0] + 2 * alpha * beta[1] + 4 * beta[2];
    b[0] = alphaSq / D;
    b[1] = 2 * b[0];
    b[2] = b[0];
    a[0] = -(2 * alphaSq * beta[0] - 8 * beta[2]) / D;
    a[1] = -(beta[0] * alphaSq - 2 * beta[1] * alpha + 4 * beta[2]) / D;
  }
}

float AdaptiveLowPassFilter::filter(float xn)
{
  // Provide me with the current raw value: x
  // I will give you the current filtered value: y

  setCoef(); // Update coefficients if necessary

  y[0] = 0;
  x[0] = xn;

  // Compute the filtered values
  for (int k = 0; k < order; k++)
  {
    y[0] += a[k] * y[k + 1] + b[k] * x[k];
  }
  y[0] += b[order] * x[order];

  // Save the historical values
  for (int k = order; k > 0; k--)
  {
    y[k] = y[k - 1];
    x[k] = x[k - 1];
  }

  // Return the filtered value
  return y[0];
}


#endif

