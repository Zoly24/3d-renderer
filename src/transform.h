#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "geometry.h"

typedef struct Transform {
    fVec3 position;
    fVec3 rotation;
    fVec3 scale;
    fVec3 movement;
} Transform;

fVec3 create_translation_vec(float, float, float);
fVec3 create_rotation_vec_deg(float, float, float);
fVec3 create_rotation_vec_rad(float, float, float);
fVec3 create_uniform_scale_vec(float);
fVec3 create_scale_vec(float, float, float);

fMatrix44 *create_translation_mat(fVec3);
fMatrix44 *create_rotation_mat(fVec3);
fMatrix44 *create_scale_mat(fVec3);

#endif
