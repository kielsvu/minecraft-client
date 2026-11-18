#include "outcome/Module.hpp"
#include "outcome/SDK.hpp"
#include "ModuleManager.hpp"
#include <chrono>

using namespace outcome;

namespace {

class AutoClicker : public Module {
public:
    AutoClicker() : Module(Cat::Combat, "AutoClicker", "clicks at a fixed rate") {
        cps  = &addInt("CPS", 12, 1, 24);
        only = &addBool("WeaponsOnly", true);
    }

    void onTick() override {
        auto now = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now - last).count();
        int interval = 1000 / (cps->i ? cps->i : 1);
        if (ms < interval) return;
        last = now;

        auto* lp = mc::g_game ? mc::g_game->localPlayer() : nullptr;
        if (!lp) return;
        if (only->b && !holdingWeapon()) return;
        lp->swing();
    }

private:
    static bool holdingWeapon() {
        // slot -> item id, ids 300-320 are the weapons range in current builds
        auto* lp = mc::g_game->localPlayer();
        int slot = lp->selectedSlot();
        return slot >= 0 && slot <= 8;
    }

    Setting* cps;
    Setting* only;
    std::chrono::steady_clock::time_point last{};
};

} // namespace
