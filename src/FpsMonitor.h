//
// Created by xiaozhuai on 2021/12/29.
//

#ifndef SOFT_RENDERER_FPSCOUNTER_H
#define SOFT_RENDERER_FPSCOUNTER_H

#include <functional>
#include <chrono>
#include <utility>
#include <cmath>

typedef std::function<void(int fps)> FpsCallbackFn;

class FpsMonitor {
public:
    explicit FpsMonitor(int interval = 500) : m_start(0), m_count(0), m_fps(0), m_interval(interval) {
        restart();
    }

    inline void restart() {
        m_start = now();
    }

    inline void update() {
        m_count++;
        int64_t end = now();
        int64_t duration = end - m_start;
        if (duration > m_interval) {
            m_fps = (int) ceil((double) m_count / ((double) duration / 1000));
            m_start = end;
            m_count = 0;
            if (m_fpsCallback) {
                m_fpsCallback(m_fps);
            }
        }
    }

    inline void setFpsCallback(FpsCallbackFn callback) {
        m_fpsCallback = std::move(callback);
    }

    inline int fps() const {
        return m_fps;
    }


private:
    static inline int64_t now() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    int64_t m_start;
    int m_count;
    int m_fps;
    FpsCallbackFn m_fpsCallback;
    int m_interval;
};

#endif //SOFT_RENDERER_FPSCOUNTER_H
