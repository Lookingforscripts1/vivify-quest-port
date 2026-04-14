#include "main.hpp"
#include "SongCore/shared/SongCore.hpp"
#include "custom-types/shared/register.hpp"

extern "C" void load() {
    VivifyLogger::Logger.info("Loading Vivify Standalone Port...");

    auto& config = getVivifyConfig();
    config.Init(modInfo);

    custom_types::Register::AutoRegister();

    // CRITICAL: Register Capability for SongCore
    SongCore::API::Capabilities::RegisterCapability("Vivify");

    VivifyLogger::Logger.info("Vivify 1.0.7 Port Loaded.");
}
