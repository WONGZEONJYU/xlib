#ifndef SONIC_PRIVATE_HPP
#define SONIC_PRIVATE_HPP

#include "xhelper.hpp"
#include <functional>

class SonicPrivate {

protected:
    explicit SonicPrivate() = default;

    struct {
        float m_speed_{},m_volume_{},m_pitch_{},m_rate_{},m_avePower_{};
        int m_oldRatePosition_{},m_newRatePosition_{},m_useChordPitch_{},m_quality_{},
                m_numChannels_{},m_inputBufferSize_{},m_pitchBufferSize_{},m_outputBufferSize_{},
                m_numInputSamples_{},m_numOutputSamples_{},m_numPitchSamples_{},m_minPeriod_{},
                m_maxPeriod_{},m_maxRequired_{},m_remainingInputToCopy_{},m_sampleRate_{},
                m_prevPeriod_{},m_prevMinDiff_{};
        bool m_is_init_{};
    }d;

    std::vector<int16_t> m_inputBuffer_,m_outputBuffer_,
                        m_pitchBuffer_,m_downSampleBuffer_;

    using Middle_Logic_type = std::function<void(int16_t * ,const int &n)>;
    bool WriteToStream_helper(const void *,const int &,Middle_Logic_type &&);
    int ReadFromStream_helper(const void *,const int&,Middle_Logic_type &&);

    static void scaleSamples(int16_t * ,
                             const int&,
                             const float &);

    bool allocateStreamBuffers(const int &sampleRate,
                               const int &numChannels);

    bool sonicCreateStream(const int &sampleRate,
                           const int &numChannels);

    bool enlargeOutputBufferIfNeeded(const int &numSamples);

    bool enlargeInputBufferIfNeeded(const int &numSamples);
#if 0
    bool addDoubleSamplesToInputBuffer(const double *samples,
                                      const int &numSamples);

    bool addS64SamplesToInputBuffer(const int64_t *samples,
                                      const int &numSamples);

    bool addU64SamplesToInputBuffer(const uint64_t *samples,
                                  const int &numSamples);

    bool addFloatSamplesToInputBuffer(const float *samples,
                                      const int &numSamples);

    bool addS32SamplesToInputBuffer(const int32_t *samples,
                                    const int &numSamples);

    bool addU32SamplesToInputBuffer(const uint32_t *samples,
                                    const int &numSamples);

    bool addShortSamplesToInputBuffer(const int16_t *samples,
                                      const int &numSamples);

    bool addUnsignedShortSamplesToInputBuffer(const uint16_t *samples,
                                            const int &numSamples);

    bool addCharSamplesToInputBuffer(const int8_t *samples,
                                    const int &numSamples);

    bool addUnsignedCharSamplesToInputBuffer(const uint8_t *samples,
                                             const int &numSamples);
#endif
    void removeInputSamples(const int &position);

    bool copyToOutput(const int16_t *samples,
                      const int &numSamples);

    int copyInputToOutput(const int &position);

    void downSampleInput(const int16_t *samples,
                         const int &skip);

    static int findPitchPeriodInRange(const int16_t *samples,
                                      const int &minPeriod,
                                      const int &maxPeriod,
                                      int &retMinDiff,
                                      int &retMaxDiff);

    [[nodiscard]] bool prevPeriodBetter(const int &minDiff,
                                        const int &maxDiff,
                                        const int &preferNewPeriod) const;

    int findPitchPeriod(const int16_t *samples,const int &preferNewPeriod);

    static void overlapAdd(const int &numSamples,
                           const int &numChannels,
                           int16_t *out,
                           const int16_t *rampDown,
                           const int16_t *rampUp);

    static void overlapAddWithSeparation(const int &numSamples,
                                         const int &numChannels,
                                         const int &separation,
                                         int16_t *out,
                                         const int16_t *rampDown,
                                         const int16_t *rampUp);

    bool moveNewSamplesToPitchBuffer(const int &originalNumOutputSamples);

    void removePitchSamples(const int &numSamples);

    bool adjustPitch(const int &originalNumOutputSamples);

    static int findSincCoefficient(const int &i,
                                   const int &ratio,
                                   const int &width);

    int16_t interpolate(const int16_t *in,
                        const int &oldSampleRate,
                        const int &newSampleRate) const;

    bool adjustRate(const float &rate,
                    const int &originalNumOutputSamples);

    int skipPitchPeriod( const int16_t *samples,
                         const float &speed,
                         const int &period);

    int insertPitchPeriod(const int16_t *samples,
                          const float &speed,
                          const int &period);

    bool changeSpeed(const float &speed);

    bool processStreamInput();

    void Move_(SonicPrivate *) noexcept(true);

public:
    void Close();
    virtual ~SonicPrivate() = default;
};

#endif
