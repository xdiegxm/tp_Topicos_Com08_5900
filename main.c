#include <stdio.h>
#include <stdlib.h>
#include "validacion.h"
#include "funciones.h"

int main()
{
    t_Fecha* fechaProceso = malloc(sizeof(t_Fecha));
    t_Miembro* miembro = malloc(sizeof(t_Miembro));

    if(!fechaProceso || !miembro) {
        return ERR_MEMORIA;
    }

    ingresoFechaProceso(fechaProceso);

    char* nombreArchivoError = concatenarFechaConArchivo("error-VC-", fechaProceso, ".txt");
    char* nombreArchivoBin = concatenarFechaConArchivo("miembros-VC-", fechaProceso, ".dat");

    printf("%s \n", nombreArchivoError);
    printf("%s \n", nombreArchivoBin);

    convertirTxtABin("miembros-VC.txt", nombreArchivoBin, nombreArchivoError, sizeof(t_Miembro), fechaProceso, txtABin, esErrorFatal);
    mostrarArchivoBinario(nombreArchivoBin);

    free(fechaProceso);
    return 0;
}
