#include "menu.h"

// ====================================================================
//                 FUNCIONES DE ARCHIVO
// ====================================================================

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

    pos_indice = indice_buscar(indice, &reg_busqueda, sizeof(t_reg_indice), comparar_dni);

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
    pos_indice = indice_buscar(indice, &reg_busqueda, sizeof(t_reg_indice), comparar_dni);

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

    if (indice_eliminar(indice, &reg_busqueda, sizeof(t_reg_indice), comparar_dni) != OK)
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
    pos_indice = indice_buscar(indice, &reg_busqueda, sizeof(t_reg_indice), comparar_dni);
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

// ====================================================================
//                 FUNCIONES DE LISTADOS
// ====================================================================
void mostrarMiembrosIndice(FILE *pf, const t_indice *indice)
{
    t_Miembro miembro;
    t_reg_indice *reg;
    unsigned i;

    printf("%-8s | %-25s | %-4s | %-10s | %-8s | %-10s\n",
           "DNI", "APELLIDO Y NOMBRE", "S", "PLAN", "CATEGORIA", "MAIL TUTOR");

    for (i = 0; i < indice->cantidad_elementos_actual; i++)
    {
        reg = (t_reg_indice *)((char *)indice->vindice + i * sizeof(t_reg_indice));
        if (leer_registro_archivo(pf, reg->nro_reg, &miembro) == OK && miembro.Estado == 'A')
        {
            printf("%08ld | %-25s | %c | %-10s | %-8s | %-10s\n",
                   miembro.DNI, miembro.ApellidosNombres,
                   miembro.Sexo, miembro.Plan,
                   miembro.Categoria, miembro.EmailTutor);
        }
    }
}

int ListadoOrdenadoDNI(const char *archivo, t_indice *indice)
{
    FILE *pf = abrir_archivo(archivo, "rb");
    if (!pf)
        return FALSE;

    printf("\n=== LISTADO DE MIEMBROS ORDENADOS POR DNI ===\n");
    mostrarMiembrosIndice(pf, indice);

    fclose(pf);
    return TRUE;
}

int ListadoAgrupadoPlan(const char *archivo, int (*cmp)(const void *, const void *))
{
    FILE *pf = abrir_archivo(archivo, "rb");
    if (!pf)
        return FALSE;

    t_Miembro miembro;
    unsigned nro_reg = 0;
    t_indice iBasic, iPremium, iVip, iFamily;
    t_reg_indice nuevo;

    //se crea un indice para cada plan
    indice_crear(&iBasic, CANTIDAD_ELEMENTOS, sizeof(t_reg_indice));
    indice_crear(&iPremium, CANTIDAD_ELEMENTOS, sizeof(t_reg_indice));
    indice_crear(&iVip, CANTIDAD_ELEMENTOS, sizeof(t_reg_indice));
    indice_crear(&iFamily, CANTIDAD_ELEMENTOS, sizeof(t_reg_indice));

    while (fread(&miembro, sizeof(t_Miembro), 1, pf) == 1) //leo el archivo miembros
    {
        if (miembro.Estado == 'A') //solo los miembros activos
        {
            nuevo.nro_reg = nro_reg;
            nuevo.dni = miembro.DNI;
            //se copian los datos a un indice nuevo, filtrando por tipo de plan
            if (strcmp(miembro.Plan, "BASIC") == 0)
                indice_insertar(&iBasic, &nuevo, sizeof(t_reg_indice), comparar_dni);
            else if (strcmp(miembro.Plan, "PREMIUM") == 0)
                indice_insertar(&iPremium, &nuevo, sizeof(t_reg_indice), comparar_dni);
            else if (strcmp(miembro.Plan, "VIP") == 0)
                indice_insertar(&iVip, &nuevo, sizeof(t_reg_indice), comparar_dni);
            else if (strcmp(miembro.Plan, "FAMILY") == 0)
                indice_insertar(&iFamily, &nuevo, sizeof(t_reg_indice), comparar_dni);
        }
        nro_reg++;
    }

    //muestro listado usando indices creados
    printf("\n=== LISTADO DE MIEMBROS AGRUPADOS POR PLAN ===\n");
    rewind(pf);

    printf("\n--- PLAN BASIC ---\n");
    mostrarMiembrosIndice(pf, &iBasic);

    printf("\n--- PLAN PREMIUM ---\n");
    mostrarMiembrosIndice(pf, &iPremium);

    printf("\n--- PLAN VIP ---\n");
    mostrarMiembrosIndice(pf, &iVip);

    printf("\n--- PLAN FAMILY ---\n");
    mostrarMiembrosIndice(pf, &iFamily);

    //vacio cada indice creado previamente
    indice_vaciar(&iBasic);
    indice_vaciar(&iPremium);
    indice_vaciar(&iVip);
    indice_vaciar(&iFamily);

    fclose(pf);
    return TRUE;
}
