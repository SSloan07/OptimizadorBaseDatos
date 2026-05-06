#define _POSIX_C_SOURCE 200809L

#include "BenchmarkUtils.h"
#include <stdio.h>
#include <time.h>

double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

void format_bytes(size_t bytes, char *out, size_t out_size) {
    if (bytes < 1024) {
        snprintf(out, out_size, "%zu B", bytes);
    } else if (bytes < 1024 * 1024) {
        snprintf(out, out_size, "%.2f KB", (double)bytes / 1024.0);
    } else {
        snprintf(out, out_size, "%.2f MB", (double)bytes / (1024.0 * 1024.0));
    }
}

void print_phase_line(const char *label, double seconds, double total, int width) {
    double pct = (total > 0.0) ? (seconds / total) * 100.0 : 0.0;
    int filled = (total > 0.0) ? (int)((seconds / total) * width + 0.5) : 0;
    if (filled > width) filled = width;

    printf("  %s%-22s%s %s%9.3f s%s  %s%5.1f%%%s  %s",
           C_BOLD, label, C_RESET,
           C_CYAN, seconds, C_RESET,
           C_YELLOW, pct, C_RESET,
           C_GREEN);
    for (int i = 0; i < filled; i++) putchar('#');
    printf("%s", C_DIM);
    for (int i = filled; i < width; i++) putchar('.');
    printf("%s\n", C_RESET);
}