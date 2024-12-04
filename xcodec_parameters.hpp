#ifndef XAVCODEC_PARAMETERS_HPP_
#define XAVCODEC_PARAMETERS_HPP_

extern "C"{
#include <libavcodec/codec_par.h>
}

#include "xhelper.hpp"

class XLIB_API XCodecParameters final : AVCodecParameters {
    static void Reset(AVCodecParameters *);
    void Move(XCodecParameters *) noexcept(true);
public:

    bool from_AVFormatContext(const AVCodecParameters *);
    bool to_AVCodecParameters(AVCodecParameters *) const;
    bool from_context(const AVCodecContext *) ;
    bool to_context(AVCodecContext *) const ;

    [[nodiscard]] auto MediaType() const{
        return codec_type;
    }

    [[nodiscard]] auto Codec_id() const{
        return codec_id;
    }

    [[nodiscard]] std::string Codec_name() const;

    [[nodiscard]] auto Ch_layout() const{
        return &ch_layout;
    }

    [[nodiscard]] auto Audio_sample_format() const{
        return format;
    }

    [[nodiscard]] auto Video_pixel_format() const{
        return format;
    }

    [[nodiscard]] auto Sample_Format_Size() const ->int;

    [[nodiscard]] auto Audio_nbSamples() const {
        return frame_size;
    }

    [[nodiscard]] auto Sample_rate() const{
        return sample_rate;
    }

    [[nodiscard]] auto Width() const{
        return width;
    }

    [[nodiscard]] auto Height() const{
        return height;
    }

    void set_time_base(const AVRational &tb) {
        m_time_base_ = tb;
    }

    void set_x_time_base(const XRational &tb) {
        m_x_time_base_ = {tb.num, tb.den};
    }

    [[nodiscard]] auto time_base() const{return m_time_base_;}
    [[nodiscard]] auto x_time_base() const{return m_x_time_base_;}

    void set_total_ms(const int64_t &total) {
        m_total_ms_ = total;
    }

    [[nodiscard]] auto total_ms() const {return m_total_ms_;}

    XCodecParameters();
    explicit XCodecParameters(const AVCodecContext *,
        const AVRational & = {1,1},
        const int64_t &total_ms = {}) ;

    explicit XCodecParameters(const AVCodecParameters *,
        const AVRational & = {1,1},
        const int64_t &total_ms = {}) ;

    XCodecParameters(const XCodecParameters &);

    XCodecParameters(XCodecParameters &&) noexcept(true);

    XCodecParameters& operator=(const XCodecParameters &);

    XCodecParameters& operator=(XCodecParameters &&) noexcept(true);

    ~XCodecParameters();
private:
    AVRational m_time_base_{1,1};
    XRational m_x_time_base_{1,1};
    int64_t m_total_ms_{};
};

XLIB_API XCodecParameters_sp new_XCodecParameters();

XLIB_API XCodecParameters_sp new_XCodecParameters(const AVCodecParameters *src,
    const AVRational &tb = {1,1},
    const int64_t &total_ms = {});

XLIB_API XCodecParameters_sp new_XCodecParameters(const AVCodecParameters *src,
    const XRational &tb = {1,1},
    const int64_t &total_ms = {});

XLIB_API XCodecParameters_sp new_XCodecParameters(const AVCodecContext *src,
    const AVRational &tb = {1,1},
    const int64_t &total_ms = {});

XLIB_API XCodecParameters_sp new_XCodecParameters(const AVCodecContext *src,
    const XRational &tb = {1,1},
    const int64_t &total_ms = {});

#endif

