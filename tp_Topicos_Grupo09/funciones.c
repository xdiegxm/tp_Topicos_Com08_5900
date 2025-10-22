/// ARCHIVO BINARIO PARTE 1
#define TAM_LINEA 500
#define TODO_OK 0
#define ERR_ARCHIVO 1
#define ERR_MEMORIA 2
#define ERR_LINEA_LARGA 3

typedef int (*TxtABin)(const char* linea, void* reg);
typedef bool (*EsErrorFatal)(int cod);

int convertirTxtABin(const char* nombreArchTxt, const char* nombreArchBin, size_t tamReg, TxtABin txtABin, EsErrorFatal esErrorFatal) {
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
        ret = txtABin(linea, reg);

        if(ret == TODO_OK) {
            fwrite(reg, tamReg, 1, pBin);
        }
    }

    fclose(pTxt);
    fclose(pBin);
    free(reg);

    return TODO_OK;
}

int txtABin(const char* linea, void* reg) {
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
    miembro->Plan[MAX_PLAN] = '\0';

    // Estado
    *act = '\0';
    act = strrchr(linea, '|');
    miembro->Estado = *(act + 1);

    // Fecha Ultima Cuota Paga
    *act = '\0';
    act = strrchr(linea, '|');
    sscanf(act + 1, "%d/%d/%d", &miembro->FechaUltimaCuotaPaga.dia, &miembro->FechaUltimaCuotaPaga.mes, &miembro->FechaUltimaCuotaPaga.anio);

    // Categoria
    *act = '\0';
    act = strrchr(linea, '|');
    strncpy(miembro->Categoria, act + 1, MAX_CAT);
    miembro->Categoria[MAX_CAT] = '\0';

    // Fecha Afiliacion
    *act = '\0';
    act = strrchr(linea, '|');
    sscanf(act + 1, "%d/%d/%d", &miembro->FechaAfiliacion.dia, &miembro->FechaAfiliacion.mes, &miembro->FechaAfiliacion.anio);

    // Sexo
    *act = '\0';
    act = strrchr(linea, '|');
    miembro->Sexo = *(act + 1);

    // Fecha Nacimiento
    *act = '\0';
    act = strrchr(linea, '|');
    sscanf(act + 1, "%d/%d/%d", &miembro->FechaNacimiento.dia, &miembro->FechaNacimiento.mes, &miembro->FechaNacimiento.anio);

    // AyN
    *act = '\0';
    act = strrchr(linea, '|');
    strncpy(miembro->ApellidosNombres, act + 1, MAX_APENOM);

    // DNI
    *act = '\0';
    sscanf(linea, "%d", &miembro->DNI);

    return TODO_OK;
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

int leerArchivoBinValidar(const char* nombreArchBin) {
    FILE* pBin = fopen(nombreArchBin, "rb");

    if(!pBin) {
        return ERR_ARCHIVO;
    }

    t_Miembro miembro;

    fread(&miembro, sizeof(t_Miembro), 1, pBin);
    int ret = 0;
    while(!feof(pBin)) {
       ret = validarMiembro(miembro);

       switch(ret) {
        case ERR_DNI:
            guardarRegistroError(&miembro, size_t(t_Miembro), "DNI Error");

        case ERR_NAC:
            guardarRegistroError(&miembro, size_t(t_Miembro), "Nacimiento Error");

        case ERR_SEXO:
            guardarRegistroError(&miembro, size_t(t_Miembro), "Sexo Error");

        case ERR_AFIL:
            guardarRegistroError(&miembro, size_t(t_Miembro), "Afiliacion Error");

        case ERR_CAT:
            guardarRegistroError(&miembro, size_t(t_Miembro), "Categoria Error");

        case ERR_ULT_CUOTA:
            guardarRegistroError(&miembro, size_t(t_Miembro), "Ult Cuota Error");

        case ERR_ESTADO:
            guardarRegistroError(&miembro, size_t(t_Miembro), "Estado Error");

        case ERR_PLAN:
            guardarRegistroError(&miembro, size_t(t_Miembro), "Plan Error");

        case ERR_MAIL:
            guardarRegistroError(&miembro, size_t(t_Miembro), "Mail Error");

        case ERR_MAIL_TUTOR:
            guardarRegistroError(&miembro, size_t(t_Miembro), "Mail Tutor Error");
       }

        fread(&miembro, sizeof(t_Miembro), 1, pBin);
    }
}

int validarMiembro(t_Miembro miembro) {
        if(ValidarDni(miembro.DNI) == FALSE) return ERR_DNI;

        if(Validar_Nacimiento(&miembro.FechaNacimiento, &miembro.FechaProceso) == FALSE) return ERR_NAC;

        if(ValidarSexo(miembro.Sexo) == FALSE) return ERR_SEXO;

        if(Validar_Afiliacion(&miembro.FechaAfiliacion, &miembro.FechaNacimiento, &miembro.FechaProceso) == FALSE) return ERR_AFIL;

        if(Validar_Categoria(&miembro.Categoria, &miembro.FechaNacimiento, &miembro.FechaProceso) == FALSE) return ERR_CAT;

        if(Validar_UltimaCuota(&miembro.FechaUltimaCuotaPaga, &miembro.FechaAfiliacion, &miembro.FechaProceso) == FALSE) return ERR_ULT_CUOTA;

        if(ValidarEstado(miembro.Estado) == FALSE) return ERR_ESTADO;

        if(validar_Plan(miembro.Plan) == FALSE) return ERR_PLAN;

        if(validar_Mail(&miembro.EmailTutor) == FALSE) return ERR_MAIL;

        if(Validar_MailTutor(&miembro.EmailTutor, &miembro.Categoria) == FALSE) return ERR_MAIL_TUTOR;
}
