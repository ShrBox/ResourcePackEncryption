#include "Entry.h"

#include "Config.h"
#include "ll/api/plugin/NativePlugin.h"
#include "ll/api/plugin/RegisterHelper.h"
#include <algorithm>
#include <cctype>
#include <exception>
#include <fmt/format.h>
#include <functional>
#include <ll/api/Config.h>
#include <ll/api/io/FileUtils.h>
#include <ll/api/memory/Hook.h>
#include <ll/api/plugin/NativePlugin.h>
#include <ll/api/plugin/PluginManagerRegistry.h>
#include <mc/network/packet/ResourcePacksInfoPacket.h>
#include <memory>
#include <stdexcept>

namespace resourcepack_encryption {

static std::unique_ptr<ResourcePackEncryption> instance;
resourcepack_encryption::Config                mConfig;

LL_TYPE_INSTANCE_HOOK(
    ResourcePacksInfoPacketHook,
    HookPriority::Normal,
    ResourcePacksInfoPacket,
    "??0ResourcePacksInfoPacket@@QEAA@_NAEAV?$vector@UPackInfoData@@V?$allocator@UPackInfoData@@@std@@@std@@10$$QEAV?$"
    "vector@U?$pair@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V12@@std@@V?$allocator@U?$pair@V?$"
    "basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V12@@std@@@2@@2@0@Z",
    ResourcePacksInfoPacket*,
    bool                                               resourcePackRequired,
    std::vector<PackInfoData>&                         behaviorPacks,
    std::vector<PackInfoData>&                         resourcePacks,
    bool                                               forceServerPacksEnabled,
    std::vector<std::pair<std::string, std::string>>&& cdnUrls,
    bool                                               unk
) {
    try {
        for (auto& info : resourcePacks) {
            std::string uuid = info.mPackIdVersion.mId.asString();
            std::transform(uuid.begin(), uuid.end(), uuid.begin(), tolower);
            info.mContentKey = mConfig.ResourcePacks[uuid];
        }
    } catch (...) {
        return origin(
            resourcePackRequired,
            behaviorPacks,
            resourcePacks,
            forceServerPacksEnabled,
            std::move(cdnUrls),
            unk
        );
    }
    return origin(resourcePackRequired, behaviorPacks, resourcePacks, forceServerPacksEnabled, std::move(cdnUrls), unk);
}

ResourcePackEncryption& ResourcePackEncryption::getInstance() { return *instance; }

bool ResourcePackEncryption::load() {
    getSelf().getLogger().info("Loading...");
    // Code for loading the plugin goes here.
    if (!ll::config::loadConfig(mConfig, getSelf().getConfigDir() / "config.json")) {
        ll::config::saveConfig(mConfig, getSelf().getConfigDir() / "config.json");
    }
    ResourcePacksInfoPacketHook::hook();
    getSelf().getLogger().info("Found {0} resource packs's ContentKey", mConfig.ResourcePacks.size());
    return true;
}

bool ResourcePackEncryption::enable() {
    getSelf().getLogger().info("Enabling...");
    // Code for enabling the plugin goes here.
    return true;
}

bool ResourcePackEncryption::disable() {
    getSelf().getLogger().info("Disabling...");
    // Code for disabling the plugin goes here.
    ll::config::saveConfig(mConfig, getSelf().getConfigDir() / "config.json");
    ResourcePacksInfoPacketHook::unhook();
    return true;
}

bool ResourcePackEncryption::unload() {
    ll::config::saveConfig(mConfig, getSelf().getConfigDir() / "config.json");
    ResourcePacksInfoPacketHook::unhook();
    return true;
}

} // namespace resourcepack_encryption

LL_REGISTER_PLUGIN(resourcepack_encryption::ResourcePackEncryption, resourcepack_encryption::instance);
