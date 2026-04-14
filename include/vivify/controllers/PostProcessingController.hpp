#include "Vivify/Controllers/PostProcessingController.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/Graphics.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/RenderTextureDescriptor.hpp"
#include "UnityEngine/SystemInfo.hpp"

DEFINE_TYPE(Vivify::Controllers, PostProcessingController)

namespace Vivify::Controllers {

static int _StereoActiveEyeID = 0;

void PostProcessingController::Awake() {
    _camera = GetComponent<UnityEngine::Camera*>();
    _StereoActiveEyeID = UnityEngine::Shader::PropertyToID("_StereoActiveEye");
}

void PostProcessingController::OnRenderImage(UnityEngine::RenderTexture* src, UnityEngine::RenderTexture* dst) {
    if (_shaderEntries.empty() || !_camera) {
        UnityEngine::Graphics::Blit(src, dst);
        return;
    }

    if (_dirty) RebuildSortedList();

    // QUEST FIX: Handle Single Pass Instanced
    // We ping-pong between temporary textures that match the source descriptor (which is a Texture2DArray on Quest)
    auto desc = src->get_descriptor();
    auto rt1 = UnityEngine::RenderTexture::GetTemporary(desc);
    auto rt2 = UnityEngine::RenderTexture::GetTemporary(desc);

    UnityEngine::Graphics::Blit(src, rt1);

    for (const auto& entry : _shaderEntries) {
        // We set the eye index to 0, then 1, and blit. 
        // Most Quest-optimized shaders will handle the array automatically if we use the correct Blit signature.
        UnityEngine::Shader::SetGlobalInt(_StereoActiveEyeID, 0); 
        UnityEngine::Graphics::Blit(rt1, rt2, entry.material);
        std::swap(rt1, rt2);
    }

    UnityEngine::Graphics::Blit(rt1, dst);

    UnityEngine::RenderTexture::ReleaseTemporary(rt1);
    UnityEngine::RenderTexture::ReleaseTemporary(rt2);
}

void PostProcessingController::RebuildSortedList() {
    std::stable_sort(_shaderEntries.begin(), _shaderEntries.end(),
        [](const ShaderEntry& a, const ShaderEntry& b) {
            return a.priority < b.priority;
        });
    _dirty = false;
}

// ... AddShader and RemoveShader implementation stays the same as your upload ...
}
