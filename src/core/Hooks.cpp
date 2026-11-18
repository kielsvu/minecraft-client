#include "Hooks.hpp"
#include "Sigs.hpp"
#include "ModuleManager.hpp"
#include "outcome/SDK.hpp"
#include <And64InlineHook.hpp>
#include <android/log.h>

#define LOG(...) __android_log_print(ANDROID_LOG_INFO, "outcome", __VA_ARGS__)

namespace hooks {

static std::unordered_map<std::string, Hook*> s_hooks;

bool install(const char* name, void* target, void* detour, void** original) {
    if (!target || !detour) { LOG("bad hook %s", name); return false; }
    if (s_hooks.count(name)) return true;

    A64HookFunction(target, detour, original);

    auto* h = new Hook{ name, target, detour, original };
    s_hooks[name] = h;
    LOG("hooked %s @ %p -> %p", name, target, detour);
    return true;
}

bool remove(const char* name) {
    auto it = s_hooks.find(name);
    if (it == s_hooks.end()) return false;
    // And64InlineHook has no public unhook; keep the trampoline parked
    // and clear the map so callers stop treating it as live.
    delete it->second;
    s_hooks.erase(it);
    return true;
}

// ---- detours -------------------------------------------------------------

using LevelTickFn = void(*)(void* level);
static LevelTickFn oLevelTick = nullptr;

static void hkLevelTick(void* level) {
    oLevelTick(level);
    outcome::ModuleManager::get().tickAll();
}

using MoveFn = void(*)(void* player, void* input, float dt);
static MoveFn oMove = nullptr;

static void hkPlayerMove(void* player, void* input, float dt) {
    if (auto* m = outcome::ModuleManager::get().find("Fly"); m && m->enabled) {
        // fly owns the vertical axis, vanilla move still runs for ground collision
        oMove(player, input, dt);
        return;
    }
    oMove(player, input, dt);
}

using HurtFn = void(*)(void* actor, void* src, float* damage);
static HurtFn oHurt = nullptr;

static void hkPlayerHurt(void* actor, void* src, float* damage) {
    if (auto* m = outcome::ModuleManager::get().find("NoFall"); m && m->enabled) {
        if (*damage > 3.f) { *damage = 0.f; return; }
    }
    oHurt(actor, src, damage);
}

using VelocityFn = void(*)(void* actor, float* vx, float* vy, float* vz);
static VelocityFn oVelocity = nullptr;

static void hkSetVelocity(void* actor, float* vx, float* vy, float* vz) {
    if (auto* m = outcome::ModuleManager::get().find("Velocity"); m && m->enabled) {
        for (auto& s : m->settings) {
            if (s.name == "Horizontal") { *vx *= (1.f - s.f); *vz *= (1.f - s.f); }
            if (s.name == "Vertical")   { *vy *= (1.f - s.f); }
        }
    }
    oVelocity(actor, vx, vy, vz);
}

// ---- boot ----------------------------------------------------------------

void installAll() {
    auto hook = [](const char* sig, void* detour, void** orig) {
        uintptr_t a = sigs::addr(sig);
        if (!a) { LOG("no addr for %s", sig); return; }
        install(sig, reinterpret_cast<void*>(a), detour, orig);
    };

    hook("Level_tick",        (void*)hkLevelTick,    (void**)&oLevelTick);
    hook("Player_move",       (void*)hkPlayerMove,   (void**)&oMove);
    hook("Player_hurt",       (void*)hkPlayerHurt,   (void**)&oHurt);
    hook("Actor_setVelocity", (void*)hkSetVelocity,  (void**)&oVelocity);
}

void removeAll() {
    s_hooks.clear();
}

} // namespace hooks
