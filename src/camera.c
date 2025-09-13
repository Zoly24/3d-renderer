#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camera.h"
#include "constants.h"
#include "geometry.h"

UserCamera *create_camera() {
    UserCamera *camera = (UserCamera *)malloc(sizeof(UserCamera));
    if (!camera) {
        printf("Could not allocate mem for camera");
        return NULL;
    }

    camera->camera_mat = create_identity_fmatrix44();
    camera->projection_mat = create_identity_fmatrix44();

    camera->settings.fov = FIELD_OF_VIEW;
    camera->settings.near = NEAR_FRUSTUM;
    camera->settings.far = FAR_FRUSTUM;
    camera->settings.aspect_ratio = SCREEN_WIDTH / (float)SCREEN_HEIGHT;

    perspective(&camera->settings.fov,
                &camera->settings.aspect_ratio,
                &camera->settings.near,
                &camera->settings.far,
                &camera->settings.bottom,
                &camera->settings.top,
                &camera->settings.left,
                &camera->settings.right);

    update_projection_mat(camera);
    update_frustum_planes(camera);
    return camera;
}

void camera_look_at(UserCamera *camera, fVec3 *eye, fVec3 *target, fVec3 *up) {

    // Find direction camera is looking at
    fVec3 *camera_direction = sub_fvec3(eye, target);
    normalize_fvec3(camera_direction);

    // Generate the right (x) of the camera
    fVec3 *camera_right = cross_fvec3(up, camera_direction);
    normalize_fvec3(camera_right);

    // Genearete the up (y) of the camera
    fVec3 *camera_up = cross_fvec3(camera_direction, camera_right);
    normalize_fvec3(camera_up);

    fMatrix44 *translation_mat = create_identity_fmatrix44();
    translation_mat->mat[3][0] = -eye->x;
    translation_mat->mat[3][1] = -eye->y;
    translation_mat->mat[3][2] = -eye->z;

    fMatrix44 *rotation_mat = create_identity_fmatrix44();
    rotation_mat->mat[0][0] = camera_right->x;
    rotation_mat->mat[1][0] = camera_right->y;
    rotation_mat->mat[2][0] = camera_right->z;

    rotation_mat->mat[0][1] = camera_up->x;
    rotation_mat->mat[1][1] = camera_up->y;
    rotation_mat->mat[2][1] = camera_up->z;

    rotation_mat->mat[0][2] = camera_direction->x;
    rotation_mat->mat[1][2] = camera_direction->y;
    rotation_mat->mat[2][2] = camera_direction->z;

    if (camera->camera_mat) {
        free(camera->camera_mat);
    }

    camera->camera_mat = mult_fmatrix44(translation_mat, rotation_mat);

    free(translation_mat);
    free(rotation_mat);

    free(camera_direction);
    free(camera_up);
    free(camera_right);
}

void camera_look_at_front(UserCamera *camera, fVec3 *eye, fVec3 *front, fVec3 *up) {
    fVec3 *target = add_fvec3(eye, front);
    camera_look_at(camera, eye, target, up);
    free(target);
}

void update_projection_mat(UserCamera *camera) {
    if (camera->projection_mat) {
        free(camera->projection_mat);
    }

    camera->projection_mat = create_identity_fmatrix44();
    frustum(&camera->settings.bottom, &camera->settings.top,
            &camera->settings.left, &camera->settings.right,
            &camera->settings.near, &camera->settings.far,
            camera->projection_mat);
}

