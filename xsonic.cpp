#include "xsonic.hpp"
#include <algorithm>

XSonic::XSonic(const int &sampleRate,const int &numChannels) {
    if (!Open(sampleRate, numChannels)){
        LOG_ERROR(GET_STR(XSonic Construct failed!));
    }
}

XSonic::XSonic(XSonic &&obj) noexcept(true) {
    Move_(std::addressof(obj));
}

XSonic &XSonic::operator=(XSonic &&obj) noexcept(true) {
    if (const auto src_{std::addressof(obj)}; this != src_){
        Move_(src_);
    }
    return *this;
}

bool XSonic::Open(const int &sampleRate,const int &numChannels) {
    const auto ret{sonicCreateStream(sampleRate, numChannels)};
    d.m_is_init_ = ret;
    return ret;
}


int XSonic::ReadFromStream_helper(const void *samples,const int &maxSamples,Middle_Logic_type &&f) {
    if (!d.m_is_init_ || !samples || maxSamples <= 0) {
        return -1;
    }

    auto numSamples{d.m_numOutputSamples_};
    if(!numSamples) {
        return {};
    }

    int remainingSamples {};
    if(numSamples > maxSamples) {
        remainingSamples = numSamples - maxSamples;
        numSamples = maxSamples;
    }

    const auto out_buffer{m_outputBuffer_.data()};
    const auto count{numSamples * d.m_numChannels_};

    f(out_buffer,count);

    if(remainingSamples > 0) {
        const auto src_{m_outputBuffer_.data() + count};
        const auto dst_{m_outputBuffer_.data()};
        const auto size_{remainingSamples * d.m_numChannels_};
        std::move(src_,src_ + size_,dst_);
    }

    d.m_numOutputSamples_ = remainingSamples;
    return numSamples;
}

int XSonic::sonicReadDoubleFromStream(double *samples, const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int& n) {
        for(int i{};i < n;++i) {
            samples[i] = static_cast<double>(src[i]) / 32768.0;
        }
    });
#if 0
    if (!d.m_is_init_ || !samples || maxSamples <= 0) {
        return -1;
    }

    auto numSamples{d.m_numOutputSamples_};
    if(!numSamples) {
        return {};
    }

    int remainingSamples {};
    if(numSamples > maxSamples) {
        remainingSamples = numSamples - maxSamples;
        numSamples = maxSamples;
    }

    const auto out_buffer{m_outputBuffer_.data()};
    const auto count{numSamples * d.m_numChannels_};

    for(int i{};i < count;++i) {
        samples[i] = out_buffer[i] / 32768.0;
    }

    if(remainingSamples > 0) {
        const auto src_{m_outputBuffer_.data() + count};
        const auto dst_{m_outputBuffer_.data()};
        const auto size_{remainingSamples * d.m_numChannels_};
        std::move(src_,src_ + size_,dst_);
    }

    d.m_numOutputSamples_ = remainingSamples;
    return numSamples;
#endif
}

int XSonic::sonicReadS64FromStream(int64_t *samples, const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int &n){
        for(int i{};i < n;++i) {
            samples[i] = static_cast<int64_t>(src[i]) << 48;
        }
    });
#if 0
    if (!d.m_is_init_ || !samples || maxSamples <= 0) {
        return -1;
    }

    auto numSamples {d.m_numOutputSamples_};
    if(!numSamples) {
        return {};
    }

    int remainingSamples {};
    if(numSamples > maxSamples) {
        remainingSamples = numSamples - maxSamples;
        numSamples = maxSamples;
    }

    const auto out_buffer{m_outputBuffer_.data()};
    const auto count{numSamples * d.m_numChannels_};

    for(int i{};i < count;++i) {
        samples[i] = static_cast<int64_t>(out_buffer[i]) << 48;
    }

    if(remainingSamples > 0) {
        const auto src_{m_outputBuffer_.data() + count};
        const auto dst_{m_outputBuffer_.data()};
        const auto size_{remainingSamples * d.m_numChannels_};
        std::move(src_,src_ + size_,dst_);
    }

    d.m_numOutputSamples_ = remainingSamples;
    return numSamples;
#endif
}

int XSonic::sonicReadU64FromStream(uint64_t *samples, const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int &n) {
        for(int i{};i < n;++i) {
            samples[i] = static_cast<uint64_t>(src[i] + 32768) << 48;
        }
    });
