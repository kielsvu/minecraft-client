#include "outcome/Module.hpp"
#include "outcome/SDK.hpp"

using namespace outcome;

namespace {

class Speed : public Module {
public:
    Speed() : Module(Cat::Movement, "Speed", "horizontal velocity multiplier") {
        mult = &addFloat("Multiplier", 1.6f, 1.f, 4.f);
        only = &addBool("GroundOnly", true);
    }

    void onTick() override {
        auto* lp = mc::g_game ? mc::g_game->localPlayer() : nullptr;
        if (!lp) return;

        uintptr_t e = reinterpret_cast<uintptr_t>(lp->entity());
        bool ground = *reinterpret_cast<uint8_t*>(e + 0x1F4) != 0;
        if (only->b && !ground) return;

        mc::Vec3 v = lp->vel();
        v.x *= mult->f;
        v.z *= mult->f;
        lp->setVelocity(v);
    }

private:
    Setting* mult;
    Setting* only;
};

} // namespace
