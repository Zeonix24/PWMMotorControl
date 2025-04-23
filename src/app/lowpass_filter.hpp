#pragma once

#include <cmath>
#include "pico/time.h"

template <int order>
class LowPass
{
  private:
    float a[order];
    float b[order + 1];
    float omega0;
    float dt;
    bool adapt;
    float tn1;
    float x[order + 1];
    float y[order + 1];

    void setCoef();

  public:
    LowPass(float f0, float fs, bool adaptive);
    float filt(float xn);
};

#include "lowpass_filter.tpp"  // Template implementation