#if 0
    if (!d.m_is_init_ || !samples || maxSamples <= 0) {
        return -1;
    }

    auto numSamples {d.m_numOutputSamples_};
    if(!numSamples) {
        return {};
    }

    int remainingSamples {};
    if(numSamples > maxSamples) {
        remainingSamples = numSamples - maxSamples;
        numSamples = maxSamples;
    }

    const auto out_buffer{m_outputBuffer_.data()};
    const auto count{numSamples * d.m_numChannels_};

    for(int i{};i < count;++i) {
        samples[i] = static_cast<uint64_t>(out_buffer[i] + 32768) << 48;
    }

    if(remainingSamples > 0) {
        const auto src_{m_outputBuffer_.data() + numSamples * d.m_numChannels_};
        const auto dst_{m_outputBuffer_.data()};
        const auto size_{remainingSamples * d.m_numChannels_};
        std::move(src_,src_ + size_,dst_);
    }

    d.m_numOutputSamples_ = remainingSamples;
    return numSamples;
#endif
}

int XSonic::sonicReadFloatFromStream(float *samples,
                                     const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int &n) {
        for(int i{};i < n;++i) {
            samples[i] = static_cast<float>(src[i]) / 32767.0f;
        }
    });

#if 0
    if (!d.m_is_init_ || !samples || maxSamples <= 0) {
        return -1;
    }

    auto numSamples {d.m_numOutputSamples_};
    if(!numSamples) {
        return {};
    }

    int remainingSamples {};
    if(numSamples > maxSamples) {
        remainingSamples = numSamples - maxSamples;
        numSamples = maxSamples;
    }

    const auto out_buffer{m_outputBuffer_.data()};
    const auto count{numSamples * d.m_numChannels_};

    for(int i{};i < count;++i) {
        samples[i] = static_cast<float>(out_buffer[i]) / 32767.0f;
    }

    if(remainingSamples > 0) {
        const auto src_{m_outputBuffer_.data() + count};
        const auto dst_{m_outputBuffer_.data()};
        const auto size_{remainingSamples * d.m_numChannels_};
        std::move(src_,src_ + size_,dst_);
    }

    d.m_numOutputSamples_ = remainingSamples;
    return numSamples;
#endif
}

int XSonic::sonicReadU32FromStream(uint32_t *samples,const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int &n) {
        for(int i{};i < n;++i) {
            samples[i] = static_cast<uint32_t>(src[i] + 32768) << 16;
        }
    });

#if 0
    if (!d.m_is_init_ || !samples || maxSamples <= 0) {
        return -1;
    }

    auto numSamples {d.m_numOutputSamples_};
    if(!numSamples) {
        return {};
    }

    int remainingSamples {};
    if(numSamples > maxSamples) {
        remainingSamples = numSamples - maxSamples;
        numSamples = maxSamples;
    }

    const auto out_buffer{m_outputBuffer_.data()};
    const auto count{numSamples * d.m_numChannels_};

    for(int i{};i < count;++i) {
        samples[i] = static_cast<uint32_t>(out_buffer[i] + 32768) << 16;
    }

    if(remainingSamples > 0) {
        const auto src_{m_outputBuffer_.data() + count};
        const auto dst_{m_outputBuffer_.data()};
        const auto size_{remainingSamples * d.m_numChannels_};
        std::move(src_,src_ + size_,dst_);
    }

    d.m_numOutputSamples_ = remainingSamples;
    return numSamples;
#endif
}

int XSonic::sonicReadS32FromStream(int32_t *samples, const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int &n){
        for(int i{};i < n;++i) {
            samples[i] = src[i] << 16;
        }
    });
#if 0
    if (!d.m_is_init_ || !samples || maxSamples <= 0) {
        return -1;
    }

    auto numSamples {d.m_numOutputSamples_};
    if(!numSamples) {
        return {};
    }

    int remainingSamples {};
    if(numSamples > maxSamples) {
        remainingSamples = numSamples - maxSamples;
        numSamples = maxSamples;
    }

    const auto out_buffer{m_outputBuffer_.data()};
    const auto count{numSamples * d.m_numChannels_};

    for(int i{};i < count;++i) {
        samples[i] = out_buffer[i] << 16;
    }

    if(remainingSamples > 0) {
        const auto src_{m_outputBuffer_.data() + count};
        const auto dst_{m_outputBuffer_.data()};
        const auto size_{remainingSamples * d.m_numChannels_};
        std::move(src_,src_ + size_,dst_);
    }

    d.m_numOutputSamples_ = remainingSamples;
    return numSamples;
#endif
}

