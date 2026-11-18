#include "outcome/Module.hpp"
#include "outcome/SDK.hpp"
#include "Sigs.hpp"
#include "core/Mem.hpp"
#include <cmath>

using namespace outcome;

namespace {

class Fly : public Module {
public:
    Fly() : Module(Cat::Movement, "Fly", "creative flight in survival") {
        speed = &addFloat("Speed", 1.f, 0.1f, 5.f);
        vert  = &addFloat("Vertical", 1.f, 0.1f, 3.f);
        glide = &addBool("Glide", false);
    }

    void onEnable() override { savedVy = 0.f; }

    void onTick() override {
        auto* lp = mc::g_game ? mc::g_game->localPlayer() : nullptr;
        if (!lp) return;

        uintptr_t p = reinterpret_cast<uintptr_t>(lp->entity());
        if (!p) return;

        // onGround flag lives at a fixed offset, zero it every tick so
        // gravity never re-engages
        mem::write<uint8_t>(p + sigs::off("Player_onGround"), 0);

        mc::Vec3 v = lp->vel();
        if (glide->b) {
            v.y = -0.05f;
        } else {
            v.y = vert->f * (inputJump() ? 1.f : (inputSneak() ? -1.f : 0.f));
        }

        float yaw = lp->rot().y * 0.0174533f;
        float fwd = inputForward();
        float str = inputStrafe();

        v.x = (-std::sin(yaw) * fwd + std::cos(yaw) * str) * speed->f;
        v.z = ( std::cos(yaw) * fwd + std::sin(yaw) * str) * speed->f;

        lp->setVelocity(v);
        lp->setPos({ lp->pos().x, lp->pos().y + v.y, lp->pos().z });
    }

private:
    static float inputForward();
    static float inputStrafe();
    static bool  inputJump();
    static bool  inputSneak();

    Setting* speed;
    Setting* vert;
    Setting* glide;
    float savedVy = 0.f;
};

} // namespace
