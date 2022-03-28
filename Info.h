typedef struct Info{
    char ID[10];
    char Nombre[60];
    int Cantidad;
} Info;

typedef struct SELECT{
    int Seleccion;
    int pid;
}SELECT;

typedef struct IDdef{
    char IDSHARED[10];
}IDdef;

typedef struct dataU{
    int IDUsuario;
    char NUsuario[30];
    char Password[10];
} dataU;

typedef struct IDCarrito{
    int IDUsuario;
    char IDCARRITO[10];
    int Aceptacion;
}IDCarrito;

typedef struct CHNGQTY{
    char IDCATALOG[10];
    int QTY;
}CHNGQTY;