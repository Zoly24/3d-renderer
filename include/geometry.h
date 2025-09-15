#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef enum TypePlanes {
    LEFT_PLANE,
    RIGHT_PLANE,
    BOTTOM_PLANE,
    TOP_PLANE,
    NEAR_PLANE,
    FAR_PLANE
} pType;

typedef struct fMatrix44 {
    float mat[4][4];
} fMatrix44;

typedef struct fPlane {
    float a, b, c, d;
} fPlane;

typedef struct fVec4 {
    float x, y, z, w;
} fVec4;

typedef struct fVec3 {
    float x, y, z;
} fVec3;

typedef struct iVec3 {
    int x, y, z;
} iVec3;

typedef struct fVec2 {
    float x, y;
} fVec2;

typedef struct iVec2 {
    int x, y;
} iVec2;

fVec3 *create_fvec3(float, float, float);
iVec3 *create_ivec3(int, int, int);

fVec2 *create_fvec2(float, float);
iVec2 *create_ivec2(int, int);

fVec4 *create_fvec4(float, float, float, float);
float calculate_length_fvec4(fVec4 *);
void normalize_fvec4(fVec4 *);
float dot_fvec4(fVec4 *, fVec4 *);
fVec4 *cross_fvec4(fVec4 *, fVec4 *);
void cross_fvec4_in_place(fVec4 *, fVec4 *);

fVec4 *add_fvec4(fVec4 *, fVec4 *);
fVec4 *sub_fvec4(fVec4 *, fVec4 *);
fVec4 *scale_fvec4(fVec4 *, float);

void add_fvec4_in_place(fVec4 *, fVec4 *);
void sub_fvec4_in_place(fVec4 *, fVec4 *);
void scale_fvec4_in_place(fVec4 *, float);

float calculate_length_fvec3(fVec3 *);
float calculate_length_ivec3(iVec3 *);

void normalize_fvec3(fVec3 *);
void normalize_ivec3(iVec3 *, fVec3 *); // resulting vec will change to a fVec3

float dot_fvec3(fVec3 *, fVec3 *);
int dot_ivec3(iVec3 *, iVec3 *);

fVec3 *cross_fvec3(fVec3 *, fVec3 *);
iVec3 *cross_ivec3(iVec3 *, iVec3 *);

void cross_fvec3_in_place(fVec3 *, fVec3 *);
void cross_ivec3_in_place(iVec3 *, iVec3 *);

fVec3 *add_fvec3(fVec3 *, fVec3 *);
fVec3 *sub_fvec3(fVec3 *, fVec3 *);
fVec3 *scale_fvec3(fVec3 *, float);

void add_fvec3_in_place(fVec3 *, fVec3 *);
void sub_fvec3_in_place(fVec3 *, fVec3 *);
void scale_fvec3_in_place(fVec3 *, float);

iVec3 *add_ivec3(iVec3 *, iVec3 *);
iVec3 *sub_ivec3(iVec3 *, iVec3 *);
iVec3 *scale_ivec3(iVec3 *, int);

void add_ivec3_in_place(iVec3 *, iVec3 *);
void sub_ivec3_in_place(iVec3 *, iVec3 *);
void scale_ivec3_in_place(iVec3 *, float);

int x_intersection_ivec2(iVec2 *, iVec2 *, iVec2 *, iVec2 *);
int y_intersection_ivec2(iVec2 *, iVec2 *, iVec2 *, iVec2 *);

fPlane *create_empty_fplane(void);

fMatrix44 *create_empty_fmatrix44(void);
fMatrix44 *create_identity_fmatrix44(void);
fMatrix44 *create_inverse_matrix(fMatrix44 *);
fMatrix44 *create_deep_copy_fmatrix44(fMatrix44 *);
void swap(float *, float *);

fMatrix44 *add_fmatrix44(fMatrix44 *, fMatrix44 *);
fMatrix44 *sub_fmatrix44(fMatrix44 *, fMatrix44 *);
fMatrix44 *mult_fmatrix44(fMatrix44 *, fMatrix44 *);

void multiply_fvec3_matrix44(fVec3 *, fVec3 *, fMatrix44 *);
void multiply_fvec4_matrix44(fVec4 *, fVec4 *, fMatrix44 *);

float get_w_fvec3_matrix44(fVec3 *, fMatrix44 *);

int plane_distance_to_fvec4(fPlane, fVec4 *);
int plane_distance_to_fvec3(fPlane, fVec3 *);

void print_matrix(fMatrix44 *);

float convert_deg_to_rad(float);
float convert_rad_to_deg(float);

void clear_vector(void *);
void clear_matrix(void *);

#endif
