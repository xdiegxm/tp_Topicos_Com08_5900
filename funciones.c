/// ARCHIVO BINARIO PARTE 1
#include "validacion.h"
#include "funciones.h"


int convertirTxtABin(const char* nombreArchTxt, const char* nombreArchBin, const char* nombreArchError, size_t tamReg, const t_Fecha* fechaProceso, TxtABin txtABin, EsErrorFatal esErrorFatal) {
    FILE* pTxt = fopen(nombreArchTxt, "rt");

    if(!pTxt) {
        return ERR_ARCHIVO;
    }

    FILE* pBin = fopen(nombreArchBin, "wb");

    if(!pBin) {
        fclose(pTxt);
        return ERR_ARCHIVO;
    }

    void* reg = malloc(tamReg);

    char linea[TAM_LINEA + 1];
    int ret = TODO_OK;
    while(!esErrorFatal(ret) && fgets(linea, TAM_LINEA, pTxt)) {
        ret = txtABin(linea, reg, fechaProceso);

        if(ret == TODO_OK) {
            fwrite(reg, tamReg, 1, pBin);
        } else {
            if(ret != TODO_OK && ret != ERR_LINEA_LARGA) {
                char mensaje[30];
                strcpy(mensaje, obtenerMensajeError(ret));

                guardarRegistroError(nombreArchError, reg, tamReg, mensaje);
            }
        }
    }

    fclose(pTxt);
    fclose(pBin);
    free(reg);

    return TODO_OK;
}

int txtABin(const char* linea, void* reg, const t_Fecha* fechaProceso) {
    t_Miembro* miembro = reg;

    char* act = strchr(linea, '\n');

    if(!act) {
        return ERR_LINEA_LARGA;
    }

    // Email
    *act = '\0';
    act = strrchr(linea, '|');
    strncpy(miembro->EmailTutor, act + 1, MAX_EMAIL);
    miembro->EmailTutor[MAX_EMAIL] = '\0';

    // Plan
    *act = '\0';
    act = strrchr(linea, '|');
    strncpy(miembro->Plan, act + 1, MAX_PLAN);
    // miembro->Plan[MAX_PLAN] = '\0';

    // Estado
    *act = '\0';
    act = strrchr(linea, '|');
    miembro->Estado = *(act + 1);

    // Fecha Ultima Cuota Paga
    *act = '\0';
    act = strrchr(linea, '|');
    sscanf(act + 1, "%d-%d-%d", &miembro->FechaUltimaCuotaPaga.anio, &miembro->FechaUltimaCuotaPaga.mes, &miembro->FechaUltimaCuotaPaga.dia);

    // Categoria
    *act = '\0';
    act = strrchr(linea, '|');
    strncpy(miembro->Categoria, act + 1, MAX_CAT);
    // miembro->Categoria[MAX_CAT] = '\0';

    // Fecha Afiliacion
    *act = '\0';
    act = strrchr(linea, '|');
    sscanf(act + 1, "%d-%d-%d", &miembro->FechaAfiliacion.anio, &miembro->FechaAfiliacion.mes, &miembro->FechaAfiliacion.dia);

    // Sexo
    *act = '\0';
    act = strrchr(linea, '|');
    miembro->Sexo = *(act + 1);

    // Fecha Nacimiento
    *act = '\0';
    act = strrchr(linea, '|');
    sscanf(act + 1, "%d-%d-%d", &miembro->FechaNacimiento.anio, &miembro->FechaNacimiento.mes, &miembro->FechaNacimiento.dia);

    // Fecha Proceso
    miembro->FechaProceso.anio = fechaProceso->anio;
    miembro->FechaProceso.mes = fechaProceso->mes;
    miembro->FechaProceso.dia = fechaProceso->dia;

    // AyN
    *act = '\0';
    act = strrchr(linea, '|');
    strncpy(miembro->ApellidosNombres, act + 1, MAX_APENOM);
    normalizar_apellido_nombre(miembro);

    // DNI
    *act = '\0';
    sscanf(linea, "%ld", &miembro->DNI);

    int ret = validarMiembro(miembro);

    return ret;
}

bool esErrorFatal(int cod)
{
    switch(cod)
    {
        case ERR_LINEA_LARGA:
            return true;

        default:
            return false;
    }
}

char* obtenerMensajeError(int ret) {
   switch(ret) {
    case ERR_DNI:
        return "Dni invalido";

    case ERR_NAC:
        return "Fecha nacimiento invalido";

    case ERR_SEXO:
        return "Sexo invalido";

    case ERR_AFIL:
        return "Fecha afiliacion invalido";

    case ERR_CAT:
        return "Categoria invalido";

    case ERR_ULT_CUOTA:
        return "Fecha ultima cuota invalido";

    case ERR_ESTADO:
        return "Estado invalido";

    case ERR_PLAN:
        return "Plan invalido";

    case ERR_MAIL:
        return "Email invalido";

    case ERR_MAIL_TUTOR:
        return "Email tutor invalido";

    default:
        return "";
   }
}

