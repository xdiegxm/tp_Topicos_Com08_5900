#include "menu.h"

//ABRE EL ARCHIVO EN EL MODO QUE SE LE MANDE
FILE *abrir_archivo(const char *nombre_archivo, const char *modo)
{
    FILE *fp = fopen(nombre_archivo, modo);
    if (!fp)
    {
        fprintf(stderr, "ERROR: No se pudo abrir el archivo: %s en modo %s\n", nombre_archivo, modo);
    }
    return fp;
}


int leer_registro_archivo(FILE *fp, unsigned nro_reg, t_Miembro *reg)
{
    long offset = nro_reg * sizeof(t_Miembro);

    if (fseek(fp, offset, SEEK_SET) != 0)
    {
        fprintf(stderr, "ERROR: Fallo fseek en leer_registro_archivo para %u\n", nro_reg);
        return ERROR;
    }
    if (fread(reg, sizeof(t_Miembro), 1, fp) != 1)
    {
        fprintf(stderr, "ERROR: Fallo fread en leer_registro_archivo para %u\n", nro_reg);
        return ERROR;
    }
    return OK;
}

int escribir_registro_archivo(FILE *fp, unsigned nro_reg, const t_Miembro *reg)
{
    long offset = nro_reg * sizeof(t_Miembro);

    if (fseek(fp, offset, SEEK_SET) != 0)
    {
        fprintf(stderr, "ERROR: Fallo fseek en escribir_registro_archivo para reg %u\n", nro_reg);
        return ERROR;
    }
    if (fwrite(reg, sizeof(t_Miembro), 1, fp) != 1)
    {
        fprintf(stderr, "ERROR: Fallo fwrite en escribir_registro_archivo para reg %u\n", nro_reg);
        return ERROR;
    }

    fflush(fp);
    return OK;
}

int validar_miembro(const t_Miembro *miembro)
{
    if (miembro->DNI < 1000000)
        return ERROR;
    // asumo que la validación es exitosa si pasa un chequeo básico xd.
    return OK;
}

void miembro_mostrar_datos(const t_Miembro *miembro)
{
    printf("\n--- Informacion del Miembro ---\n");
    printf("DNI: %ld\n", miembro->DNI);
    printf("Nombre: %s\n", miembro->ApellidosNombres);
    printf("F. Nacimiento: %02d/%02d/%d\n", miembro->FechaNacimiento.dia, miembro->FechaNacimiento.mes, miembro->FechaNacimiento.anio);
    printf("Plan: %s\n", miembro->Plan);
    printf("Estado: %c (%s)\n", miembro->Estado, miembro->Estado == 'A' ? "Activo" : "Baja");
    printf("Email Tutor: %s\n", miembro->EmailTutor);
    printf("-------------------------------\n");
}

// ====================================================================
//                 FUNCIONES DEL MENU
// ====================================================================

int miembro_modificar(t_indice *indice, FILE *fp)
{
    long dni_a_modificar;
    t_reg_indice reg_busqueda, *reg_indice_encontrado;
    t_Miembro miembro_actual;
    int pos_indice;

    printf("\n--- MODIFICACION DE MIEMBRO ---\n");
    printf("Ingrese DNI del miembro a modificar: ");
    if (scanf("%ld", &dni_a_modificar) != 1)
    {
        printf("Error: Entrada invalida.\n");
        while (getchar() != '\n');
        return ERROR;
    }
    while (getchar() != '\n');

    reg_busqueda.dni = dni_a_modificar;

    pos_indice = indice_buscar(indice, &reg_busqueda, sizeof(t_reg_indice), cmp);

    if (pos_indice == NO_EXISTE)
    {
        printf("ERROR: El DNI %ld NO existe en el indice.\n", dni_a_modificar);
        return ERROR;
    }

    reg_indice_encontrado = (t_reg_indice*)((char*)indice->vindice + pos_indice * sizeof(t_reg_indice));
    unsigned nro_reg_archivo = reg_indice_encontrado->nro_reg;

    if (leer_registro_archivo(fp, nro_reg_archivo, &miembro_actual) != OK)
    {
        printf("ERROR: No se pudo leer el registro %u del archivo.\n", nro_reg_archivo);
        return ERROR;
    }

    printf("--- Datos Actuales ---\n");
    printf("Plan actual: %s\n", miembro_actual.Plan);
    printf("Ingrese NUEVO Plan (Max %d chars): ", MAX_PLAN - 1);

    char buffer_plan[MAX_PLAN];
    if (fgets(buffer_plan, MAX_PLAN, stdin) == NULL)
    {
        printf("Error: Lectura de Plan fallida.\n");
        return ERROR;
    }
    buffer_plan[strcspn(buffer_plan, "\n")] = 0;

    strcpy(miembro_actual.Plan, buffer_plan);

    if (validar_miembro(&miembro_actual) != OK)
    {
        printf("ERROR: Se detectaron errores en la validacion de la nueva informacion. Se IGNORA la modificacion.\n");
        return ERROR;
    }

    if (escribir_registro_archivo(fp, nro_reg_archivo, &miembro_actual) != OK)
    {
        printf("ERROR: No se pudo escribir el registro modificado en el archivo.\n");
        return ERROR;
    }

    printf("EXITO: El miembro %ld ha sido modificado correctamente.\n", dni_a_modificar);
    return OK;
}


