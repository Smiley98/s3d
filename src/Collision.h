#pragma once
#include "Math.h"
// All formulas for circle, capsule, AABB, and infinite plane collision!
// No need to add lines or polygons. SAT is overkill for this game.

// Overlap along a single axis
RMAPI bool Overlaps(float min1, float max1, float min2, float max2)
{
    return !((max1 < min2) || (max2 < min1));
}

// Outputs nearest two points along line top1_bot1 and line top2_bot2
RMAPI void NearestPoints(
    Vector2 top1, Vector2 bot1,
    Vector2 top2, Vector2 bot2,
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
    Vector2 top1 = pos1 + dir1 * hh1;
    Vector2 bot1 = pos1 - dir1 * hh1;
    Vector2 top2 = pos2 + dir2 * hh2;
    Vector2 bot2 = pos2 - dir2 * hh2;
    Vector2 near1, near2;
    NearestPoints(top1, bot1, top2, bot2, near1, near2);
    return CircleCircle(near1, rad1, near2, rad2, mtv);
}

// mtv resolves circle from plane
RMAPI bool CirclePlane(
    Vector2 circle, float radius,
    Vector2 plane, Vector2 normal,
    Vector2* mtv = nullptr)
{
    float distance = Dot(circle - plane, normal);
    bool collision = distance <= radius;
    if (collision && mtv != nullptr)
        *mtv = normal * (radius - distance);
    return collision;
}

// mtv resolves capsule from plane ***unimplemented***
RMAPI bool CapsulePlane(
    Vector2 cap, Vector2 dir, float radius, float hh,
    Vector2 plane, Vector2 normal,
    Vector2* mtv = nullptr)
{
    return false;
}

// mtv resolves rectangle from plane
RMAPI bool RectanglePlane(
    Vector2 rect, Vector2 extents,
    Vector2 plane, Vector2 normal,
    Vector2* mtv = nullptr)
{
    Vector2 near = Clamp(rect - normal * LengthSqr(extents), rect - extents, rect + extents);
    return CirclePlane(near, 1.0f, plane, normal, mtv);
}

// mtv resolves circle from rectangle
RMAPI bool CircleRectangle(
    Vector2 circle, float radius,
    Vector2 rect, Vector2 extents,
    Vector2* mtv = nullptr)
{
    Vector2 near = Clamp(circle, rect - extents, rect + extents);
    return CircleCircle(circle, radius, near, 1.0f, mtv);
}

// mtv resolves capsule from rectangle
RMAPI bool CapsuleRectangle(
    Vector2 cap, Vector2 dir, float radius, float hh,
    Vector2 rect, Vector2 extents,
    Vector2* mtv = nullptr)
{
    Vector2 top = cap + dir * hh;
    Vector2 bot = cap - dir * hh;
    Vector2 proj = ProjectPointLine(top, bot, rect);
    Vector2 near = Clamp(proj, rect - extents, rect + extents);
    return CircleCircle(proj, radius, near, 1.0f, mtv);
}

// mtv resolves 1 from 2
RMAPI bool RectangleRectangle(
    Vector2 rect1, Vector2 extents1,
    Vector2 rect2, Vector2 extents2,
    Vector2* mtv = nullptr)
{
    float xMin1 = rect1.x - extents1.x;
    float xMax1 = rect1.x + extents1.x;
    float yMin1 = rect1.y - extents1.y;
    float yMax1 = rect1.y + extents1.y;

    float xMin2 = rect2.x - extents2.x;
    float xMax2 = rect2.x + extents2.x;
    float yMin2 = rect2.y - extents2.y;
    float yMax2 = rect2.y + extents2.y;

    bool collision =
        Overlaps(xMin1, xMax1, xMin2, xMax2) &&
        Overlaps(yMin1, yMax1, yMin2, yMax2);
    if (collision && mtv != nullptr)
    {
        // Overlapping area (rectangle)
        float xMin = fmaxf(xMin1, xMin2);
        float xMax = fminf(xMax1, xMax2);
        float yMin = fmaxf(yMin1, yMin2);
        float yMax = fminf(yMax1, yMax2);

        // Resolve along the shortest axis (x vs y)
        float x = xMax - xMin;
        float y = yMax - yMin;
        if (x < y)
        {
            *mtv = { x, 0.0f };
            if (rect2.x > xMin) mtv->x *= -1.0f;
        }
        else if (y < x)
        {
            *mtv = { 0.0f, y };
            if (rect2.y > yMin) mtv->y *= -1.0f;
        }
        else
        {
            *mtv = { x, y };
            if (rect2.x > xMin) mtv->x *= -1.0f;
            if (rect2.y > yMin) mtv->y *= -1.0f;
        }
    }
    return collision;
}

RMAPI void NearestPoints(
    Vector2 top1, Vector2 bot1,
    Vector2 top2, Vector2 bot2,
    Vector2& near1, Vector2& near2)
{
    Vector2 A = ProjectPointLine(top2, bot2, top1);
    Vector2 B = ProjectPointLine(top2, bot2, bot1);
    Vector2 C = ProjectPointLine(top1, bot1, A);
    Vector2 D = ProjectPointLine(top1, bot1, B);
    near1 = LengthSqr(A - C) < LengthSqr(B - D) ? C : D;

    Vector2 E = ProjectPointLine(top1, bot1, top2);
    Vector2 F = ProjectPointLine(top1, bot1, bot2);
    Vector2 G = ProjectPointLine(top2, bot2, E);
    Vector2 H = ProjectPointLine(top2, bot2, F);
    near2 = LengthSqr(E - G) < LengthSqr(F - H) ? G : H;
}

// mtv resolves capsule from line
// I feel like this needs 4 points (nearest-circles) instead of 2...
//RMAPI bool LineCapsule(Vector2 p0, Vector2 p1, Vector2 cap, Vector2 dir, float rad, float hh, Vector2* mtv = nullptr)
//{
//    Vector2 top = cap + dir * hh;
//    Vector2 bot = cap - dir * hh;
//    Vector2 projTop = ProjectPointLine(p0, p1, top);
//    Vector2 projBot = ProjectPointLine(p0, p1, bot);
//    Vector2 projCapsuleLine = DistanceSqr(top, projTop) < DistanceSqr(bot, projBot) ? projTop : projBot;
//    Vector2 projLineCapsule = ProjectPointLine(top, bot, projCapsuleLine);
//    return CircleCircle(projCapsuleLine, 1.0f, projLineCapsule, rad, mtv);
//}
