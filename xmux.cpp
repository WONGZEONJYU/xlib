#include "xmux.hpp"
extern "C"{
#include <libavformat/avformat.h>
}
#include "xavpacket.hpp"
#include "xcodec_parameters.hpp"

using namespace std;

AVFormatContext *XMux::Open(const string &url,
                            const XCodecParameters& video_parm,
                            const XCodecParameters& audio_parm) {

    CHECK_FALSE_(!url.empty(),PRINT_ERR_TIPS(GET_STR(url is empty!));
    return {});

    bool need_free{};
    AVFormatContext *c{};

    const Destroyer d([&c,&need_free]{
        if (need_free){
            avformat_free_context(c);
        }
    });

    FF_ERR_OUT(avformat_alloc_output_context2(&c,nullptr,nullptr,url.c_str()),
               return {});

    if (video_parm.Video_pixel_format() >= 0) {
        const AVStream *vs{};
        IS_NULLPTR(vs = avformat_new_stream(c, nullptr),need_free = true;return {};);
        video_parm.to_AVCodecParameters(vs->codecpar);
    }

    if (audio_parm.Audio_sample_format() >= 0) {
        const AVStream *as{};
        IS_NULLPTR(as = avformat_new_stream(c, nullptr),need_free = true;return {});
        audio_parm.to_AVCodecParameters(as->codecpar);
    }

    FF_ERR_OUT(avio_open(&c->pb,url.c_str(),AVIO_FLAG_WRITE),
               need_free = true;return {});

    av_dump_format(c,0,url.c_str(),1);
    return c;
}

AVFormatContext *XMux::Open(const string &url,
                            const XCodecParameters* video_parm,
                            const XCodecParameters* audio_parm) {

    return Open(url,video_parm ? *video_parm : XCodecParameters(),
        audio_parm ? *audio_parm : XCodecParameters());
}

void XMux::set_video_time_base(const AVRational &tb){
    set_video_time_base(XRational{tb.num,tb.den});
}

void XMux::set_video_time_base(const XRational &tb) {
    unique_lock locker(m_mux_);
    m_src_video_time_base_ = tb;
}

void XMux::set_audio_time_base(const AVRational &tb){
    set_audio_time_base(XRational{tb.num,tb.den});
}

void XMux::set_audio_time_base(const XRational &tb) {
    unique_lock locker(m_mux_);
    m_src_audio_time_base_ = tb;
}

bool XMux::WriteHead(){
    check_fmt_ctx();
    FF_ERR_OUT(avformat_write_header(m_fmt_ctx_,nullptr),return {});
    av_dump_format(m_fmt_ctx_,0, m_fmt_ctx_->url,1);
    m_src_begin_video_pts_ = m_src_begin_audio_pts_ = -1;
    return true;
}

bool XMux::Write(XAVPacket &packet){
    if (!packet.data){
        return {};
    }

    check_fmt_ctx();

    //没读取到pts 重构考虑通过duration计算
    if (AV_NOPTS_VALUE == packet.pts){
        packet.pts = 0;
        packet.dts = 0;
    }

    if (m_video_index_ == packet.stream_index){
        if (m_src_begin_video_pts_ < 0){
            m_src_begin_video_pts_ = packet.pts;
        }

        const XRAII r([&] {
            locker.unlock();
        },[&] {
            locker.lock();
        });

        RescaleTime(packet,m_src_begin_video_pts_,m_src_video_time_base_);

    } else if (m_audio_index_ == packet.stream_index){
        if (m_src_begin_audio_pts_ < 0){
            m_src_begin_audio_pts_ = packet.pts;
        }

        const XRAII r([&] {
            locker.unlock();
        },[&] {
            locker.lock();
        });

        RescaleTime(packet,m_src_begin_audio_pts_,m_src_audio_time_base_);

    }else{}

    cout << "Write pts = " << packet.pts << flush;
    FF_ERR_OUT(av_interleaved_write_frame(m_fmt_ctx_,&packet),return {});
    return true;
}

bool XMux::Write(XAVPacket *packet) {
    if (!packet) {
        return {};
    }
    return Write(*packet);
}

bool XMux::WriteEnd(){
    check_fmt_ctx();
    FF_ERR_OUT(av_interleaved_write_frame(m_fmt_ctx_, nullptr)); //写入缓冲
    FF_ERR_OUT(av_write_trailer(m_fmt_ctx_),return {});
    m_src_begin_video_pts_ = m_src_begin_audio_pts_ = -1;
    return true;
}
