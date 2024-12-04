#include "xsonic.hpp"

XSonic::XSonic(const int &sampleRate,const int &numChannels) {
    if (!Open(sampleRate, numChannels)){
        LOG_ERROR(GET_STR(XSonic Construct failed!));
    }
}

XSonic::XSonic(XSonic &&obj) noexcept(true) {
    Move_(&obj);
}

XSonic &XSonic::operator=(XSonic &&obj) noexcept(true) {
    if (this != &obj){
        Move_(&obj);
    }
    return *this;
}

bool XSonic::Open(const int &sampleRate,const int &numChannels) {
    const auto ret{sonicCreateStream(sampleRate, numChannels)};
    d.m_is_init_ = ret;
    return ret;
}

int XSonic::ReadDoubleFromStream(double *samples, const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int& n) {
        for(int i{};i < n;++i) {
            samples[i] = static_cast<double>(src[i]) / 32768.0;
        }
    });
}

int XSonic::ReadS64FromStream(int64_t *samples, const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int &n){
        for(int i{};i < n;++i) {
            samples[i] = static_cast<int64_t>(src[i]) << 48;
        }
    });
}

int XSonic::ReadU64FromStream(uint64_t *samples, const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int &n) {
        for(int i{};i < n;++i) {
            samples[i] = static_cast<uint64_t>(src[i] + 32768) << 48;
        }
    });
}

int XSonic::ReadFloatFromStream(float *samples,
                                     const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int &n) {
        for(int i{};i < n;++i) {
            samples[i] = static_cast<float>(src[i]) / 32767.0f;
        }
    });
}

int XSonic::ReadU32FromStream(uint32_t *samples,const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int &n) {
        for(int i{};i < n;++i) {
            samples[i] = static_cast<uint32_t>(src[i] + 32768) << 16;
        }
    });
}

int XSonic::ReadS32FromStream(int32_t *samples, const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int &n){
        for(int i{};i < n;++i) {
            samples[i] = src[i] << 16;
        }
    });
}

int XSonic::ReadShortFromStream(int16_t *samples,
                                     const int &maxSamples) {
    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *read_,const int &w_size_) {
            std::copy_n(read_,w_size_,samples);
    });
}

int XSonic::ReadUnsignedShortFromStream(uint16_t *samples, const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *read_,const int &w_size_) {
        for(int i{}; i < w_size_;++i) {
            samples[i] = read_[i] + 32768;
        }
    });
}

int XSonic::ReadUnsignedCharFromStream(uint8_t *samples,
                                            const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int &n) {
        for(int i{};i < n;++i) {
            const auto v1{src[i] >> 8},v2{v1 + 128};
            samples[i] = static_cast<uint8_t>(v2);
        }
    });
}

int XSonic::ReadSignedCharFromStream(int8_t *samples, const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int &n) {
        for(int i{}; i < n; ++i) {
            samples[i] = static_cast<int8_t>(std::round(src[i] >> 8));
        }
    });
}

bool XSonic::FlushStream() {

    if (!d.m_is_init_){
        return {};
    }

    const auto maxRequired{d.m_maxRequired_},
                remainingSamples{d.m_numInputSamples_};

    const auto speed{d.m_speed_ / d.m_pitch_},
                rate{d.m_rate_ * d.m_pitch_};

    const auto f_remainingSamples{static_cast<float >(remainingSamples)},
                f_m_numPitchSamples{static_cast<float >(d.m_numPitchSamples_)},
                temp_{(f_remainingSamples / speed + f_m_numPitchSamples) / rate + 0.5f};

    const auto expectedOutputSamples{d.m_numOutputSamples_ + static_cast<int>(temp_)};

    /* Add enough silence to flush both input and pitch buffers. */
    if(!enlargeInputBufferIfNeeded(remainingSamples + 2 * maxRequired)) {
        return {};
    }

    const auto dst_{m_inputBuffer_.data() + remainingSamples * d.m_numChannels_};
    const auto size_{2 * maxRequired * d.m_numChannels_};
    std::fill_n(dst_,size_,0);

    d.m_numInputSamples_ += 2 * maxRequired;

    if (!processStreamInput()){
        return {};
    }

    /* Throw away any extra samples we generated due to the silence we added */
    if(d.m_numOutputSamples_ > expectedOutputSamples) {
        d.m_numOutputSamples_ = expectedOutputSamples;
    }

    /* Empty input and pitch buffers */
    d.m_numInputSamples_ = d.m_remainingInputToCopy_ = d.m_numPitchSamples_ = 0;

    return true;
}

