#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int n_faces;
    int face_idx[1]; //...n_faces
} face_group_t;

typedef struct {
    double coord[3];
    int face_group_size;
} vertex_t;

typedef int face_t[3];

typedef struct {
    int n_vertices;
    int n_faces;
    int face_groups_arr_size;
    vertex_t vertices[1]; //...n_vertices
    // followed by:
    // face_t faces[1]; // 1..n fac
    // starting at &vertices[n_vertices]
    // followed by:
    // int face_groups[1]; // 1..n_face_groups
    // starting at &faces[n_faces]
} mesh_struct;

typedef struct {
    int n_face_groups;
    face_group_t face_groups[1]; //...n_groups
} shmem_face_groups;

int main()
{
    int n_face_groups, n_vertices, n_faces;
    n_vertices = 5;
    n_face_groups = 5;
    n_faces = 3;
    int face_groups_arr_size = 9;
    int n_faces0 = 2;
    int n_faces1 = 3;
    int n_faces2 = 2;
    int n_faces3 = 1;
    int n_faces4 = 1;
    size_t ms_size = sizeof(int) * 3 +
        sizeof(vertex_t) * n_vertices +
        sizeof(face_t) * n_faces +
        sizeof(int) * face_groups_arr_size;

    printf("Bytes allocated: %d\n", ms_size);

    mesh_struct *mp = (mesh_struct*) malloc(ms_size);
    memset(mp, 0, ms_size);

    mp->n_vertices = n_vertices;
    mp->n_faces = n_faces;
    mp->face_groups_arr_size = face_groups_arr_size;
    mp->vertices[0].coord[0] = -150.0;
    mp->vertices[0].coord[1] = 111.0;
    mp->vertices[0].coord[2] = 2.1;
    mp->vertices[0].face_group_size = 2;

    mp->vertices[1].coord[0] = -12.0;
    mp->vertices[1].coord[1] = 180.0;
    mp->vertices[1].coord[2] = 1.4;
    mp->vertices[1].face_group_size = 3;

    mp->vertices[2].coord[0] = -44.0;
    mp->vertices[2].coord[1] = -121.0;
    mp->vertices[2].coord[2] = 1.2;
    mp->vertices[2].face_group_size = 2;

    mp->vertices[3].coord[0] = 210.0;
    mp->vertices[3].coord[1] = -120.0;
    mp->vertices[3].coord[2] = 0.2;
    mp->vertices[3].face_group_size = 1;

    mp->vertices[4].coord[0] = -100.0;
    mp->vertices[4].coord[1] = 200.0;
    mp->vertices[4].coord[2] = 0.5;
    mp->vertices[4].face_group_size = 1;

    face_t* mp_faces = (face_t*) &mp->vertices[n_vertices];
    mp_faces[0][0] = 0;
    mp_faces[0][1] = 1;
    mp_faces[0][2] = 2;

    mp_faces[1][0] = 1;
    mp_faces[1][1] = 3;
    mp_faces[1][2] = 2;

    mp_faces[2][0] = 4;
    mp_faces[2][1] = 1;
    mp_faces[2][2] = 0;

    int* mp_face_groups_arr = (int*) &mp_faces[3];
    mp_face_groups_arr[0] = 0;
    mp_face_groups_arr[1] = 1;
    mp_face_groups_arr[2] = 0;
    mp_face_groups_arr[3] = 1;
    mp_face_groups_arr[4] = 2;
    mp_face_groups_arr[5] = 0;
    mp_face_groups_arr[6] = 1;
    mp_face_groups_arr[7] = 1;
    mp_face_groups_arr[8] = 2;

    int i;

    printf("Number of vertices: %d\n", mp->n_vertices);
    printf("Number of faces: %d\n", mp->n_faces);
    printf("Size of face groups array: %d\n", mp->face_groups_arr_size);

    for (i = 0; i < sh_fg->n_face_groups; ++i)
    {
        printf("\tGroup %d\n", i);
        printf("\t n_faces %d: ", sh_fg->face_groups[i].n_faces);
        int j;
        for (j = 0; j < sh_fg->face_groups[i].n_faces; ++j)
            printf("%d ", sh_fg->face_groups[i].face_idx[j]);
        printf("\n");
    }

    free(mp);
}
