#include <stdio.h>
#include <stdlib.h>
#include "validacion.h"
#include "funciones.h"
#include "indice.h"

int main()
{
    t_Fecha* fechaProceso = malloc(sizeof(t_Fecha));
    t_Miembro* miembro = malloc(sizeof(t_Miembro));
    t_indice* indice = malloc(sizeof(t_indice));
    if(!fechaProceso || !miembro || !indice) {
        return ERR_MEMORIA;
    }

    ingresoFechaProceso(fechaProceso);

    char* nombreArchivoError = concatenarFechaConArchivo("error-VC-", fechaProceso, ".txt");
    char* nombreArchivoBin = concatenarFechaConArchivo("miembros-VC-", fechaProceso, ".dat");

    printf("%s \n", nombreArchivoError);
    printf("%s \n", nombreArchivoBin);

    convertirTxtABin(nombreArchivoBin, nombreArchivoBin, nombreArchivoError, sizeof(t_Miembro), fechaProceso, txtABin, esErrorFatal);
    mostrarArchivoBinario(nombreArchivoBin);

    free(fechaProceso);
    indice_crear(indice,CANTIDAD_ELEMENTOS,sizeof(t_reg_indice));
    int flag=indice_cargar(nombreArchivoBin,indice,sizeof(t_reg_indice),comparar_dni);
    if(flag==ERROR)
        system("PAUSE");

    t_reg_indice* regAMB=(t_reg_indice*)indice->vindice;
    for(int i=0; i<indice->cantidad_elementos_actual; i++)
    {
        printf("DNI: %-8ld\tPOS:%-5i\n", regAMB[i].dni, regAMB[i].nro_reg);
    }

    const char opciones [][TAM_MENU] = {"ABCDEFG",
                                        "Alta de miembro",
                                        "Baja de miembro",
                                        "Modificar miembro",
                                        "Mostrar informacion de un miembro",
                                        "Listado de miembros ordenados por DNI",
                                        "Listado de todos los socios agrupados por plan",
                                        "Salir"
                                       };

    char opc;
    do{
        opc=menu(opciones, "Menu Principal");
        switch(opc){
        case 'A':
            ///codigo
            system("PAUSE");
            break;
        case 'B':
            ///codigo
            system("PAUSE");
            break;
        case 'C':
            ///codigo
            system("PAUSE");
            break;
        case 'D':
            ///codigo
            system("PAUSE");
            break;
        case 'E':
            ///codigo
            system("PAUSE");
            break;
        case 'F':
            ///codigo
            system("PAUSE");
            break;
        }
    }while(opc!='G');
    return 0;
}