int validarMiembro(t_Miembro* miembro) {
        if(ValidarDni(miembro->DNI) == FALSE) return ERR_DNI;

        if(Validar_Nacimiento(&miembro->FechaNacimiento, &miembro->FechaProceso) == FALSE) return ERR_NAC;

        if(ValidarSexo(miembro->Sexo) == FALSE) return ERR_SEXO;

        if(Validar_Afiliacion(&miembro->FechaAfiliacion, &miembro->FechaNacimiento, &miembro->FechaProceso) == FALSE) return ERR_AFIL;

        if(Validar_Categoria(miembro->Categoria, &miembro->FechaNacimiento, &miembro->FechaProceso) == FALSE) return ERR_CAT;

        if(Validar_UltimaCuota(&miembro->FechaUltimaCuotaPaga, &miembro->FechaAfiliacion, &miembro->FechaProceso) == FALSE) return ERR_ULT_CUOTA;

        if(ValidarEstado(miembro->Estado) == FALSE) return ERR_ESTADO;

        if(validar_Plan(miembro->Plan) == FALSE) return ERR_PLAN;

        if(validar_Mail(miembro->EmailTutor) == FALSE) return ERR_MAIL;

        if(Validar_MailTutor(miembro->EmailTutor, miembro->Categoria) == FALSE) return ERR_MAIL_TUTOR;

    return TODO_OK;
}

int guardarRegistroError(const char* nombreArch, const void* reg, size_t tamElem, const char* mensaje) {
    FILE* pTxtError = fopen(nombreArch, "a+");

    if(!pTxtError) {
        return ERR_ARCHIVO;
    }

    const t_Miembro* miembro = reg;

    fprintf(pTxtError, "%ld|%s|%d-%d-%d|%c|%d-%d-%d|%s|%d-%d-%d|%c|%s|%s|%s\n", miembro->DNI, miembro->ApellidosNombres, miembro->FechaNacimiento.dia, miembro->FechaNacimiento.mes, miembro->FechaNacimiento.anio, miembro->Sexo, miembro->FechaAfiliacion.dia, miembro->FechaAfiliacion.mes, miembro->FechaAfiliacion.anio, miembro->Categoria, miembro->FechaUltimaCuotaPaga.dia, miembro->FechaUltimaCuotaPaga.mes, miembro->FechaUltimaCuotaPaga.anio, miembro->Estado, miembro->Plan, miembro->EmailTutor, mensaje);

    fclose(pTxtError);

    return TODO_OK;
}

/// Ingreso de fecha proceso
void ingresoFechaProceso(t_Fecha* fProceso) {
    do {
        printf("Ingrese la fecha de proceso (aaaa): ");
        scanf("%d",&fProceso->anio);

        printf("Ingrese la fecha de proceso (mm): ");
        scanf("%d",&fProceso->mes);

        printf("Ingrese la fecha de proceso (dd): ");
        scanf("%d",&fProceso->dia);

    } while(validar_Fecha(fProceso) == FALSE);
}

/// Concatenar fecha proceso con nombre de archivos
char* concatenarFechaConArchivo(char* nombreArch, const t_Fecha* fProceso, const char* extension) {
    char fechaFormatoTexto[20];

    // Calcular el tamaño total necesario
    size_t tam = strlen(nombreArch) + strlen(fechaFormatoTexto) + strlen(extension) + 1;

    // Reservar memoria dinámica
    char* nombreArchivo = malloc(tam);
    if (!nombreArchivo) {
        return NULL;
    }

    strcpy(nombreArchivo, nombreArch);
    sprintf(fechaFormatoTexto, "%04d%02d%02d", fProceso->anio, fProceso->mes, fProceso->dia);
    strcat(nombreArchivo, fechaFormatoTexto);
    strcat(nombreArchivo, extension);

    return nombreArchivo;
}

int mostrarArchivoBinario(const char* nombreArchivoBin) {
    FILE* pBin = fopen(nombreArchivoBin, "rb");

    if(!pBin) {
        return ERR_ARCHIVO;
    }

    t_Miembro miembro;

    printf("\n ******** MIEMBROS *********** \n");
    while(fread(&miembro, sizeof(t_Miembro), 1, pBin)) {
        printf("%ld|%s|%d-%d-%d|%c|%d-%d-%d|%s|%d-%d-%d|%c|%s|%s\n", miembro.DNI, miembro.ApellidosNombres, miembro.FechaNacimiento.dia, miembro.FechaNacimiento.mes, miembro.FechaNacimiento.anio, miembro.Sexo, miembro.FechaAfiliacion.dia, miembro.FechaAfiliacion.mes, miembro.FechaAfiliacion.anio, miembro.Categoria, miembro.FechaUltimaCuotaPaga.dia, miembro.FechaUltimaCuotaPaga.mes, miembro.FechaUltimaCuotaPaga.anio, miembro.Estado, miembro.Plan, miembro.EmailTutor);
    }

    fclose(pBin);

    return TODO_OK;
}
/**********************************************************
*                FUNCIONES MENU                           *
***********************************************************/

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






