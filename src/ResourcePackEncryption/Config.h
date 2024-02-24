
#include <string>
#include <unordered_map>
namespace ResourcePackEncryption {
struct Config {
    int                                          version = 0;
    std::unordered_map<std::string, std::string> ResourcePacks;
};
} // namespace ResourcePackEncryption
