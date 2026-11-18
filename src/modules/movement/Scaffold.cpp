#include "outcome/Module.hpp"
#include "outcome/SDK.hpp"
#include "Sigs.hpp"
#include "core/Mem.hpp"
#include <cmath>

using namespace outcome;

namespace {

class Scaffold : public Module {
public:
    Scaffold() : Module(Cat::Movement, "Scaffold", "places blocks under feet") {
        extend = &addFloat("Extend", 2.f, 0.f, 5.f);
        tower  = &addBool("Tower", true);
    }

    void onTick() override {
        auto* lp = mc::g_game ? mc::g_game->localPlayer() : nullptr;
        if (!lp) return;

        int slot = findBlockSlot();
        if (slot < 0) return;

        mc::Vec3 p = lp->pos();
        float yaw = lp->rot().y * 0.0174533f;

        float ox = -std::sin(yaw) * (extend->f + 0.3f);
        float oz =  std::cos(yaw) * (extend->f + 0.3f);

        place(floorf(p.x + ox), floorf(p.y) - 1.f, floorf(p.z + oz));
        if (tower->b && inputJump()) {
            lp->setVelocity({ 0.f, 0.42f, 0.f });
            place(floorf(p.x), floorf(p.y) - 1.f, floorf(p.z));
        }
    }

private:
    static int  findBlockSlot();
    static void place(float x, float y, float z);
    static bool inputJump();

    Setting* extend;
    Setting* tower;
};

} // namespace
