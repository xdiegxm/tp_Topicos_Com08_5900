#ifndef VALIDACION_H_INCLUDED
#define VALIDACION_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define MAX_APENOM 61  // 60 caracteres + '\0'
#define MAX_CAT 11     // 10 caracteres + '\0'
#define MAX_PLAN 11    // 10 caracteres + '\0'
#define MAX_EMAIL 31   // 30 caracteres + '\0

typedef struct{
    int dia;
    int mes;
    int anio;
}t_Fecha;
typedef struct {
    long DNI;
    char ApellidosNombres[MAX_APENOM];
    t_Fecha FechaProceso;
    t_Fecha FechaNacimiento;
    char Sexo;
    t_Fecha FechaAfiliacion;
    char Categoria[MAX_CAT];
    char Estado;
    char Plan[MAX_PLAN];
    char EmailTutor[MAX_EMAIL];
    t_Fecha FechaUltimaCuotaPaga;
}t_Miembro;
/**********************************************************
*                         MACROS                          *
***********************************************************/
#define toUpper(X) (((X) >= 'a' && (X) <= 'z') ? (X) - 32 : (X))
/**********************************************************
*                VALIDACION   DATOS                       *
***********************************************************/
int validar_Plan (char plan[10]);
int ValidarDni(long dni);
int ValidarSexo(char s);
int validar_Mail(const char* mail);
int ValidarEstado(char e);
/**********************************************************
**                FUNCIONES STRING                        *
***********************************************************/
void str_toupper(char* str);
bool normalizar_apellido_nombre(const char *input_str, char *output_str, size_t max1);
/**********************************************************
*                VALIDACION   FECHA                       *
***********************************************************/
int anioBisiesto(int anio);
t_Fecha difFechas(const t_Fecha *fecIni, const t_Fecha *fecFin);
#endif // VALIDACION_H_INCLUDED
