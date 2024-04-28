
#include <string>
#include <unordered_map>
namespace resourcepack_encryption {
struct Config {
    int                                          version = 0;
    std::unordered_map<std::string, std::string> ResourcePacks;
};
} // namespace resourcepack_encryption
