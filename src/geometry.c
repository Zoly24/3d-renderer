#include <linux/limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "geometry.h"

fVec4 *create_fvec4(float x, float y, float z, float w) {
    fVec4 *vector = (fVec4 *)malloc(sizeof(fVec4));
    if (!vector) {
        printf("Unable to create fvec4");
        return NULL;
    }

    vector->x = x;
    vector->y = y;
    vector->z = z;
    vector->w = w;

    return vector;
}

float calculate_length_fvec4(fVec4 *v) {
    return sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}
void normalize_fvec4(fVec4 *v) {
    float length = dot_fvec4(v, v);

    if (length > 0) {
        float inv_length = 1 / sqrt(length);
        v->x *= inv_length;
        v->y *= inv_length;
        v->z *= inv_length;
    }
}
float dot_fvec4(fVec4 *a, fVec4 *b) {
    return (a->x * b->x + a->y * b->y + a->z * b->z);
}

fVec4 *cross_fvec4(fVec4 *a, fVec4 *b) {
    return create_fvec4((a->y * b->z) - (a->z * b->y),
                        (a->z * b->x) - (a->x * b->z),
                        (a->x * b->y) - (a->y * b->x),
                        0);
}
void cross_fvec4_in_place(fVec4 *dest, fVec4 *other) {
    dest->x = (dest->y * other->z) - (dest->z * other->y);
    dest->y = (dest->z * other->x) - (dest->x * other->z);
    dest->z = (dest->x * other->y) - (dest->y * other->x);
}
fVec4 *add_fvec4(fVec4 *a, fVec4 *b) {
    return create_fvec4(a->x + b->x,
                        a->y + b->y,
                        a->z + b->z,
                        a->w + b->w);
}

fVec4 *sub_fvec4(fVec4 *a, fVec4 *b) {
    return create_fvec4(a->x - b->x,
                        a->y - b->y,
                        a->z - b->z,
                        a->w - b->w);
}

fVec4 *scale_fvec4(fVec4 *v, float scale) {
    return create_fvec4(v->x * scale,
                        v->y * scale,
                        v->z * scale,
                        v->w);
}

void add_fvec4_in_place(fVec4 *dest, fVec4 *other) {
    dest->x = dest->x + other->x;
    dest->y = dest->y + other->y;
    dest->z = dest->z + other->z;
    dest->w = dest->w + other->w;
}

void sub_fvec4_in_place(fVec4 *dest, fVec4 *other) {
    dest->x = dest->x - other->x;
    dest->y = dest->y - other->y;
    dest->z = dest->z - other->z;
    dest->w = dest->w - other->w;
}

void scale_fvec4_in_place(fVec4 *dest, float scale) {
    dest->x = dest->x * scale;
    dest->y = dest->y * scale;
    dest->z = dest->z * scale;
}

fVec3 *create_fvec3(float x, float y, float z) {
    fVec3 *vector = (fVec3 *)malloc(sizeof(fVec3));
    if (!vector) {
        printf("Unable to create fvec3");
        return NULL;
    }
    vector->x = x;
    vector->y = y;
    vector->z = z;

    return vector;
}

iVec3 *create_ivec3(int x, int y, int z) {
    iVec3 *vector = (iVec3 *)malloc(sizeof(iVec3));
    if (!vector) {
        printf("Unable to create ivec3");
        return NULL;
    }
    vector->x = x;
    vector->y = y;
    vector->z = z;

    return vector;
}

fVec2 *create_fvec2(float x, float y) {
    fVec2 *vector = (fVec2 *)malloc(sizeof(fVec2));
    if (!vector) {
        printf("Unable to create fvec2");
        return NULL;
    }
    vector->x = x;
    vector->y = y;

    return vector;
}

iVec2 *create_ivec2(int x, int y) {
    iVec2 *vector = (iVec2 *)malloc(sizeof(iVec2));
    if (!vector) {
        printf("Unable to create ivec2");
        return NULL;
    }
    vector->x = x;
    vector->y = y;

    return vector;
}

// Calculate Lengths

