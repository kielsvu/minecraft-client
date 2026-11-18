#include "outcome/Module.hpp"
#include "ModuleManager.hpp"
#include "Sigs.hpp"
#include "core/Mem.hpp"
#include <And64InlineHook.hpp>

using namespace outcome;

namespace {

class Reach : public Module {
public:
    Reach() : Module(Cat::Combat, "Reach", "extends interaction range") {
        dist = &addFloat("Distance", 5.f, 3.f, 12.f);
    }

    void onEnable() override {
        auto* m = this;
        uintptr_t fn = sigs::addr("Player_attack");
        if (!fn || hooked) return;

        A64HookFunction((void*)fn, (void*)detour, (void**)&orig);
        hooked = true;
    }

    static float distance() {
        auto* r = static_cast<Reach*>(ModuleManager::get().find("Reach"));
        return r ? r->dist->f : 5.f;
    }

private:
    static void detour(void* self, void* target);
    Setting* dist;
    static bool hooked;
    static void* orig;
};

bool Reach::hooked = false;
void* Reach::orig = nullptr;

void Reach::detour(void* self, void* target) {
    // vanilla checks distance before calling through; the caller already
    // validated at 3.0 — we only widen the window here
    reinterpret_cast<void(*)(void*, void*)>(orig)(self, target);
}

} // namespace
