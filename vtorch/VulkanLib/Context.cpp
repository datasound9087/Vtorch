#include "Context.h"
#include "LoggingLib/Logging.h"
#include <GLFW/glfw3.h>
#include <map>
#include <ranges>
#include <string>
#include <vector>

namespace
{
    const std::vector<const char *> requiredDeviceExtensions = {
        vk::KHRSwapchainExtensionName};

    static VKAPI_ATTR vk::Bool32 VKAPI_CALL
    debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                  vk::DebugUtilsMessageTypeFlagsEXT /*type*/,
                  const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
                  void * /*pUserData*/)
    {
        if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
        {
            LOG_ERROR("VK Error: {}", pCallbackData->pMessage);
        }

        return vk::False;
    }

    std::vector<const char *> GetInstanceExtensions(vk::raii::Context &context)
    {
        // Get the required instance extensions from GLFW.
        uint32_t glfwExtensionCount = 0;
        auto glfwExtensions =
            glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions{};

        // Check if the required GLFW extensions are supported by the Vulkan
        // implementation.
        auto extensionProperties =
            context.enumerateInstanceExtensionProperties();
        for (uint32_t i = 0; i < glfwExtensionCount; ++i)
        {
            if (std::ranges::none_of(
                    extensionProperties,
                    [glfwExtension =
                         glfwExtensions[i]](auto const &extensionProperty) {
                        return strcmp(extensionProperty.extensionName,
                                      glfwExtension) == 0;
                    }))
            {
                throw std::runtime_error(
                    "Required GLFW extension not supported: " +
                    std::string(glfwExtensions[i]));
            }

            extensions.push_back(glfwExtensions[i]);
        }

        // Needed on MacOS
        extensions.push_back(vk::KHRPortabilityEnumerationExtensionName);
        extensions.push_back(vk::EXTDebugUtilsExtensionName);
        return extensions;
    }

    vk::raii::PhysicalDevice PickPhysicalDevice(
        const std::vector<vk::raii::PhysicalDevice> &physicalDevices)
    {
        // Use an ordered map to automatically sort candidates by increasing
        // score
        std::multimap<int, vk::raii::PhysicalDevice> candidates;

        for (const auto &device : physicalDevices)
        {
            const auto properties = device.getProperties();
            const auto features = device.getFeatures();
            uint32_t score = 0;

            if (properties.apiVersion < vk::ApiVersion13)
            {
                continue;
            }

            const auto queueFamilies = device.getQueueFamilyProperties();
            bool supportsAllQueues = std::ranges::all_of(
                queueFamilies,
                [](auto const &qfp)
                {
                    return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) ||
                           (qfp.queueFlags & vk::QueueFlagBits::eCompute) ||
                           (qfp.queueFlags & vk::QueueFlagBits::eTransfer);
                });
            if (!supportsAllQueues)
            {
                continue;
            }

            const auto deviceExtensions =
                device.enumerateDeviceExtensionProperties();
            bool supportsAllRequiredExtensions = std::ranges::all_of(
                requiredDeviceExtensions,
                [&](auto const &deviceExtension)
                {
                    const auto result = std::ranges::any_of(
                        deviceExtensions,
                        [&](auto const &availableDeviceExtension)
                        {
                            return strcmp(
                                       availableDeviceExtension.extensionName,
                                       deviceExtension) == 0;
                        });
                    return result;
                });
            if (!supportsAllRequiredExtensions)
            {
                continue;
            }

            // Discrete GPUs have a significant performance advantage
            if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                score += 1000;
            }
            else
            {
                score += 100;
            }

            // Maximum possible size of textures affects graphics quality
            score += properties.limits.maxImageDimension2D;
            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.empty())
        {
            throw std::runtime_error(
                "Cannot find GPU that supports all required features");
        }
        else
        {
            return candidates.rbegin()->second;
        }
    }
} // namespace

Context::Context(GLFWwindow *window)
{
    LOG_DEBUG("Creating context");
    const vk::ApplicationInfo appInfo{.pApplicationName = "VTorch Application",
                                      .applicationVersion =
                                          VK_MAKE_VERSION(1, 0, 0),
                                      .pEngineName = "VTorch",
                                      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                                      .apiVersion = vk::ApiVersion14};

    const auto instanceExtensions = GetInstanceExtensions(m_context);
    const std::vector<char const *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"};

    const vk::InstanceCreateInfo createInfo{
        .flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
        .ppEnabledLayerNames = validationLayers.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(instanceExtensions.size()),
        .ppEnabledExtensionNames = instanceExtensions.data()};

    m_instance = vk::raii::Instance(m_context, createInfo);

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{
        .messageSeverity = severityFlags,
        .messageType = messageTypeFlags,
        .pfnUserCallback = &debugCallback};

    m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(
        debugUtilsMessengerCreateInfoEXT);

    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(*m_instance, window, nullptr, &surface) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
    m_surface = vk::raii::SurfaceKHR(m_instance, surface);

    const auto physicalDevices = m_instance.enumeratePhysicalDevices();
    if (physicalDevices.empty())
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support");
    }

    m_physicalDevice = PickPhysicalDevice(physicalDevices);
    LOG_ALL("GPU: {}", m_physicalDevice.getProperties().deviceName.data());

    std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
        m_physicalDevice.getQueueFamilyProperties();

    const auto graphicsQueueFamilyProperty = std::ranges::find_if(
        queueFamilyProperties,
        [](auto const &qfp)
        {
            return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) !=
                   static_cast<vk::QueueFlags>(0);
        });
    const auto graphicsIndex = static_cast<uint32_t>(std::distance(
        queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
    // If the found physical device does not support presentation from its
    // graphics queue, fail
    if (!m_physicalDevice.getSurfaceSupportKHR(graphicsIndex, *m_surface))
    {
        throw std::runtime_error(
            "Graphics queue does not support presentation");
    }

    const float queuePriority = 0.5f;
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = {
        {.queueFamilyIndex = graphicsIndex,
         .queueCount = 1,
         .pQueuePriorities = &queuePriority}};

    // Create a chain of feature structures
    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                       vk::PhysicalDeviceVulkan13Features,
                       vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
        featureChain = {// vk::PhysicalDeviceFeatures2
                        {},
                        // Enable dynamic rendering from Vulkan 1.3
                        {.dynamicRendering = true},
                        // Enable extended dynamic state
                        {.extendedDynamicState = true}};

    vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(requiredDeviceExtensions.size()),
        .ppEnabledExtensionNames = requiredDeviceExtensions.data()};

    m_device = vk::raii::Device(m_physicalDevice, deviceCreateInfo);

    m_graphicsQueue = vk::raii::Queue(m_device, graphicsIndex, 0);
}

const SwapchainInfo Context::GetSwapchainInfo() const
{
    return SwapchainInfo{
        .surface = *m_surface,
        .capabilities = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface),
        .formats = m_physicalDevice.getSurfaceFormatsKHR(m_surface),
        .presentModes = m_physicalDevice.getSurfacePresentModesKHR(m_surface)};
}

const vk::raii::Device &Context::GetDevice() const { return m_device; }