int XSonic::sonicReadShortFromStream(int16_t *samples,
                                     const int &maxSamples) {
    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *read_,const int &w_size_) {
            std::copy_n(read_,w_size_,samples);
    });
#if 0
    // if (!d.m_is_init_ || !samples || maxSamples <= 0){
    //     return -1;
    // }
    //
    // auto numSamples{d.m_numOutputSamples_};
    //
    // if(!numSamples) {
    //     return {};
    // }
    //
    // int remainingSamples {};
    // if(numSamples > maxSamples) {
    //     remainingSamples = numSamples - maxSamples;
    //     numSamples = maxSamples;
    // }
    //
    // const auto read_{m_outputBuffer_.data()};
    // const auto write_{samples};
    // const auto w_size_{numSamples * d.m_numChannels_};
    // std::copy_n(read_,w_size_,write_);
    //
    // if(remainingSamples > 0) {
    //     const auto src_{m_outputBuffer_.data() + w_size_};
    //     const auto dst_{m_outputBuffer_.data()};
    //     const auto size_{remainingSamples * d.m_numChannels_};
    //     std::move(src_,src_ + size_,dst_);
    // }
    //
    // d.m_numOutputSamples_ = remainingSamples;
    // return numSamples;
#endif
}

int XSonic::sonicReadUnsignedShortFromStream(uint16_t *samples, const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *read_,const int &w_size_) {
        for(int i{}; i < w_size_;++i) {
            samples[i] = read_[i] + 32768;
        }
    });
#if 0
    if (!d.m_is_init_ || !samples || maxSamples <= 0){
        return -1;
    }

    auto numSamples{d.m_numOutputSamples_};

    if(!numSamples) {
        return {};
    }

    int remainingSamples {};
    if(numSamples > maxSamples) {
        remainingSamples = numSamples - maxSamples;
        numSamples = maxSamples;
    }

    const auto read_{m_outputBuffer_.data()};
    const auto w_size_{numSamples * d.m_numChannels_};

    for(int i{}; i < w_size_;++i) {
        samples[i] = read_[i] + 32768;
    }

    if(remainingSamples > 0) {
        const auto src_{m_outputBuffer_.data() + w_size_};
        const auto dst_{m_outputBuffer_.data()};
        const auto size_{remainingSamples * d.m_numChannels_};
        std::move(src_,src_ + size_,dst_);
    }

    d.m_numOutputSamples_ = remainingSamples;
    return numSamples;
#endif
}

int XSonic::sonicReadUnsignedCharFromStream(uint8_t *samples,
                                            const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int &n) {
        for(int i{};i < n;++i) {
            const auto v1{src[i] >> 8},v2{v1 + 128};
            samples[i] = static_cast<uint8_t>(v2);
        }
    });
#if 0
    if (!d.m_is_init_ || !samples || maxSamples <= 0){
        return -1;
    }

    auto numSamples{d.m_numOutputSamples_};

    if(!numSamples) {
        return 0;
    }

    int remainingSamples{};
    if(numSamples > maxSamples) {
        remainingSamples = numSamples - maxSamples;
        numSamples = maxSamples;
    }

    const auto out_buffer{m_outputBuffer_.data()};
    const auto count{numSamples * d.m_numChannels_};

    for(int i{};i < count;++i) {
        const auto v1{out_buffer[i] >> 8},v2{v1 + 128};
        samples[i] = static_cast<uint8_t>(v2);
    }

    if(remainingSamples > 0) {
        const auto src_{m_outputBuffer_.data() + count};
        const auto dst_{m_outputBuffer_.data()};
        const auto size_{remainingSamples * d.m_numChannels_};
        std::move(src_,src_ + size_,dst_);
    }
    d.m_numOutputSamples_ = remainingSamples;
    return numSamples;
#endif
}

int XSonic::sonicReadSignedCharFromStream(int8_t *samples, const int &maxSamples) {

    return ReadFromStream_helper(samples,maxSamples,[samples](const int16_t *src,const int &n) {
        for(int i{}; i < n; ++i) {
            samples[i] = static_cast<int8_t>(std::round(src[i] >> 8));
        }
    });
#if 0
    if (!d.m_is_init_ || !samples || maxSamples <= 0){
        return -1;
    }

    auto numSamples{d.m_numOutputSamples_};

    if(!numSamples) {
        return 0;
    }

    int remainingSamples{};
    if(numSamples > maxSamples) {
        remainingSamples = numSamples - maxSamples;
        numSamples = maxSamples;
    }

    const auto out_buffer{m_outputBuffer_.data()};
    const auto count{numSamples * d.m_numChannels_};

    for(int i{}; i < count; ++i) {
        samples[i] = static_cast<int8_t>(std::round(out_buffer[i] >> 8));
    }

    if(remainingSamples > 0) {
        const auto src_{m_outputBuffer_.data() + count};
        const auto dst_{m_outputBuffer_.data()};
        const auto size_{remainingSamples * d.m_numChannels_};
        std::move(src_,src_ + size_,dst_);
    }

    d.m_numOutputSamples_ = remainingSamples;
    return numSamples;
#endif
}

