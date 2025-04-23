#pragma once

#include "lowpass_filter.hpp"

template <int order>
LowPass<order>::LowPass(float f0, float fs, bool adaptive)
{
    omega0 = 2.0f * M_PI * f0;
    dt = 1.0f / fs;
    adapt = adaptive;
    tn1 = -dt;

    for (int k = 0; k < order + 1; k++) {
        x[k] = 0.0f;
        y[k] = 0.0f;
    }

    setCoef();
}

template <int order>
void LowPass<order>::setCoef()
{
    if (adapt) {
        float t = time_us_32() / 1.0e6f;
        dt = t - tn1;
        tn1 = t;
    }

    float alpha = omega0 * dt;

    if (order == 1) {
        a[0] = -(alpha - 2.0f) / (alpha + 2.0f);
        b[0] = alpha / (alpha + 2.0f);
        b[1] = b[0];
    }

    if (order == 2) {
        float alphaSq = alpha * alpha;
        const float beta[] = {1.0f, sqrtf(2.0f), 1.0f};
        float D = alphaSq * beta[0] + 2 * alpha * beta[1] + 4 * beta[2];

        b[0] = alphaSq / D;
        b[1] = 2.0f * b[0];
        b[2] = b[0];

        a[0] = -(2 * alphaSq * beta[0] - 8 * beta[2]) / D;
        a[1] = -(beta[0] * alphaSq - 2 * beta[1] * alpha + 4 * beta[2]) / D;
    }
}

template <int order>
float LowPass<order>::filt(float xn)
{
    if (adapt) {
        setCoef();
    }

    y[0] = 0.0f;
    x[0] = xn;

    for (int k = 0; k < order; k++) {
        y[0] += a[k] * y[k + 1] + b[k] * x[k];
    }
    y[0] += b[order] * x[order];

    for (int k = order; k > 0; k--) {
        y[k] = y[k - 1];
        x[k] = x[k - 1];
    }

    return y[0];
}

