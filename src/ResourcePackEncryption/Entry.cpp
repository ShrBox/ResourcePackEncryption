#include "Entry.h"

#include "Config.h"
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


namespace ResourcePackEncryption {

namespace {

Config mConfig;
std::unique_ptr<std::reference_wrapper<ll::plugin::NativePlugin>>
    selfPluginInstance; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

LL_TYPE_INSTANCE_HOOK(
    ResourcePacksInfoPacketHook,
    HookPriority::Normal,
    ResourcePacksInfoPacket,
    "??0ResourcePacksInfoPacket@@QEAA@_NAEAV?$vector@UPackInfoData@@V?$allocator@UPackInfoData@@@std@@@std@@10$$QEAV?$"
    "vector@U?$pair@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V12@@std@@V?$allocator@U?$pair@V?$"
    "basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V12@@std@@@2@@2@@Z",
    ResourcePacksInfoPacket*,
    bool                                               resourcePackRequired,
    std::vector<PackInfoData>&                         behaviorPacks,
    std::vector<PackInfoData>&                         resourcePacks,
    bool                                               forceServerPacksEnabled,
    std::vector<std::pair<std::string, std::string>>&& cdnUrls
) {
    try {
        for (auto& info : resourcePacks) {
            std::string uuid = info.mPackIdVersion.mId.asString();
            std::transform(uuid.begin(), uuid.end(), uuid.begin(), tolower);
            info.mContentKey = mConfig.ResourcePacks[uuid];
        }
    } catch (...) {
        return origin(resourcePackRequired, behaviorPacks, resourcePacks, forceServerPacksEnabled, std::move(cdnUrls));
    }
    return origin(resourcePackRequired, behaviorPacks, resourcePacks, forceServerPacksEnabled, std::move(cdnUrls));
}

auto disable(ll::plugin::NativePlugin& /*self*/) -> bool { return true; }

auto enable(ll::plugin::NativePlugin& /*self*/) -> bool { return true; }

auto load(ll::plugin::NativePlugin& self) -> bool {
    auto& logger       = self.getLogger();
    selfPluginInstance = std::make_unique<std::reference_wrapper<ll::plugin::NativePlugin>>(self);
    if (!ll::config::loadConfig(mConfig, self.getConfigDir() / "config.json")) {
        ll::config::saveConfig(mConfig, self.getConfigDir() / "config.json");
    }
    ResourcePacksInfoPacketHook::hook();
    logger.info("Found {0} resource packs's ContentKey", mConfig.ResourcePacks.size());
    return true;
}

auto unload(ll::plugin::NativePlugin& self) -> bool {
    auto& logger = self.getLogger();
    selfPluginInstance.reset();
    ll::config::saveConfig(mConfig, self.getConfigDir() / "config.json");
    ResourcePacksInfoPacketHook::unhook();
    logger.info("unloaded");

    return true;
}

} // namespace

auto getSelfPluginInstance() -> ll::plugin::NativePlugin& {
    if (!selfPluginInstance) {
        throw std::runtime_error("selfPluginInstance is null");
    }

    return *selfPluginInstance;
}

} // namespace ResourcePackEncryption

extern "C" {
_declspec(dllexport) auto ll_plugin_disable(ll::plugin::NativePlugin& self) -> bool {
    return ResourcePackEncryption::disable(self);
}
_declspec(dllexport) auto ll_plugin_enable(ll::plugin::NativePlugin& self) -> bool {
    return ResourcePackEncryption::enable(self);
}
_declspec(dllexport) auto ll_plugin_load(ll::plugin::NativePlugin& self) -> bool {
    return ResourcePackEncryption::load(self);
}
_declspec(dllexport) auto ll_plugin_unload(ll::plugin::NativePlugin& self) -> bool {
    return ResourcePackEncryption::unload(self);
}
}
