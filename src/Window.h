//
// Created by xiaozhuai on 2021/12/29.
//

#ifndef SOFT_RENDERER_WINDOW_H
#define SOFT_RENDERER_WINDOW_H

#include "Framebuffer.h"
#include "FpsMonitor.h"
#include "Signals.h"
#include "Log.h"
#include <string>
#include <utility>
#include <functional>
#include <cmath>
#include <MiniFB.h>

class Window {
public:
    Window(const std::string &title, int width, int height, int flags = 0)
            : m_framebuffer(width, height) {
        m_window = mfb_open_ex(title.c_str(), width, height, flags);
        mfb_set_active_callback(m_window, this, &Window::onActiveInternal);
        mfb_set_resize_callback(m_window, this, &Window::onResizeInternal);
        mfb_set_close_callback(m_window, this, &Window::onCloseInternal);
        mfb_set_keyboard_callback(m_window, this, &Window::onKeyboardInternal);
        mfb_set_char_input_callback(m_window, this, &Window::onCharInputInternal);
        mfb_set_mouse_button_callback(m_window, this, &Window::onMouseButtonInternal);
        mfb_set_mouse_move_callback(m_window, this, &Window::onMouseMoveInternal);
        mfb_set_mouse_scroll_callback(m_window, this, &Window::onMouseScrollInternal);
        m_fpsMonitor.setFpsCallback([&](int fps) {
            printf("FPS: %d\n", fps);
        });
    }

    inline void setHighDpi(bool enable) { m_highDpi = enable; }

    inline void catchSignals() {
#if defined(_WIN32)
        auto catchSignals = {SIGINT, SIGTERM};
#else
        auto catchSignals = {SIGINT, SIGTERM, SIGHUP, SIGQUIT};
#endif
        Signals::catchSome(catchSignals, [&]() {
            close();
        });
    }

    inline void exec() {
        m_fpsMonitor.restart();
        do {
            int fbWidth = getFramebufferWidth();
            int fbHeight = getFramebufferHeight();
            if (m_framebuffer.width() != fbWidth || m_framebuffer.height() != fbHeight) {
                m_framebuffer.resize(fbWidth, fbHeight);
            }

            m_onUpdateCallback(m_framebuffer);

            if (m_rgbaColorBuffer.size() != fbWidth * fbHeight * 4) {
                m_rgbaColorBuffer.resize(fbWidth * fbHeight * 4);
            }

            m_framebuffer.pixels(m_rgbaColorBuffer.data(), Framebuffer::BGRA8888);

            int state = mfb_update_ex(
                    m_window, m_rgbaColorBuffer.data(),
                    m_framebuffer.width(), m_framebuffer.height()
            );

            if (state < 0) {
                m_window = nullptr;
                break;
            }

            m_fpsMonitor.update();
        } while (mfb_wait_sync(m_window));
    }

    inline void close() {
        mfb_close(m_window);
        m_window = nullptr;
    }

    inline int getWidth() const {
        return mfb_get_window_width(m_window);
    }

    inline int getHeight() const {
        return mfb_get_window_height(m_window);
    }

    inline void getMonitorScale(float *scaleX, float *scaleY) const {
        mfb_get_monitor_scale(m_window, scaleX, scaleY);
    }

    inline int getFramebufferWidth() const {
        if (m_highDpi) {
            float scaleX, scaleY;
            getMonitorScale(&scaleX, &scaleY);
            return (int) std::round((float) getWidth() * scaleX);
        } else {
            return getWidth();
        }
    }

    inline int getFramebufferHeight() const {
        if (m_highDpi) {
            float scaleX, scaleY;
            getMonitorScale(&scaleX, &scaleY);
            return (int) std::round((float) getHeight() * scaleY);
        } else {
            return getHeight();
        }
    }

    static inline uint32_t getTargetFps() {
        return mfb_get_target_fps();
    }

