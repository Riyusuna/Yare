#include "Graphics/Vulkan/Devices.h"

#include <set>

#include "Application/Application.h"
#include "Core/Glfw.h"
#include "Utilities/Logger.h"

namespace Yare::Graphics {

    Devices::Devices() {}

    Devices::~Devices() {
        if (m_Device) {
            vkDestroyDevice(m_Device, nullptr);
        }
        if (m_Surface) {
            vkDestroySurfaceKHR(m_InstanceRef, m_Surface, nullptr);
        }
    }

    void Devices::init(VkInstance instance) {
        if (m_Surface || m_Device || m_PhysicalDevice) {
            YZ_INFO("Device has already been initialized.");
            return;
        }
        m_InstanceRef = instance;
        // Surface must be created before picking a physical device
        createSurface();
        // Pick a Gpu that is suitable for rendering
        pickPhysicalDevice();
        // Create a logical device, such that we can interface with the physical device we selected
        createLogicalDevice();
    }

    void Devices::waitIdle() { vkDeviceWaitIdle(m_Device); }

    void Devices::createSurface() {
        GLFWwindow* windowInstance =
            static_cast<GLFWwindow*>(Application::getAppInstance()->getWindow()->getNativeWindow());

        if (glfwCreateWindowSurface(m_InstanceRef, windowInstance, nullptr, &m_Surface) != VK_SUCCESS) {
            YZ_CRITICAL("Could not create a window surface.");
        }
    }

    void Devices::pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_InstanceRef, &deviceCount, nullptr);

        if (deviceCount == 0) {
            YZ_CRITICAL("Failed to find GPUs with Vulkan Support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_InstanceRef, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                m_PhysicalDevice = device;
                break;
            }
        }
        if (m_PhysicalDevice == VK_NULL_HANDLE) {
            YZ_CRITICAL("Failed to find a suitable GPU");
        }

        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_PhysicalDeviceProperties);
    }

    void Devices::createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<int>                        uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

        float queuePriority = 1.0f;
        for (int queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        // Required for MacOS
        auto availableExtensions = getAvailableDeviceExtensions(m_PhysicalDevice);
        for (auto extension : availableExtensions) {
            if (strcmp(extension.extensionName, "VK_KHR_portability_subset") == 0) m_DeviceExtensions.push_back("VK_KHR_portability_subset");
        }
        VkDeviceCreateInfo createInfo = {};

        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

        ////To support older implementations of vulkan
        // if (enableValidationLayers) {
        //    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        //    createInfo.ppEnabledLayerNames = validationLayers.data();
        //} else {
        //    createInfo.enabledLayerCount = 0;
        //}

        if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
            YZ_CRITICAL("Failed to create a logical device.");
        }

        vkGetDeviceQueue(m_Device, indices.graphicsFamily, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, indices.presentFamily, 0, &m_PresentQueue);
    }

    bool Devices::isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);
        bool swapChainAdequate = false;

        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }
        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    }

    std::vector<VkExtensionProperties> Devices::getAvailableDeviceExtensions(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        return availableExtensions;
    }

    bool Devices::checkDeviceExtensionSupport(VkPhysicalDevice device) {

        auto availableExtensions = getAvailableDeviceExtensions(device);

        std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices Devices::findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;
        uint32_t           queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            VkBool32 presentSupport = false;

            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

            if (indices.presentFamily < 0 && presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.graphicsFamily < 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            if (indices.isComplete()) break;

            i++;
        }

        return indices;
    }

    QueueFamilyIndices Devices::getQueueFamilyIndicies() { return findQueueFamilies(m_PhysicalDevice); }

    SwapChainSupportDetails Devices::getSwapChainSupport() { return querySwapChainSupport(m_PhysicalDevice); }

    SwapChainSupportDetails Devices::querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount,
                                                      details.presentModes.data());
        }
        return details;
    }

}  // namespace Yare::Graphics
