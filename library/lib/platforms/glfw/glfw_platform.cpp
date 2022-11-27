/*
    Copyright 2021 natinusala

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <borealis/core/i18n.hpp>
#include <borealis/core/logger.hpp>
#include <borealis/platforms/glfw/glfw_platform.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <strings.h>

#ifdef __PSV__
extern "C" {
    unsigned int sceLibcHeapSize = 64 * 1024 * 1024;
};
#endif

// glfw video and input code inspired from the glfw hybrid app by fincs
// https://github.com/fincs/hybrid_app

namespace brls
{

static void glfwErrorCallback(int errorCode, const char* description)
{
    Logger::error("glfw: error {}: {}", errorCode, description);
}

GLFWPlatform::GLFWPlatform()
{
    // Init glfw
    glfwSetErrorCallback(glfwErrorCallback);
    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_TRUE);

    if (!glfwInit())
    {
        Logger::error("glfw: failed to initialize");
        return;
    }

    // Theme
    char* themeEnv = getenv("BOREALIS_THEME");
    if (themeEnv != nullptr && !strcasecmp(themeEnv, "DARK"))
        this->themeVariant = ThemeVariant::DARK;

    // Misc
    glfwSetTime(0.0);

    // Platform impls
    this->fontLoader  = new GLFWFontLoader();
    this->audioPlayer = new NullAudioPlayer();
}

void GLFWPlatform::createWindow(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight)
{
    this->videoContext = new GLFWVideoContext(windowTitle, windowWidth, windowHeight);
    this->inputManager = new GLFWInputManager(this->videoContext->getGLFWWindow());
}

bool GLFWPlatform::canShowBatteryLevel()
{
    return true;
}

int battery = 50;
int GLFWPlatform::getBatteryLevel()
{
    battery %= 100;
    battery++;
    return battery;
}

bool GLFWPlatform::isBatteryCharging()
{
    return true;
}

bool GLFWPlatform::hasWirelessConnection()
{
    return true;
}

int GLFWPlatform::getWirelessLevel()
{
    return battery / 20;
}

std::string GLFWPlatform::getIpAddress()
{
    return "0.0.0.0";
}

std::string GLFWPlatform::getDnsServer()
{
    return "0.0.0.0";
}

bool GLFWPlatform::isApplicationMode()
{
    return true;
}

void GLFWPlatform::exitToHomeMode(bool value)
{
    return;
}

void GLFWPlatform::forceEnableGamePlayRecording()
{
    return;
}

void GLFWPlatform::openBrowser(std::string url)
{
    brls::Logger::debug("open url: {}", url);
#ifdef __APPLE__
    std::string cmd = "open " + url;
    system(cmd.c_str());
#endif
#ifdef __linux__
    std::string cmd = "xdg-open " + url;
    system(cmd.c_str());
#endif
#ifdef _WIN32
    std::string cmd = "explorer " + url;
    system(cmd.c_str());
#endif
    return;
}

std::string GLFWPlatform::getName()
{
    return "GLFW";
}

bool GLFWPlatform::mainLoopIteration()
{
    bool isActive;
    do
    {
        isActive = !glfwGetWindowAttrib(this->videoContext->getGLFWWindow(), GLFW_ICONIFIED);

        if (isActive)
            glfwPollEvents();
        else
            glfwWaitEvents();
    } while (!isActive);

    return !glfwWindowShouldClose(this->videoContext->getGLFWWindow());
}

AudioPlayer* GLFWPlatform::getAudioPlayer()
{
    return this->audioPlayer;
}

VideoContext* GLFWPlatform::getVideoContext()
{
    return this->videoContext;
}

InputManager* GLFWPlatform::getInputManager()
{
    return this->inputManager;
}

FontLoader* GLFWPlatform::getFontLoader()
{
    return this->fontLoader;
}

ThemeVariant GLFWPlatform::getThemeVariant()
{
    return this->themeVariant;
}

void GLFWPlatform::setThemeVariant(ThemeVariant theme)
{
    this->themeVariant = theme;
}

std::string GLFWPlatform::getLocale()
{
    char* langEnv = getenv("BOREALIS_LANG");
    if (langEnv == nullptr)
        return LOCALE_DEFAULT;
    return std::string(langEnv);
}

GLFWPlatform::~GLFWPlatform()
{
    delete this->fontLoader;
    delete this->audioPlayer;
    delete this->videoContext;
    delete this->inputManager;
}

} // namespace brls