void update_frustum_planes(UserCamera *camera) {
    fMatrix44 *view_projection_mat = mult_fmatrix44(camera->camera_mat, camera->projection_mat);

    // ROW ORDER (NOT COLUMN ORDER LIKE OPENGL)
    camera->frustum.planes[LEFT_PLANE].a = view_projection_mat->mat[0][3] + view_projection_mat->mat[0][0];
    camera->frustum.planes[LEFT_PLANE].b = view_projection_mat->mat[1][3] + view_projection_mat->mat[1][0];
    camera->frustum.planes[LEFT_PLANE].c = view_projection_mat->mat[2][3] + view_projection_mat->mat[2][0];
    camera->frustum.planes[LEFT_PLANE].d = view_projection_mat->mat[3][3] + view_projection_mat->mat[3][0];

    camera->frustum.planes[RIGHT_PLANE].a = view_projection_mat->mat[0][3] - view_projection_mat->mat[0][0];
    camera->frustum.planes[RIGHT_PLANE].b = view_projection_mat->mat[1][3] - view_projection_mat->mat[1][0];
    camera->frustum.planes[RIGHT_PLANE].c = view_projection_mat->mat[2][3] - view_projection_mat->mat[2][0];
    camera->frustum.planes[RIGHT_PLANE].d = view_projection_mat->mat[3][3] - view_projection_mat->mat[3][0];

    camera->frustum.planes[BOTTOM_PLANE].a = view_projection_mat->mat[0][3] + view_projection_mat->mat[0][1];
    camera->frustum.planes[BOTTOM_PLANE].b = view_projection_mat->mat[1][3] + view_projection_mat->mat[1][1];
    camera->frustum.planes[BOTTOM_PLANE].c = view_projection_mat->mat[2][3] + view_projection_mat->mat[2][1];
    camera->frustum.planes[BOTTOM_PLANE].d = view_projection_mat->mat[3][3] + view_projection_mat->mat[3][1];

    camera->frustum.planes[TOP_PLANE].a = view_projection_mat->mat[0][3] - view_projection_mat->mat[0][1];
    camera->frustum.planes[TOP_PLANE].b = view_projection_mat->mat[1][3] - view_projection_mat->mat[1][1];
    camera->frustum.planes[TOP_PLANE].c = view_projection_mat->mat[2][3] - view_projection_mat->mat[2][1];
    camera->frustum.planes[TOP_PLANE].d = view_projection_mat->mat[3][3] - view_projection_mat->mat[3][1];

    camera->frustum.planes[NEAR_PLANE].a = view_projection_mat->mat[0][3] + view_projection_mat->mat[0][2];
    camera->frustum.planes[NEAR_PLANE].b = view_projection_mat->mat[1][3] + view_projection_mat->mat[1][2];
    camera->frustum.planes[NEAR_PLANE].c = view_projection_mat->mat[2][3] + view_projection_mat->mat[2][2];
    camera->frustum.planes[NEAR_PLANE].d = view_projection_mat->mat[3][3] + view_projection_mat->mat[3][2];

    camera->frustum.planes[FAR_PLANE].a = view_projection_mat->mat[0][3] - view_projection_mat->mat[0][2];
    camera->frustum.planes[FAR_PLANE].b = view_projection_mat->mat[1][3] - view_projection_mat->mat[1][2];
    camera->frustum.planes[FAR_PLANE].c = view_projection_mat->mat[2][3] - view_projection_mat->mat[2][2];
    camera->frustum.planes[FAR_PLANE].d = view_projection_mat->mat[3][3] - view_projection_mat->mat[3][2];
}

void perspective(float *angle_of_view, float *aspect_ratio, float *near, float *far, float *bottom, float *top, float *left, float *right) {
    float scale = tan((*angle_of_view) * 0.5 * M_PI / 180) * (*near);
    *right = (*aspect_ratio) * scale;
    *left = -(*right);
    *top = scale;
    *bottom = -(*top);
}

void frustum(float *bottom, float *top, float *left, float *right, float *near, float *far, fMatrix44 *mat) {
    mat->mat[0][0] = 2 * (*near) / ((*right) - (*left));
    mat->mat[0][1] = 0;
    mat->mat[0][2] = 0;
    mat->mat[0][3] = 0;

    mat->mat[1][0] = 0;
    mat->mat[1][1] = 2 * (*near) / ((*top) - (*bottom));
    mat->mat[1][2] = 0;
    mat->mat[1][3] = 0;

    mat->mat[2][0] = ((*right) + (*left)) / ((*right) - (*left));
    mat->mat[2][1] = ((*top) + (*bottom)) / ((*top) - (*bottom));
    mat->mat[2][2] = -((*far) + (*near)) / ((*far) - (*near));
    mat->mat[2][3] = -1;

    mat->mat[3][0] = 0;
    mat->mat[3][1] = 0;
    mat->mat[3][2] = -2 * (*far) * (*near) / ((*far) - (*near));
    mat->mat[3][3] = 0;
}
