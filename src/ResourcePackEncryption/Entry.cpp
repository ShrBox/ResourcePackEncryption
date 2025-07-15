#include "Entry.h"

#include "Config.h"
#include "ll/api/Config.h"
#include "ll/api/memory/Hook.h"
#include "ll/api/mod/RegisterHelper.h"
#include "ll/api/service/Bedrock.h"
#include "ll/api/service/TargetedBedrock.h"
#include "mc/deps/core/resource/PackIdVersion.h"
#include "mc/network/ServerNetworkHandler.h"
#include "mc/network/packet/PackInfoData.h"
#include "mc/network/packet/ResourcePacksInfoPacket.h"
#include "mc/platform/UUID.h"
#include "mc/resources/PackManifest.h"
#include "mc/resources/ResourcePack.h"
#include "mc/resources/ResourcePackRepository.h"
#include "mc/server/CDNConfig.h"
#include "mc/server/ServerInstance.h"

namespace resourcepack_encryption {

resourcepack_encryption::Config mConfig;

ResourcePackEncryption& ResourcePackEncryption::getInstance() {
    static ResourcePackEncryption instance;
    return instance;
}

bool ResourcePackEncryption::load() {
    if (!ll::config::loadConfig(mConfig, getSelf().getConfigDir() / "config.json")) {
        ll::config::saveConfig(mConfig, getSelf().getConfigDir() / "config.json");
    }
    getSelf().getLogger().info("Found {0} resource packs's ContentKey", mConfig.ResourcePacks.size());
    getSelf().getLogger().info("Found {0} resource packs's CDN URL", mConfig.ResourcePacksCDN.size());
    return true;
}

bool ResourcePackEncryption::enable() {
    for (auto& [id, key] : mConfig.ResourcePacks) {
        auto pack = ll::service::getResourcePackRepository()->getResourcePackByUUID(mce::UUID(id));
        if (pack) {
            ll::service::getServerNetworkHandler()->mPackIdToContentKey->insert({pack->getManifest().mIdentity, key});
        }
    }
    auto& cdnUrls = ll::service::getServerInstance()->mCDNConfig.get()->mPackCDNUrls.get();
    for (auto& [uuid, url] : mConfig.ResourcePacksCDN) {
        auto pack = ll::service::getResourcePackRepository()->getResourcePackByUUID(mce::UUID(uuid));
        if (pack) {
            cdnUrls.emplace_back(pack->getManifest().mIdentity->asString(), url);
        }
    }
    getSelf().getLogger().info(
        "Loaded {0} resource packs's ContentKey",
        ll::service::getServerNetworkHandler()->mPackIdToContentKey->size()
    );
    getSelf().getLogger().info("Loaded {0} resource packs's CDN URL", cdnUrls.size());
    return true;
}

bool ResourcePackEncryption::disable() { return true; }

bool ResourcePackEncryption::unload() {
    ll::config::saveConfig(mConfig, getSelf().getConfigDir() / "config.json");
    return true;
}

} // namespace resourcepack_encryption

LL_REGISTER_MOD(
    resourcepack_encryption::ResourcePackEncryption,
    resourcepack_encryption::ResourcePackEncryption::getInstance()
);
