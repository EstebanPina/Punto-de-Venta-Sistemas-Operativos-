#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <term.h>
#include <curses.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include </home/estebanpina/Desktop/Programas/keyread.h>
#include </home/estebanpina/Desktop/Programas/semaforo.h>
#include </home/estebanpina/Desktop/Proyecto1.2/Info.h> 
Info *lugares,*getProducto,prodconsul;
int mem=0;
IDdef *IDConsulta;
dataU *ConsulU,InicioSes;
key_t llaveshm;
key_t llavecambios;
IDCarrito *idPedido;
CHNGQTY *CambioSH;
int shmid;
int shmidinfo;
int shmidcambios;
int shmid2;
int shmid3;
int shmidU;
int shmidCarr;
int shmidchn;
pid_t pid;
int i;
int main(){
i=0;
int ch=0;
int id_cliente=0;
init_keyboard();
int semaforo;
key_t llavesem;
lugares=(int*)malloc(sizeof(int));
llavesem = ftok("Semaforo",'k');
llaveshm = ftok ("Memoria",'m');
shmid=shmget(llaveshm,sizeof(int),IPC_CREAT|0660);
SELECT *conect=shmat(shmid,NULL,SHM_R|SHM_W);
conect->pid=250;
    while(ch != 'q'){
        system("clear");  
        printf("Servidor Activado\n");
        sleep(1);
        if(kbhit()){
            ch = readch();
            if(ch=='q'){
                printf("Cerrando servidor\n");
                shmdt(&conect);
                shmctl(shmid,IPC_RMID,0);
                shmdt(&lugares);
                shmctl(shmidinfo,IPC_RMID,0);
                shmctl(shmidcambios,IPC_RMID,0);
                shmctl(shmid2,IPC_RMID,0);
                shmctl(shmid3,IPC_RMID,0);
                shmctl(shmidU,IPC_RMID,0);
                shmctl(shmidCarr,IPC_RMID,0);
                shmctl(shmidchn,IPC_RMID,0);
                }
        }
        if(conect->pid!=250){
            if(conect->Seleccion==1){
                Consulta_Info();
                conect->Seleccion=0;
            }
            else if(conect->Seleccion==2){
                InsertaInfo();
                conect->Seleccion=0;
            }
            else if(conect->Seleccion==3){
                ModificaCatalogo();
                conect->Seleccion=0;
            }
            else if(conect->Seleccion==11){
                Consulta_InfoU();
                conect->Seleccion=0;
            }
            else if(conect->Seleccion==12){
                InsertaCarrito();
                conect->Seleccion=0;
            }
            system("clear");
            printf("Hubo conexion del programa");
            id_cliente=conect->pid;
            printf("\nId Cliente Conectado=%i\n",id_cliente);
            sleep(2);
            pid=fork();
            if(pid==-1){
                printf("\nError al crear el proceso del cliente %i\n",id_cliente);
                exit(-1);
            }
            if (pid!=0){
                while(i){
                semaforo=Crea_semaforo(llavesem,1);
                down(semaforo);
                printf("Proceso cliente %i",id_cliente);
                up(semaforo);
                }
                kill(pid,SIGKILL);
            }
            conect->pid=250;
        }
        
        
    }
    close_keyboard();
    
    exit(0);
}
void InsertaInfo(){
    FILE *fp;
    key_t llaveinfo=ftok("Esteban",'i');
    shmidinfo=shmget(llaveinfo,sizeof(int),IPC_CREAT|0660);
    lugares=shmat (shmidinfo,0,0);
	fp = fopen ( "Catalogo.dat", "a" );        
	if (fp==NULL) {fputs ("File error",stderr); exit (1);}
    __fpurge(stdin);
    fprintf(fp,"ID:%s:IDEND,NOMBRE:%s:NOMBREEND,CANTIDAD:%i:CANTIDADEND\n",lugares->ID,lugares->Nombre,lugares->Cantidad);
	fclose ( fp );
    shmdt(&lugares);
}
void InsertaCarrito(){
    FILE *fp;
    key_t llaveCarr=ftok("ConexionCarr",'s');
    shmidCarr=shmget(llaveCarr,sizeof(int),IPC_CREAT|0660);
    idPedido=shmat (shmidCarr,0,0);
	fp = fopen ("Carrito.dat", "a" );        
	if (fp==NULL) {fputs ("File error",stderr); exit (1);}
    FILE *fc;
    fc = fopen("Catalogo.dat", "rb");
    if (fc == NULL)
    {
        printf("Error en archivo");
        exit(1);
    }
    char buffer[150];
    char ch[1];
    int contador = 0;
    while (fgets(buffer, 150, fc) != NULL)
    {
        char *INICIO = strstr(buffer, "ID:");
        char *FIN = strstr(buffer, ":IDEND");
        char extraida[10] = "";
        if(FIN!=NULL){
            int Iindice = INICIO - buffer;
            int Iindice2 = FIN - buffer;
            int ITamano = Iindice2 - (Iindice+3);
            strncpy(extraida, buffer + (Iindice+3), ITamano);
            printf("IDU EXTRAIDA %s!!!!\n\n",extraida);
        }
        if(strcmp(extraida,idPedido->IDCARRITO)==0){
            __fpurge(stdin);
            fprintf(fp,"IDU:%i:IDUEND,IDP:%s:IDPEND\n",idPedido->IDUsuario,idPedido->IDCARRITO);
	        fclose ( fp );
            shmdt(&idPedido);
            break;
        }
    }
    fclose(fc);
    
}
void ModificaCatalogo(){
    FILE *fp;
    key_t llavechn=ftok("Esteban",'g');
    shmidchn=shmget(llavechn,sizeof(int),IPC_CREAT|0660);
    CambioSH=shmat (shmidchn,0,SHM_R|SHM_W);
	fp = fopen ("Catalogo.dat.temp", "w" );        
	if (fp==NULL) {fputs ("File error",stderr); exit (1);}
    fclose(fp);
    FILE *fc;
    fc = fopen("Catalogo.dat", "rb");
    if (fc == NULL)
    {
        printf("Error en archivo");
        exit(1);
    }
    char buffer[150];
    char ch[1];
    int contador = 0;
    while (fgets(buffer, 150, fc) != NULL)
    {
        //ID:UIXZ25B:IDEND,NOMBRE:SILLA XZ25:NOMBREEND,CANTIDAD:20:CANTIDADEND
        char *INICIO = strstr(buffer, "ID:");
        char *FIN = strstr(buffer, ":IDEND");
        char *INICIO2 = strstr(buffer, "NOMBRE:");
        char *FIN2 = strstr(buffer, ":NOMBREEND");
        char *INICIO3 = strstr(buffer, "CANTIDAD:");
        char *FIN3 = strstr(buffer, ":CANTIDADEND");
        char extraida[10] = "";
        char Nextraida[30] = "";
        char Qextraida[8] = "";
        if(FIN!=NULL){
            int indice = INICIO - buffer;
            int indice2 = FIN - buffer;
            int Tamano = indice2 - (indice+3);
            strncpy(extraida, buffer + (indice+3), Tamano);
        }
        if(FIN2!=NULL){
            int indice = INICIO2 - buffer;
            int indice2 = FIN2 - buffer;
            int Tamano = indice2 - (indice+7);
            strncpy(Nextraida, buffer + (indice+7), Tamano);
        }
        if(FIN3!=NULL){
            int indice = INICIO3 - buffer;
            int indice2 = FIN3 - buffer;
            int Tamano = indice2 - (indice+9);
            strncpy(Qextraida, buffer + (indice+9), Tamano);
        }
        if(strcmp(extraida,CambioSH->IDCATALOG)==0){
            fp = fopen ("Catalogo.dat.temp", "a" );        
	        if (fp==NULL) {fputs ("File error",stderr); exit (1);}
            __fpurge(stdin);
            int Cantidad=atoi(Qextraida);
            Cantidad=Cantidad+CambioSH->QTY;
            fprintf(fp,"ID:%s:IDEND,NOMBRE:%s:NOMBREEND,CANTIDAD:%i:CANTIDADEND\n",extraida,Nextraida,Cantidad);
	        fclose ( fp );
            shmdt(&idPedido);
        }
        else{
            fp = fopen ("Catalogo.dat.temp", "a" );        
	        if (fp==NULL) {fputs ("File error",stderr); exit (1);}
             __fpurge(stdin);
             fprintf(fp,"%s",buffer);
             fclose ( fp );
        }
    }
    fclose(fc);
    fp = fopen ("Catalogo.dat","w");
    if (fp==NULL) {fputs ("File error",stderr); exit (1);}
    fclose ( fp );
    fc = fopen("Catalogo.dat.temp", "rb");
    if (fc == NULL){printf("Error en archivo");exit(1);}
    char buffer2[150];
    char ch2[1];
    int contador2 = 0;
    while (fgets(buffer, 150, fc) != NULL)
    {
        fp = fopen ("Catalogo.dat", "a" );        
	    if (fp==NULL) {fputs ("File error",stderr); exit (1);}
        __fpurge(stdin);
        fprintf(fp,"%s",buffer);
        fclose ( fp );
    }
    system("rm Catalogo.dat.temp");
}
void Consulta_Info(){
    FILE *fc;
    fc= fopen("Catalogo.dat","rb");
    if(fc==NULL){
        printf("Error en archivo");
        exit(1);
    }
    key_t llaveConsulta=ftok("Ids",'l');
    shmid2=shmget(llaveConsulta,sizeof(int),IPC_CREAT|0660);
    IDConsulta=shmat (shmid2,0,SHM_R|SHM_W);
    char buffer[150];
    char ch[1];
    int contador=0;
    while(fgets(buffer,150,fc)!=NULL){
        char *INICIO=strstr(buffer,"ID:");
        char *FIN = strstr(buffer, ":IDEND");
        if (FIN != NULL){
        int indice = INICIO - buffer;
        int indice2= FIN - buffer;
        char extraida[11]="";
        char Cextraida[12]="";
        char Nextraida[50]="";
        int Tamano=indice2-3;
        strncpy(extraida, buffer + 3, Tamano);
        if(strcmp(IDConsulta->IDSHARED,extraida)==0){
            strcpy(prodconsul.ID,extraida);
            printf("El producto se encuentra en la Base de datos %s , %s\n", IDConsulta->IDSHARED,extraida);
            char *INICIO2=strstr(buffer,"NOMBRE:");
            char *FIN2 = strstr(buffer, ":NOMBREEND");
            if (FIN2 != NULL){
            int Nindice = INICIO2 - buffer;
            int Nindice2= FIN2 - buffer;
            int NTamano=Nindice2-(Nindice+7);
            strncpy(Nextraida, buffer+(Nindice+7), NTamano);
            printf("Nombre del producto: %s \n",Nextraida);
            strcpy(prodconsul.Nombre,Nextraida);
            }
            char *INICIO3=strstr(buffer,"CANTIDAD:");
            char *FIN3 = strstr(buffer, ":CANTIDADEND");
            if (FIN3 != NULL){
            int Cindice = INICIO3 - buffer;
            int Cindice2= FIN3 - buffer;
            int CTamano=Cindice2-(Cindice+9);
            strncpy(Cextraida, buffer+(Cindice+9), CTamano);
            printf("Cantidad del producto: %s \n",Cextraida);
            prodconsul.Cantidad=atoi(Cextraida);
            }
        key_t llaver=ftok("Esteban",'r');
        int shmidr=shmget(llaver,sizeof(int),IPC_CREAT|0660);
        getProducto=shmat (shmidr,0,0);
        *getProducto=prodconsul;
        shmdt(&getProducto);
        break;
        }
    }
    }
    fclose (fc);
    sleep(5);
}