bool XSonic::sonicFlushStream() {

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

//    if(!sonicWriteShortToStream({},{})) { //这个逻辑感觉有点多余,直接调用processStreamInput进行处理
//        return {};
//    }

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

bool XSonic::sonicWriteDoubleToStream(const double *samples, const int &numSamples) {
    if (!d.m_is_init_ || !samples || numSamples <= 0){
        return {};
    }

    if(!addDoubleSamplesToInputBuffer(samples, numSamples)) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::sonicWriteFloatToStream(const float *samples,const int &numSamples) {

    if (!d.m_is_init_ || !samples || numSamples <= 0){
        return {};
    }

    if(!addFloatSamplesToInputBuffer(samples, numSamples)) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::sonicWriteU64ToStream(const uint64_t *samples, const int &numSamples) {

    if (!d.m_is_init_ || !samples || numSamples <= 0){
        return {};
    }

    if(!addU64SamplesToInputBuffer(samples, numSamples)) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::sonicWriteS64ToStream(const int64_t *samples, const int &numSamples) {
    if (!d.m_is_init_ || !samples || numSamples <= 0){
        return {};
    }

    if(!addS64SamplesToInputBuffer(samples, numSamples)) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::sonicWriteS32ToStream(const int32_t *samples, const int &numSamples) {

    if (!d.m_is_init_ || !samples || numSamples <= 0){
        return {};
    }

    if (!addS32SamplesToInputBuffer(samples, numSamples)) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::sonicWriteU32ToStream(const uint32_t *samples, const int &numSamples) {

    if (!d.m_is_init_ || !samples || numSamples <= 0){
        return {};
    }

    if (!addU32SamplesToInputBuffer(samples, numSamples)) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::sonicWriteShortToStream(const int16_t *samples,const int &numSamples) {

    if (!d.m_is_init_ || !samples || numSamples <= 0) {
        return {};
    }

    if(!addShortSamplesToInputBuffer( samples, numSamples)) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::sonicWriteUnsignedShortToStream(const uint16_t *samples, const int &numSamples) {
    if (!d.m_is_init_ || !samples || numSamples <= 0) {
        return {};
    }

    if(!addUnsignedShortSamplesToInputBuffer(samples, numSamples)) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::sonicWriteCharToStream(const int8_t *samples, const int &numSamples) {
    if (!d.m_is_init_ || !samples || numSamples <= 0) {
        return {};
    }

    if(!addCharSamplesToInputBuffer(samples, numSamples)) {
        return {};
    }

    return processStreamInput();
}

bool XSonic::sonicWriteUnsignedCharToStream(const uint8_t *samples,const int &numSamples) {

    if (!d.m_is_init_ || !samples || numSamples <= 0) {
        return {};
    }

    if(!addUnsignedCharSamplesToInputBuffer(samples, numSamples)) {
        return {};
    }
    return processStreamInput();
}

int sonicChangeFloatSpeed(float *samples,
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
    xSonic.sonicSetSpeed(speed);
    xSonic.sonicSetPitch(pitch);
    xSonic.sonicSetRate(rate);
    xSonic.sonicSetVolume(volume);
    xSonic.sonicSetChordPitch(useChordPitch);
    xSonic.sonicWriteFloatToStream(samples,numSamples);
    xSonic.sonicFlushStream();
    const auto ret{xSonic.sonicSamplesAvailable()};
    xSonic.sonicReadFloatFromStream(samples,ret);
    return ret;
}

int sonicChangeShortSpeed(int16_t *samples,
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
    xSonic.sonicSetSpeed(speed);
    xSonic.sonicSetPitch(pitch);
    xSonic.sonicSetRate(rate);
    xSonic.sonicSetVolume(volume);
    xSonic.sonicSetChordPitch(useChordPitch);
    xSonic.sonicWriteShortToStream(samples,numSamples);
    xSonic.sonicFlushStream();
    const auto ret{xSonic.sonicSamplesAvailable()};
    xSonic.sonicReadShortFromStream(samples,ret);
    return ret;
}
