#ifndef SONIC_HPP
#define SONIC_HPP

#include "xsonicprivate.hpp"

class XLIB_API XSonic final: SonicPrivate {
    X_DISABLE_COPY(XSonic)
public:
    bool Open(const int &sampleRate,const int &Channels);

    bool WriteDoubleToStream(const double *samples,const int &numSamples);

    /* Use this to write floating point data to be speed up or down into the stream.
   Values must be between -1 and 1.  Return 0 if memory realloc failed, otherwise 1 */
    bool WriteFloatToStream(const float *samples,const int &numSamples);
/* Use this to write 16-bit data to be speed up or down into the stream.
   Return 0 if memory realloc failed, otherwise 1 */

    bool WriteU64ToStream(const uint64_t *samples,const int &numSamples);

    bool WriteS64ToStream(const int64_t *samples,const int &numSamples);

    bool WriteS32ToStream(const int32_t *samples,const int &numSamples);

    bool WriteU32ToStream(const uint32_t *samples,const int &numSamples);

    bool WriteShortToStream(const int16_t *samples,const int &numSamples);
/* Use this to write 8-bit unsigned data to be speed up or down into the stream.
   Return 0 if memory realloc failed, otherwise 1 */

    bool WriteUnsignedShortToStream(const uint16_t *samples,const int &numSamples);

    bool WriteCharToStream(const int8_t *samples,const int &numSamples);

    bool WriteUnsignedCharToStream(const uint8_t *samples,const int &numSamples);

    int ReadDoubleFromStream(double *samples,const int &maxSamples);

    int ReadS64FromStream(int64_t *samples,const int &maxSamples);

    int ReadU64FromStream(uint64_t *samples,const int &maxSamples);

/* Use this to read floating point data out of the stream.  Sometimes no data
   will be available, and zero is returned, which is not an error condition. */
    int ReadFloatFromStream(float *samples,const int &maxSamples);

    int ReadU32FromStream(uint32_t *samples,const int &maxSamples);

    int ReadS32FromStream(int32_t *samples,const int &maxSamples);

/* Use this to read 16-bit data out of the stream.  Sometimes no data will
   be available, and zero is returned, which is not an error condition. */
    int ReadShortFromStream(int16_t *samples,const int &maxSamples);

    int ReadUnsignedShortFromStream(uint16_t *samples,const int &maxSamples);

/* Use this to read 8-bit unsigned data out of the stream.  Sometimes no data will
   be available, and zero is returned, which is not an error condition. */
    int ReadUnsignedCharFromStream(uint8_t *samples,const int &maxSamples);

    int ReadSignedCharFromStream(int8_t *samples,const int &maxSamples);

/* Force the sonic stream to generate output using whatever data it currently
   has.  No extra delay will be added to the output, but flushing in the middle of
   words could introduce distortion. */
    bool FlushStream();
/* Return the number of samples in the output buffer */
    [[nodiscard]] inline int samples_available() const {
        return d.m_numOutputSamples_;
    }
/* Get the speed of the stream. */
    [[nodiscard]] [[maybe_unused]] inline auto speed() const {
        return d.m_speed_;
    }
/* Set the speed of the stream. */
    [[maybe_unused]] inline void set_speed(const float &speed) {
        d.m_speed_ = speed;
    }
/* Get the pitch of the stream. */
    [[nodiscard]] inline auto pitch() const {
        return d.m_pitch_;
    }
/* Set the pitch of the stream. */
    inline void set_pitch(const float &pitch) {
        d.m_pitch_ = pitch;
    }
/* Get the rate of the stream. */
    [[nodiscard]] inline auto rate() const {
        return d.m_rate_;
    }
/* Set the rate of the stream. */
    inline void set_rate(const float &rate) {
        d.m_rate_ = rate;
        d.m_oldRatePosition_ = d.m_newRatePosition_ = {};
    }
/* Get the scaling factor of the stream. */
    [[nodiscard]] inline auto volume() const {
        return d.m_volume_;
    }
/* Set the scaling factor of the stream. */
    inline void set_volume(const float &volume) {
        d.m_volume_ = volume;
    }
/* Get the chord pitch setting. */
    [[nodiscard]] inline int chord_pitch() const {
        return d.m_useChordPitch_;
    }
/* Set chord pitch mode on or off.  Default is off.  See the documentation
   page for a description of this feature. */
    inline void set_chord_pitch(const int &useChordPitch) {
        d.m_useChordPitch_ = useChordPitch;
    }
/* Get the quality setting. */
    [[nodiscard]] auto quality() const {
        return d.m_quality_;
    }
/* Set the "quality".  Default 0 is virtually as good as 1, but very much faster. */
    inline void set_quality(const int &quality) {
        d.m_quality_ = quality;
    }
/* Get the sample rate of the stream. */
    [[nodiscard]] inline auto sample_rate() const {
        return d.m_sampleRate_;
    }
/* Get the number of channels. */
    [[nodiscard]] inline auto channels() const {
        return d.m_numChannels_;
    }

    explicit XSonic() = default;
    explicit XSonic(const int &sampleRate,const int &numChannels);
    XSonic(XSonic &&) noexcept(true);
    XSonic &operator=(XSonic &&) noexcept(true);
    ~XSonic() override = default;
};

/* This is a non-stream oriented interface to just change the speed of a sound
   sample.  It works in-place on the sample array, so there must be at least
   speed*numSamples available space in the array. Returns the new number of samples. */
XLIB_API int sonicChangeFloatSpeed(float *samples,
                          const int &numSamples,
                          const float &speed = 1.0f,
                          const float &pitch = 1.0f,
                          const float &rate = 1.0f,
                          const float &volume = 1.0f,
                          const int &useChordPitch = 0,
                          const int &sampleRate = 44100,
                          const int &numChannels = 2);

/* This is a non-stream oriented interface to just change the speed of a sound
   sample.  It works in-place on the sample array, so there must be at least
   speed*numSamples available space in the array. Returns the new number of samples. */
XLIB_API int sonicChangeShortSpeed(int16_t *samples,
                          const int &numSamples,
                          const float &speed = 1.0f,
                          const float &pitch = 1.0f,
                          const float &rate = 1.0f,
                          const float &volume = 1.0f,
                          const int &useChordPitch = 0,
                          const int &sampleRate = 44100,
                          const int &numChannels = 2);
#endif
