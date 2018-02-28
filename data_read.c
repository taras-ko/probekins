#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "probekins2.h"
#include "data_pass.h"

#define SHM_MODE 0666

int main()
{
    int shmid;
    int shmsz = 40;//244;

    if ((shmid = shmget(4713, shmsz, SHM_MODE)) == -1)
    {
        perror("failed"); exit(1);
    }
    char *shmptr;
    if ((shmptr = shmat(shmid, 0, 0)) == (void *)-1)
    {
        perror("shmat failed"); exit(1);
    }
    printf("shared memory attached from %p to %p\n", (void*)shmptr, (void*)shmptr+shmsz);

//    print_mesh_struct((mesh_struct *)shmptr);

    struct S {
        int i1,i2,i3;
        vertex_t v;
    };

    if (shmdt(shmptr) == -1)
        perror("shmdt failed");

}
