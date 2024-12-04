#include "xaudio_triple_speed.hpp"

using namespace std;

bool Audio_Playback_Speed::Open(const int &sampleRate,const int &numChannels) {
    unique_lock locker(m_mux_);
    m_is_init_ = m_son_.Open(sampleRate,numChannels);
    return m_is_init_;
}

void Audio_Playback_Speed::Set_Speed(const double &speed) {

    if (!m_is_init_) {
        PRINT_ERR_TIPS(GET_STR(Uninitialized));
        return;
    }
    unique_lock locker(m_mux_);
    m_son_.set_speed(static_cast<float>(speed));
}

int Audio_Playback_Speed::SamplesAvailable() const {
    if (!m_is_init_) {
        PRINT_ERR_TIPS(GET_STR(Uninitialized));
        return -1;
    }
    unique_lock locker(const_cast<decltype(m_mux_) &>(m_mux_));
    return m_son_.samples_available();
}
