#include "xsonicprivate.hpp"

static inline constexpr auto SINC_FILTER_POINTS{12},
SINC_TABLE_SIZE{601},
SONIC_MIN_PITCH_{65},
SONIC_MAX_PITCH_{400},
SONIC_AMDF_FREQ_{4000};

static inline constexpr int16_t sincTable[SINC_TABLE_SIZE] = {
        0, 0, 0, 0, 0, 0, 0, -1, -1, -2, -2, -3, -4, -6, -7, -9, -10, -12, -14,
        -17, -19, -21, -24, -26, -29, -32, -34, -37, -40, -42, -44, -47, -48, -50,
        -51, -52, -53, -53, -53, -52, -50, -48, -46, -43, -39, -34, -29, -22, -16,
        -8, 0, 9, 19, 29, 41, 53, 65, 79, 92, 107, 121, 137, 152, 168, 184, 200,
        215, 231, 247, 262, 276, 291, 304, 317, 328, 339, 348, 357, 363, 369, 372,
        374, 375, 373, 369, 363, 355, 345, 332, 318, 300, 281, 259, 234, 208, 178,
        147, 113, 77, 39, 0, -41, -85, -130, -177, -225, -274, -324, -375, -426,
        -478, -530, -581, -632, -682, -731, -779, -825, -870, -912, -951, -989,
        -1023, -1053, -1080, -1104, -1123, -1138, -1149, -1154, -1155, -1151,
        -1141, -1125, -1105, -1078, -1046, -1007, -963, -913, -857, -796, -728,
        -655, -576, -492, -403, -309, -210, -107, 0, 111, 225, 342, 462, 584, 708,
        833, 958, 1084, 1209, 1333, 1455, 1575, 1693, 1807, 1916, 2022, 2122, 2216,
        2304, 2384, 2457, 2522, 2579, 2625, 2663, 2689, 2706, 2711, 2705, 2687,
        2657, 2614, 2559, 2491, 2411, 2317, 2211, 2092, 1960, 1815, 1658, 1489,
        1308, 1115, 912, 698, 474, 241, 0, -249, -506, -769, -1037, -1310, -1586,
        -1864, -2144, -2424, -2703, -2980, -3254, -3523, -3787, -4043, -4291,
        -4529, -4757, -4972, -5174, -5360, -5531, -5685, -5819, -5935, -6029,
        -6101, -6150, -6175, -6175, -6149, -6096, -6015, -5905, -5767, -5599,
        -5401, -5172, -4912, -4621, -4298, -3944, -3558, -3141, -2693, -2214,
        -1705, -1166, -597, 0, 625, 1277, 1955, 2658, 3386, 4135, 4906, 5697, 6506,
        7332, 8173, 9027, 9893, 10769, 11654, 12544, 13439, 14335, 15232, 16128,
        17019, 17904, 18782, 19649, 20504, 21345, 22170, 22977, 23763, 24527,
        25268, 25982, 26669, 27327, 27953, 28547, 29107, 29632, 30119, 30569,
        30979, 31349, 31678, 31964, 32208, 32408, 32565, 32677, 32744, 32767,
        32744, 32677, 32565, 32408, 32208, 31964, 31678, 31349, 30979, 30569,
        30119, 29632, 29107, 28547, 27953, 27327, 26669, 25982, 25268, 24527,
        23763, 22977, 22170, 21345, 20504, 19649, 18782, 17904, 17019, 16128,
        15232, 14335, 13439, 12544, 11654, 10769, 9893, 9027, 8173, 7332, 6506,
        5697, 4906, 4135, 3386, 2658, 1955, 1277, 625, 0, -597, -1166, -1705,
        -2214, -2693, -3141, -3558, -3944, -4298, -4621, -4912, -5172, -5401,
        -5599, -5767, -5905, -6015, -6096, -6149, -6175, -6175, -6150, -6101,
        -6029, -5935, -5819, -5685, -5531, -5360, -5174, -4972, -4757, -4529,
        -4291, -4043, -3787, -3523, -3254, -2980, -2703, -2424, -2144, -1864,
        -1586, -1310, -1037, -769, -506, -249, 0, 241, 474, 698, 912, 1115, 1308,
        1489, 1658, 1815, 1960, 2092, 2211, 2317, 2411, 2491, 2559, 2614, 2657,
        2687, 2705, 2711, 2706, 2689, 2663, 2625, 2579, 2522, 2457, 2384, 2304,
        2216, 2122, 2022, 1916, 1807, 1693, 1575, 1455, 1333, 1209, 1084, 958, 833,
        708, 584, 462, 342, 225, 111, 0, -107, -210, -309, -403, -492, -576, -655,
        -728, -796, -857, -913, -963, -1007, -1046, -1078, -1105, -1125, -1141,
        -1151, -1155, -1154, -1149, -1138, -1123, -1104, -1080, -1053, -1023, -989,
        -951, -912, -870, -825, -779, -731, -682, -632, -581, -530, -478, -426,
        -375, -324, -274, -225, -177, -130, -85, -41, 0, 39, 77, 113, 147, 178,
        208, 234, 259, 281, 300, 318, 332, 345, 355, 363, 369, 373, 375, 374, 372,
        369, 363, 357, 348, 339, 328, 317, 304, 291, 276, 262, 247, 231, 215, 200,
        184, 168, 152, 137, 121, 107, 92, 79, 65, 53, 41, 29, 19, 9, 0, -8, -16,
        -22, -29, -34, -39, -43, -46, -48, -50, -52, -53, -53, -53, -52, -51, -50,
        -48, -47, -44, -42, -40, -37, -34, -32, -29, -26, -24, -21, -19, -17, -14,
        -12, -10, -9, -7, -6, -4, -3, -2, -2, -1, -1, 0, 0, 0, 0, 0, 0, 0
};

