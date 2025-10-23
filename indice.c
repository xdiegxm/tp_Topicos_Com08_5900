#include "indice.h"
#include "validacion.h"
/**
void indice_crear (t_indice*):
toma memoria para 100 elementos e inicializa la estructura a índice vacío.
**/
void indice_crear (t_indice *indice, size_t nmemb, size_t tamanyo){
    indice->vindice=malloc(tamanyo*nmemb);
    if(!indice->vindice){
        printf("ERROR AL CREAR EL INDICE...")
        return;
    }
    indice->cantidad_elementos_maxima = nmemb;
    indice->cantidad_elementos_actual=0;
}
/**
Aumenta la capacidad del indice un 30 por ciento
**/
void indice_redimensionar (t_indice *indice, size_t nmemb, size_t tamanyo){
    void *temp;
    size_t inc = (indice->cantidad_elementos_maxima*nmemb);
    temp = realloc(indice->vindice,tamanyo*inc);
    if(!temp){
        printf("ERROR AL DIMENSIONAR EL INDICE...");
        return;
    }
    indice->cantidad_elementos_maxima=inc;
    indice->vindice=temp;
}
/**
int indice_insertar (t_indice*, const t_reg_indice*):
inserta en orden según el dni. Si no hay lugar en el array,
toma memoria para un 30 % más de lo que tenía
**/
int indice_insertar(t_indice *indice, const void *registro, size_t tamanyo, int (*cmp)(const void*, const void*)){
    int pos=indice_buscar(indice,registro,tamanyo,cmp);
    if(pos!=-1)
        return ERROR; //ya existe, no se puede insertar
    if(indice_lleno(indice)){
        indice_redimensionar(indice, INCREMENTO, tamanyo);
    }
    char* base=(char*)indice->vindice;
    size_t i = indice->cantidad_elementos_actual;
    while(i>0&&cmp(registro,base+(i-1)*tamanyo)<0){
        memcpy(base+(i)*tamanyo,base+(i-1)*tamanyo,tamanyo);
        i--;
    }
    memcpy(base+i*tamanyo,registro,tamanyo);
    indice->cantidad_elementos_actual++;
    return OK;
}
/**int indice_buscar (const t_indice*, t_reg_indice* reg_indice):
si el DNI existe deja el registro en reg_ind.
**/
int indice_buscar (const t_indice* indice, const void* registro, size_t tamanyo, int (*cmp)(const void*, const void*)){
    size_t low = 0, high = indice->cantidad_elementos_actual-1, mid;
    int comp_result;
    while(low <= high){
        mid = (low+high)/2;
        void *actual = (char *)indice->vindice + mid * tamanyo;
        comp_result = cmp(registro, actual);
        if(comp_result == 0){
            return (int)mid;
        }
        else if(comp_result<0){
            if(mid==0)break;
            high=mid-1;
        }else{
            low=mid+1;
        }
    }
    return NO_EXISTE;
}
/**
int indice_eliminar (t_indice*, t_reg_indice*):
elimina el DNI informado.
**/
int indice_eliminar(t_indice *indice, const void *registro, size_t tamanyo, int (*cmp)(const void*, const void*)){
    int pos=indice_buscar(indice,registro,tamanyo,cmp);
    if(pos==-1)
        return ERROR;
    char* base=(char*)indice->vindice;
    memmove(base+pos*tamanyo,base+(pos+1)*tamanyo,(indice->cantidad_elementos_actual-pos-1)*tamanyo);
    indice->cantidad_elementos_actual--;
    return OK;
}
/**int indice_vacio(const t_indice *): indica se el índice está vacío.
**/
int indice_vacio(const t_indice *indice){
    if(indice->cantidad_elementos_actual==0)
        return OK;
    return VACIO;
}
/**
int indice_lleno(const t_indice *, unsigned tamDato):
indica si ya no se pueden añadir más registros.
**/
int indice_lleno(const t_indice *indice){
    if(indice->cantidad_elementos_actual==indice->cantidad_elementos_maxima)
        return OK;
    return LLENO;
}
/**
void indice_vaciar(t_indice*):
libera la memoria utilizada por el índice.
**/
void indice_vaciar(t_indice* indice){
    if(indice->vindice!=NULL){
        free(indice->vindice);
        indice->vindice=NULL;
    }
    indice->cantidad_elementos_actual=0;
    indice->cantidad_elementos_maxima=0;
}














