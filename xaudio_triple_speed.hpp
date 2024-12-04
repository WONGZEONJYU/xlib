#ifndef XAUDIOTRIPLESPEED_HPP
#define XAUDIOTRIPLESPEED_HPP

#include "xsonic.hpp"

class XLIB_API Audio_Playback_Speed {

X_DISABLE_COPY_MOVE(Audio_Playback_Speed)

public:
    explicit Audio_Playback_Speed() = default;
    bool Open(const int &sampleRate,const int &numChannels);
    void Set_Speed(const double &speed);

    [[nodiscard]] int SamplesAvailable() const;

    template<typename T>
    inline bool Send(const T *src,const int &samples) {

        if (!m_is_init_) {
            PRINT_ERR_TIPS(GET_STR(Uninitialized));
            return {};
        }

        std::unique_lock locker(m_mux_);

        if constexpr (std::is_same_v<T,float>) {
            return m_son_.WriteFloatToStream(src,samples);
        }else if constexpr (std::is_same_v<T,short>) {
            return m_son_.WriteShortToStream(src,samples);
        }else if constexpr (std::is_same_v<T,uint8_t>) {
            return m_son_.WriteUnsignedCharToStream(src,samples);
        } else if constexpr (std::is_same_v<T,int8_t>){
            return m_son_.WriteCharToStream(src,samples);
        }else if constexpr (std::is_same_v<T,uint16_t>) {
            return m_son_.WriteUnsignedShortToStream(src,samples);
        }else if constexpr (std::is_same_v<T,uint32_t>) {
            return m_son_.WriteU32ToStream(src,samples);
        }else if constexpr (std::is_same_v<T,int32_t>) {
            return m_son_.WriteS32ToStream(src,samples);
        }else if constexpr (std::is_same_v<T,uint64_t>) {
            return m_son_.WriteU64ToStream(src,samples);
        }else if constexpr (std::is_same_v<T,int64_t>) {
            return m_son_.WriteS64ToStream(src,samples);
        }else if constexpr (std::is_same_v<T,double>) {
            return m_son_.WriteDoubleToStream(src,samples);
        }else {
            static_assert(false,GET_STR("not support type"));
        }
        return {};
    }

    template<typename T>
    inline int Receive(T *dst,const int &samples) {

        if (!m_is_init_) {
            PRINT_ERR_TIPS(GET_STR(Uninitialized));
            return -1;
        }

        std::unique_lock locker(m_mux_);

        if constexpr (std::is_same_v<T,float>) {
            return m_son_.ReadFloatFromStream(dst,samples);
        }else if constexpr (std::is_same_v<T,short>) {
            return m_son_.ReadShortFromStream(dst,samples);
        }else if constexpr (std::is_same_v<T,uint8_t>) {
            return m_son_.ReadUnsignedCharFromStream(dst,samples);
        }else if constexpr (std::is_same_v<T,int8_t>) {
            return m_son_.ReadSignedCharFromStream(dst,samples);
        }else if constexpr (std::is_same_v<T,uint16_t>) {
            return m_son_.ReadUnsignedShortFromStream(dst,samples);
        }else if constexpr (std::is_same_v<T,uint32_t>) {
            return m_son_.ReadU32FromStream(dst,samples);
        }else if constexpr (std::is_same_v<T,int32_t>) {
            return m_son_.ReadS32FromStream(dst,samples);
        }else if constexpr (std::is_same_v<T,uint64_t>) {
            return m_son_.ReadU64FromStream(dst,samples);
        }else if constexpr (std::is_same_v<T,int64_t>) {
            return m_son_.ReadS64FromStream(dst,samples);
        }else if constexpr (std::is_same_v<T,double>) {
            return m_son_.ReadDoubleFromStream(dst,samples);
        }else {
            static_assert(false,GET_STR("not support type"));
            return -1;
        }
    }

    [[nodiscard]] auto get_channels() const {
        std::unique_lock locker(const_cast<decltype(m_mux_) &>(m_mux_));
        return m_son_.channels();
    }

    [[nodiscard]] auto get_samples_rate() const {
        std::unique_lock locker(const_cast<decltype(m_mux_) &>(m_mux_));
        return m_son_.sample_rate();
    }

    inline explicit operator bool() const {
        return m_is_init_;
    }

    inline auto operator !() const {
        return !m_is_init_;
    }

    inline bool is_open() const {
        return m_is_init_;
    }

    [[maybe_unused]] bool FlushStream(){
        std::unique_lock locker(m_mux_);
        return m_son_.FlushStream();
    }

private:
    std::mutex m_mux_;
    XSonic m_son_;
    std::atomic_bool m_is_init_{};
};

#endif