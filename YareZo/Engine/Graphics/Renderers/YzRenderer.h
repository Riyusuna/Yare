#ifndef YAREZO_YZ_RENDERER_H
#define YAREZO_YZ_RENDERER_H

#include "Graphics/Model.h"
#include "Graphics/RenderCommand.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

namespace Yarezo::Graphics {

    typedef std::vector<RenderCommand> CommandQueue;

    class YzRenderer {
    public:
        virtual ~YzRenderer() = default;

        virtual void init() = 0;
        virtual void renderScene() = 0;
        virtual void submitModel(Model* model, const glm::mat4& transform) = 0;
        virtual void present() = 0;
        virtual void begin() = 0;
        virtual void end() = 0;

    protected:
        CommandQueue m_CommandQueue;
    };

}
#endif
