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
/**********************************************************
**                FUNCIONES STRING                        *
***********************************************************/
void str_toupper(char* str){
    while(*str){
        *str = toUpper(*str);
        str++;
    }
}
// aclaracion: input_str seria un puntero al dato extraido del archivo, output_str es un buffer donde hay espacio para poner el string y max es el define de MAX_APENOM
bool normalizar_apellido_nombre(const char *input_str, char *output_str, size_t max)
{
    if (input_str==NULL || *input_str == '\0')
        return false;

    const char *lectura=input_str;
    char *escritura=output_str;
    char *inicio=output_str;

    bool nueva_palabra=true;

    while (*lectura!='\0'&&(size_t)(escritura-inicio)<max-1)
    {
        char AUX=*lectura;

        if (isspace(AUX))
        {
            if (escritura>inicio&&!isspace(*(escritura - 1)))
            {
                *escritura=' ';
                escritura++;
                nueva_palabra = true;
            }
        }
        else
        {

            if (nueva_palabra)
            {
                *escritura=toupper(AUX);
                nueva_palabra=false;
            }
            else
                *escritura=tolower(AUX);
            escritura++;
        }
        lectura++;
    }
    if (*lectura!='\0') return false;

    while (escritura>inicio&&isspace(*(escritura-1)))
    {
        escritura--;
    }
    *escritura ='\0';


    char *comma=NULL;
    char *temp=output_str;
    while (*temp!='\0')
    {
        if (*temp==',')
        {
            comma=temp;
            break;
        }
        temp++;
    }
    if (comma==NULL)
    {
        char *espacio_despues_primera_palabra=NULL;
        temp=output_str;

        while (*temp!='\0')
        {
            if (isspace(*temp))
            {
                espacio_despues_primera_palabra= temp;
                break;
            }
            temp++;
        }

        if (espacio_despues_primera_palabra!=NULL)
        {
            escritura=inicio+strlen(inicio);

            if ((size_t)(escritura-inicio)+2>=max) return false;
            memmove(espacio_despues_primera_palabra+2,espacio_despues_primera_palabra,(escritura-espacio_despues_primera_palabra)+1);
            *espacio_despues_primera_palabra=',';
            *(espacio_despues_primera_palabra+1) = ' ';
        }
        else
        {
            if ((size_t)(escritura-inicio)+2>= max) return false;

            *escritura = ',';
            escritura++;
            *escritura = ' ';
            escritura++;
            *escritura = '\0';
        }
    }
    if (strlen(output_str) >= max)
        return false;

    return true;
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

