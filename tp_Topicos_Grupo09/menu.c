#include "menu.h"

char menu(const char m[][TAM_MENU], const char *txt)
{
    printf("\n");
    char opcion;
    opcion = elegir_opcion(m, txt, "Elegir opcion: ");

    while(!strchr(m[0], opcion))
        opcion = elegir_opcion(m, txt, "Opcion erronea. Elegir nuevamente: ");

    return opcion;
}

char elegir_opcion(const char m[][TAM_MENU], const char *txt, const char *msj)
{
    char opcion;
    int i;

    system("CLS");
    printf("\n %s \n", txt);
    for ( i=1 ; i <= strlen(m[0]) ; i++)
        printf("\n %c - %s", m[0][i-1], m[i]);

    printf("\n\n%s", msj);
    fflush(stdin);
    scanf(" %c", &opcion);

    return toupper(opcion);
}
/**********************************************************
*                FUNCIONES ABM
***********************************************************/
void generar_Alta (long dniABM, t_indice* indice, const char* path, t_Fecha* fecha_proceso, int(*cmp)(const void*, const void*)){
    FILE* ab = fopen(path,"r+b");
    if(!ab){
        printf("\nERROR AL ABRIR EL ARCHIVO PARA GENERAR EL ALTA.");
        return;
    }
    t_Miembro miembro;
    miembro.DNI=dniABM;
    if(pedir_datos_miembro(&miembro,fecha_proceso)!=TRUE){
        printf("\nError al ingresar los datos.");
        fclose(ab);
        return;
    }
    /// Validación completa del miembro
    if (validar_miembro_alta(&miembro, fecha_proceso)==FALSE){
        printf("\nDatos invalidos. Alta cancelada.");
        fclose(ab);
        return;
    }

    /// Guardar en archivo al final
    fseek(ab,0L,SEEK_END);
    long nro_reg=ftell(ab)/sizeof(t_Miembro);
    fwrite(&miembro,sizeof(t_Miembro),1,ab);

    t_reg_indice nuevo_reg = {miembro.DNI,nro_reg};
    if(indice_insertar(indice,&nuevo_reg,sizeof(t_reg_indice),cmp)!=OK){
        printf("\nError al insertar en el indice.");
        fclose(ab);
        return;
    }
    fclose(ab);
    printf("\nUsuario %ld dado de alta con exito.", dniABM);
}
int pedir_datos_miembro (t_Miembro* miembro, t_Fecha* fechaproceso){
    int c;
    printf("Ingrese Apellido y Nombre:  ");
    while ((c = getchar()) != '\n' && c != EOF);
    fgets(miembro->ApellidosNombres, sizeof(miembro->ApellidosNombres), stdin);
    miembro->ApellidosNombres[strlen(miembro->ApellidosNombres)-1]='\0';
    normalizar_apellido_nombre(miembro);
    printf("Ingrese Fecha de Nacimiento (DD/MM/AAAA):  ");
    ingresarFecha(&miembro->FechaNacimiento);
    fflush(stdin);
    printf("Ingrese Sexo (M/F):  ");
    scanf(" %c", &miembro->Sexo);
    miembro->Sexo = toupper(miembro->Sexo);
    printf("Ingrese Fecha de Afiliacion (DD/MM/AAAA): ");
    ingresarFecha(&miembro->FechaAfiliacion);
    printf("Ingrese Categoria: ");
    while ((c = getchar()) != '\n' && c != EOF);
    scanf(" %10s", miembro->Categoria);
    printf("Ingrese Fecha de ultima cuota (DD/MM/AAAA): ");
    ingresarFecha(&miembro->FechaUltimaCuotaPaga);
    miembro->Estado='A';
    printf("Ingrese el Plan (BASIC/PREMIUM/VIP/FAMILY):  ");
    fgets(miembro->Plan, sizeof(miembro->Plan),stdin);
    miembro->Plan[strlen(miembro->Plan)-1]='\0';
    if(es_menor_de_edad(&miembro->FechaNacimiento, fechaproceso)){
        printf("\nEl miembro es menor de edad. Debe ingresar datos del tutor");
        printf("Ingrese Email del Tutor:  ");
        fgets(miembro->EmailTutor, sizeof(miembro->EmailTutor), stdin);
        miembro->EmailTutor[strlen(miembro->EmailTutor)-1]='\0';
    }
    else{
        strcpy(miembro->EmailTutor,"");
    }
    return TRUE;
}
void ingresarFecha(t_Fecha *fProceso){
    do{
        fflush(stdin);
        int cantidad = scanf("%d/%d/%d", &fProceso->dia, &fProceso->mes, &fProceso->anio);
        if(cantidad != 3){
            printf("Formato invalido. Ingrese nuevamente. \n");
            fflush(stdin);
            while(getchar() != '\n');
            continue;
        }
    }while(!validar_Fecha(fProceso));
}
int es_menor_de_edad(const t_Fecha* nacimiento, const t_Fecha* fecha_actual) {
    t_Fecha edad=difFechas(nacimiento, fecha_actual);
    return edad.anio<18;
}
int validar_miembro_alta(t_Miembro* miembro,const t_Fecha* fecha_proceso){
    if (ValidarDni(miembro->DNI)==FALSE){
        printf("\nERROR: DNI invalido.");
        return FALSE;
    }
    if (validarStringAndTwo(miembro->ApellidosNombres,60)==0) {
        printf("\nERROR: Apellido y nombre invalidos.");
        return FALSE;
    }
    t_Fecha aux;
    aux = difFechas(&miembro->FechaNacimiento, fecha_proceso);
    if (validar_Fecha(&miembro->FechaNacimiento)==FALSE||aux.anio<10) {
        printf("\nERROR: Fecha de nacimiento invalida.");
        return FALSE;
    }
    if (ValidarSexo(miembro->Sexo)==FALSE) {
        printf("\nERROR: Sexo invalido. Debe ser M o F.");
        return FALSE;
    }

    if (validar_Fecha(&miembro->FechaAfiliacion)==FALSE){
        printf("\nERROR: Fecha de afiliacion invalida.");
        return FALSE;
    }
    if(!(cmpFecha(miembro->FechaAfiliacion,*fecha_proceso)<=0&&cmpFecha(miembro->FechaAfiliacion, miembro->FechaNacimiento)>0)){
    printf("\nError: La fecha de afiliacion debe ser posterior a la fecha de nacimiento y no mayor a la fecha de proceso.");
    return FALSE;
    }

    if (Validar_Categoria(miembro->Categoria,&miembro->FechaNacimiento,fecha_proceso)==FALSE) {
        printf("\nERROR: Categoria invalida.");
        return FALSE;
    }
    if(validar_Fecha(&miembro->FechaUltimaCuotaPaga)==FALSE){
        printf("\nError: Fecha de ultima cuota invalida.");
        return FALSE;
    }
    if(cmpFecha(miembro->FechaUltimaCuotaPaga, miembro->FechaAfiliacion)<=0||cmpFecha(miembro->FechaUltimaCuotaPaga, *fecha_proceso)>0){
        printf("\nError: La fecha de ultima cuota debe ser posterior a la de afiliacion y no mayor a la fecha de proceso.");
        return FALSE;
    }
    if (validar_Plan(miembro->Plan)==FALSE) {
        printf("\nERROR: Plan invalido.");
        return FALSE;
    }
    if (es_menor_de_edad(&miembro->FechaNacimiento, fecha_proceso)){
        if (Validar_MailTutor(miembro->EmailTutor,miembro->Categoria)==FALSE){
            printf("\nERROR: Email del tutor invalido.");
            return FALSE;
        }
    }
    return ValidarEstado(miembro->Estado);;
}

