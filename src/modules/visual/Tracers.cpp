#include "outcome/Module.hpp"
#include "outcome/SDK.hpp"
#include <imgui.h>

using namespace outcome;

namespace {

class Tracers : public Module {
public:
    Tracers() : Module(Cat::Visual, "Tracers", "lines to entities") {
        maxDist = &addFloat("MaxDistance", 128.f, 8.f, 512.f);
    }

    void onRender(void*) override {
        auto* game = mc::g_game;
        if (!game) return;
        auto* lp = game->localPlayer();
        if (!lp) return;

        ImVec2 sz = ImGui::GetIO().DisplaySize;
        ImVec2 origin(sz.x / 2.f, sz.y);

        game->level()->eachEntity(this, [](void* ctx, mc::Actor& a) -> bool {
            auto* m = static_cast<Tracers*>(ctx);
            if (!a.isValid() || !a.isPlayer()) return true;

            float d = mc::g_game->localPlayer()->distTo(a);
            if (d > m->maxDist->f) return true;

            // reuse the ESP projection entry point
            extern bool projectToScreen(const mc::Vec3&, float*, float*);
            float sx = 0, sy = 0;
            if (!projectToScreen(a.pos(), &sx, &sy)) return true;

            ImVec2 sz2 = ImGui::GetIO().DisplaySize;
            ImGui::GetBackgroundDrawList()->AddLine(
                ImVec2(sz2.x / 2.f, sz2.y),
                ImVec2(sx, sy), IM_COL32(80, 200, 255, 200), 1.2f);
            return true;
        });
    }

private:
    Setting* maxDist;
};

} // namespace
