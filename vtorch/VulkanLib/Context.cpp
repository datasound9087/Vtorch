#include "Context.h"
#include "LoggingLib/Logging.h"
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

namespace
{
    std::vector<const char *> GetExtensions(vk::raii::Context &context)
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

        return extensions;
    }
} // namespace

Context::Context()
{
    LOG_DEBUG("Creating context");
    const vk::ApplicationInfo appInfo{.pApplicationName = "VTorch Application",
                                      .applicationVersion =
                                          VK_MAKE_VERSION(1, 0, 0),
                                      .pEngineName = "VTorch",
                                      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                                      .apiVersion = vk::ApiVersion14};

    const auto extensions = GetExtensions(m_context);
    const vk::InstanceCreateInfo createInfo{
        .flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()};

    m_instance = vk::raii::Instance(m_context, createInfo);
}