float calculate_length_fvec3(fVec3 *v) {
    return sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

float calculate_length_ivec3(iVec3 *v) {
    return sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

// Normalize Vectors

void normalize_fvec3(fVec3 *v) {
    float length = dot_fvec3(v, v);

    if (length > 0) {
        float inv_length = 1 / sqrt(length);
        v->x *= inv_length;
        v->y *= inv_length;
        v->z *= inv_length;
    }
}

void normalize_ivec3(iVec3 *v, fVec3 *dest) {
    int length = dot_ivec3(v, v);

    if (length > 0) {
        float inv_length = 1.0f / sqrt(length);

        dest->x = v->x * inv_length;
        dest->y = v->y * inv_length;
        dest->z = v->z * inv_length;
    }
}

// Dot Products

float dot_fvec3(fVec3 *a, fVec3 *b) {
    return (a->x * b->x + a->y * b->y + a->z * b->z);
}

int dot_ivec3(iVec3 *a, iVec3 *b) {
    return (a->x * b->x + a->y * b->y + a->z * b->z);
}

// Cross Products

fVec3 *cross_fvec3(fVec3 *a, fVec3 *b) {
    return create_fvec3((a->y * b->z) - (a->z * b->y),
                        (a->z * b->x) - (a->x * b->z),
                        (a->x * b->y) - (a->y * b->x));
}

iVec3 *cross_ivec3(iVec3 *a, iVec3 *b) {
    return create_ivec3((a->y * b->z) - (a->z * b->y),
                        (a->z * b->x) - (a->x * a->z),
                        (a->x * b->y) - (a->y * b->x));
}

// Cross Products (in place)

void cross_fvec3_in_place(fVec3 *dest, fVec3 *other) {
    dest->x = (dest->y * other->z) - (dest->z * other->y);
    dest->y = (dest->z * other->x) - (dest->x * other->z);
    dest->z = (dest->x * other->y) - (dest->y * other->x);
}

void cross_ivec3_in_place(iVec3 *dest, iVec3 *other) {
    dest->x = (dest->y * other->z) - (dest->z * other->y);
    dest->y = (dest->z * other->x) - (dest->x * other->z);
    dest->z = (dest->x * other->y) - (dest->y * other->x);
}

// fVec3 Arithmetic

fVec3 *add_fvec3(fVec3 *a, fVec3 *b) {
    return create_fvec3(a->x + b->x,
                        a->y + b->y,
                        a->z + b->z);
}

fVec3 *sub_fvec3(fVec3 *a, fVec3 *b) {
    return create_fvec3(a->x - b->x,
                        a->y - b->y,
                        a->z - b->z);
}

fVec3 *scale_fvec3(fVec3 *v, float scale) {
    return create_fvec3(v->x * scale,
                        v->y * scale,
                        v->z * scale);
}

// fVec3 Arithmetic (in place)

void add_fvec3_in_place(fVec3 *dest, fVec3 *other) {
    dest->x = dest->x + other->x;
    dest->y = dest->y + other->y;
    dest->z = dest->z + other->z;
}

void sub_fvec3_in_place(fVec3 *dest, fVec3 *other) {
    dest->x = dest->x - other->x;
    dest->y = dest->y - other->y;
    dest->z = dest->z - other->z;
}

void scale_fvec3_in_place(fVec3 *dest, float scale) {
    dest->x = dest->x * scale;
    dest->y = dest->y * scale;
    dest->z = dest->z * scale;
}

// iVec3 Arithmetic

iVec3 *add_ivec3(iVec3 *a, iVec3 *b) {
    return create_ivec3(a->x + b->x,
                        a->y + b->y,
                        a->z + b->z);
}

iVec3 *sub_ivec3(iVec3 *a, iVec3 *b) {
    return create_ivec3(a->x - b->x,
                        a->y - b->y,
                        a->z - b->z);
}

iVec3 *scale_ivec3(iVec3 *v, int scale) {
    return create_ivec3(v->x * scale,
                        v->y * scale,
                        v->z * scale);
}

// iVec3 Arithmetic (in place)

void add_ivec3_in_place(iVec3 *dest, iVec3 *other) {
    dest->x = dest->x + other->x;
    dest->y = dest->y + other->y;
    dest->z = dest->z + other->z;
}

void sub_ivec3_in_place(iVec3 *dest, iVec3 *other) {
    dest->x = dest->x - other->x;
    dest->y = dest->y - other->y;
    dest->z = dest->z - other->z;
}

void scale_ivec3_in_place(iVec3 *dest, float scale) {
    dest->x = dest->x * scale;
    dest->y = dest->y * scale;
    dest->z = dest->z * scale;
}

// iVec2 Arithmetic

int x_intersection_ivec2(iVec2 *v1, iVec2 *v2, iVec2 *v3, iVec2 *v4) {
    int numerator = (v1->x * v2->y - v1->y * v2->x) * (v3->x - v4->x) -
                    (v1->x - v2->x) * (v3->x * v4->y - v3->y * v4->x);

    int demoninator = (v1->x - v2->x) * (v3->y - v4->y) - (v1->y - v2->y) * (v3->x - v4->x);

    return numerator / demoninator;
}

int y_intersection_ivec2(iVec2 *v1, iVec2 *v2, iVec2 *v3, iVec2 *v4) {
    int numerator = (v1->x * v2->y - v1->y * v2->x) * (v3->y - v4->y) -
                    (v1->y - v2->y) * (v3->x * v4->y - v3->y * v4->x);

    int demoninator = (v1->x - v2->x) * (v3->y - v4->y) - (v1->y - v2->y) * (v3->x - v4->x);

    return numerator / demoninator;
}

// Plane Creation

fPlane *create_empty_fplane() {
    fPlane *plane = (fPlane *)malloc(sizeof(fPlane));
    if (!plane) {
        printf("Unable to create empty plane");
        return NULL;
    }
    return plane;
}

// Matrix Creation

fMatrix44 *create_empty_fmatrix44() {
    fMatrix44 *mat = (fMatrix44 *)malloc(sizeof(fMatrix44));
    if (!mat) {
        printf("Unable to create identity matrix");
        return NULL;
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            mat->mat[i][j] = 0;
        }
    }

    return mat;
}

fMatrix44 *create_identity_fmatrix44() {
    fMatrix44 *mat = (fMatrix44 *)malloc(sizeof(fMatrix44));
    if (!mat) {
        printf("Unable to create identity matrix");
        return NULL;
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            mat->mat[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }

    return mat;
}

fMatrix44 *create_inverse_matrix(fMatrix44 *mat) {
    fMatrix44 *inv = create_identity_fmatrix44();
    for (int i = 0; i < 3; i++) {
        int pivot = i;
        float pivot_size = fabs(mat->mat[i][i]);
        for (int j = i + 1; j < 4; j++) {
            float temp = fabs(mat->mat[j][i]);
            if (temp > pivot_size) {
                pivot = j;
                pivot_size = temp;
            }
        }

        if (pivot_size == 0) {
            return NULL;
        }

        if (pivot != i) {
            for (int j = 0; j < 4; j++) {
                swap(&mat->mat[i][j], &mat->mat[pivot][j]);
                swap(&inv->mat[i][j], &inv->mat[pivot][j]);
            }
        }

        for (int j = i + 1; j < 4; j++) {
            float multiplier = mat->mat[j][i] / mat->mat[i][i];
            for (int k = 0; k < 4; k++) {
                mat->mat[j][k] -= multiplier * mat->mat[i][k];
                inv->mat[j][k] -= multiplier * inv->mat[i][k];
            }
            mat->mat[j][i] = 0.0f;
        }
    }

    for (int i = 0; i < 4; i++) {
        float divisor = mat->mat[i][i];
        for (int j = 0; j < 4; j++) {
            mat->mat[i][j] = mat->mat[i][j] / divisor;
            inv->mat[i][j] = inv->mat[i][j] / divisor;
        }
        mat->mat[i][i] = 1.0f;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 4; j++) {
            float multiplier = mat->mat[i][j];
            for (int k = 0; k < 4; k++) {
                mat->mat[i][k] -= mat->mat[j][k] * multiplier;
                inv->mat[i][k] -= inv->mat[j][k] * multiplier;
            }
            mat->mat[i][j] = 0.0f;
        }
    }

    return inv;
}

void swap(float *a, float *b) {
    float temp = *a;
    *a = *b;
    *b = temp;
}

fMatrix44 *create_deep_copy_fmatrix44(fMatrix44 *mat) {
    fMatrix44 *copy = create_empty_fmatrix44();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            copy->mat[i][j] = mat->mat[i][j];
        }
    }

    return copy;
}

