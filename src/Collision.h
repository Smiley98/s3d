#pragma once
#include "Math.h"

bool CircleCircle(Vector2 position1, Vector2 position2, float radius1, float radius2, Vector2* mtv = nullptr)
{
    float distance = Distance(position1, position2);
    float radiiSum = radius1 + radius2;
    bool collision = distance <= radiiSum;
    if (collision && mtv != nullptr)
    {
        *mtv = Normalize(position1 - position2) * (radiiSum - distance);
    }
    return collision;
}
