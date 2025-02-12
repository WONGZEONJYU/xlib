extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavcodec/codec_par.h>
}

#include "xformat.hpp"
#include "xavpacket.hpp"
#include "xcodec_parameters.hpp"

using namespace std;

void XFormat::destroy_fmt_ctx(){
    if (m_fmt_ctx_){
        if (m_fmt_ctx_->oformat){ //输出上下文
            if (m_fmt_ctx_->pb) {
                avio_closep(&m_fmt_ctx_->pb);
            }
            avformat_free_context(m_fmt_ctx_);
        } else if (m_fmt_ctx_->iformat){ //输入上下文
            avformat_close_input(&m_fmt_ctx_);
        } else{
            avformat_free_context(m_fmt_ctx_);
        }
        m_fmt_ctx_ = nullptr;
    }
}

void XFormat::set_fmt_ctx(AVFormatContext *ctx) {
    unique_lock locker(m_mux_);
    destroy_fmt_ctx();
    m_fmt_ctx_ = ctx;
    if (!m_fmt_ctx_){
        m_is_connected_ = false;
        return;
    }

    m_is_connected_ = true;
    m_last_time_ = XHelper::Get_time_ms();

    if (m_time_out_ms_ > 0){
        m_fmt_ctx_->interrupt_callback = {Time_out_callback,this};
    }

    m_audio_index_ = m_video_index_ = -1;
    m_video_timebase_ = {1,25};
    m_audio_timebase_ = {1,44100};
    for (int i {}; i < m_fmt_ctx_->nb_streams; ++i) {
        const auto stream{m_fmt_ctx_->streams[i]};
        const auto codec_par{stream->codecpar};
        if (const auto type{codec_par->codec_type}; AVMEDIA_TYPE_VIDEO == type){
            m_video_index_ = i;
            m_video_timebase_= {stream->time_base.num, stream->time_base.den};
            m_codec_id_ = codec_par->codec_id;
        } else if (AVMEDIA_TYPE_AUDIO == type){
            m_audio_index_ = i;
            m_audio_timebase_= {stream->time_base.num, stream->time_base.den};
        } else{}
    }
}

bool XFormat::CopyParm(const int &stream_index,AVCodecParameters *dst) const{
    if (!dst){
        LOG_ERROR(GET_STR(dst is empty));
        return {};
    }
    check_fmt_ctx();
    if (stream_index < 0 || stream_index >= m_fmt_ctx_->nb_streams){
        PRINT_ERR_TIPS(GET_STR(stream_index not in range));
        return {};
    }

    FF_ERR_OUT(avcodec_parameters_copy(dst,m_fmt_ctx_->streams[stream_index]->codecpar),return {});
    return true;
}

bool XFormat::CopyParm(const int &stream_index,AVCodecContext *dst) const{

    if (!dst) {
        LOG_ERROR(GET_STR(dst is empty));
        return {};
    }
    check_fmt_ctx();
    if (stream_index < 0 || stream_index >= m_fmt_ctx_->nb_streams){
        LOG_ERROR(GET_STR(stream_index not in range));
        return {};
    }

    FF_ERR_OUT(avcodec_parameters_to_context(dst,m_fmt_ctx_->streams[stream_index]->codecpar),return {});
    return true;
}

XCodecParameters_sp XFormat::CopyMediaParm(const int &stream_index) const {

    check_fmt_ctx();
    if (stream_index < 0 || stream_index >= m_fmt_ctx_->nb_streams){
        LOG_ERROR(GET_STR(stream_index not in range));
        return {};
    }

    return copy_parm_helper(stream_index);
}

XCodecParameters_sp XFormat::copy_parm_helper(const int &index) const {
    const auto st{m_fmt_ctx_->streams[index]};

    auto total_{m_fmt_ctx_->streams[index]->duration};

    total_ = XHelper::XRescale(total_,st->time_base,{1,1000});

    return new_XCodecParameters(st->codecpar,st->time_base,total_);
}

XCodecParameters_sp XFormat::CopyVideoParm() const{
    check_fmt_ctx();

    const auto index{m_video_index_.load(memory_order_relaxed)};
    if (index < 0){
        LOG_ERROR(GET_STR(no video!));
        return {};
    }

    return copy_parm_helper(index);
}

XCodecParameters_sp XFormat::CopyAudioParm() const{
    check_fmt_ctx();
    const auto index{m_audio_index_.load(memory_order_relaxed)};
    if (index < 0){
        LOG_ERROR(GET_STR(no audio!));
        return {};
    }

    return copy_parm_helper(index);
}

Media_Parameters XFormat::CopyAllMediaParm() const {

    check_fmt_ctx();
    const auto nb_streams{m_fmt_ctx_->nb_streams};
    Media_Parameters list;
    for (int i{}; i < nb_streams; ++i) {
        list.emplace_back(copy_parm_helper(i));
    }
    return list;
}

bool XFormat::RescaleTime(XAVPacket &packet,
                          const int64_t &offset_pts,
                          const AVRational &time_base) const{

    if (!packet.data){
        return {};
    }

    check_fmt_ctx();

    const auto out_stream{m_fmt_ctx_->streams[packet.stream_index]};

    packet.pts = av_rescale_q_rnd(packet.pts - offset_pts,time_base,
                                  out_stream->time_base,
                                  static_cast<AVRounding>(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));

    packet.dts = av_rescale_q_rnd(packet.dts - offset_pts,time_base,
                                  out_stream->time_base,
                                  static_cast<AVRounding>(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));

    packet.duration = av_rescale_q(packet.duration,time_base,out_stream->time_base);
    packet.pos = -1;
    return true;
}

bool XFormat::RescaleTime(XAVPacket &packet, const int64_t &offset_pts, const XRational &time_base) const{
    return RescaleTime(packet,offset_pts,AVRational{time_base.num,time_base.den});
}

int64_t XFormat::RescaleToMs(const int64_t &pts,const int &index) const{

    check_fmt_ctx();
    if (index < 0 || index >= m_fmt_ctx_->nb_streams) {
        PRINT_ERR_TIPS(GET_STR(index not in range));
        return -1;
    }
    const auto in_tb{m_fmt_ctx_->streams[index]->time_base};
    locker.unlock();
    return av_rescale_q(pts,in_tb,{1,1000});
}

void XFormat::destroy_() {
    unique_lock locker(m_mux_);
    destroy_fmt_ctx();
}

XFormat::~XFormat() {
    destroy_();
}

int XFormat::Time_out_callback(void *const arg) {
    const auto this_{static_cast<XFormat*>(arg)};
    return this_->IsTimeout();
}

bool XFormat::set_timeout_ms(const uint64_t &ms) {
    m_time_out_ms_ = ms;
    check_fmt_ctx();
    m_fmt_ctx_->interrupt_callback = {Time_out_callback,this};
    return true;
}

void XFormat::Clear() {
    check_fmt_ctx_no_ret();
    avformat_flush(m_fmt_ctx_);
}

bool XFormat::IsTimeout() {
    if (const auto curr_time{XHelper::Get_time_ms()};
            curr_time - m_last_time_ > m_time_out_ms_) {
        m_is_connected_ = false;
        m_last_time_ = curr_time;
        return true;
    }
    return {};
}
