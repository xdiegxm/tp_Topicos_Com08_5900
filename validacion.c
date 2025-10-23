#include "validacion.h"
/**********************************************************
*                VALIDACION   DATOS                       *
***********************************************************/
int validar_Plan (char plan[10]){
    char *opc[] = {"BASIC", "PREMIUM", "VIP", "FAMILY"};
    int i;
    str_toupper(plan);
    for(i=0;i<4;i++){
        if(strcmp(plan,opc[i])==0)
            return TRUE;
    }
    return FALSE;
}
int ValidarDni(long dni){
    return (dni>10000 && dni<100000000)?TRUE:FALSE;
}
int ValidarSexo(char s){
    s = toUpper(s);
    return (s=='F' || s=='M')? TRUE:FALSE;
}
int ValidarEstado(char e){
    e= toUpper(e);
    return (e=='A' || e=='B')? TRUE:FALSE;
}
int validar_Mail(const char* mail){
    if(mail == NULL || strlen(mail)==0)
        return FALSE;
    if(strchr(mail,' ')!=NULL||strchr(mail,'\t')!=NULL||strchr(mail,'\n')!=NULL)
        return FALSE;
    const char  *arroba = strchr(mail,'@');
    if(!arroba)
        return FALSE;
    if(arroba==mail||*(arroba+1)=='\0')
        return FALSE;
    if(strchr(arroba+1,'@'))
        return FALSE;
    const char *punto = strrchr(arroba,'.');
    if(!punto||punto<arroba+2||*(punto+1)=='\0')
        return FALSE;
    return TRUE;
}
int Validar_Categoria(const char *categ, const t_Fecha *fechaNac, const t_Fecha *fechaProc){
    t_Fecha edad = difFechas(fechaNac, fechaProc);

    if(edad.anio<18)
        return(strcmp(categ,"MENOR")==0)? TRUE:FALSE;
    else
        return(strcmp(categ,"ADULTO")==0)? TRUE:FALSE;
}
int Validar_MailTutor(const char *mail, const char *categ){

    if(strcmp(categ,"MENOR")==0)
       return validar_Mail(mail);

    return (mail==NULL || strlen(mail)==0)? TRUE:FALSE;
}
int Validar_Afiliacion(const t_Fecha *fechaAfil, const t_Fecha *fechaNac, const t_Fecha *fechaProc){

    if(!validar_Fecha(fechaAfil))
        return FALSE;
    if(difFechas(fechaAfil,fechaProc).anio<0)
        return FALSE;
    if(difFechas(fechaAfil,fechaNac).anio>=0)
        return FALSE;

    return TRUE;
}
int Validar_UltimaCuota(const t_Fecha *fechaUltCuota, const t_Fecha *fechaAfil, const t_Fecha *fechaProc){

    if(difFechas(fechaUltCuota,fechaAfil).anio>=0)
        return FALSE;
    if(difFechas(fechaUltCuota,fechaProc).anio<0)
        return FALSE;

    return TRUE;
}
int Validar_Nacimiento(const t_Fecha *fechaNac, const t_Fecha *fechaProc){
    if(!validar_Fecha(fechaNac))
        return FALSE;
    if(difFechas(fechaNac,fechaProc).anio<10)
        return FALSE;

    return TRUE;
}

/**********************************************************
**                FUNCIONES STRING                        *
***********************************************************/
void str_toupper(char* str){
    while(*str){
        *str = toUpper(*str);
        str++;
    }
}

void normalizar_apellido_nombre(t_Miembro *miembro_ptr)
{
  char buffer_temporal[MAX_APENOM];
  char *fin=miembro_ptr->ApellidosNombres;
  char *lectura=fin;
  char *escritura=buffer_temporal;

  bool nueva_palabra=true;

  while(*lectura==' ')
    lectura++;

  while(*lectura!='\0')
  {
      if (isspace(*lectura)) {
            if (!isspace(*(escritura - 1))) {
                *escritura = ' ';
                escritura++;
                nueva_palabra = true;
            }
        } else {
            if (nueva_palabra) {
                *escritura = toupper(*lectura);
                nueva_palabra = false;
            } else {
                *escritura = tolower(*lectura);
            }
            escritura++;
        }
        lectura++;
    }
    while (escritura > buffer_temporal && isspace(*(escritura - 1))) {
        escritura--;
    }
    *escritura='\0';
    char *comma=strchr(buffer_temporal,',');

    if(comma==NULL)
    {
        char *primer_espacio=strchr(buffer_temporal,' ');

        if(primer_espacio!=NULL){
            escritura=buffer_temporal+strlen(buffer_temporal);
            memmove(primer_espacio+2,primer_espacio,(escritura-primer_espacio)+1);

            *primer_espacio=',';
            *(primer_espacio+1)=' ';
        }
    }else{
        if (*(comma+ 1) != ' ') {
            memmove(comma + 2, comma + 1, strlen(comma));
            *(comma + 1) = ' ';
             *(comma+2)=toupper(*(comma+2));
         }
    }

    strncpy(fin, buffer_temporal, MAX_APENOM);
    *(fin+MAX_APENOM - 1) = '\0';
  }

/**********************************************************
*                VALIDACION   FECHA                       *
***********************************************************/
int anioBisiesto(int anio)
{
    return ((anio % 4 == 0 && anio % 100 != 0) || anio % 400 == 0)?TRUE:FALSE;
}
t_Fecha difFechas(const t_Fecha *fecIni, const t_Fecha *fecFin){
    static const char dias[2][13] = {
        { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }};
    t_Fecha edad;
    edad = *fecFin;
    if((edad.dia -= fecIni->dia)<0){
        edad.mes--;
        edad.dia += dias[anioBisiesto(edad.anio)][edad.mes];
    }
    if((edad.mes -= fecIni->mes)<0){
        edad.mes +=12;
        edad.anio--;
    }
    edad.anio -= fecIni->anio;
    return edad;
}
int validar_Fecha(const t_Fecha *fecha){
    static const char dias[2][13] = {
        { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }};

    if(fecha->anio<1900 || fecha->anio>9999)
        return FALSE;
    if(fecha->mes<1 || fecha->mes>12)
        return FALSE;
    if(fecha->dia<1 || fecha->dia> dias[anioBisiesto(fecha->anio)][fecha->mes])
        return FALSE;

    return TRUE;
}
/**********************************************************
*                       COMPARACION                       *
***********************************************************/
int comparar_dni (const void* a, const void* b){
    t_reg_indice* ra = (t_reg_indice*)a;
    t_reg_indice* pb = (t_reg_indice*)b;
    return ra.dni - pb.dni;
}