    static inline void setTargetFps(uint32_t fps) {
        mfb_set_target_fps(fps);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    using OnUpdateCallback = std::function<void(Framebuffer &framebuffer)>;

    inline void onUpdate(OnUpdateCallback callback) { m_onUpdateCallback = std::move(callback); }

private:
    OnUpdateCallback m_onUpdateCallback;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    using OnActiveCallback = std::function<void(bool isActive)>;

    inline void onActive(OnActiveCallback callback) { m_onActiveCallback = std::move(callback); }

private:
    OnActiveCallback m_onActiveCallback;

    inline void onActiveInternal(struct mfb_window *window, bool isActive) {
        if (!m_onActiveCallback) return;
        m_onActiveCallback(isActive);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    using OnResizeCallback = std::function<void(int width, int height)>;

    inline void onResize(OnResizeCallback callback) { m_onResizeCallback = std::move(callback); }

private:
    OnResizeCallback m_onResizeCallback;

    inline void onResizeInternal(struct mfb_window *window, int width, int height) {
        if (!m_onResizeCallback) return;
        m_onResizeCallback(width, height);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    using OnCloseCallback = std::function<bool()>;

    inline void onClose(OnCloseCallback callback) { m_onCloseCallback = std::move(callback); }

private:
    OnCloseCallback m_onCloseCallback;

    inline bool onCloseInternal(struct mfb_window *window) {
        if (!m_onCloseCallback) return true;
        return m_onCloseCallback();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    using OnKeyboardCallback = std::function<void(mfb_key key, mfb_key_mod mod, bool isPressed)>;

    inline void onKeyboard(OnKeyboardCallback callback) { m_onKeyboardCallback = std::move(callback); }

private:
    OnKeyboardCallback m_onKeyboardCallback;

    inline void onKeyboardInternal(struct mfb_window *window, mfb_key key, mfb_key_mod mod, bool isPressed) {
        if (!m_onKeyboardCallback) return;
        m_onKeyboardCallback(key, mod, isPressed);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    using OnCharInputCallback = std::function<void(unsigned int code)>;

    inline void onCharInput(OnCharInputCallback callback) { m_onCharInputCallback = std::move(callback); }

private:
    OnCharInputCallback m_onCharInputCallback;

    inline void onCharInputInternal(struct mfb_window *window, unsigned int code) {
        if (!m_onCharInputCallback) return;
        m_onCharInputCallback(code);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    using OnMouseButtonCallback = std::function<void(mfb_mouse_button button, mfb_key_mod mod, bool isPressed)>;

    inline void onMouseButton(OnMouseButtonCallback callback) { m_onMouseButtonCallback = std::move(callback); }

private:
    OnMouseButtonCallback m_onMouseButtonCallback;

    inline void
    onMouseButtonInternal(struct mfb_window *window, mfb_mouse_button button, mfb_key_mod mod, bool isPressed) {
        if (!m_onMouseButtonCallback) return;
        m_onMouseButtonCallback(button, mod, isPressed);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    using OnMouseMoveCallback = std::function<void(int x, int y)>;

    inline void onMouseMove(OnMouseMoveCallback callback) { m_onMouseMoveCallback = std::move(callback); }

private:
    OnMouseMoveCallback m_onMouseMoveCallback;

    inline void onMouseMoveInternal(struct mfb_window *window, int x, int y) {
        if (!m_onMouseMoveCallback) return;
        m_onMouseMoveCallback(x, y);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    using OnMouseScrollCallback = std::function<void(mfb_key_mod mod, float deltaX, float deltaY)>;

    inline void onMouseScroll(OnMouseScrollCallback callback) { m_onMouseScrollCallback = std::move(callback); }

private:
    OnMouseScrollCallback m_onMouseScrollCallback;

    inline void onMouseScrollInternal(struct mfb_window *window, mfb_key_mod mod, float deltaX, float deltaY) {
        if (!m_onMouseScrollCallback) return;
        m_onMouseScrollCallback(mod, deltaX, deltaY);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    struct mfb_window *m_window = nullptr;
    bool m_highDpi = false;
    Framebuffer m_framebuffer;
    std::vector<uint8_t> m_rgbaColorBuffer;
    FpsMonitor m_fpsMonitor;

};


#endif //SOFT_RENDERER_WINDOW_H
