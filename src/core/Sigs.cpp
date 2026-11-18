#include "Sigs.hpp"
#include "Mem.hpp"
#include <android/log.h>
#include <cstring>

#define LOG(...) __android_log_print(ANDROID_LOG_INFO, "outcome", __VA_ARGS__)

namespace sigs {

static const char* kLib = "libminecraftpe.so";
static std::unordered_map<std::string, uintptr_t> s_addr;
static std::unordered_map<std::string, size_t>    s_off;
static uint32_t s_ver = 0;

// per-version tables. add a new block when a game build drops,
// don't touch the old ones.
static const Entry kV121120[] = {
    { "Level_tick",           "F5 0F 1E F8 ? ? ? A9 ? ? ? ? F5 03 00 AA F4 4F 03 A9", 0 },
    { "LocalPlayer_tick",     "FF 43 01 D1 FD 7B 02 A9 ? ? ? ? F9 08 00 94", 0 },
    { "Player_move",          "F5 0F 1E F8 ? ? 00 F9 F4 4F 01 A9", 0 },
    { "Player_attack",        "FF 83 00 D1 F5 0F 1E F8 F4 4F 01 A9", 0 },
    { "Player_hurt",          "F6 57 BD A9 F4 4F 01 A9 F5 03 00 AA", 0 },
    { "Actor_setVelocity",    "F5 0F 1E F8 ? ? ? ? F9 F4 4F 01 A9", 1 },
    { "Level_getEntities",    "FF 03 01 D1 F6 57 05 A9 F4 4F 02 A9", 0 },
    { "GameMode_breakBlock",  "FF C3 00 D1 ? ? ? ? F9 F4 4F 01 A9", 0 },
    { "SwapBuffers",          nullptr, 0 },
};

static const Offset kOff121120[] = {
    { "Player_pos",           0x4A0 },
    { "Player_rot",           0x4AC },
    { "Player_vel",           0x4B8 },
    { "Player_health",        0x2D8 },
    { "Player_onGround",      0x1F4 },
    { "Player_flying",        0x6E8 },
    { "Player_selectedSlot",  0x8C0 },
    { "Actor_entity",         0x8   },
    { "Level_entities",       0x2F0 },
    { "Game_localPlayer",     0x9A8 },
    { "Game_level",           0x1B0 },
};

uint32_t detectVersion() {
    // 1.21.120 and 1.21.132 share the same .so layout for what we need,
    // bump this when the offsets stop resolving.
    return 121120;
}

void init(uint32_t mcVersion) {
    s_ver = mcVersion ? mcVersion : detectVersion();
    LOG("initialising sigs for %u", s_ver);

    const Entry*  table = kV121120;
    size_t        count = sizeof(kV121120) / sizeof(Entry);

    for (size_t i = 0; i < count; ++i) {
        if (!table[i].pattern) continue;
        uintptr_t p = mem::scan(kLib, table[i].pattern, table[i].skip);
        if (!p) { LOG("MISS %s", table[i].name); continue; }
        s_addr[table[i].name] = p;
        LOG("ok   %s @ %p", table[i].name, (void*)p);
    }

    for (auto& o : kOff121120) s_off[o.name] = o.value;
}

uintptr_t addr(const char* n) {
    auto it = s_addr.find(n);
    return it == s_addr.end() ? 0 : it->second;
}

size_t off(const char* n) {
    auto it = s_off.find(n);
    return it == s_off.end() ? 0 : it->second;
}

std::string versionTag() { return std::to_string(s_ver); }

}
