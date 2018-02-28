#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "probekins2.h"
#include "data_pass.h"

#define SHM_MODE 0666 | IPC_CREAT
int shmid;
int shmsz;

int main()
{
    int n_face_groups, n_vertices, n_faces;
    n_vertices = 5;
    n_faces = 3;
    n_face_groups = 9;

    int n_faces0 = 2;
    int n_faces1 = 3;
    int n_faces2 = 2;
    int n_faces3 = 1;
    int n_faces4 = 1;
    size_t ms_size = sizeof(int) * 3 +
        sizeof(vertex_t) * n_vertices +
        sizeof(face_t) * n_faces +
        sizeof(int) * n_face_groups;

    shmsz = ms_size;
    if ((shmid = shmget(SHMEM_KEY, shmsz, SHM_MODE)) == -1)
    {
        perror("failed"); exit(1);
    }

    char *shmptr;
    if ((shmptr = shmat(shmid, 0, 0)) == (void *)-1)
    {
        perror("shmat failed"); exit(1);
    }
    printf("shared memory attached from %p to %p\n", (void*)shmptr, (void*)shmptr+shmsz);

    printf("Bytes allocated: %d\n", ms_size);

    mesh_struct *mp = (mesh_struct*) malloc(ms_size);
    memset(mp, 0, ms_size);

    mp->n_vertices = n_vertices;
    mp->n_faces = n_faces;
    mp->n_face_groups = n_face_groups;
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

    int* mp_face_groups_arr = (int*) &mp_faces[n_faces];
    mp_face_groups_arr[0] = 0;
    mp_face_groups_arr[1] = 1;
    mp_face_groups_arr[2] = 0;
    mp_face_groups_arr[3] = 1;
    mp_face_groups_arr[4] = 2;
    mp_face_groups_arr[5] = 0;
    mp_face_groups_arr[6] = 1;
    mp_face_groups_arr[7] = 1;
    mp_face_groups_arr[8] = 2;

    printf("print data struct");
    print_mesh_struct(mp);

    memcpy(shmptr, mp, ms_size);

    print_mesh_struct((mesh_struct *)shmptr);

    free(mp);

    getchar();

    if (shmctl(shmid, IPC_RMID, 0) <0)
    {
        perror("shmctl failed"); exit(1);
    }
}
