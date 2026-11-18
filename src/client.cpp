#include "outcome/Module.hpp"
#include "ModuleManager.hpp"
#include "Hooks.hpp"
#include "Sigs.hpp"
#include "Mem.hpp"
#include "outcome/SDK.hpp"
#include <android/log.h>
#include <unistd.h>

#define LOG(...) __android_log_print(ANDROID_LOG_INFO, "outcome", __VA_ARGS__)

namespace mc {
Game* g_game = nullptr;
}

extern void registerCombat();
extern void registerMovement();
extern void registerVisual();
extern void registerWorld();
extern void registerPlayer();
extern void render_init();
extern void resolveGame();

void registerCombat() {
    extern void addKillAura();
    extern void addVelocity();
    extern void addReach();
    extern void addAutoClicker();
    // each translation unit exposes its own add, keep this file dumb
}

__attribute__((constructor))
static void boot() {
    LOG("outcome loading");

    sigs::init(0);
    resolveGame();

    outcome::ModuleManager::get().initAll();
    hooks::installAll();
    render_init();

    outcome::ModuleManager::get().load("/sdcard/outcome.cfg");
    LOG("outcome ready, %zu modules", outcome::ModuleManager::get().all().size());
}

__attribute__((destructor))
static void shutdown() {
    outcome::ModuleManager::get().save("/sdcard/outcome.cfg");
    hooks::removeAll();
    LOG("outcome unloaded");
}
