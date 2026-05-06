#ifndef BENCHMARK_UTILS_H
#define BENCHMARK_UTILS_H

#include <stdio.h>
#include <time.h>

/* ---- Colores ANSI ---- */
#ifdef NO_COLOR
  #define C_RESET   ""
  #define C_BOLD    ""
  #define C_DIM     ""
  #define C_RED     ""
  #define C_GREEN   ""
  #define C_YELLOW  ""
  #define C_BLUE    ""
  #define C_MAGENTA ""
  #define C_CYAN    ""
#else
  #define C_RESET   "\x1b[0m"
  #define C_BOLD    "\x1b[1m"
  #define C_DIM     "\x1b[2m"
  #define C_RED     "\x1b[31m"
  #define C_GREEN   "\x1b[32m"
  #define C_YELLOW  "\x1b[33m"
  #define C_BLUE    "\x1b[34m"
  #define C_MAGENTA "\x1b[35m"
  #define C_CYAN    "\x1b[36m"
#endif

/* ---- Funciones compartidas ---- */
double now_sec(void);
void format_bytes(size_t bytes, char *out, size_t out_size);
void print_phase_line(const char *label, double seconds, double total, int width);

#endif