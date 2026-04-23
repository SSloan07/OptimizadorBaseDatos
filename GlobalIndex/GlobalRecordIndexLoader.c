#include "GlobalRecordIndexLoader.h"

void inicializar_indice_global(GlobalRecordIndex *index) {
    RecordLocation loc1;
    loc1.id_registro = 245678;
    loc1.id_comuna = 14;
    loc1.id_bloque = 2;
    insert_global_record_index(index, loc1.id_registro, loc1);

    RecordLocation loc2;
    loc2.id_registro = 245679;
    loc2.id_comuna = 14;
    loc2.id_bloque = 2;
    insert_global_record_index(index, loc2.id_registro, loc2);

    RecordLocation loc3;
    loc3.id_registro = 245680;
    loc3.id_comuna = 14;
    loc3.id_bloque = 2;
    insert_global_record_index(index, loc3.id_registro, loc3);
}