#ifndef YAREZO_VK_COMMANDBUFFER_H
#define YAREZO_VK_COMMANDBUFFER_H

#include "Graphics/Vulkan/Vk.h"
#include "Graphics/Vulkan/Vk_CommandPool.h"

namespace Yarezo::Graphics {
    class YzVkCommandBuffer {
    public:
        YzVkCommandBuffer();
        ~YzVkCommandBuffer();

        void beginRecording();
        void endRecording();
        void submitGfxQueue(VkPipelineStageFlags flags, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, bool waitFence);

        const VkCommandBuffer& getCommandBuffer() const { return m_CommandBuffer; }
        const VkFence& getFence() const { return m_Fence; }
    private:
        void init();
        VkCommandBuffer m_CommandBuffer;
        VkFence m_Fence = VK_NULL_HANDLE;
    };
}

#endif //YAREZO_VK_COMMANDBUFFER_H
