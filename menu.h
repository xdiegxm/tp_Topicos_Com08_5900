#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED



// FUNCIONES QUE USO PARA MODIFICAR/LEER EL ARCHIVO
FILE *abrir_archivo(const char *nombre_archivo, const char *modo);
int leer_registro_archivo(FILE *fp, unsigned nro_reg, t_Miembro *reg);
int escribir_registro_archivo(FILE *fp, unsigned nro_reg, const t_Miembro *reg);
int validar_miembro(const t_Miembro *miembro);

//FUNCIONES QUE SI VAN EN MENU
int miembro_modificar(t_indice *indice, FILE *fp);
int miembro_dar_baja(t_indice *indice, FILE *fp);
int miembro_mostrar_info(const t_indice *indice, FILE *fp);
void miembro_mostrar_datos(const t_Miembro *miembro);
#endif // MENU_H_INCLUDED
