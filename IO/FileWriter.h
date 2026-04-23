#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include <stddef.h>

int escribir_archivo_binario(const char *ruta, const unsigned char *buffer, size_t size);

#endif