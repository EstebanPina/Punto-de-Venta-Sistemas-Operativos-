#include <stdio.h>
#include <stdio_ext.h>
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
#include </home/estebanpina/Desktop/Proyecto1.2/Info.h>
#include </home/estebanpina/Desktop/Proyecto1.2/semaforo.h>
int fd,shmid,shmidinfo,shmidcambios,shmid2,shmid3,shmidchn;

int cambios[2];
IDdef *IDConsulta;
pid_t pid;
char c,idchar,shmitd;
char sel;
void Enviar_Informacion();
Info producto,*lugares,*getProducto;
CHNGQTY CambioLocal, *CambioSH;
void Conectar();
int main(){
    while(1){
    system("clear");
    printf("Bienvenido Proveedor\n Menu de acciones:\n1-Buscar Producto\n2-Insertar Nuevo Producto\n3-Agregar Existencias\n4-Salir\n");
    scanf("%s",&sel);
    if(sel=='1'){
        IDdef idBusqueda;
        printf("Inserte el ID del producto\n");
        scanf("%10s",idBusqueda.IDSHARED);
        __fpurge(stdin);
        Enviar_Consulta(idBusqueda);
        Conectar(1);
        sleep(6);
        Get_Respuesta();
    }
    else if(sel=='2'){
        printf("Inserte el ID del nuevo producto. (No insertar mas de 10 caracteres)\n");
        scanf("%10s",producto.ID);
        __fpurge(stdin);
        printf("Inserte el Nombre del nuevo producto. (No insertar mas de 60 caracteres)\n");
        scanf("%60[^\n]",producto.Nombre);
        __fpurge(stdin);
        printf("Inserte la cantidad de productos en stock. (Unicamente enteros)\n");
        scanf("%i",&producto.Cantidad);
        __fpurge(stdin);
        printf("%s,%s,%i",producto.ID,producto.Nombre,producto.Cantidad);
        Enviar_Informacion(producto);
        Conectar(2);
    }
    else if(sel=='3'){
        getCatalogo();
        printf("\nIngresa el id del producto que deseas agregar existencias\n");
        scanf("%10s",CambioLocal.IDCATALOG);
        __fpurge(stdin);
        printf("\nIngresa la cantidad que desea agregar\n");
        scanf("%i",&CambioLocal.QTY);
        __fpurge(stdin);
        AgregarCantidad(CambioLocal);
        Conectar(3);
    }
    else if(sel=='4'){
        break;
    }
    else{
        printf("Error. No existe esta accion");
    }
    
    }
}
void Conectar(int Sel){
    key_t llaveshm=ftok ("Memoria",'m');
    shmid=shmget(llaveshm,sizeof(int),0660);
    SELECT *conect = (int*)shmat(shmid,NULL,SHM_R|SHM_W);
    pid=getpid();
    conect->pid=pid;
    conect->Seleccion=Sel;
    shmdt(&conect);
}
void Enviar_Informacion(Info producto){
    key_t llaveinfo=ftok("Esteban",'i');
    key_t llavesem;
    llavesem = ftok("Semaforo",'k');
    shmidinfo=shmget(llaveinfo,sizeof(int),IPC_CREAT|0660);
    lugares=shmat (shmidinfo,0,SHM_R|SHM_W);
    *lugares=producto;
    shmdt(&lugares);
}
void Enviar_Consulta(IDdef Consulta){
    key_t llaveConsulta=ftok("Ids",'l');
    key_t llavesem;
    llavesem = ftok("Semaforo",'k');
    shmid2=shmget(llaveConsulta,sizeof(int),IPC_CREAT|0660);
    IDConsulta=shmat (shmid2,0,SHM_R|SHM_W);
    *IDConsulta=Consulta;
    shmdt(&IDConsulta);
}
void Get_Respuesta(){
    key_t llaveRespuesta=ftok("Esteban",'r');
    key_t llavesem;
    llavesem = ftok("Semaforo",'k');
    shmid3=shmget(llaveRespuesta,sizeof(int),IPC_CREAT|0660);
    getProducto= shmat (shmid3,0,SHM_R|SHM_W);
    printf("Informacion Obtenida: %s, %s, %i\n\n",getProducto->ID,getProducto->Nombre,getProducto->Cantidad);
    printf("Presione cualquier tecla para continuar");
    getchar();
    shmdt(&getProducto);
    shmctl(shmid3,IPC_RMID,0);
}
void AgregarCantidad(CHNGQTY CambioLcl){
    key_t llavechn=ftok("Esteban",'g');
    key_t llavesem;
    llavesem = ftok("Semaforo",'k');
    shmidchn=shmget(llavechn,sizeof(int),IPC_CREAT|0660);
    CambioSH=shmat (shmidchn,0,SHM_R|SHM_W);
    *CambioSH=CambioLcl;
    shmdt(&CambioSH);
}
void getCatalogo(){
    printf("ID PRODUCTO.\t\t NOMBRE PROD.\t\t\t\t\t\t CANTIDAD\n");
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
        char *INICIO2 = strstr(buffer, "NOMBRE:");
        char *FIN2 = strstr(buffer, ":NOMBREEND");
        char extraida[50] = "";
        char Nextraida[10] = "";
        char *INICIO3 = strstr(buffer, "CANTIDAD:");
        char *FIN3 = strstr(buffer, ":CANTIDADEND");
        char Iextraida[10] = "";
        if(FIN!=NULL){
            int Iindice = INICIO - buffer;
            int Iindice2 = FIN - buffer;
            int ITamano = Iindice2 - (Iindice+3);
            strncpy(Iextraida, buffer + (Iindice+3), ITamano);
        }
        if (FIN2 != NULL)
        {
            int indice = INICIO2 - buffer;
            int indice2 = FIN2 - buffer;
            int Tamano = indice2 - (indice+7);
            strncpy(extraida, buffer + (indice+7), Tamano);
        }
        if (FIN3 != NULL)
        {
            int Nindice = INICIO3 - buffer;
            int Nindice2 = FIN3 - buffer;
            int NTamano = Nindice2 - (Nindice + 9);
            strncpy(Nextraida, buffer + (Nindice + 9), NTamano);
        }
        printf("|%10s|\t\t|%40s|\t\t|%5s|\n",Iextraida,extraida,Nextraida);
        
    }
    fclose(fc);
    __fpurge(stdin);
}