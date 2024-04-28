#pragma once

#include "ll/api/plugin/NativePlugin.h"

namespace resourcepack_encryption {

class ResourcePackEncryption {

public:
    static ResourcePackEncryption& getInstance();

    ResourcePackEncryption(ll::plugin::NativePlugin& self) : mSelf(self) {}

    [[nodiscard]] ll::plugin::NativePlugin& getSelf() const { return mSelf; }

    /// @return True if the plugin is loaded successfully.
    bool load();

    /// @return True if the plugin is enabled successfully.
    bool enable();

    /// @return True if the plugin is disabled successfully.
    bool disable();

    /// @return True if the plugin is unloaded successfully.
    bool unload();

private:
    ll::plugin::NativePlugin& mSelf;
};

} // namespace resourcepack_encryption
