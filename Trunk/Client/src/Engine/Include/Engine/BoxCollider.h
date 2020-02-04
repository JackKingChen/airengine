#ifndef __BoxCollider_H__
#define __BoxCollider_H__

#include <float.h>
#include <vector>
#include "Collision.h"

int BoxBoxCollision(BaseShape* box1, BaseShape* box2, AIRVector<SContact>& vtContact);

#endif