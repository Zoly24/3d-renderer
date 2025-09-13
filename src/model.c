#include <stdio.h>
#include <stdlib.h>

#include "geometry.h"
#include "model.h"

ModelObject *create_model_object(Mesh *mesh) {
    ModelObject *model = (ModelObject *)malloc(sizeof(ModelObject));
    if (!model) {
        printf("Could not allocate mem for model");
        return NULL;
    }

    // Store its own model matrix
    model->model_mat = create_identity_fmatrix44();

    // Store the mesh we generated
    model->mesh = mesh;

    model->transform.position = create_translation_vec(0.0f, 0.0f, 0.0f);
    model->transform.rotation = create_rotation_vec_rad(0.0f, 0.0f, 0.0f);
    model->transform.scale = create_uniform_scale_vec(1.0f);

    update_model_mat(model);
    return model;
}

void update_model_mat(ModelObject *model) {
    fMatrix44 *translation_mat = create_translation_mat(model->transform.position);
    fMatrix44 *rotation_mat = create_rotation_mat(model->transform.rotation);
    fMatrix44 *scale_mat = create_scale_mat(model->transform.scale);

    fMatrix44 *temp = mult_fmatrix44(rotation_mat, scale_mat);

    if (model->model_mat) {
        free(model->model_mat);
    }

    model->model_mat = mult_fmatrix44(translation_mat, temp);

    free(translation_mat);
    free(rotation_mat);
    free(scale_mat);
    free(temp);
}
