#include "Entry.h"

#include "Config.h"
#include "ll/api/Config.h"
#include "ll/api/memory/Hook.h"
#include "ll/api/mod/RegisterHelper.h"
#include "mc/deps/core/resource/PackIdVersion.h"
#include "mc/network/packet/PackInfoData.h"
#include "mc/network/packet/ResourcePacksInfoPacket.h"
#include "mc/platform/UUID.h"

#include <algorithm>
#include <cctype>
#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>

namespace resourcepack_encryption {

resourcepack_encryption::Config                mConfig;

LL_TYPE_INSTANCE_HOOK(
    ResourcePacksInfoPacketHook,
    HookPriority::Normal,
    ResourcePacksInfoPacket,
    &ResourcePacksInfoPacket::$ctor,
    void*,
    bool                           resourcePackRequired,
    ::std::vector<::PackInfoData>& resourcePacks,
    bool                           hasAddonPacks,
    ::PackIdVersion const&         worldTemplateIdVersion
) {
    try {
        for (auto& info : resourcePacks) {
            std::string uuid = info.mPackIdVersion->mId->asString();
            std::transform(uuid.begin(), uuid.end(), uuid.begin(), tolower);
            info.mContentKey = mConfig.ResourcePacks[uuid];
        }
    } catch (...) {
        return origin(resourcePackRequired, resourcePacks, hasAddonPacks, worldTemplateIdVersion);
    }
    return origin(resourcePackRequired, resourcePacks, hasAddonPacks, worldTemplateIdVersion);
}

ResourcePackEncryption& ResourcePackEncryption::getInstance() {
    static ResourcePackEncryption instance;
    return instance;
}

bool ResourcePackEncryption::load() {
    if (!ll::config::loadConfig(mConfig, getSelf().getConfigDir() / "config.json")) {
        ll::config::saveConfig(mConfig, getSelf().getConfigDir() / "config.json");
    }
    ResourcePacksInfoPacketHook::hook();
    getSelf().getLogger().info("Found {0} resource packs's ContentKey", mConfig.ResourcePacks.size());
    return true;
}

bool ResourcePackEncryption::enable() { return true; }

bool ResourcePackEncryption::disable() { return true; }

bool ResourcePackEncryption::unload() {
    ll::config::saveConfig(mConfig, getSelf().getConfigDir() / "config.json");
    ResourcePacksInfoPacketHook::unhook();
    return true;
}

} // namespace resourcepack_encryption

LL_REGISTER_MOD(
    resourcepack_encryption::ResourcePackEncryption,
    resourcepack_encryption::ResourcePackEncryption::getInstance()
);
