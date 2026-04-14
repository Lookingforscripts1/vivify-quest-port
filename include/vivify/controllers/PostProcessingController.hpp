#pragma once

#include "main.hpp"
#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Shader.hpp"

#include <vector>
#include <string>

namespace Vivify::Controllers {

    DECLARE_CLASS_CODEGEN(Vivify::Controllers, PostProcessingController, UnityEngine::MonoBehaviour,
        
        // --- Unity lifecycle ---
        DECLARE_INSTANCE_METHOD(void, Awake);
        DECLARE_INSTANCE_METHOD(void, OnDestroy);

        // This is the core of the Quest rendering fix
        DECLARE_INSTANCE_METHOD(void, OnRenderImage,
            UnityEngine::RenderTexture* src,
            UnityEngine::RenderTexture* dst);

    public:
        struct ShaderEntry {
            int priority;
            std::string name;
            UnityEngine::Material* material;
        };

        void AddShader(std::string name, UnityEngine::Material* mat, int priority);
        void RemoveShader(std::string_view name);
        void ClearShaders();
        void SetDepthTextureEnabled(bool enabled);

    private:
        UnityEngine::Camera* _camera = nullptr;
        std::vector<ShaderEntry> _shaderEntries;
        bool _dirty = false;

        // Quest SPI helper
        static inline int _StereoActiveEyeID = 0;

        void RebuildSortedList();
        // Updated signature to handle the SPI ping-pong logic
        void BlitChain(UnityEngine::RenderTexture* src, UnityEngine::RenderTexture* dst);
    )

} // namespace Vivify::Controllers
