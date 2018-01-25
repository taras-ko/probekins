#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int n_faces;
    int face_idx[1];
} face_group_t;

typedef struct {
    int n_face_groups;
    face_group_t face_groups[1]; //...n_groups
} shmem_face_groups;

int main()
{
    int n_face_groups = 5;
    int n_faces0 = 2;
    int n_faces1 = 3;
    int n_faces2 = 2;
    int n_faces3 = 1;
    int n_faces4 = 1;
    size_t fg_size = sizeof(int) * (1 + n_face_groups + n_faces0 + n_faces1 + n_faces2
            + n_faces3 + n_faces4);

    shmem_face_groups *sh_fg = (shmem_face_groups*) malloc(fg_size);

    sh_fg->n_face_groups = n_face_groups;

    sh_fg->face_groups[0].n_faces = n_faces0;
    sh_fg->face_groups[0].face_idx[0] = 0;
    sh_fg->face_groups[0].face_idx[1] = 1;

    sh_fg->face_groups[1].n_faces = n_faces1;
    sh_fg->face_groups[1].face_idx[0] = 0;
    sh_fg->face_groups[1].face_idx[1] = 1;
    sh_fg->face_groups[1].face_idx[2] = 2;

    sh_fg->face_groups[2].n_faces = n_faces2;
    sh_fg->face_groups[2].face_idx[0] = 0;
    sh_fg->face_groups[2].face_idx[1] = 1;

    sh_fg->face_groups[3].n_faces = n_faces3;
    sh_fg->face_groups[3].face_idx[0] = 1;

    sh_fg->face_groups[4].n_faces = n_faces4;
    sh_fg->face_groups[4].face_idx[0] = 2;

    int i;

    printf("number of face groups: %d\n", sh_fg->n_face_groups);
    for (i = 0; i < sh_fg->n_face_groups; ++i)
    {
    }

    free(sh_fg);
}
