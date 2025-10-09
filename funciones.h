#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

#define MAX_APENOM 61  // 60 caracteres + '\0'
#define MAX_CAT 11     // 10 caracteres + '\0'
#define MAX_PLAN 11    // 10 caracteres + '\0'
#define MAX_EMAIL 31   // 30 caracteres + '\0

typedef time_t t_fecha;

typedef struct {

    long DNI;
    char ApellidosNombres[MAX_APENOM];
    t_fecha FechaProceso;
    t_fecha FechaNacimiento;
    char Sexo;
    t_fecha FechaAfiliacion;
    char Categoria[MAX_CAT];
    char Estado;
    char Plan[MAX_PLAN];
    char EmailTutor[MAX_EMAIL];
    t_fecha FechaUltimaCuotaPaga;

} RegistroMiembro;

bool validar_dni(const char *dni_str, long *dni_num_out);
bool normalizar_apellido_nombre(const char *input_str, char *output_str, size_t max1);

#endif // FUNCIONES_H_INCLUDED