bool XSonic::WriteDoubleToStream(const double *samples, const int &numSamples) {

    if (!WriteToStream_helper(samples,numSamples,[samples](int16_t *buffer,const int &n) {
        for(int i{}; i < n; ++i) {
            auto sample{samples[i]};
            if (sample > 1.0) {
                sample = 1.0;
            }

            if (sample < -1.0) {
                sample = -1.0;
            }
            buffer[i] = static_cast<int16_t>(std::round(sample * 32767.0));
        }
    })) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::WriteFloatToStream(const float *samples,const int &numSamples) {

    if (!WriteToStream_helper(samples,numSamples,[samples](int16_t *buffer,const int &count) {
        for(int i{}; i < count; ++i) {
            const auto v{samples[i] * 32767.0f};
            buffer[i] = static_cast<int16_t>(v);
        }
    })) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::WriteU64ToStream(const uint64_t *samples, const int &numSamples) {

    if (!WriteToStream_helper(samples,numSamples,[samples](int16_t *buffer,const int &count) {
        for(int i{}; i < count; ++i) {
            const auto v1{samples[i] - 9223372036854775808ULL},
                    v2{v1 >> 48};
            buffer[i] = static_cast<int16_t>(v2);
        }
    })) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::WriteS64ToStream(const int64_t *samples, const int &numSamples) {

    if (!WriteToStream_helper(samples,numSamples,[samples](int16_t *buffer,const int &count) {
        for(int i{}; i < count; ++i) {
            const auto v1{samples[i] + (1LL << 47)},
                    v2{v1 >> 48};
            buffer[i] = static_cast<int16_t>(v2);
        }
    })) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::WriteS32ToStream(const int32_t *samples, const int &numSamples) {

    if (!WriteToStream_helper(samples,numSamples,[samples](int16_t *buffer,const int &count) {
        for(int i{}; i < count; ++i) {
            const auto v1{samples[i] + 32768},
                        v2{v1 >> 16};
            buffer[i] = static_cast<int16_t>(v2);
        }
    })) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::WriteU32ToStream(const uint32_t *samples, const int &numSamples) {

    if (!WriteToStream_helper(samples,numSamples,[samples](int16_t *buffer,const int &count) {
        for(int i{}; i < count; ++i) {
            const auto v1{samples[i] - 2147483648L},
                    v2{v1 >> 16};
            buffer[i] = static_cast<int16_t>(v2);
        }
    })) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::WriteShortToStream(const int16_t *samples,const int &numSamples) {

    if (!WriteToStream_helper(samples,numSamples,[samples](int16_t *buffer,const int &count) {
        const auto src_{samples};
        const auto dst_{buffer};
        const auto size_{count};
        std::copy_n(src_,size_,dst_);
    })) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::WriteUnsignedShortToStream(const uint16_t *samples,const int &numSamples) {

    if (!WriteToStream_helper(samples,numSamples,[samples](int16_t *buffer,const int &n) {
        for(int i{}; i < n; ++i) {
            buffer[i] = static_cast<int16_t>(samples[i] - 32768);
        }
    })) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::WriteCharToStream(const int8_t *samples, const int &numSamples) {

    if (!WriteToStream_helper(samples,numSamples,[samples](int16_t *buffer,const int &count) {
        for (int i{};i < count;++i){
            buffer[i] = static_cast<int16_t>(samples[i] << 8);
        }
    })) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::WriteUnsignedCharToStream(const uint8_t *samples,const int &numSamples) {

    if (!WriteToStream_helper(samples,numSamples,[samples](int16_t *buffer,const int &count) {
        for (int i{};i < count;++i){
            const auto v1{samples[i] - 128},
                    v2{v1 << 8};
            buffer[i] = static_cast<int16_t>(v2);
        }
    })) {
        return {};
    }

    return processStreamInput();
}

int ChangeFloatSpeed(float *samples,
                          const int &numSamples,
                          const float &speed,
                          const float &pitch,
                          const float &rate,
                          const float &volume,
                          const int &useChordPitch,
                          const int &sampleRate,
                          const int &numChannels) {

    if (!samples || numSamples <= 0){
        return -1;
    }

    XSonic xSonic;
    if (!xSonic.Open(sampleRate,numChannels)){
        return -1;
    }
    xSonic.set_speed(speed);
    xSonic.set_pitch(pitch);
    xSonic.set_rate(rate);
    xSonic.set_volume(volume);
    xSonic.set_chord_pitch(useChordPitch);
    xSonic.WriteFloatToStream(samples,numSamples);
    xSonic.FlushStream();
    const auto ret{xSonic.samples_available()};
    xSonic.ReadFloatFromStream(samples,ret);
    return ret;
}

int ChangeShortSpeed(int16_t *samples,
                          const int &numSamples,
                          const float &speed,
                          const float &pitch,
                          const float &rate,
                          const float &volume,
                          const int &useChordPitch,
                          const int &sampleRate,
                          const int &numChannels) {
    XSonic xSonic;
    if (xSonic.Open(sampleRate,numChannels)){
        return -1;
    }
    xSonic.set_speed(speed);
    xSonic.set_pitch(pitch);
    xSonic.set_rate(rate);
    xSonic.set_volume(volume);
    xSonic.set_chord_pitch(useChordPitch);
    xSonic.WriteShortToStream(samples,numSamples);
    xSonic.FlushStream();
    const auto ret{xSonic.samples_available()};
    xSonic.ReadShortFromStream(samples,ret);
    return ret;
}
