#pragma once
#include <ll/api/plugin/NativePlugin.h>


namespace ResourcePackEncryption {

[[nodiscard]] auto getSelfPluginInstance() -> ll::plugin::NativePlugin&;

} // namespace ResourcePackEncryption
