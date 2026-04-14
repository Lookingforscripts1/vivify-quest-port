#pragma once

#include "main.hpp"
#include "custom-types/shared/macros.hpp"
#include "Vivify/Controllers/PostProcessingController.hpp"
#include "Vivify/AssetBundle/BundleLoader.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/GameObject.hpp"

#include <string>
#include <vector>
#include <unordered_map>

DECLARE_CLASS_CODEGEN(Vivify::Managers, VivifyManager, UnityEngine::MonoBehaviour,
    // --- Unity lifecycle ---
    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, Start);
    DECLARE_INSTANCE_METHOD(void, OnDestroy);

    public:
        void InitWithSongPath(std::string songFolderPath);
        
        // This is the "brain" that receives instructions from the map
        void ProcessEvent(const std::string& eventType, const std::string& jsonPayload);

        bool IsActive() const { return _active; }

    private:
        std::string _songFolderPath;
        bool _active = false;
        UnityEngine::AssetBundle* _bundle = nullptr;

        // CRITICAL: Keep track of objects so we don't leak memory on Quest
        std::vector<UnityEngine::GameObject*> _spawnedObjects;

        // Tracks the controller on the Main Camera
        Vivify::Controllers::PostProcessingController* _mainController = nullptr;

        void OnBundleLoaded(UnityEngine::AssetBundle* bundle);
        void Cleanup();

        // --- Event handlers (These MUST match the definitions in your .cpp) ---
        void HandleInstantiateGameObject(const std::string& json);
        void HandleSetPostProcessingEffect(const std::string& json);
        void HandleAnimateComponent(const std::string& json);
)
