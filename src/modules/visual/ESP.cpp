#include "outcome/Module.hpp"
#include "outcome/SDK.hpp"
#include "Sigs.hpp"
#include "core/Mem.hpp"
#include <imgui.h>
#include <cmath>

using namespace outcome;

namespace {

struct ScreenPt { bool ok; float x, y; };

// world -> screen via the game's view projection matrix
static ScreenPt project(const mc::Vec3& world) {
    static uintptr_t fn = 0;
    if (!fn) fn = sigs::addr("ClientInstance_worldToScreen");
    if (!fn) return { false, 0, 0 };

    float out[2] = { 0, 0 };
    bool ok = mem::call<bool>(fn, mc::g_game->clientInstance(),
                              world.x, world.y, world.z, out);
    return { ok, out[0], out[1] };
}

class ESP : public Module {
public:
    ESP() : Module(Cat::Visual, "ESP", "entity boxes and health") {
        players = &addBool("Players", true);
        mobs    = &addBool("Mobs", false);
        boxes   = &addBool("Boxes", true);
        health  = &addBool("Health", true);
        names   = &addBool("Names", true);
        maxDist = &addFloat("MaxDistance", 128.f, 8.f, 512.f);
    }

    void onRender(void*) override {
        auto* game = mc::g_game;
        if (!game) return;
        auto* lp = game->localPlayer();
        auto* lvl = game->level();
        if (!lp || !lvl) return;

        lvl->eachEntity(this, [](void* ctx, mc::Actor& a) -> bool {
            auto* m = static_cast<ESP*>(ctx);
            if (!a.isValid()) return true;
            if (a.entity() == mc::g_game->localPlayer()->entity()) return true;
            if (!m->players->b && a.isPlayer()) return true;
            if (!m->mobs->b && a.isMob()) return true;
            if (a.isPlayer() == false && a.isMob() == false) return true;

            float d = mc::g_game->localPlayer()->distTo(a);
            if (d > m->maxDist->f) return true;

            mc::Vec3 p = a.pos();
            ScreenPt head = project({ p.x, p.y + 1.8f, p.z });
            ScreenPt foot = project({ p.x, p.y, p.z });
            if (!head.ok || !foot.ok) return true;

            float h = foot.y - head.y;
            float w = h * 0.45f;
            if (m->boxes->b)
                ImGui::GetBackgroundDrawList()->AddRect(
                    ImVec2(head.x - w, head.y), ImVec2(head.x + w, foot.y),
                    IM_COL32(255, 60, 60, 220), 0.f, 0, 1.4f);

            if (m->health->b) {
                float hp = a.health() / 20.f;
                ImGui::GetBackgroundDrawList()->AddRectFilled(
                    ImVec2(head.x - w, head.y - 6),
                    ImVec2(head.x - w + (w * 2 * hp), head.y - 3),
                    IM_COL32(60, 220, 90, 220));
            }

            if (m->names->b) {
                char buf[64];
                snprintf(buf, sizeof(buf), "%s [%.0fm]", a.isPlayer() ? "player" : "mob", d);
                ImGui::GetBackgroundDrawList()->AddText(
                    ImVec2(head.x - w, head.y - 18), IM_COL32(255, 255, 255, 255), buf);
            }
            return true;
        });
    }

private:
    Setting* players;
    Setting* mobs;
    Setting* boxes;
    Setting* health;
    Setting* names;
    Setting* maxDist;
};

} // namespace
