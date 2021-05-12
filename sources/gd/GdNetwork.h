// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "GdLine.h"
#include "GdTapFx.h"
#include "GdDefs.h"
#include "utility/LinearSmoother.h"
#include <vector>
#include <memory>

class GdNetwork {
public:
    enum ChannelMode {
        Mono,
        Stereo,
    };

    explicit GdNetwork(ChannelMode channelMode);
    ~GdNetwork();
    void clear();
    void setSampleRate(float sampleRate);
    void setBufferSize(unsigned bufferSize);
    void setParameter(unsigned parameter, float value);
    void process(const float *const inputs[], const float *dry, const float *wet, float *const outputs[], unsigned count);

//==============================================================================
private:
    void mixMonoToStereo(unsigned tapIndex, const float *input, const float *level, const float *pan, const float *wet, float *const outputs[], unsigned count);
    void mixStereoToStereo(unsigned tapIndex, const float *const inputs[], const float *level, const float *pan, const float *width, const float *wet, float *const outputs[], unsigned count);

//==============================================================================
private:
    struct TapDsp {
        TapDsp();
        void clear();
        void setSampleRate(float sampleRate);
        void setBufferSize(unsigned bufferSize);

        // parts
        GdLine line_;
        GdTapFx fx_;
    };

    struct ChannelDsp {
        void clear();
        void setSampleRate(float sampleRate);
        void setBufferSize(unsigned bufferSize);

        // internal
        float feedback_ = 0;

        // taps
        TapDsp taps_[GdMaxLines];
    };

    // channels
    std::vector<ChannelDsp> channels_;

    // parameters + smoothers
    unsigned fbTapIndex_ = 0;
    float fbTapGainDB_ = GdMinGainDB;
    LinearSmoother smoothFbGainLinear_;

    struct TapControl {
        TapControl();
        void clear();
        void setSampleRate(float sampleRate);

        // parameters
        bool enable_ = false;
        float levelDB_ = 0;
        float delay_ = 0;
        float pan_ = 0;
        float width_ = 0;
        // smoothers
        LinearSmoother smoothLevelLinear_;
        LinearSmoother smoothDelay_;
        LinearSmoother smoothPan_;
        LinearSmoother smoothWidth_;
    };

    TapControl tapControls_[GdMaxLines];

    // internal
    std::vector<float> temp_[11];
};
