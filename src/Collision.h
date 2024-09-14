#pragma once
#include "Math.h"

RMAPI void NearestCirclePoints(
    Vector2 pos1, Vector2 dir1, float rad1, float hh1,
    Vector2 pos2, Vector2 dir2, float rad2, float hh2,
    Vector2& near1, Vector2& near2);

// mtv resolves 1 from 2
RMAPI bool CircleCircle(
    Vector2 pos1, float rad1,
    Vector2 pos2, float rad2,
    Vector2* mtv = nullptr)
{
    float rad = rad1 + rad2;
    bool collision = DistanceSqr(pos1, pos2) <= rad * rad;
    if (collision && mtv != nullptr)
    {
        *mtv = Normalize(pos1 - pos2) * (rad - Distance(pos1, pos2));
    }
    return collision;
}

// mtv resolves 1 from 2
RMAPI bool CircleCapsule(
    Vector2 pos1, float rad1,
    Vector2 pos2, Vector2 dir2, float rad2, float hh2,
    Vector2* mtv = nullptr)
{
    Vector2 top2 = pos2 + dir2 * hh2;
    Vector2 bot2 = pos2 - dir2 * hh2;
    Vector2 proj = ProjectPointLine(top2, bot2, pos1);
    return CircleCircle(pos1, rad1, proj, rad2, mtv);
}

// mtv resolves 1 from 2
RMAPI bool CapsuleCapsule(
    Vector2 pos1, Vector2 dir1, float rad1, float hh1,
    Vector2 pos2, Vector2 dir2, float rad2, float hh2,
    Vector2* mtv = nullptr)
{
    Vector2 near1, near2;
    NearestCirclePoints(pos1, dir1, rad1, hh1, pos2, dir2, rad2, hh2, near1, near2);
    return CircleCircle(near1, rad1, near2, rad2, mtv);
}

// mtv resolves circle from plane
RMAPI bool CirclePlane(Vector2 circle, float radius, Vector2 plane, Vector2 normal, Vector2* mtv = nullptr)
{
    float distance = Dot(circle - plane, normal);
    bool collision = distance <= radius;
    if (collision && mtv != nullptr)
        *mtv = normal * (radius - distance);
    return collision;
}

// mtv resolves circle from rectangle
RMAPI bool CircleRectangle(Vector2 circle, float radius, Vector2 rect, Vector2 extents, Vector2* mtv = nullptr)
{
    Vector2 nearest = Clamp(circle, rect - extents, rect + extents);
    return CirclePlane(circle, radius, nearest, Normalize(circle - nearest), mtv);
}

// mtv resolves capsule from rectangle
RMAPI bool CapsuleRectangle(Vector2 cap, Vector2 dir, float radius, float hh, Vector2 rect, Vector2 extents, Vector2* mtv = nullptr)
{
    Vector2 top = cap + dir * hh;
    Vector2 bot = cap - dir * hh;
    Vector2 proj = ProjectPointLine(top, bot, rect);
    Vector2 nearest = Clamp(proj, rect - extents, rect + extents);

    bool collision = DistanceSqr(proj, nearest) <= radius * radius;
    if (collision && mtv != nullptr)
    {
        float depth = radius - Distance(proj, nearest);
        *mtv = Normalize(proj - nearest) * depth;
    }

    return collision;
}

RMAPI void NearestCirclePoints(
    Vector2 pos1, Vector2 dir1, float rad1, float hh1,
    Vector2 pos2, Vector2 dir2, float rad2, float hh2,
    Vector2& near1, Vector2& near2)
{
    Vector2 top1 = pos1 + dir1 * hh1;
    Vector2 bot1 = pos1 - dir1 * hh1;
    Vector2 top2 = pos2 + dir2 * hh2;
    Vector2 bot2 = pos2 - dir2 * hh2;

    Vector2 lines[4]
    {
        top2 - top1,
        bot2 - top1,
        top2 - bot1,
        bot2 - bot1,
    };

    size_t min = 0;
    for (size_t i = 1; i < 4; i++)
    {
        if (LengthSqr(lines[i]) < LengthSqr(lines[min]))
            min = i;
    }

    // top1 = 1st half of array, bot1 = 2nd half of array
    near1 = min < 2 ? top1 : bot1;
    near2 = ProjectPointLine(bot2, top2, near1); // Project 1 onto 2
    near1 = ProjectPointLine(bot1, top1, near2); // Project 2 onto 1
}
