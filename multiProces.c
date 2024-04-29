
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <stdbool.h>

#define SHM_SIZE 1024
#define SIZE 3 * NZ * sizeof(int)

void priQ(int * arr){
    int i, j;
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                printf("%d ", arr[i * 3 + j]);
            }
            printf("\n");
        }
    printf("\n");
}

void zakaznik(sem_t *semOut, sem_t *semQu, int * val,int idZ, int TZ, int *arr, int NZ, sem_t *semClose, bool *CloseFlag)
{
    sem_wait(semOut);
        *val+=1;
        printf("%d: Z %d: started\n", *val, idZ);
    sem_post(semOut);
    useconds_t time = rand()%(TZ+1);
    sem_wait(semClose);
        if(*CloseFlag == false){
            *val+=1;
            printf("%d: Z %d: going home\n", *val, idZ);
            sem_post(semClose);
            return;
        }
    sem_post(semClose);
    usleep(time);
    int X = rand() % 3;
    sem_wait(semOut);
        *val+=1;
        printf("%d: Z %d: entering office for service %d\n", *val, idZ, (X+1));
    sem_post(semOut);
    sem_wait(semQu);
        for(int i = 0; i<NZ; i++){
            if(arr[X * NZ + i] == 0){
                arr[X * NZ + i] = idZ;
                break;
            }
        }
    sem_post(semQu);
}
void urednik(sem_t *semOut, sem_t *semQu, int * val, int idU, int NZ, int *arr, sem_t *semClose, bool *CloseFlag, int TU)
{
    sem_wait(semOut);
        *val+=1;
        printf("%d: U %d: started\n", *val, idU);
    sem_post(semOut);
    int X = rand() % 3;
    while(true){
        int service = 0;
        sem_wait(semQu);
        for(int j = 0; j<3; j++){
            for(int i = 0; i<NZ; i++){
                if(arr[X * NZ + i] != 0 && arr[X * NZ + i] != -1){
                    service = X + 1;
                    arr[X * NZ + i] = -1;
                    break;
                }
            }
            if(service != 0){
                break;
            }
            X = (X + 1)%3;
        }
        sem_post(semQu);
        sem_wait(semOut);
            if(service != 0){

                *val+=1;
                printf("%d: U %d: serving service of type %d\n", *val, idU, service);
                sem_post(semOut);
                
            }

            else{
                sem_wait(semClose);
                if(*CloseFlag == false){
                    *val+=1;
                    printf("%d: U %d: going home\n", *val, idU);
                    sem_post(semClose);
                    return;
                }
                sem_post(semClose);
                *val+=1;
                printf("%d: U %d: taking break\n", *val, idU);
                sem_post(semOut);
                int uBreak = rand() % (TU +1);
                usleep(uBreak);
            }
    }
}

int main(void) {
    
    int NZ = 3;
    int NU = 2;
    int TZ = 100;
    int TU = 100;
    int F = 100;
    int i, pid = 0;
    char * nameCodenName1 = "prd1_sem";
    char * nameCodenName2 = "prd2_sem";
    char * nameCodenName3 = "prd3_sem";
    key_t key = 1234;
    int sharedMemId;
    int *sharedMem;
    
    int shmBoolId = shmget(IPC_PRIVATE, sizeof(bool), IPC_CREAT | 0666);
    if (shmBoolId == -1) {
        perror("shmget");
        exit(1);
    }

    bool *Close = (bool *)shmat(shmBoolId, NULL, 0);
    if (Close == (bool *)(-1)) {
        perror("shmat");
        exit(1);
    }
    
    int shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT | 0666);
     if (shmid < 0) {
         perror("shmget");
         exit(1);
     }

     // attach shared memory segment
     int * Que = (int *) shmat(shmid, NULL, 0);
     if (Que == (int *) -1) {
         perror("shmat");
         exit(1);
     }

     // copy array values to shared memory
     for (int i = 0; i < 3; i++) {
         for (int j = 0; j < NZ; j++) {
             Que[i * NZ + j] = 0;
         }
     }


    // register child handler signal
//    dopsat druhy semafor pro radu na poste
    sem_t * semOut = sem_open(nameCodenName1, O_CREAT | O_EXCL, 0644, 1);
    if (semOut == SEM_FAILED) {
        perror("sem_open");
        sem_close(semOut);
        sem_unlink(nameCodenName1);
        exit(1);
    }
    sem_t * semQu = sem_open(nameCodenName2, O_CREAT | O_EXCL, 0644, 1);
    if (semQu == SEM_FAILED) {
        perror("sem_open");
        sem_close(semQu);
        sem_unlink(nameCodenName1);
        sem_unlink(nameCodenName2);
        exit(1);
    }
    sem_t * semClose = sem_open(nameCodenName3, O_CREAT | O_EXCL, 0644, 1);
    if (semClose == SEM_FAILED) {
        perror("sem_open");
        sem_close(semClose);
        sem_unlink(nameCodenName1);
        sem_unlink(nameCodenName2);
        sem_unlink(nameCodenName3);
        exit(1);
    }

    // shared mem alocation
    if ((sharedMemId = shmget(key, sizeof(int*), IPC_CREAT | 0656)) < 0) {
       perror("shmget");
       exit(1);
    }

    // Open shared mem
    if ((sharedMem = shmat(sharedMemId, NULL, 0)) == (void *) -1) {
       perror("shmat");
       exit(1);
    }
    sem_wait(semClose);
        *Close = true;
    sem_post(semClose);
    for (i = 0; i < NZ; i++) {
        pid = fork();
        if (pid == 0) { // child process
            int idZ = 0;
            idZ = i+1;
            zakaznik(semOut, semQu, sharedMem, idZ, TZ, Que, NZ, semClose, Close);
            _exit(0);
        } else if (pid > 0) { // parent process
            continue;
            // continue to next iteration of the loop
        } else { // error
            printf("Fork failed\n");
        }
    }
    for (i = 0; i < NU; i++) {
        pid = fork();
        if (pid == 0) { // child process
            int idU = 0;
            idU = i+1;
            urednik(semOut, semQu, sharedMem, idU, NZ, Que, semClose, Close, TU);
            _exit(0);
        } else if (pid > 0) { // parent process
            continue;
            // continue to next iteration of the loop
        } else { // error
            printf("Fork failed\n");
        }
    }
    
    int Closing = (rand() % (F - F/2 + 1)) + F/2;
    usleep(Closing);
    sem_wait(semClose);
        *Close = false;
    sem_post(semClose);
    sem_wait(semOut);
        *sharedMem+=1;
        printf("%d: closing\n", *sharedMem);
    sem_post(semOut);
    
    
    while (wait(NULL) > 0);
    
    priQ(Que);
    shmctl(sharedMemId, IPC_RMID, NULL);
    shmctl(shmid, IPC_RMID, NULL);
    shmctl(shmBoolId, IPC_RMID, NULL);
    sem_close(semOut);
    sem_close(semQu);
    sem_close(semClose);
    sem_unlink(nameCodenName1);
    sem_unlink(nameCodenName2);
    sem_unlink(nameCodenName3);
    shmdt(sharedMem);
    shmdt(Que);
    shmdt(Close);

    return 0;
}
