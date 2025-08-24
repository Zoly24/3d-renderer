#ifndef CAMERA_H
#define CAMERA_H

#include "constants.h"
#include "geometry.h"
#include "transform.h"

typedef struct Frustum {
    fPlane planes[NUM_FRUSTUM_PLANES];
} CameraFrustum;

typedef struct CameraSettings {
    float fov;
    float aspect_ratio;

    float near;
    float far;
    float left;
    float right;
    float bottom;
    float top;
} CameraSettings;

typedef struct Camera {
    fVec3 *camera_position;
    fVec3 *camera_front;
    fVec3 *camera_up;
    fVec3 *direction;

    fMatrix44 *camera_mat;
    fMatrix44 *projection_mat;

    CameraFrustum frustum;
    CameraSettings settings;
} UserCamera;

UserCamera *create_camera(void);
void camera_look_at(UserCamera *, fVec3 *, fVec3 *, fVec3 *);
void camera_look_at_front(UserCamera *, fVec3 *, fVec3 *, fVec3 *);
void update_projection_mat(UserCamera *);
void update_frustum_planes(UserCamera *);
void perspective(float *, float *, float *, float *, float *, float *, float *, float *);
void frustum(float *, float *, float *, float *, float *, float *, fMatrix44 *);

#endif
