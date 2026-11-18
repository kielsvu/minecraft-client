#include "outcome/Module.hpp"
#include "outcome/SDK.hpp"
#include <chrono>

using namespace outcome;

namespace {

class Bhop : public Module {
public:
    Bhop() : Module(Cat::Movement, "Bhop", "auto jump on ground contact") {
        rate = &addInt("Rate", 3, 1, 10);
    }

    void onTick() override {
        auto* lp = mc::g_game ? mc::g_game->localPlayer() : nullptr;
        if (!lp) return;
        uintptr_t e = reinterpret_cast<uintptr_t>(lp->entity());
        if (!*reinterpret_cast<uint8_t*>(e + 0x1F4)) return;

        auto now = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count();
        if (ms < 1000 / rate->i) return;
        last = now;

        mc::Vec3 v = lp->vel();
        v.y = 0.42f;
        lp->setVelocity(v);
    }
private:
    Setting* rate;
    std::chrono::steady_clock::time_point last{};
};

} // namespace