int SonicPrivate::ReadFromStream_helper(const void *samples,const int &maxSamples,
    Middle_Logic_type &&f) {

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

bool SonicPrivate::WriteToStream_helper(const void *samples,const int &numSamples,
    Middle_Logic_type &&f) {

    if (!d.m_is_init_ || !samples || numSamples <= 0){
        return {};
    }

    if(!enlargeInputBufferIfNeeded(numSamples)) {
        return {};
    }

    const auto count{numSamples * d.m_numChannels_};
    const auto buffer{m_inputBuffer_.data() + d.m_numInputSamples_ * d.m_numChannels_};

    f(buffer,count);

    d.m_numInputSamples_ += numSamples;
    return true;
}

void SonicPrivate::Close(){
    m_inputBuffer_.clear();
    m_outputBuffer_.clear();
    m_pitchBuffer_.clear();
    m_downSampleBuffer_.clear();
    d= {};
}

void SonicPrivate::scaleSamples(int16_t *samples,
                          const int &numSamples,
                          const float & volume) {

    const auto fixedPointVolume{static_cast<int>(volume * 4096.0f)};

    for (int i {}; i < numSamples; i++) {
        auto value{samples[i] * fixedPointVolume >> 12};
        if(value > 32767) {
            value = 32767;
        }else if(value < -32767) {
            value = -32767;
        }else{}
        samples[i] = static_cast<int16_t>(value);;
    }
}

bool SonicPrivate::allocateStreamBuffers(const int &sampleRate,const int &numChannels) {

    if (sampleRate <= 0 || numChannels <= 0){
        return {};
    }

    Close();
    const auto minPeriod{sampleRate / SONIC_MAX_PITCH_},
                maxPeriod{sampleRate/SONIC_MIN_PITCH_},
                maxRequired{2 * maxPeriod};

    d.m_inputBufferSize_ = d.m_outputBufferSize_ = d.m_pitchBufferSize_ = maxRequired;
    try {
        const auto alloc_size{maxRequired * numChannels};
        CHECK_EXC(m_inputBuffer_.resize(alloc_size));
        CHECK_EXC(m_outputBuffer_.resize(alloc_size));
        CHECK_EXC(m_pitchBuffer_.resize(alloc_size));
        CHECK_EXC(m_downSampleBuffer_.resize(maxRequired));
    } catch (const std::exception &e) {
        Close();
        std::cerr << e.what() << "\n";
        return {};
    }

    d.m_sampleRate_ = sampleRate;
    d.m_numChannels_ = numChannels;
    d.m_minPeriod_ = minPeriod;
    d.m_maxPeriod_ = maxPeriod;
    d.m_maxRequired_ = maxRequired;
    d.m_speed_ = d.m_pitch_ = d.m_volume_ = d.m_rate_ = 1.0f;
    d.m_oldRatePosition_ = d.m_newRatePosition_ = d.m_useChordPitch_ = d.m_quality_ = d.m_prevPeriod_ = 0;
    d.m_avePower_ = 50.0f;
    return true;
}

bool SonicPrivate::sonicCreateStream(const int &sampleRate,const int &numChannels){

    if (!allocateStreamBuffers(sampleRate,numChannels)){
        return {};
    }
    return true;
}

bool SonicPrivate::enlargeOutputBufferIfNeeded(const int &numSamples) {

    if(d.m_numOutputSamples_ + numSamples > d.m_outputBufferSize_) {
        d.m_outputBufferSize_ += (d.m_outputBufferSize_ >> 1) + numSamples;
        m_outputBuffer_.clear();
        TRY_CATCH(CHECK_EXC(m_outputBuffer_.resize(d.m_outputBufferSize_ * d.m_numChannels_)),return {});
    }
    return true;
}

bool SonicPrivate::enlargeInputBufferIfNeeded(const int &numSamples) {

    if(d.m_numInputSamples_ + numSamples > d.m_inputBufferSize_) {
        d.m_inputBufferSize_ += (d.m_inputBufferSize_ >> 1) + numSamples;
        m_inputBuffer_.clear();
        TRY_CATCH(CHECK_EXC(m_inputBuffer_.resize(d.m_inputBufferSize_ * d.m_numChannels_)),return {});
    }
    return true;
}

#if 0

bool SonicPrivate::addDoubleSamplesToInputBuffer(const double *samples, const int &numSamples) {

    if(numSamples <= 0 || !samples) {
        return {};
    }

    if(!enlargeInputBufferIfNeeded(numSamples)) {
        return {};
    }

    const auto count{numSamples * d.m_numChannels_};
    const auto buffer{m_inputBuffer_.data() + d.m_numInputSamples_ * d.m_numChannels_};

    for(int i{}; i < count; ++i) {
        auto sample{samples[i]};
        if (sample > 1.0) {
            sample = 1.0;
        }

        if (sample < -1.0) {
            sample = -1.0;
        }

        buffer[i] = static_cast<int16_t>(std::round(sample * 32767.0));
    }

    d.m_numInputSamples_ += numSamples;
    return true;
}

bool SonicPrivate::addS64SamplesToInputBuffer(const int64_t *samples,const int &numSamples) {

    if(numSamples <= 0 || !samples) {
        return {};
    }

    if(!enlargeInputBufferIfNeeded(numSamples)) {
        return {};
    }

    const auto count{numSamples * d.m_numChannels_};
    const auto buffer{m_inputBuffer_.data() + d.m_numInputSamples_ * d.m_numChannels_};

    for(int i{}; i < count; ++i) {
        const auto v1{samples[i] + (1LL << 47)},
                v2{v1 >> 48};
        buffer[i] = static_cast<int16_t>(v2);
    }

    d.m_numInputSamples_ += numSamples;
    return true;
}

bool SonicPrivate::addU64SamplesToInputBuffer(const uint64_t *samples, const int &numSamples) {

    if(numSamples <= 0 || !samples) {
        return {};
    }

    if(!enlargeInputBufferIfNeeded(numSamples)) {
        return {};
    }

    const auto count{numSamples * d.m_numChannels_};
    const auto buffer{m_inputBuffer_.data() + d.m_numInputSamples_ * d.m_numChannels_};

    for(int i{}; i < count; ++i) {
        const auto v1{samples[i] - 9223372036854775808ULL},
                v2{v1 >> 48};
        buffer[i] = static_cast<int16_t>(v2);
    }

    d.m_numInputSamples_ += numSamples;
    return true;
}

bool SonicPrivate::addFloatSamplesToInputBuffer(const float *samples,const int &numSamples) {

    if(numSamples <= 0 || !samples) {
        return {};
    }

    if(!enlargeInputBufferIfNeeded(numSamples)) {
        return {};
    }

    const auto count{numSamples * d.m_numChannels_};
    const auto buffer{m_inputBuffer_.data() + d.m_numInputSamples_ * d.m_numChannels_};

    for(int i{}; i < count; ++i) {
        const auto v{samples[i] * 32767.0f};
        buffer[i] = static_cast<int16_t>(v);
    }

    d.m_numInputSamples_ += numSamples;
    return true;
}

bool SonicPrivate::addS32SamplesToInputBuffer(const int32_t *samples, const int &numSamples) {

    if(numSamples <= 0 || !samples) {
        return {};
    }

    if(!enlargeInputBufferIfNeeded(numSamples)) {
        return {};
    }

    const auto count{numSamples * d.m_numChannels_};
    const auto buffer{m_inputBuffer_.data() + d.m_numInputSamples_ * d.m_numChannels_};

    for(int i{}; i < count; ++i) {
        const auto v1{samples[i] + 32768},
                    v2{v1 >> 16};
        buffer[i] = static_cast<int16_t>(v2);
    }

    d.m_numInputSamples_ += numSamples;
    return true;
}

bool SonicPrivate::addU32SamplesToInputBuffer(const uint32_t *samples, const int &numSamples) {

    if(numSamples <= 0 || !samples) {
        return {};
    }

    if(!enlargeInputBufferIfNeeded(numSamples)) {
        return {};
    }

    const auto count{numSamples * d.m_numChannels_};
    const auto buffer{m_inputBuffer_.data() + d.m_numInputSamples_ * d.m_numChannels_};
    for(int i{}; i < count; ++i) {
        const auto v1{samples[i] - 2147483648L},
                v2{v1 >> 16};
        buffer[i] = static_cast<int16_t>(v2);
    }

    d.m_numInputSamples_ += numSamples;
    return true;
}

bool SonicPrivate::addShortSamplesToInputBuffer(const int16_t *samples,const int &numSamples) {

    if(numSamples <= 0 || !samples) {
        return {};
    }

    if(!enlargeInputBufferIfNeeded(numSamples)) {
        return {};
    }

    const auto src_{samples};
    const auto dst_{m_inputBuffer_.data() + d.m_numInputSamples_ * d.m_numChannels_};
    const auto size_{numSamples * d.m_numChannels_};
    std::copy_n(src_,size_,dst_);

    d.m_numInputSamples_ += numSamples;
    return true;
}

bool SonicPrivate::addUnsignedShortSamplesToInputBuffer(const uint16_t *samples, const int &numSamples) {

    if(numSamples <= 0 || !samples) {
        return {};
    }

    if(!enlargeInputBufferIfNeeded(numSamples)) {
        return {};
    }

    const auto dst_{m_inputBuffer_.data() + d.m_numInputSamples_ * d.m_numChannels_};
    const auto size_{numSamples * d.m_numChannels_};

    for(int i{}; i < size_; ++i) {
        dst_[i] = static_cast<int16_t>(samples[i] - 32768);
    }

    d.m_numInputSamples_ += numSamples;
    return true;
}

bool SonicPrivate::addUnsignedCharSamplesToInputBuffer(const uint8_t *samples,const int &numSamples) {

    if(numSamples <= 0 || !samples) {
        return {};
    }

    if(!enlargeInputBufferIfNeeded(numSamples)) {
        return {};
    }

    const auto count{numSamples * d.m_numChannels_};
    const auto buffer{m_inputBuffer_.data() + d.m_numInputSamples_ * d.m_numChannels_};

    for (int i{};i < count;++i){
        const auto v1{samples[i] - 128},
                v2{v1 << 8};
        buffer[i] = static_cast<int16_t>(v2);
    }

    d.m_numInputSamples_ += numSamples;
    return true;
}

bool SonicPrivate::addCharSamplesToInputBuffer(const int8_t *samples, const int &numSamples) {

    if(numSamples <= 0 || !samples) {
        return {};
    }

    if(!enlargeInputBufferIfNeeded(numSamples)) {
        return {};
    }

    const auto buffer{m_inputBuffer_.data() + d.m_numInputSamples_ * d.m_numChannels_};
    const auto count{numSamples * d.m_numChannels_};

    for (int i{};i < count;++i){
        buffer[i] = static_cast<int16_t>(samples[i] << 8);
    }

    d.m_numInputSamples_ += numSamples;
    return true;
}

#endif

void SonicPrivate::removeInputSamples(const int &position) {

    const auto remainingSamples{d.m_numInputSamples_ - position};

    if(remainingSamples > 0) {
        const auto src_{m_inputBuffer_.data() + position * d.m_numChannels_};
        const auto dst_{m_inputBuffer_.data()};
        const auto size_{remainingSamples * d.m_numChannels_};
        std::move(src_,src_ + size_ ,dst_);
    }

    d.m_numInputSamples_ = remainingSamples;
}

bool SonicPrivate::copyToOutput(const int16_t *samples,const int &numSamples) {

    if (!samples || numSamples <= 0){
        return {};
    }

    if(!enlargeOutputBufferIfNeeded(numSamples)) {
        return {};
    }

    const auto src_{samples};
    const auto dst_{m_outputBuffer_.data() + d.m_numOutputSamples_ * d.m_numChannels_};
    const auto size_{numSamples * d.m_numChannels_};
    std::copy_n(src_,size_,dst_);
    d.m_numOutputSamples_ += numSamples;
    return true;
}

int SonicPrivate::copyInputToOutput(const int &position) {

    auto numSamples{d.m_remainingInputToCopy_};

    if(numSamples > d.m_maxRequired_) {
        numSamples = d.m_maxRequired_;
    }

    if(const auto in_buffer{m_inputBuffer_.data() + position * d.m_numChannels_};
        !copyToOutput(in_buffer,numSamples)) {
        return -1;
    }

    d.m_remainingInputToCopy_ -= numSamples;
    return numSamples;
}

void SonicPrivate::downSampleInput(const int16_t *samples,
                             const int &skip) {

    if (!samples){
        return;
    }

    const auto numSamples{d.m_maxRequired_ / skip},
                samplesPerValue{d.m_numChannels_ * skip};

    auto downSamples{m_downSampleBuffer_.data()};

    for(int i{}; i < numSamples; ++i) {
        int value{};
        for(int j {}; j < samplesPerValue; ++j) {
            value += *samples++;
        }
        value /= samplesPerValue;
        *downSamples++ = static_cast<int16_t>(value);
    }
}

int SonicPrivate::findPitchPeriodInRange(const int16_t *samples,
                                   const int &minPeriod,
                                   const int &maxPeriod,
                                   int &retMinDiff,
                                   int &retMaxDiff) {
    if (!samples){
        return -1;
    }

    int worstPeriod{255},bestPeriod{},minDiff{1},maxDiff{};

    for(auto period{minPeriod}; period <= maxPeriod; period++) {
        int diff{};
        auto s{samples},p{samples + period};
        for(int i{}; i < period; i++) {
            const auto sVal{*s++},
                pVal{*p++};
            diff += sVal >= pVal? static_cast<int16_t>(sVal - pVal) :
                    static_cast<int16_t>(pVal - sVal);
        }
        /* Note that the highest number of samples we add into diff will be less
           than 256, since we skip samples.  Thus, diff is a 24 bit number, and
           we can safely multiply by numSamples without overflow */
        /* if (bestPeriod == 0 || (bestPeriod*3/2 > period && diff*bestPeriod < minDiff*period) ||
                diff*bestPeriod < (minDiff >> 1)*period) {*/
        if (!bestPeriod || diff * bestPeriod < minDiff * period) {
            minDiff = diff;
            bestPeriod = period;
        }

        if(diff * worstPeriod > maxDiff * period) {
            maxDiff = diff;
            worstPeriod = period;
        }
    }

    retMinDiff = minDiff / bestPeriod;
    retMaxDiff = maxDiff / worstPeriod;
    return bestPeriod;
}

bool SonicPrivate::prevPeriodBetter(const int &minDiff,
                              const int &maxDiff,
                              const int &preferNewPeriod) const {

    if(!minDiff || !d.m_prevPeriod_) {
        return {};
    }

    if(preferNewPeriod) {
        if(maxDiff > minDiff * 3) {
            /* Got a reasonable match this period */
            return {};
        }

        if(minDiff * 2 <= d.m_prevMinDiff_ * 3) {
            /* Mismatch is not that much greater this period */
            return {};
        }
    } else {
        if(minDiff <= d.m_prevMinDiff_) {
            return {};
        }
    }

    return true;
}

int SonicPrivate::findPitchPeriod(const int16_t *samples,const int &preferNewPeriod) {

    if (!samples){
        return -1;
    }

    auto minPeriod{d.m_minPeriod_},maxPeriod{d.m_maxPeriod_};
    const auto sampleRate{d.m_sampleRate_};

    auto skip {1};
    if(sampleRate > SONIC_AMDF_FREQ_ && !d.m_quality_) {
        skip = sampleRate / SONIC_AMDF_FREQ_;
    }

    int period,minDiff{},maxDiff{};
    if(1 == d.m_numChannels_ && 1 == skip) {
        period = findPitchPeriodInRange(samples, minPeriod, maxPeriod, minDiff, maxDiff);
    } else {
        downSampleInput(samples, skip);
        period = findPitchPeriodInRange(m_downSampleBuffer_.data(), minPeriod/skip,
                                        maxPeriod/skip, minDiff, maxDiff);
        if(1 != skip) {
            period *= skip;
            minPeriod = period - (skip << 2);
            maxPeriod = period + (skip << 2);

            if(minPeriod < d.m_minPeriod_) {
                minPeriod = d.m_minPeriod_;
            }

            if(maxPeriod > d.m_maxPeriod_) {
                maxPeriod = d.m_maxPeriod_;
            }

            if(1 == d.m_numChannels_) {
                period = findPitchPeriodInRange(samples, minPeriod, maxPeriod,
                                                minDiff, maxDiff);
            } else {
                downSampleInput(samples, 1);
                period = findPitchPeriodInRange(m_downSampleBuffer_.data(), minPeriod,
                                                maxPeriod, minDiff, maxDiff);
            }
        }
    }

    const auto retPeriod{prevPeriodBetter(minDiff, maxDiff, preferNewPeriod) ?
                         d.m_prevPeriod_ : period};

    d.m_prevMinDiff_ = minDiff;
    d.m_prevPeriod_ = period;
    return retPeriod;
}

void SonicPrivate::overlapAdd(const int &numSamples,
                        const int &numChannels,
                        int16_t *out,
                        const int16_t *rampDown,
                        const int16_t *rampUp) {

    for(uint32_t i{};i < numChannels;i++) {

        auto o{out + i};
        auto u{rampUp + i},d{rampDown + i};

        for(int t{}; t < numSamples; t++) {
            const auto v_d{*d},v_u{*u};
#ifdef SONIC_USE_SIN
            float ratio = sin(t*M_PI/(2*numSamples));
            *o = v_d * (1.0f - ratio) + v_u * ratio;
#else
            *o = static_cast<int16_t>((v_d * (numSamples - t) + v_u *t) / numSamples);
#endif
            o += numChannels;
            d += numChannels;
            u += numChannels;
        }
    }
}

void SonicPrivate::overlapAddWithSeparation(const int &numSamples,
                                      const int &numChannels,
                                      const int &separation,
                                      int16_t *out,
                                      const int16_t *rampDown,
                                      const int16_t *rampUp) {

    for(uint32_t i{}; i < numChannels; i++) {

        auto o{out + i};
        auto u{rampUp + i},d{rampDown + i};

        for(int t{}; t < numSamples + separation; t++) {

            const auto v_d{*d},v_u{*u};

            if(t < separation) {
                *o = static_cast<int16_t>(v_d * (numSamples - t) / numSamples);
                d += numChannels;
            } else if(t < numSamples) {
                *o = static_cast<int16_t>((v_d * (numSamples - t) + v_u * (t - separation)) / numSamples);
                d += numChannels;
                u += numChannels;
            } else {
                *o = static_cast<int16_t>(v_u * (t - separation) / numSamples);
                u += numChannels;
            }
            o += numChannels;
        }
    }
}

bool SonicPrivate::moveNewSamplesToPitchBuffer(const int &originalNumOutputSamples) {

    const auto numSamples{d.m_numOutputSamples_ - originalNumOutputSamples},
                numChannels{d.m_numChannels_};

    if(d.m_numPitchSamples_ + numSamples > d.m_pitchBufferSize_) {
        d.m_pitchBufferSize_ += (d.m_pitchBufferSize_ >> 1) + numSamples;
        m_pitchBuffer_.clear();
        TRY_CATCH(CHECK_EXC(m_pitchBuffer_.resize(d.m_pitchBufferSize_ * numChannels)),return {});
    }

    const auto src_{m_outputBuffer_.data() + originalNumOutputSamples * numChannels};
    const auto dst_{m_pitchBuffer_.data() + d.m_numPitchSamples_ * numChannels};
    const auto size_{numSamples * numChannels};
    std::copy_n(src_,size_,dst_);

    d.m_numOutputSamples_ = originalNumOutputSamples;
    d.m_numPitchSamples_ += numSamples;
    return true;
}

void SonicPrivate::removePitchSamples(const int &numSamples) {

    if(!numSamples) {
        return;
    }

    if(numSamples != d.m_numPitchSamples_) {
        const auto numChannels{d.m_numChannels_};
        const auto src_{m_pitchBuffer_.data() + numSamples * numChannels};
        const auto dst_{m_pitchBuffer_.data()};
        const auto size_{(d.m_numPitchSamples_ - numSamples) * numChannels};
        std::move(src_,src_ + size_,dst_);
    }

    d.m_numPitchSamples_ -= numSamples;
}

bool SonicPrivate::adjustPitch(const int &originalNumOutputSamples) {

    const auto pitch{d.m_pitch_};
    const auto numChannels{d.m_numChannels_};

    if(originalNumOutputSamples == d.m_numOutputSamples_) {
        return true;
    }

    if(!moveNewSamplesToPitchBuffer(originalNumOutputSamples)) {
        return {};
    }

    int position {};
    while(d.m_numPitchSamples_ - position >= d.m_maxRequired_) {
        const auto period{findPitchPeriod(m_pitchBuffer_.data() + position * numChannels, 0)};

        const auto float_period{static_cast<float >(period)};
        const auto newPeriod{static_cast<int>(float_period / pitch)};

        if(!enlargeOutputBufferIfNeeded( newPeriod)) {
            return {};
        }

        const auto out{m_outputBuffer_.data() + d.m_numOutputSamples_ * numChannels};

        if(pitch >= 1.0f) {
            const auto rampDown{m_pitchBuffer_.data() + position * numChannels},
                rampUp{m_pitchBuffer_.data() + (position + period - newPeriod) * numChannels};
            overlapAdd(newPeriod, numChannels, out, rampDown, rampUp);
        } else {
            const auto rampDown{m_pitchBuffer_.data() + position * numChannels},
                rampUp{m_pitchBuffer_.data() + position * numChannels};
            const auto separation{newPeriod - period};
            overlapAddWithSeparation(period, numChannels, separation, out, rampDown, rampUp);
        }

        d.m_numOutputSamples_ += newPeriod;
        position += period;
    }

    removePitchSamples(position);
    return true;
}

int SonicPrivate::findSincCoefficient(const int &i,
                                const int &ratio,
                                const int &width) {

    static constexpr auto lobePoints {(SINC_TABLE_SIZE - 1) / SINC_FILTER_POINTS};

    const auto left{i * lobePoints + ratio * lobePoints / width},
                right{left + 1};

    const auto position{i * lobePoints * width + ratio * lobePoints - left * width};

    const auto leftVal{sincTable[left]},
                rightVal{sincTable[right]};

    return (leftVal * (width - position) + rightVal * position << 1) / width;
}

int16_t SonicPrivate::interpolate(const int16_t *in,
                          const int &oldSampleRate,
                          const int &newSampleRate) const {

    if (!in){
        return -1;
    }

    constexpr auto getSign{[](const int &value){
        return value >= 0;
    }};

    /* Compute N-point sinc FIR-filter here.  Clip rather than overflow. */

    const auto position{d.m_newRatePosition_ * oldSampleRate},
                leftPosition{d.m_oldRatePosition_ * newSampleRate},
                rightPosition{(d.m_oldRatePosition_ + 1) * newSampleRate},
                ratio{rightPosition - position - 1},
                width{rightPosition - leftPosition};

    int total{},overflowCount{};
    for (int i{}; i < SINC_FILTER_POINTS; i++) {

        const auto weight{findSincCoefficient(i, ratio, width)};
        /* printf("%u %f\n", i, weight); */
        const auto value{in[i * d.m_numChannels_] * weight};
        const auto oldSign {getSign(total)};
        total += value;

        if (oldSign != getSign(total) && getSign(value) == oldSign) {
            /* We must have overflowed.  This can happen with a sinc filter. */
            overflowCount += oldSign;
        }
    }

    /* It is better to clip than to wrap if there was a overflow. */
    if (overflowCount > 0) {
        return SHRT_MAX;
    }

    if(overflowCount < 0) {
        return SHRT_MIN;
    }

    return static_cast<int16_t>(total >> 16);
}

bool SonicPrivate::adjustRate(const float &rate,const int &originalNumOutputSamples) {

    auto oldSampleRate{d.m_sampleRate_},
        newSampleRate{static_cast<int>(static_cast<float>(oldSampleRate) / rate)};
    const auto numChannels{d.m_numChannels_};

    static constexpr auto N{SINC_FILTER_POINTS};
    /* Set these values to help with the integer math */
    while(newSampleRate > 1 << 14 || oldSampleRate > 1 << 14) {
        newSampleRate >>= 1;
        oldSampleRate >>= 1;
    }

    if(originalNumOutputSamples == d.m_numOutputSamples_) {
        return true;
    }

    if(!moveNewSamplesToPitchBuffer(originalNumOutputSamples)) {
        return {};
    }

    /* Leave at least N pitch sample in the buffer */
    int position{};
    for(;position < d.m_numPitchSamples_ - N; position++) {

        while((d.m_oldRatePosition_ + 1) * newSampleRate > d.m_newRatePosition_ * oldSampleRate) {

            if(!enlargeOutputBufferIfNeeded(1)) {
                return {};
            }

            auto out{m_outputBuffer_.data() + d.m_numOutputSamples_ * numChannels},
                in{m_pitchBuffer_.data() + position * numChannels};

            for(int i{}; i < numChannels; i++) {
                *out++ = interpolate(in++, oldSampleRate, newSampleRate);
            }

            ++d.m_newRatePosition_;
            ++d.m_numOutputSamples_;
        }

        //d.m_oldRatePosition_++;

        if(++d.m_oldRatePosition_ == oldSampleRate) {

            d.m_oldRatePosition_ = 0;

            if(d.m_newRatePosition_ != newSampleRate) {
                std::cerr << "Assertion failed: m_newRatePosition != newSampleRate\n";
                return {};
            }

            d.m_newRatePosition_ = 0;
        }
    }

    removePitchSamples(position);
    return true;
}

int SonicPrivate::skipPitchPeriod(const int16_t *samples,
                            const float &speed,
                            const int &period) {

    const auto numChannels{d.m_numChannels_};
    const auto f_period{static_cast<float>(period)};

    int newSamples;
    if(speed >= 2.0f) {
        newSamples = static_cast<decltype(newSamples) >(f_period / (speed - 1.0f));
    } else {
        newSamples = period;
        d.m_remainingInputToCopy_ = static_cast<decltype(d.m_remainingInputToCopy_)>(f_period * (2.0f - speed) / (speed - 1.0f));
    }

    if(!enlargeOutputBufferIfNeeded(newSamples)) {
        return -1;
    }

    const auto out_buffer{m_outputBuffer_.data() + d.m_numOutputSamples_ * numChannels};

    overlapAdd(newSamples,numChannels,out_buffer,samples,samples + period * numChannels);

    d.m_numOutputSamples_ += newSamples;

    return newSamples;
}

int SonicPrivate::insertPitchPeriod(const int16_t *samples,
                              const float &speed,
                              const int &period) {

    const auto numChannels{d.m_numChannels_};
    const auto f_period{static_cast<float>(period)};

    int newSamples;
    if(speed < 0.5f) {
        newSamples = static_cast<decltype(newSamples)>(f_period * speed / (1.0f - speed));
    } else {
        newSamples = period;
        d.m_remainingInputToCopy_ =
            static_cast<decltype(d.m_remainingInputToCopy_)>(f_period * (2.0f * speed - 1.0f) / (1.0f - speed));
    }

    if(!enlargeOutputBufferIfNeeded(period + newSamples)) {
        return -1;
    }

    auto out{m_outputBuffer_.data() + d.m_numOutputSamples_ * numChannels};

    std::copy_n(samples,period * numChannels,out);

    out = m_outputBuffer_.data() + (d.m_numOutputSamples_ + period) * numChannels;

    overlapAdd(newSamples,numChannels,out,samples + period * numChannels,samples);

    d.m_numOutputSamples_ += period + newSamples;

    return newSamples;
}

bool SonicPrivate::changeSpeed(const float &speed) {

    const auto numSamples{d.m_numInputSamples_},
                maxRequired{d.m_maxRequired_};

    if(d.m_numInputSamples_ < maxRequired) {
        return true;
    }

    int position{};
    do {
        int newSamples;
        if(d.m_remainingInputToCopy_ > 0) {
            newSamples = copyInputToOutput(position);

            if (newSamples < 0){
                return {};
            }

            position += newSamples;
        } else {

            const auto samples{m_inputBuffer_.data() + position * d.m_numChannels_};
            const auto period{findPitchPeriod(samples, 1)};

            if (!period) {
                return {};
            }

            if(speed > 1.0) {
                newSamples = skipPitchPeriod(samples,speed,period);

                if (!newSamples){
                    return {};
                }

                position += period + newSamples;
            } else {
                newSamples = insertPitchPeriod( samples, speed, period);

                if (!newSamples) {
                    return {};
                }

                position += newSamples;
            }
        }

        if(!newSamples) {
            return {}; /* Failed to resize output buffer */
        }

    } while(position + maxRequired <= numSamples);

    removeInputSamples(position);

    return true;
}

bool SonicPrivate::processStreamInput() {

    const auto originalNumOutputSamples{d.m_numOutputSamples_};
    const auto speed{d.m_speed_ / d.m_pitch_};
    auto rate{d.m_rate_};

    if(!d.m_useChordPitch_) {
        rate *= d.m_pitch_;
    }

    if(speed > 1.00001f || speed < 0.99999f) {
        changeSpeed(speed);
    } else {
        if(!copyToOutput(m_inputBuffer_.data(),d.m_numInputSamples_)) {
            return {};
        }
        d.m_numInputSamples_ = 0;
    }

    if(d.m_useChordPitch_) {
        if(1.0f != d.m_pitch_) {
            if(!adjustPitch(originalNumOutputSamples)) {
                return {};
            }
        }
    } else if(1.0f != rate) {
        if(!adjustRate(rate,originalNumOutputSamples)) {
            return {};
        }
    }else{}

    if(1.0f != d.m_volume_) {
        /* Adjust output volume. */
        scaleSamples(m_outputBuffer_.data() + originalNumOutputSamples * d.m_numChannels_,
                     (d.m_numOutputSamples_ - originalNumOutputSamples) * d.m_numChannels_,
                     d.m_volume_);
    }

    return true;
}

void SonicPrivate::Move_(SonicPrivate *src) noexcept(true){
    m_inputBuffer_ = std::move(src->m_inputBuffer_);
    m_outputBuffer_ = std::move(src->m_outputBuffer_);
    m_pitchBuffer_ = std::move(src->m_pitchBuffer_);
    m_downSampleBuffer_ = std::move(src->m_downSampleBuffer_);
    d = src->d;
    src->d = {};
}
