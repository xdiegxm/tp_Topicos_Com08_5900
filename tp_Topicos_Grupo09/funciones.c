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
    strncpy(miembro->EmailTutor, act + 1, MAX_EMAIL - 1);
    miembro->EmailTutor[MAX_EMAIL-1] = '\0';

    // Plan
    *act = '\0';
    act = strrchr(linea, '|');
    strncpy(miembro->Plan, act + 1, MAX_PLAN - 1);
    miembro->Plan[MAX_PLAN - 1] = '\0';
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
    strncpy(miembro->Categoria, act + 1, MAX_CAT - 1);
    miembro->Categoria[MAX_CAT - 1] = '\0';
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
    strncpy(miembro->ApellidosNombres, act + 1, MAX_APENOM - 1);
    miembro->ApellidosNombres[MAX_APENOM - 1] = '\0';
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

    sprintf(fechaFormatoTexto, "%04d%02d%02d", fProceso->anio, fProceso->mes, fProceso->dia);

    // Calcular el tamaño total necesario
    size_t tam = strlen(nombreArch) + strlen(fechaFormatoTexto) + strlen(extension) + 1;

    // Reservar memoria dinámica
    char* nombreArchivo = malloc(tam);
    if (!nombreArchivo) {
        return NULL;
    }

    strcpy(nombreArchivo, nombreArch);
    strcat(nombreArchivo, fechaFormatoTexto);
    strcat(nombreArchivo, extension);

    return nombreArchivo;
}
