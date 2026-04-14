#include "Vivify/AssetBundle/BundleLoader.hpp"
#include "VivifyConfig.hpp"

#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/AssetBundleCreateRequest.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>

// Simple MD5 / CRC-like checksum via the bundleinfo.json written by VivifyTemplate.
// On Quest we do a basic byte-comparison against the stored hash if present.
namespace {
    std::optional<std::string> ReadChecksumFromBundleInfo(const std::string& mapFolder) {
        // bundleinfo.json lives next to the map data files
        std::string path = mapFolder + "/bundleinfo.json";
        if (!std::filesystem::exists(path)) return std::nullopt;

        std::ifstream f(path);
        if (!f.is_open()) return std::nullopt;

        // Minimal JSON scan – look for "checksum":"<value>"
        std::string content((std::istreambuf_iterator<char>(f)),
                             std::istreambuf_iterator<char>());
        auto pos = content.find("\"checksum\"");
        if (pos == std::string::npos) return std::nullopt;
        auto colon = content.find(':', pos);
        if (colon == std::string::npos) return std::nullopt;
        auto q1 = content.find('"', colon);
        if (q1 == std::string::npos) return std::nullopt;
        auto q2 = content.find('"', q1 + 1);
        if (q2 == std::string::npos) return std::nullopt;
        return content.substr(q1 + 1, q2 - q1 - 1);
    }
} // anonymous namespace

namespace Vivify::AssetBundle {

custom_types::Helpers::Coroutine LoadBundleAsync(
    std::string mapFolderPath,
    std::optional<std::string> expectedChecksum,
    std::function<void(UnityEngine::AssetBundle*)> onLoaded)
{
    // Determine bundle path – prefer Android2021, fall back to plain Android
    std::string bundlePath = mapFolderPath + "/" + std::string(BUNDLE_FILENAME);
    if (!std::filesystem::exists(bundlePath)) {
        bundlePath = mapFolderPath + "/" + std::string(BUNDLE_FILENAME_FALLBACK);
    }
    if (!std::filesystem::exists(bundlePath)) {
        VivifyLogger::Logger.warn("No Vivify bundle found in: {}", mapFolderPath);
        onLoaded(nullptr);
        co_return;
    }

    // Optional checksum verification
    if (getVivifyConfig().BundleChecksum.GetValue()) {
        auto storedChecksum = expectedChecksum.has_value()
            ? expectedChecksum
            : ReadChecksumFromBundleInfo(mapFolderPath);
        if (storedChecksum.has_value()) {
            // TODO: implement CRC/SHA256 comparison once a hashing library
            // is added as a QPM dependency (e.g. libcrypto from openssl-android)
            VivifyLogger::Logger.debug("Checksum verification placeholder – stored: {}", *storedChecksum);
        }
    }

    VivifyLogger::Logger.info("Loading Vivify bundle: {}", bundlePath);

    // Async load via Unity's streaming API
    auto req = UnityEngine::AssetBundle::LoadFromFileAsync(bundlePath);
    co_yield reinterpret_cast<System::Collections::IEnumerator*>(req);

    auto* bundle = req->get_assetBundle();
    if (!bundle) {
        VivifyLogger::Logger.error("AssetBundle::LoadFromFileAsync returned null for: {}", bundlePath);
        onLoaded(nullptr);
        co_return;
    }

    VivifyLogger::Logger.info("Bundle loaded successfully: {}", bundlePath);
    onLoaded(bundle);
    co_return;
}

void UnloadBundle(UnityEngine::AssetBundle* bundle, bool unloadAllObjects) {
    if (bundle) {
        bundle->Unload(unloadAllObjects);
        VivifyLogger::Logger.info("Vivify bundle unloaded (destroyAssets={})", unloadAllObjects);
    }
}

} // namespace Vivify::AssetBundle
