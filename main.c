#include <stdio.h>
#include "App/Menu.h"
#include "App/SearchService.h"
#include "App/InsertService.h"
#include "App/AppContext.h"

int main(void) {
    int opcion;
    int salir = 0;
    AppContext *ctx;

    ctx = crear_contexto_app();
    if (ctx == NULL) {
        fprintf(stderr, "No se pudo inicializar el contexto de la aplicacion.\n");
        return 1;
    }

    while (!salir) {
        opcion = mostrar_menu_principal();

        switch (opcion) {
            case 1:
                ejecutar_busqueda_puntual(ctx);
                break;
            case 2:
                ejecutar_menu_insercion(ctx);
                break;
            case 3:
                salir = 1;
                printf("Saliendo del programa.\n");
                break;
            default:
                printf("Opcion invalida.\n");
                break;
        }
    }

    destruir_contexto_app(ctx);
    return 0;
}


// /* Construye el nombre del archivo temporal agregando ".tmp" al path.
//    El buffer 'out' debe tener al menos strlen(path) + 5 bytes de espacio.
//    Retorna 0 en éxito, -1 si el path es demasiado largo. */
// static int make_temp_path(const char *path, char *out, size_t out_size) {
//     size_t needed = strlen(path) + 5;  /* ".tmp" + '\0' */
//     if (needed > out_size) return -1;
//     snprintf(out, out_size, "%s.tmp", path);
//     return 0;
// }

// /* Procesa 'path' in-place: lo lee, lo procesa con la función dada, y
//    reemplaza el archivo original con el resultado de forma atómica.

//    'mode' es 'c' (compress) o 'd' (decompress), solo para mensajes.
//    Retorna 0 en éxito, != 0 en error. */
// static int process_in_place(const char *path, char mode) {
//     char temp_path[4096];
//     if (make_temp_path(path, temp_path, sizeof(temp_path)) != 0) {
//         fprintf(stderr, "Error: la ruta es demasiado larga\n");
//         return 1;
//     }

//     FILE *input = fopen(path, "rb");
//     if (!input) {
//         fprintf(stderr, "Error: no se pudo abrir '%s'\n", path);
//         return 1;
//     }

//     FILE *output = fopen(temp_path, "wb");
//     if (!output) {
//         fprintf(stderr, "Error: no se pudo crear el archivo temporal '%s'\n",
//                 temp_path);
//         fclose(input);
//         return 1;
//     }

//     /* Ejecutar la operación correspondiente */
//     int op_error = 0;
//     size_t original_size = 0;
//     size_t final_size    = 0;

//     if (mode == 'c') {
//         CompressStats s = lzw_compress(input, output);
//         op_error      = s.error;
//         original_size = s.bytes_read;
//         final_size    = s.codes_written * 2;
//     } else {
//         DecompressStats s = lzw_decompress(input, output);
//         op_error      = s.error;
//         original_size = s.codes_read * 2;
//         final_size    = s.bytes_written;
//     }

//     fclose(input);
//     fclose(output);

//     /* Si hubo error, limpiar el temporal y NO tocar el original */
//     if (op_error) {
//         fprintf(stderr, "Error durante %s (código %d). El archivo original no fue modificado.\n",
//                 mode == 'c' ? "compresión" : "descompresión", op_error);
//         remove(temp_path);  /* ignoramos el error del remove */
//         return 1;
//     }

//     /* Reemplazo atómico: rename() sobreescribe el destino en POSIX.
//        Si falla (ej: el disco se llenó), el original queda intacto. */
//     if (rename(temp_path, path) != 0) {
//         fprintf(stderr, "Error: no se pudo reemplazar '%s'. El resultado quedó en '%s'.\n",
//                 path, temp_path);
//         return 1;
//     }

//     /* Reportar resultados */
//     if (mode == 'c') {
//         double ratio = original_size > 0
//             ? (double)final_size / (double)original_size * 100.0
//             : 0.0;
//         printf("Compresión completada: %s\n", path);
//         printf("  Tamaño original:    %zu bytes\n", original_size);
//         printf("  Tamaño comprimido:  %zu bytes\n", final_size);
//         printf("  Ratio:              %.2f%% del original\n", ratio);
//         printf("  Espacio ahorrado:   %.2f%%\n", 100.0 - ratio);
//     } else {
//         printf("Descompresión completada: %s\n", path);
//         printf("  Tamaño comprimido:     %zu bytes\n", original_size);
//         printf("  Tamaño descomprimido:  %zu bytes\n", final_size);
//     }

//     return 0;
// }
