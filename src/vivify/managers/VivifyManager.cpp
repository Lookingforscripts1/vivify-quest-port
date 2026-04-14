#include "Vivify/Managers/VivifyManager.hpp"
#include "Vivify/Controllers/PostProcessingController.hpp"
#include "VivifyConfig.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Camera.hpp"
#include "SongCore/shared/SongCore.hpp"

DEFINE_TYPE(Vivify::Managers, VivifyManager)

namespace Vivify::Managers {

void VivifyManager::OnBundleLoaded(UnityEngine::AssetBundle* bundle) {
    if (!bundle) {
        VivifyLogger::Logger.error("Failed to load Vivify AssetBundle.");
        return;
    }
    _bundle = bundle;
    _active = true;
    VivifyLogger::Logger.info("Vivify Active for this map.");
}

void VivifyManager::HandleSetPostProcessingEffect(const std::string& json) {
    if (!getVivifyConfig().PostProcessing.GetValue()) return;

    auto mainCam = UnityEngine::Camera::get_main();
    if (!mainCam) return;

    // Ensure the controller exists on the camera
    auto controller = mainCam->GetComponent<Vivify::Controllers::PostProcessingController*>();
    if (!controller) {
        controller = mainCam->get_gameObject()->AddComponent<Vivify::Controllers::PostProcessingController*>();
    }

    // Example logic: Load material from bundle (replace "VFX_Mat" with your JSON-parsed name)
    auto mat = Vivify::AssetBundle::LoadAsset<UnityEngine::Material*>(_bundle, "VFX_Mat");
    if (mat) {
        controller->AddShader("MapEffect", mat, 100);
    }
}

void VivifyManager::Cleanup() {
    _active = false;
    
    // Clean up PostProcessing from Main Camera
    auto mainCam = UnityEngine::Camera::get_main();
    if (mainCam) {
        auto controller = mainCam->GetComponent<Vivify::Controllers::PostProcessingController*>();
        if (controller) UnityEngine::Object::Destroy(controller);
    }

    for (auto obj : _spawnedObjects) {
        if (obj) UnityEngine::Object::Destroy(obj);
    }
    _spawnedObjects.clear();

    if (_bundle) {
        Vivify::AssetBundle::UnloadBundle(_bundle, true);
        _bundle = nullptr;
    }
}

}
