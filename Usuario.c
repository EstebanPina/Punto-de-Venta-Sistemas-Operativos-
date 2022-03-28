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
typedef int SESSION;
int fd, shmid, shmidinfo, shmidcambios, shmConsul, shmid3,shmCarr;
char acepta;
SESSION UID;
int cambios[2];
pid_t pid;
char c, idchar, shmitd;
char sel;
void Enviar_Consulta();
Info producto, *lugares, *getProducto;
dataU usuario,*infConsulta;
IDCarrito idBusqueda,*idPedido,*respuestaPedido;
void Conectar();
int main()
{
    UID=0;
    
    do
    {
        system("clear");
        printf("Iniciar Sesion\n\nUsuario:");
        __fpurge(stdin);
        scanf("%10s",usuario.NUsuario);
        printf("\n\nPassword:");
        __fpurge(stdin);
        scanf("%10s",usuario.Password);
        Conectar(11);
        Enviar_Consulta(usuario);
        shmdt(&infConsulta);
        sleep(3);
        UID=getIDU(); 
        if(UID==0){
            printf("Los datos ingresados no son validos. ¿Deseas volver a intentarlo?");
            printf("\n1-Si\n2-No");
            scanf("%s",&acepta);
            while(acepta!='1' && acepta!='2'){
            printf("ERROR OPCION NO VALIDA\nLos datos ingresados no son validos. ¿Deseas volver a intentarlo?");
            printf("\n1-Si\n2-No");
            scanf("%s",&acepta);
            }
        }
        while (UID!=0)
        {
            system("clear");
            printf("Bienvenido %s\n Menu de acciones:\n1-Ver catalogo\n2-Agregar a carrito\n3-Cerrar Sesion\n4-Salir\n",usuario.NUsuario);
            scanf("%s", &sel);
            if (sel == '1')
            {
                getCatalogo();
            }
            else if (sel == '2')
            {
            __fpurge(stdin);
            printf("Inserte el ID del producto\n");
            scanf("%10s",idBusqueda.IDCARRITO);
            __fpurge(stdin);
            idBusqueda.IDUsuario=UID;
            idBusqueda.Aceptacion=0;
            Enviar_Carrito(idBusqueda);
            Conectar(12);
            sleep(1);
            }
            else if (sel == '3')
            {
                break;
            }
            else if (sel == '4')
            {
                exit(1);
            }
            else
            {
                printf("Error. No existe esta accion");
            }
        }
    }while(acepta='1');
}

void ObtenerRespuestaCarrito(){
    key_t llaveCarr=ftok("ConexionCarr",'z');
    int shmidCarr=shmget(llaveCarr,sizeof(int),IPC_CREAT|0660);
    respuestaPedido=shmat (shmidCarr,0,0);
    printf("%i",respuestaPedido->Aceptacion);
    if(respuestaPedido->Aceptacion==1){
        printf("Producto Agregado al carrito con exito");
    }
    else{
        printf("El id que usted ingreso no es valido");
    }
    idPedido->Aceptacion=0;
}

void Enviar_Carrito(IDCarrito idcarrito){
    key_t llaveCarr=ftok("ConexionCarr",'s');
    key_t llavesem;
    llavesem = ftok("Semaforo",'k');
    shmCarr=shmget(llaveCarr,sizeof(int),IPC_CREAT|0660);
    idPedido=shmat (shmCarr,0,SHM_R|SHM_W);
    *idPedido=idcarrito;
    shmdt(&idPedido);
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
void Enviar_Consulta(dataU Consulta){
    key_t llaveConsul=ftok("userconect",'x');
    key_t llavesem;
    llavesem = ftok("Semaforo",'k');
    shmConsul=shmget(llaveConsul,sizeof(int),IPC_CREAT|0660);
    infConsulta=shmat (shmConsul,0,SHM_R|SHM_W);
    *infConsulta=Consulta;
    shmdt(&infConsulta);
}

int getIDU(){
    key_t llaveConsul=ftok("userconect",'x');
    key_t llavesem;
    llavesem = ftok("Semaforo",'k');
    shmConsul=shmget(llaveConsul,sizeof(int),IPC_CREAT|0660);
    infConsulta=shmat (shmConsul,0,SHM_R|SHM_W);
    return infConsulta->IDUsuario;
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
    getchar();
}
