#include "transform.h"
#include "constants.h"
#include "geometry.h"
#include <stdlib.h>

fVec3 create_translation_vec(float x, float y, float z) {
    return (fVec3){x, y, z};
}
fMatrix44 *create_translation_mat(fVec3 pos) {
    fMatrix44 *mat = create_identity_fmatrix44();
    mat->mat[3][0] = pos.x;
    mat->mat[3][1] = pos.y;
    mat->mat[3][2] = pos.z;

    return mat;
}

fVec3 create_rotation_vec_deg(float xdeg, float ydeg, float zdeg) {
    return (fVec3){xdeg * DEGREES_TO_RADIANS, ydeg * DEGREES_TO_RADIANS, zdeg * DEGREES_TO_RADIANS};
}

fVec3 create_rotation_vec_rad(float xrad, float yrad, float zrad) {
    return (fVec3){xrad, yrad, zrad};
}

fMatrix44 *create_rotation_mat(fVec3 rot) {
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

fVec3 create_uniform_scale_vec(float s) {
    return (fVec3){s, s, s};
}

fVec3 create_scale_vec(float x, float y, float z) {
    return (fVec3){x, y, z};
}

fMatrix44 *create_scale_mat(fVec3 s) {
    fMatrix44 *mat = create_identity_fmatrix44();

    mat->mat[0][0] *= s.x;
    mat->mat[1][1] *= s.y;
    mat->mat[2][2] *= s.z;

    return mat;
}
