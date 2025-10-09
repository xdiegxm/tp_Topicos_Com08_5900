#include "../../include/funciones.h"

bool validar_dni(const char *dni_str, long *dni_num_out)
{
    if (dni_str == NULL || strlen(dni_str) == 0)
        return false;

    char *endptr;
    long dni_num = strtol(dni_str,&endptr,10);

    if (*endptr!='\0'&&(!isspace(*endptr)||dni_num==0))
        return false;

    if (dni_num>1000000&&dni_num<100000000)
    {
        *dni_num_out = dni_num;
        return true;
    }
    return false;
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
