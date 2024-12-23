#ifndef XFORMAT_HPP_
#define XFORMAT_HPP_

#include "xhelper.hpp"

using Media_Parameters = std::list<XCodecParameters_sp>;

class XLIB_API XFormat {

    static int Time_out_callback(void *);
    /**
     * 检查是否超时
     * @return true or false
     */
    [[nodiscard]] bool IsTimeout();
    void destroy_fmt_ctx();
    void destroy_();

    XCodecParameters_sp copy_parm_helper(const int &) const;

public:
    /**
     * 设置AVFormatContext,线程安全,ctx传nullptr,代表要销毁当前上下文
     * @param ctx
     */
    void set_fmt_ctx(AVFormatContext *ctx);

    /**
     * 拷贝编码参数,线程安全
     * @param stream_index 流编号
     * @param dst 目标位置
     * @return ture or false
     */
    bool CopyParm(const int &stream_index,AVCodecParameters *dst) const;

    bool CopyParm(const int &stream_index,AVCodecContext *dst) const;

    /**
     * 获取媒体参数
     * @param stream_index
     * @return MediaParm
     */
    XCodecParameters_sp CopyMediaParm(const int &stream_index) const;

    XCodecParameters_sp CopyVideoParm() const;

    XCodecParameters_sp CopyAudioParm() const;

    Media_Parameters CopyAllMediaParm() const;

    /**
     * 获取视频流index
     * @return index
     */
    [[nodiscard]] auto video_index() const{return m_video_index_.load();}

    /**
     * 获取音频流index
     * @return index
     */
    [[nodiscard]] auto audio_index() const{return m_audio_index_.load();}

    /**
     * 获取视频流时间基准
     * @return XRational
     */
    [[maybe_unused]] [[nodiscard]] auto video_timebase() const{return m_video_timebase_;}

    /**
     * 获取音频流时间基准
     * @return XRational
     */
    [[maybe_unused]] [[nodiscard]] auto audio_timebase() const{return m_audio_timebase_;}

    /**
     *重新计算pts dst duration
     * @param packet
     * @param offset_pts
     * @param time_base
     * @return ture or false
     */
    bool RescaleTime(XAVPacket &packet,const int64_t &offset_pts,const AVRational &time_base) const;
    bool RescaleTime(XAVPacket &packet,const int64_t &offset_pts,const XRational &time_base) const;

    /**
     * 把pts dst duration转换成毫秒
     * @param pts
     * @param index
     * @return -1 at error
     */
    int64_t RescaleToMs(const int64_t &pts,const int &index) const;

    /**
     * 获取编码/解码器id
     * @return
     */
    [[nodiscard]] [[maybe_unused]] auto codec_id() const{
        return m_codec_id_.load(std::memory_order_relaxed);
    }

    /**
     * 设置IO超时时间
     * @param ms
     * @return true or false
     */
    bool set_timeout_ms(const uint64_t &ms = 1000);

    /**
     * 是否断开重连
     * @return
     */
    [[nodiscard]] auto is_connected() const {return m_is_connected_.load();}

    void Clear();

protected:
    std::mutex m_mux_;
    AVFormatContext *m_fmt_ctx_{};
    std::atomic_bool m_is_connected_{};
    std::atomic_int m_audio_index_{-1},
        m_video_index_{-1},
        m_codec_id_{};

    std::atomic_uint64_t m_time_out_ms_{},
        m_last_time_{};

    XRational m_video_timebase_{1,25},
                m_audio_timebase_{1,44100};

protected:
    explicit XFormat() = default;
    virtual ~XFormat();
    X_DISABLE_COPY_MOVE(XFormat)

#define check_fmt_ctx() \
std::unique_lock locker(const_cast<decltype(m_mux_)&>(m_mux_));do{\
if(!m_fmt_ctx_) { \
PRINT_ERR_TIPS(GET_STR(format ctx is empty)); \
return {};}}while(false)

#define check_fmt_ctx_no_ret() \
std::unique_lock locker(const_cast<decltype(m_mux_)&>(m_mux_));do{\
if(!m_fmt_ctx_) { \
PRINT_ERR_TIPS(GET_STR(format ctx is empty)); \
return;}}while(false)

};

#endif
