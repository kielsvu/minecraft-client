#include "outcome/Module.hpp"
#include "outcome/SDK.hpp"
#include "ModuleManager.hpp"
#include "Sigs.hpp"
#include "core/Mem.hpp"
#include <cmath>
#include <vector>

using namespace outcome;

namespace {

struct Cand { mc::Actor a; float d; };

class KillAura : public Module {
public:
    KillAura() : Module(Cat::Combat, "KillAura", "attacks entities in range") {
        range   = &addFloat("Range", 6.f, 2.f, 20.f);
        delay   = &addInt("Delay", 0, 0, 20);
        mult    = &addBool("Multi", false);
        mobs    = &addBool("Mobs", false);
        through = &addBool("Walls", false);
        hurt    = &addBool("HurtTime", true);
        rot     = &addBool("Rotate", true);
        silent  = &addBool("Silent", true);
    }

    void onTick() override {
        auto* game = mc::g_game;
        if (!game) return;
        auto* lp = game->localPlayer();
        auto* lvl = game->level();
        if (!lp || !lvl || !lp->isValid()) return;

        std::vector<Cand> list;
        lvl->eachEntity(&list, [](void* ctx, mc::Actor& a) -> bool {
            auto* out = static_cast<std::vector<Cand>*>(ctx);
            if (!a.isValid()) return true;
            out->push_back({ a, 0.f });
            return true;
        });

        auto* self = static_cast<KillAura*>(this);
        std::vector<Cand> hit;
        for (auto& c : list) {
            if (!c.a.isValid()) continue;
            if (c.a.entity() == lp->entity()) continue;
            if (!self->mobs->b && !c.a.isPlayer()) continue;

            float d = lp->distTo(c.a);
            if (d > self->range->f) continue;
            if (!self->through->b && !canSee(*lp, c.a)) continue;
            if (self->hurt->b && isHurting(c.a)) continue;
            c.d = d;
            hit.push_back(c);
        }

        if (hit.empty()) return;

        std::sort(hit.begin(), hit.end(),
                  [](const Cand& a, const Cand& b) { return a.d < b.d; });

        if (!mult->b) hit.resize(1);

        for (auto& t : hit) {
            if (rot->b) {
                mc::Vec3 r = lookAt(*lp, t.a);
                lp->setRot(r);
            }
            lp->swing();
            attack(t.a);
        }
    }

private:
    static mc::Vec3 lookAt(const mc::LocalPlayer& p, const mc::Actor& t) {
        mc::Vec3 pp = p.pos(), tp = t.pos();
        float dx = tp.x - pp.x;
        float dy = tp.y - (pp.y + 1.62f);
        float dz = tp.z - pp.z;
        float hyp = std::sqrt(dx * dx + dz * dz);
        return { -std::atan2(dy, hyp) * 57.2958f,
                  std::atan2(dz, dx) * 57.2958f - 90.f, 0.f };
    }

    static bool canSee(mc::LocalPlayer& p, mc::Actor& t) {
        // raytrace between eye and target centre; falls back to true if
        // the trace symbol didn't resolve
        static uintptr_t fn = 0;
        if (!fn) fn = sigs::addr("Level_raycast");
        if (!fn) return true;
        return mem::call<bool>(fn, p.entity(), t.entity());
    }

    static bool isHurting(mc::Actor& a) {
        uintptr_t e = reinterpret_cast<uintptr_t>(a.entity());
        if (!e) return false;
        return mem::read<int>(e + 0x340) > 0;
    }

    static void attack(mc::Actor& t) {
        static uintptr_t fn = 0;
        if (!fn) fn = sigs::addr("Player_attack");
        if (!fn) return;
        mem::call<void>(fn, mc::g_game->localPlayer()->entity(),
                        t.entity());
    }

    Setting* range;
    Setting* delay;
    Setting* mult;
    Setting* mobs;
    Setting* through;
    Setting* hurt;
    Setting* rot;
    Setting* silent;
};

} // namespace

void registerCombat();

namespace {
void addKillAura() { ModuleManager::get().registerModule(std::make_unique<KillAura>()); }
}