void Consulta_InfoU()
{
    FILE *fc;
    fc = fopen("Usuarios.dat", "rb");
    if (fc == NULL)
    {
        printf("Error en archivo");
        exit(1);
    }
    key_t llaveConsU = ftok("userconect", 'x');
    shmidU = shmget(llaveConsU, sizeof(int), IPC_CREAT | 0660);
    ConsulU = shmat(shmidU, 0, SHM_R | SHM_W);
    char buffer[150];
    char ch[1];
    int contador = 0;
    while (fgets(buffer, 150, fc) != NULL)
    {
        char *INICIO = strstr(buffer, "NOMBREU:");
        char *FIN = strstr(buffer, ":NOMBREUEND");
        char *INICIO2 = strstr(buffer, "PASSU:");
        char *FIN2 = strstr(buffer, ":PASSUEND");
        char extraida[30] = "";
        char Nextraida[10] = "";
        char *INICIO3 = strstr(buffer, "IDU:");
        char *FIN3 = strstr(buffer, ":IDUEND");
        char Iextraida[10] = "";
        if(FIN3!=NULL){
            int Iindice = INICIO3 - buffer;
            int Iindice2 = FIN3 - buffer;
            int ITamano = Iindice2 - (Iindice+4);
            strncpy(Iextraida, buffer + (Iindice+4), ITamano);
            printf("IDU EXTRAIDA %s!!!!\n\n",Iextraida);
            InicioSes.IDUsuario=atoi(Iextraida);
        }
        if (FIN != NULL)
        {
            int indice = INICIO - buffer;
            int indice2 = FIN - buffer;
            int Tamano = indice2 - (indice+8);
            strncpy(extraida, buffer + (indice+8), Tamano);
            strcpy(InicioSes.NUsuario,extraida);
        }
        if (FIN2 != NULL)
        {
            int Nindice = INICIO2 - buffer;
            int Nindice2 = FIN2 - buffer;
            int NTamano = Nindice2 - (Nindice + 6);
            strncpy(Nextraida, buffer + (Nindice + 6), NTamano);
            strcpy(InicioSes.Password,Nextraida);
        }
        if(strcmp(extraida,ConsulU->NUsuario)==0 && strcmp(Nextraida,ConsulU->Password)==0){
            *ConsulU=InicioSes;
            shmdt(&ConsulU);
            //shmctl(shmidU,IPC_RMID,0);
            break;
        }
    }
    fclose(fc);
}

void CreaArchivos()
{
 	FILE *fp;
    fp = fopen ( "Catalogo.dat", "w" );        
	if (fp==NULL) {fputs ("File error",stderr); exit (1);}
	fclose ( fp );
    fp = fopen ( "Clientes.txt", "w" );        
	if (fp==NULL) {fputs ("File error",stderr); exit (1);}
	fclose ( fp );
    fp = fopen ( "Carrito.dat", "w" );        
	if (fp==NULL) {fputs ("File error",stderr); exit (1);}
	fclose ( fp );
}
