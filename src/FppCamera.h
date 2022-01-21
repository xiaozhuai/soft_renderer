//
// Created by xiaozhuai on 2020/5/15.
//

#ifndef SOFT_RENDERER_FPPCAMERA_H
#define SOFT_RENDERER_FPPCAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include "Window.h"

#ifndef clamp
#define clamp(v, min, max) ((v) < (min) ? (min) : ((v) > (max) ? (max) : (v)))
#endif


class FppCamera {
public:
    enum class MoveDirection {
        None = 0,
        Forward = 1,
        Backward = 2,
        Left = 3,
        Right = 4,
        Up = 5,
        Down = 6,
    };
public:
    FppCamera()
            : m_fov(45.0f),
              m_minFov(10.0f),
              m_maxFov(80.0f),
              m_aspect(4.0f / 3.0f),
              m_near(0.1f),
              m_far(1000.0f),
              m_position(0.0f, 0.0f, 3.0f),
              m_upVec(0.0f, 1.0f, 0.0f),
              m_front(0.0f, 0.0f, -1.0f),
              m_pitch(0.0f),
              m_yaw(-90.0f),
              m_mouseLastX(0.0),
              m_mouseLastY(0.0),
              m_firstMouseMove(true),
              m_moveDirection(MoveDirection::None),
              m_moveLastTime(0),
              m_moveSpeed(0.006f),
              m_rotateSensitivity(0.05f),
              m_scaleFactor(1.0f) {
        calcFront();
    };

    inline float getFov() const {
        return m_fov;
    }

    inline void setFov(float fov) {
        m_fov = fov;
    }

    float getMinFov() const {
        return m_minFov;
    }

    void setMinFov(float minFov) {
        m_minFov = minFov;
    }

    float getMaxFov() const {
        return m_maxFov;
    }

    void setMaxFov(float maxFov) {
        m_maxFov = maxFov;
    }

    inline float getAspect() const {
        return m_aspect;
    }

    inline void setAspect(float aspect) {
        m_aspect = aspect;
    }

    inline float getNear() const {
        return m_near;
    }

    inline void setNear(float near) {
        m_near = near;
    }

    inline float getFar() const {
        return m_far;
    }

    inline void setFar(float far) {
        m_far = far;
    }

    inline glm::mat4 getProjection() const {
        return glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
    }

    inline const glm::vec3 &getPosition() const {
        return m_position;
    }

    inline void setPosition(const glm::vec3 &position) {
        m_position = position;
    }

    inline const glm::vec3 &getUpVec() const {
        return m_upVec;
    }

    inline void setUpVec(const glm::vec3 &upVec) {
        m_upVec = glm::normalize(upVec);
    }

    inline const glm::vec3 &getFront() const {
        return m_front;
    }

    inline float getPitch() const {
        return m_pitch;
    }

    inline void setPitch(float pitch) {
        calcFront();
        m_pitch = pitch;
    }

    inline float getYaw() const {
        return m_yaw;
    }

    inline void setYaw(float yaw) {
        calcFront();
        m_yaw = yaw;
    }

    inline glm::mat4 getView() const {
        return glm::lookAt(m_position, m_position + m_front, m_upVec);
    }

    float getMoveSpeed() const {
        return m_moveSpeed;
    }

    void setMoveSpeed(float moveSpeed) {
        m_moveSpeed = moveSpeed;
    }

    float getRotateSensitivity() const {
        return m_rotateSensitivity;
    }

    void setRotateSensitivity(float rotateSensitivity) {
        m_rotateSensitivity = rotateSensitivity;
    }

    float getScaleFactor() const {
        return m_scaleFactor;
    }

    void setScaleFactor(float scaleFactor) {
        m_scaleFactor = scaleFactor;
    }

public:

    inline void processMouseScroll(double deltaX, double deltaY) {
        m_fov -= (float) deltaY * m_scaleFactor;
        m_fov = clamp(m_fov, m_minFov, m_maxFov);
    }

    inline void processMouseMove(double x, double y) {
        if (m_firstMouseMove) {
            m_mouseLastX = x;
            m_mouseLastY = y;
            m_firstMouseMove = false;
        }
        double deltaX = x - m_mouseLastX;
        double deltaY = m_mouseLastY - y;
        m_mouseLastX = x;
        m_mouseLastY = y;
        deltaX *= m_rotateSensitivity;
        deltaY *= m_rotateSensitivity;
        m_yaw += (float) deltaX;
        m_pitch += (float) deltaY;
        m_pitch = clamp(m_pitch, -89.99f, 89.99f);
        calcFront();
    }

    inline void processMove(MoveDirection direction, int64_t deltaTimeMs) {
        glm::vec3 frontXZ = glm::normalize(m_front - m_front * m_upVec);
        float distance = m_moveSpeed * (float) deltaTimeMs;
        switch (direction) {
            case MoveDirection::Forward:
                m_position += distance * frontXZ;
                break;
            case MoveDirection::Backward:
                m_position -= distance * frontXZ;
                break;
            case MoveDirection::Left:
                m_position -= distance * glm::normalize(glm::cross(frontXZ, m_upVec));
                break;
            case MoveDirection::Right:
                m_position += distance * glm::normalize(glm::cross(frontXZ, m_upVec));
                break;
            case MoveDirection::Up:
                m_position += distance * m_upVec;
                break;
            case MoveDirection::Down:
                m_position -= distance * m_upVec;
                break;
            default:
                break;
        }

//        float distance = m_moveSpeed * (float) deltaTimeMs;
//        switch (direction) {
//            case MoveDirection::Forward:
//                m_position += distance * m_front;
//                break;
//            case MoveDirection::Backward:
//                m_position -= distance * m_front;
//                break;
//            case MoveDirection::Left:
//                m_position -= distance * glm::normalize(glm::cross(m_front, m_upVec));
//                break;
//            case MoveDirection::Right:
//                m_position += distance * glm::normalize(glm::cross(m_front, m_upVec));
//                break;
//            default:
//                break;
//        }
    }

private:
    inline void calcFront() {
        m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front.y = sin(glm::radians(m_pitch));
        m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(m_front);
    }

private:
    float m_fov;
    float m_minFov;
    float m_maxFov;
    float m_aspect;
    float m_near;
    float m_far;

private:
    glm::vec3 m_position;
    glm::vec3 m_upVec;
    glm::vec3 m_front;
    float m_pitch;
    float m_yaw;

private:
    // rotate
    double m_mouseLastX;
    double m_mouseLastY;
    bool m_firstMouseMove;

private:
    // move
    MoveDirection m_moveDirection;
    int64_t m_moveLastTime;

private:
    // params
    float m_moveSpeed;
    float m_rotateSensitivity;
    float m_scaleFactor;
};



#endif //SOFT_RENDERER_FPPCAMERA_H