int miembro_dar_baja(t_indice *indice, FILE *fp)
{
    long dni_a_dar_baja;
    t_reg_indice reg_busqueda, *reg_indice_encontrado;
    t_Miembro miembro_actual;
    int pos_indice;

    printf("\n--- BAJA DE MIEMBRO ---\n");
    printf("Ingrese DNI del miembro a dar de baja: ");
    if (scanf("%ld", &dni_a_dar_baja) != 1)
    {
        printf("ERROR: Entrada invalida.\n");
        while (getchar() != '\n');
        return ERROR;
    }
    while (getchar() != '\n');
    reg_busqueda.dni = dni_a_dar_baja;
    pos_indice = indice_buscar(indice, &reg_busqueda, sizeof(t_reg_indice), cmp);

    if (pos_indice == NO_EXISTE)
    {
        printf("ERROR: El DNI %ld NO existe en el indice (posiblemente ya este de baja).\n", dni_a_dar_baja);
        return ERROR;
    }

    reg_indice_encontrado = (t_reg_indice*)((char*)indice->vindice + pos_indice * sizeof(t_reg_indice));
    unsigned nro_reg_archivo = reg_indice_encontrado->nro_reg;

    if (leer_registro_archivo(fp, nro_reg_archivo, &miembro_actual) != OK)
    {
        printf("ERROR: No se pudo leer el registro %u del archivo para la baja.\n", nro_reg_archivo);
        return ERROR;
    }

    if (miembro_actual.Estado == 'B')
    {
        printf("ERROR: El miembro %ld ya estaba marcado como baja en el archivo.\n", dni_a_dar_baja);
    }

    miembro_actual.Estado = 'B'; // Actualizar el estado a Baja

    if (escribir_registro_archivo(fp, nro_reg_archivo, &miembro_actual) != OK)
    {
        printf("ERROR: No se pudo escribri el registro marcado como Baja.\n");
        return ERROR;
    }

    if (indice_eliminar(indice, &reg_busqueda, sizeof(t_reg_indice), cmp) != OK)
    {
        printf("ERROR: Fallo al eliminar el DNI %ld del indice.\n", dni_a_dar_baja);
        return ERROR;
    }

    printf("EXITO: El miembro %ld ha sido dado de baja (marcado en archivo y eliminado del indice).\n", dni_a_dar_baja);
    return OK;
}


int miembro_mostrar_info(const t_indice *indice, FILE *fp)
{
    long dni_a_mostrar;
    t_reg_indice reg_busqueda, *reg_indice_encontrado;
    t_Miembro miembro_info;
    int pos_indice;

    printf("\n--- MOSTRAR INFORMACION DE MIEMBRO ---\n");
    printf("Ingrese DNI del miembro a mostrar: ");
    if (scanf("%ld", &dni_a_mostrar) != 1)
    {
        printf("ERRORR Entrada invalida.\n");
        while (getchar() != '\n');
        return ERROR;
    }
    while (getchar() != '\n');
    reg_busqueda.dni = dni_a_mostrar;
    pos_indice = indice_buscar(indice, &reg_busqueda, sizeof(t_reg_indice), cmp);
    if (pos_indice == NO_EXISTE)
    {
        printf("ERROR: El DNI %ld NO existe en el indice. (Miembro Inactivo o no existe)\n", dni_a_mostrar);
        return ERROR;
    }
    reg_indice_encontrado = (t_reg_indice*)((char*)indice->vindice + pos_indice * sizeof(t_reg_indice));
    unsigned nro_reg_archivo = reg_indice_encontrado->nro_reg;
    if (leer_registro_archivo(fp, nro_reg_archivo, &miembro_info) != OK)
    {
        printf("ERROR: No se pudo leer el registro %u del archivo.\n", nro_reg_archivo);
        return ERROR;
    }
    miembro_mostrar_datos(&miembro_info);
    return OK;
}

