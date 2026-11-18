#include "ModuleManager.hpp"
#include "outcome/SDK.hpp"
#include <EGL/egl.h>
#include <And64InlineHook.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <android/log.h>

#define LOG(...) __android_log_print(ANDROID_LOG_INFO, "outcome", __VA_ARGS__)

namespace render {

static EGLBoolean (*oSwap)(EGLDisplay, EGLSurface) = nullptr;
static bool s_imgui = false;
static int  s_w = 0, s_h = 0;

static void drawMenu() {
    auto& mgr = outcome::ModuleManager::get();

    static bool open = true;
    if (!open) return;

    ImGui::SetNextWindowSize(ImVec2(420, 520), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("outcome", &open)) { ImGui::End(); return; }

    static int cat = 0;
    const char* names[] = { "Combat", "Movement", "Visual", "World", "Player", "Misc" };

    ImGui::BeginChild("left", ImVec2(120, 0), true);
    for (int i = 0; i < 6; ++i)
        if (ImGui::Selectable(names[i], cat == i)) cat = i;
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("right", ImVec2(0, 0), true);

    for (auto& m : mgr.all()) {
        if ((int)m->cat != cat) continue;
        ImGui::PushID(m.get());
        if (ImGui::Checkbox(m->name.c_str(), &m->enabled)) {
            m->enabled ? m->onEnable() : m->onDisable();
        }
        if (ImGui::IsItemHovered() && !m->desc.empty())
            ImGui::SetTooltip("%s", m->desc.c_str());

        if (ImGui::TreeNode("cfg")) {
            for (auto& s : m->settings) {
                if (!s.fmax && !s.imax) ImGui::Checkbox(s.name.c_str(), &s.b);
                else if (s.imax)        ImGui::SliderInt(s.name.c_str(), &s.i, s.imin, s.imax);
                else                    ImGui::SliderFloat(s.name.c_str(), &s.f, s.fmin, s.fmax, "%.2f");
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::EndChild();
    ImGui::End();
}

static EGLBoolean hkSwap(EGLDisplay d, EGLSurface s) {
    if (!s_imgui) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::GetIO().IniFilename = nullptr;
        ImGui_ImplOpenGL3_Init("#version 300 es");
        s_imgui = true;
    }

    eglQuerySurface(d, s, EGL_WIDTH,  &s_w);
    eglQuerySurface(d, s, EGL_HEIGHT, &s_h);
    ImGui::GetIO().DisplaySize = ImVec2((float)s_w, (float)s_h);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    drawMenu();
    outcome::ModuleManager::get().renderAll(nullptr);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return oSwap(d, s);
}

void init() {
    // eglSwapBuffers lives in libEGL, not minecraftpe — resolve directly
    void* sym = (void*)eglSwapBuffers;
    A64HookFunction(sym, (void*)hkSwap, (void**)&oSwap);
    LOG("render hooked");
}

} // namespace render
