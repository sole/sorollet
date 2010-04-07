#include "SorolletEQ.h"
#include "../sys/MathUtils.h"
#include <cmath>

float SorolletEQ::verySmallAmount = (1.0f / 4294967295.0f);

SorolletEQ::SorolletEQ() {
    mfLowFreq = 800.0f;
    mfHighFreq = 5000.0f;
    mfSampleRate = 44100;
    lg = 1.0f;
    mg = 1.0f;
    hg = 1.0f;
    isActive = false;
    prepare();
    f1p0 = 0.0f;
    f1p1 = 0.0f;
    f1p2 = 0.0f;
    f1p3 = 0.0f;
    f2p0 = 0.0f;
    f2p1 = 0.0f;
    f2p2 = 0.0f;
    f2p3 = 0.0f;
    sdm1 = 0.0f;
    sdm2 = 0.0f;
    sdm3 = 0.0f;
}

void SorolletEQ::setLowFreq(float frequency) {
    mfLowFreq = frequency;
    prepare();
}

void SorolletEQ::setHighFreq(float frequency) {
    mfHighFreq = frequency;
    prepare();
}

void SorolletEQ::setSampleRate(float rate) {
    mfSampleRate = rate;
    prepare();
}

void SorolletEQ::setLowGain(float gain) {
    lg = gain;
}

void SorolletEQ::setMidGain(float gain) {
    mg = gain;
}

void SorolletEQ::setHiGain(float gain) {
    hg = gain;
}

void SorolletEQ::setActive(bool value) {
    isActive = value;
}

void SorolletEQ::prepare() {
    // Calculate filter cutoff frequencies

    lf = 2 * sinf(M_PI * (mfLowFreq / mfSampleRate));
    hf = 2 * sinf(M_PI * (mfHighFreq / mfSampleRate));
}

float SorolletEQ::update(float sample) {
    if (!isActive)
        return sample;

    // Locals

    float l, m, h; // Low / Mid / High - Sample Values

    // Filter #1 (lowpass)

    f1p0 += (lf * (sample - f1p0)) + verySmallAmount;
    f1p1 += (lf * (f1p0 - f1p1));
    f1p2 += (lf * (f1p1 - f1p2));
    f1p3 += (lf * (f1p2 - f1p3));

    l = f1p3;

    // Filter #2 (highpass)

    f2p0 += (hf * (sample - f2p0)) + verySmallAmount;
    f2p1 += (hf * (f2p0 - f2p1));
    f2p2 += (hf * (f2p1 - f2p2));
    f2p3 += (hf * (f2p2 - f2p3));

    h = sdm3 - f2p3;

    // Calculate midrange (signal - (low + high))

    m = sdm3 - (h + l);
    ; //sample - (h+l); //sdm3 - (h + l);

    // Scale, Combine and store

    l *= lg;
    m *= mg;
    h *= hg;

    // Shuffle history buffer

    sdm3 = sdm2;
    sdm2 = sdm1;
    sdm1 = sample;

    // Return result

    return (l + m + h);

}
