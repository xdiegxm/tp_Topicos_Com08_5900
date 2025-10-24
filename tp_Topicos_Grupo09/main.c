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

    int cod = 0;

    printf("************** ELEGIR OPCIONES **************\n\n");
    printf("Opcion 1: Generar nuevo archivo\n");
    printf("Opcion 2: Buscar archivo con fecha de proceso\n");
    printf("\n*********************************************\n");


    ingresarOpcionMenu(&cod);

    ingresoFechaProceso(fechaProceso);
    char* nombreArchivoError = concatenarFechaConArchivo("error-VC-", fechaProceso, ".txt");
    char* nombreArchivoBin = concatenarFechaConArchivo("miembros-VC-", fechaProceso, ".dat");

    if(cod == 1) {
        printf("\nSe generara un nuevo archivo .dat\n");
        convertirTxtABin("miembros-VC.txt", nombreArchivoBin, nombreArchivoError, sizeof(t_Miembro), fechaProceso, txtABin, esErrorFatal);
    } else {
        if(!existeArchivo(nombreArchivoBin)) {
            printf("No existe archivo con la fecha ingresada. Se generara un nuevo archivo .dat\n");
            convertirTxtABin("miembros-VC.txt", nombreArchivoBin, nombreArchivoError, sizeof(t_Miembro), fechaProceso, txtABin, esErrorFatal);
        }
    }

    mostrarArchivoBinario(nombreArchivoBin);


    free(fechaProceso);
    free(miembro);

    return 0;
}
