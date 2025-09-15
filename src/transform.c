#include "transform.h"
#include "constants.h"
#include "geometry.h"
#include <stdlib.h>

fVec4 create_translation_vec(float x, float y, float z) {
    // w = 1.0 for a position/point
    return (fVec4){x, y, z, 1.0f};
}
fMatrix44 *create_translation_mat(fVec4 pos) {
    fMatrix44 *mat = create_identity_fmatrix44();
    mat->mat[3][0] = pos.x;
    mat->mat[3][1] = pos.y;
    mat->mat[3][2] = pos.z;

    return mat;
}

fVec4 create_rotation_vec_deg(float xdeg, float ydeg, float zdeg) {
    // w = 0.0 for a direction/rotation
    return (fVec4){xdeg * DEGREES_TO_RADIANS, ydeg * DEGREES_TO_RADIANS, zdeg * DEGREES_TO_RADIANS, 0.0f};
}

fVec4 create_rotation_vec_rad(float xrad, float yrad, float zrad) {
    // w = 0.0 for a direction/rotation
    return (fVec4){xrad, yrad, zrad, 0.0f};
}

fMatrix44 *create_rotation_mat(fVec4 rot) {
    float cosx = cos(rot.x);
    float cosy = cos(rot.y);
    float cosz = cos(rot.z);

    float sinx = sin(rot.x);
    float siny = sin(rot.y);
    float sinz = sin(rot.z);

    fMatrix44 *rx = create_identity_fmatrix44();
    fMatrix44 *ry = create_identity_fmatrix44();
    fMatrix44 *rz = create_identity_fmatrix44();

    rx->mat[1][1] = cosx;
    rx->mat[1][2] = -sinx;
    rx->mat[2][1] = sinx;
    rx->mat[2][2] = cosx;

    ry->mat[0][0] = cosy;
    ry->mat[0][2] = siny;
    ry->mat[2][0] = -siny;
    ry->mat[2][2] = cosy;

    rz->mat[0][0] = cosz;
    rz->mat[0][1] = sinz;
    rz->mat[1][0] = -sinz;
    rz->mat[1][1] = cosz;

    fMatrix44 *temp = mult_fmatrix44(rz, ry);
    fMatrix44 *mat = mult_fmatrix44(temp, rx);

    free(temp);
    free(rz);
    free(ry);
    free(rx);

    return mat;
}

fVec4 create_uniform_scale_vec(float s) {
    // w = 1.0 for scaling
    return (fVec4){s, s, s, 1.0f};
}

fVec4 create_scale_vec(float x, float y, float z) {
    // w = 1.0 for scaling
    return (fVec4){x, y, z, 1.0f};
}

fMatrix44 *create_scale_mat(fVec4 s) {
    fMatrix44 *mat = create_identity_fmatrix44();

    mat->mat[0][0] *= s.x;
    mat->mat[1][1] *= s.y;
    mat->mat[2][2] *= s.z;

    return mat;
}