int mostrarArchivoBinario(const char* nombreArchivoBin) {
    FILE* pBin = fopen(nombreArchivoBin, "rb");

    if(!pBin) {
        return ERR_ARCHIVO;
    }

    t_Miembro miembro;
    fread(&miembro, sizeof(t_Miembro), 1, pBin);

    printf("\n ******** MIEMBROS *********** \n");
    while(!feof(pBin)) {
        printf("%ld|%s|%d-%d-%d|%c|%d-%d-%d|%s|%d-%d-%d|%c|%s|%s\n", miembro.DNI, miembro.ApellidosNombres, miembro.FechaNacimiento.dia, miembro.FechaNacimiento.mes, miembro.FechaNacimiento.anio, miembro.Sexo, miembro.FechaAfiliacion.dia, miembro.FechaAfiliacion.mes, miembro.FechaAfiliacion.anio, miembro.Categoria, miembro.FechaUltimaCuotaPaga.dia, miembro.FechaUltimaCuotaPaga.mes, miembro.FechaUltimaCuotaPaga.anio, miembro.Estado, miembro.Plan, miembro.EmailTutor);
        fread(&miembro, sizeof(t_Miembro), 1, pBin);
    }

    fclose(pBin);

    return TODO_OK;
}
/// --------------------------------------------
/// Checkea la existencia del archivo
int existeArchivo(const char* path){
    FILE* f = fopen(path, "rb");
    if(!f) return 0;
    fclose(f);
    return 1;
}

void ingresarOpcionMenu(int* cod) {
    do {
       printf("Ingrese la opcion que desea: ");
       scanf("%d", cod);
    } while(*cod != 1 && *cod != 2);
}

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
