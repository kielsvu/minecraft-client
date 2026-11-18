#pragma once
#include <cstdint>

namespace mc {

struct Vec3 { float x, y, z; };

struct Actor {
    void*  self;
    Vec3   pos();
    Vec3   rot();
    float  health();
    float  distTo(const Actor& o);
    bool   isValid();
    bool   isPlayer();
    bool   isMob();
    void*  entity();
};

struct LocalPlayer : Actor {
    void swing();
    void setPos(const Vec3& v);
    void setRot(const Vec3& r);
    void setVelocity(const Vec3& v);
    int  selectedSlot();
    void selectSlot(int s);
    float moveSpeed();
};

struct Level {
    void  eachEntity(void* ctx, bool(*cb)(void*, Actor&));
};

struct Game {
    LocalPlayer* localPlayer();
    Level*       level();
    void*        clientInstance();
};

extern Game* g_game;

}
