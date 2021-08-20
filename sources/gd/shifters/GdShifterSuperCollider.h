#pragma once
#include <vector>
#include <cstdint>

#define GD_SHIFTER_UPDATES_AT_K_RATE 1
#define GD_SHIFTER_CAN_REPORT_LATENCY 0

class GdShifter {
public:
    GdShifter();
    ~GdShifter();

    void clear();
    void setSampleRate(float sampleRate);
    void setBufferSize(unsigned bufferSize);
    void setShift(float shiftLinear);
    float processOne(float input);
    void process(const float *input, float *output, unsigned count);

private:
    void postUpdateSampleRateOrBufferSize();
    void processNext(const float *input, float *output, unsigned count);
    void processNextZ(const float *input, float *output, unsigned count);
    void copyNext(const float *input, float *output, unsigned count);

    // fixed characteristics
    static constexpr float getWindowSize() { return 0.1f; }
    static constexpr float getPitchDispersion() { return 0.0f; }
    static constexpr float getTimeDispersion() { return 0.004f; }

    float sampleRate_ = 0;
    unsigned bufferSize_ = 0;
    float shift_ = 1;

    struct RGen {
        uint32_t s1, s2, s3;
    };
    RGen rgen_ {};

    void (GdShifter::*calc_)(const float *input, float *output, unsigned count) = nullptr;

    struct PitchShift {
        float* dlybuf;
        float dsamp1, dsamp1_slope, ramp1, ramp1_slope;
        float dsamp2, dsamp2_slope, ramp2, ramp2_slope;
        float dsamp3, dsamp3_slope, ramp3, ramp3_slope;
        float dsamp4, dsamp4_slope, ramp4, ramp4_slope;
        float fdelaylen, slope;
        long iwrphase, idelaylen, mask;
        long counter, stage, numoutput, framesize;
    };
    PitchShift unit_{};

    std::vector<float> delayBuffer_;
};

inline void GdShifter::process(const float *input, float *output, unsigned count)
{
    (this->*calc_)(input, output, count);
}
