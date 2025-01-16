#pragma once

#include "ll/api/mod/NativeMod.h"

namespace resourcepack_encryption {

class ResourcePackEncryption {

public:
    static ResourcePackEncryption& getInstance();

    ResourcePackEncryption() : mSelf(*ll::mod::NativeMod::current()) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    bool load();

    bool enable();

    bool disable();

    bool unload();

private:
    ll::mod::NativeMod& mSelf;
};

} // namespace resourcepack_encryption