#include <string>
#include <unordered_map>

namespace resourcepack_encryption {
struct Config {
    int                                          version = 1;
    std::unordered_map<std::string, std::string> ResourcePacks;
    std::unordered_map<std::string, std::string> ResourcePacksCDN;
};
} // namespace resourcepack_encryption