// Matrix Arithmetic

fMatrix44 *add_fmatrix44(fMatrix44 *lhs, fMatrix44 *rhs) {
    fMatrix44 *res = (fMatrix44 *)malloc(sizeof(fMatrix44));

    if (!res) {
        printf("Unable to create result add matrix");
        return NULL;
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            res->mat[i][j] = lhs->mat[i][j] + rhs->mat[i][j];
        }
    }

    return res;
}

fMatrix44 *sub_fmatrix44(fMatrix44 *lhs, fMatrix44 *rhs) {
    fMatrix44 *res = (fMatrix44 *)malloc(sizeof(fMatrix44));

    if (!res) {
        printf("Unable to create result sub matrix");
        return NULL;
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            res->mat[i][j] = lhs->mat[i][j] - rhs->mat[i][j];
        }
    }

    return res;
}

fMatrix44 *mult_fmatrix44(fMatrix44 *lhs, fMatrix44 *rhs) {
    fMatrix44 *res = (fMatrix44 *)malloc(sizeof(fMatrix44));

    if (!res) {
        printf("Unable to create result mult matrix");
        return NULL;
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            res->mat[i][j] = lhs->mat[i][0] * rhs->mat[0][j] +
                             lhs->mat[i][1] * rhs->mat[1][j] +
                             lhs->mat[i][2] * rhs->mat[2][j] +
                             lhs->mat[i][3] * rhs->mat[3][j];
        }
    }

    return res;
}

