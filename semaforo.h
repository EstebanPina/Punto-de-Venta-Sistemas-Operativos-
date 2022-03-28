#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>

#define PERMISOS 0660
  
int Crea_semaforo(key_t llave,int valor_inicial){
   int semid=semget(llave,1,IPC_CREAT|PERMISOS);
   if(semid==-1)
   {
      return -1;
   }
   semctl(semid,0,SETVAL,valor_inicial);
   return semid;
}
void down(int semid){
   struct sembuf op_p[]={0,-1,0};
   semop(semid,op_p,1);
}
void up(int semid){
   struct sembuf op_v[]={0,+1,0};
   semop(semid,op_v,1);
}
