#include "Input/KeyHandler.h"

#include "Application/Application.h"
#include "Application/GlobalSettings.h"
#include "Utilities/Logger.h"

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Yare {

    KeyHandler::KeyHandler(std::shared_ptr<Graphics::Camera> currentCamera) : p_Camera(currentCamera) {
        for (int i = 0; i < MAX_KEYS; i++) {
            m_KeyState[i] = 0;
        }
        for (int i = 0; i < MAX_BUTTONS; i++) {
            m_Buttons[i] = false;
        }
        YZ_INFO("Key Input Handler Created");
    }

    KeyHandler::~KeyHandler() {}

    bool KeyHandler::isKeyDown(int key) {
        if (key > MAX_KEYS) {
            YZ_WARN("Requested the status of a key " + STR(key) + " that is outside the range of maximum keys.");
            return false;
        }

        return m_KeyState[key] == GLFW_PRESS || m_KeyState[key] == GLFW_REPEAT;
    }

    bool KeyHandler::isKeyPressed(int key) {
        if (key > MAX_KEYS) {
            YZ_WARN("Requested the status of a key " + STR(key) + " that is outside the range of maximum keys.");
            return false;
        }
        return m_KeyState[key] == GLFW_PRESS;
    }

    void KeyHandler::handle() {
        auto  posVec = p_Camera->getTransform().getTranslation();
        auto  upVec = p_Camera->getUpVector();
        auto  lookAtVec = p_Camera->getLookAtVector();
        float cameraSpeed = p_Camera->getCameraSpeed();

        bool needsUpdate = false;

        if (isKeyDown(GLFW_KEY_S)) {
            posVec -= (glm::normalize(lookAtVec) * cameraSpeed);
            needsUpdate = true;
        }

        if (isKeyDown(GLFW_KEY_W)) {
            posVec += (glm::normalize(lookAtVec) * cameraSpeed);
            needsUpdate = true;
        }

        if (isKeyDown(GLFW_KEY_A)) {
            posVec -= (glm::normalize(glm::cross(lookAtVec, upVec)) * cameraSpeed);
            needsUpdate = true;
        }

        if (isKeyDown(GLFW_KEY_D)) {
            posVec += (glm::normalize(glm::cross(lookAtVec, upVec)) * cameraSpeed);
            needsUpdate = true;
        }

        if (isKeyDown(GLFW_KEY_E)) {
            posVec += upVec * cameraSpeed;
            needsUpdate = true;
        }

        if (isKeyDown(GLFW_KEY_Q)) {
            posVec -= upVec * cameraSpeed;
            needsUpdate = true;
        }

        if ((isKeyDown(GLFW_KEY_LEFT_CONTROL) || isKeyDown(GLFW_KEY_RIGHT_CONTROL)) && isKeyPressed(GLFW_KEY_F)) {
            YZ_INFO("FPS Logging Toggled");
            GlobalSettings::instance()->logFps = !GlobalSettings::instance()->logFps;
            m_KeyState[GLFW_KEY_F] = 0;
        }

        if (isKeyDown(GLFW_KEY_ESCAPE)) {
            Application::getAppInstance()->getWindow()->releaseInputHandling();
        }
        if (needsUpdate) {
            p_Camera->setPosition(posVec);
        }
    }

}  // namespace Yare
