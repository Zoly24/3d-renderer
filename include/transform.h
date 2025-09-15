#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "geometry.h"

typedef struct Transform {
    fVec4 position;
    fVec4 rotation;
    fVec4 scale;
    fVec4 movement;
} Transform;

fVec4 create_translation_vec(float, float, float);
fVec4 create_rotation_vec_deg(float, float, float);
fVec4 create_rotation_vec_rad(float, float, float);
fVec4 create_uniform_scale_vec(float);
fVec4 create_scale_vec(float, float, float);

fMatrix44 *create_translation_mat(fVec4);
fMatrix44 *create_rotation_mat(fVec4);
fMatrix44 *create_scale_mat(fVec4);

#endif