void multiply_fvec3_matrix44(fVec3 *in, fVec3 *out, fMatrix44 *mat) {
    out->x = in->x * mat->mat[0][0] + in->y * mat->mat[1][0] + in->z * mat->mat[2][0] + mat->mat[3][0];
    out->y = in->x * mat->mat[0][1] + in->y * mat->mat[1][1] + in->z * mat->mat[2][1] + mat->mat[3][1];
    out->z = in->x * mat->mat[0][2] + in->y * mat->mat[1][2] + in->z * mat->mat[2][2] + mat->mat[3][2];
    float w = in->x * mat->mat[0][3] + in->y * mat->mat[1][3] + in->z * mat->mat[2][3] + mat->mat[3][3];

    if (w != 0 && w != 1) {
        out->x /= w;
        out->y /= w;
        out->z /= w;
    }
}

void multiply_fvec4_matrix44(fVec4 *in, fVec4 *out, fMatrix44 *mat) {
    out->x = in->x * mat->mat[0][0] + in->y * mat->mat[1][0] + in->z * mat->mat[2][0] + mat->mat[3][0];
    out->y = in->x * mat->mat[0][1] + in->y * mat->mat[1][1] + in->z * mat->mat[2][1] + mat->mat[3][1];
    out->z = in->x * mat->mat[0][2] + in->y * mat->mat[1][2] + in->z * mat->mat[2][2] + mat->mat[3][2];
    out->w = in->x * mat->mat[0][3] + in->y * mat->mat[1][3] + in->z * mat->mat[2][3] + mat->mat[3][3];
}

float get_w_fvec3_matrix44(fVec3 *in, fMatrix44 *mat) {
    float w = in->x * mat->mat[0][3] + in->y * mat->mat[1][3] + in->z * mat->mat[2][3] + mat->mat[3][3];
    return w;
}

int plane_distance_to_fvec4(fPlane plane, fVec4 *v) {
    float distance = (plane.a * v->x) + (plane.b * v->y) + (plane.c * v->z) + plane.d;

    if (distance < 0) {
        return NEGATIVE_OF_PLANE;
    } else if (distance > 0) {
        return POSITIVE_OF_PLANE;
    }
    return ON_PLANE;
}

int plane_distance_to_fvec3(fPlane plane, fVec3 *v) {
    float distance = (plane.a * v->x) + (plane.b * v->y) + (plane.c * v->z) + plane.d;

    if (distance < 0) {
        return NEGATIVE_OF_PLANE;
    } else if (distance > 0) {
        return POSITIVE_OF_PLANE;
    }
    return ON_PLANE;
}

void print_matrix(fMatrix44 *mat) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%.2f\t", mat->mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Misc Geometric Functions

float convert_deg_to_rad(float deg) {
    deg = (deg * M_PI) / 180;
    return deg;
}
float convert_rad_to_deg(float rad) {
    rad = (rad * 180) / M_PI;
    return rad;
}

// Free Vectors from memory

void clear_vector(void *vector) {
    if (vector) {
        free(vector);
        vector = NULL;
    }
}

void clear_matrix(void *matrix) {
    if (matrix) {
        free(matrix);
        matrix = NULL;
    }
}
