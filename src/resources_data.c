#include "resources_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CPUS 256

struct {
    unsigned long long prev_idle;
    unsigned long long prev_total;
} cpu_stats[MAX_CPUS + 1];

struct {
    unsigned long long prev_rx_bytes;
    unsigned long long prev_tx_bytes;
    double rx_speed;
    double tx_speed;
    struct timespec prev_time;
} net_stats = {0};

int get_cpu_count() {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return 0;
    int cpu_count = 0;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "cpu", 3) == 0) {
            if (line[3] >= '0' && line[3] <= '9') {
                cpu_count++;
            }
        }
    }
    fclose(fp);
    return cpu_count;
}

double get_cpu_usage(int cpu_num) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return 0.0;
    char line[256];
    char cpu_label[10];
    if (cpu_num == -1) {
        snprintf(cpu_label, sizeof(cpu_label), "cpu ");
    }
    else {
        snprintf(cpu_label, sizeof(cpu_label), "cpu%d ", cpu_num);
    }
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, cpu_label, strlen(cpu_label)) == 0) {
            unsigned long long user, nice, system, idle_now, iowait, irq, softirq, steal;
            sscanf(line + strlen(cpu_label), "%llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle_now, &iowait, &irq, &softirq, &steal);
            unsigned long long idle = idle_now + iowait;
            unsigned long long total = user + nice + system + idle + irq + softirq + steal;
            int idx = cpu_num + 1;
            unsigned long long diff_idle = idle - cpu_stats[idx].prev_idle;
            unsigned long long diff_total = total - cpu_stats[idx].prev_total;
            cpu_stats[idx].prev_idle = idle;
            cpu_stats[idx].prev_total = total;
            fclose(fp);
            return (1.0 - (diff_idle / (double)diff_total));
        }
    }
    fclose(fp);
    return 0.0;
}

double get_memory_usage() {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return 0.0;
    unsigned long total_memory, free_memory;
    fscanf(fp, "MemTotal: %lu kB\nMemFree: %lu kB", &total_memory, &free_memory);
    fclose(fp);
    return (total_memory - free_memory) / (double)total_memory;
}

double get_swap_usage() {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return 0.0;
    unsigned long total_swap = 0, free_swap = 0;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "SwapTotal:", 10) == 0) {
            sscanf(line, "SwapTotal: %lu kB", &total_swap);
        }
        else if (strncmp(line, "SwapFree:", 9) == 0) {
            sscanf(line, "SwapFree: %lu kB", &free_swap);
        }
    }
    fclose(fp);
    if (total_swap == 0) return 0.0;
    return (total_swap - free_swap) / (double)total_swap;
}


double get_network_speed(int direction) {
    FILE *fp = fopen("/proc/net/dev", "r");
    if (!fp) return 0.0;
    char line[256];
    unsigned long long rx_bytes = 0, tx_bytes = 0;
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    fgets(line, sizeof(line), fp);
    fgets(line, sizeof(line), fp);
    while (fgets(line, sizeof(line), fp)) {
        char interface[32];
        unsigned long long r_bytes, r_packets, r_errs, r_drop, r_fifo, r_frame, r_compressed, r_multicast;
        unsigned long long t_bytes, t_packets, t_errs, t_drop, t_fifo, t_colls, t_carrier, t_compressed;
        sscanf(line, "%s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
               interface,
               &r_bytes, &r_packets, &r_errs, &r_drop, &r_fifo, &r_frame, &r_compressed, &r_multicast,
               &t_bytes, &t_packets, &t_errs, &t_drop, &t_fifo, &t_colls, &t_carrier, &t_compressed);
        if (strstr(interface, "lo:") == NULL) {
            rx_bytes += r_bytes;
            tx_bytes += t_bytes;
        }
    }
    fclose(fp);
    double time_diff = (current_time.tv_sec - net_stats.prev_time.tv_sec) +
                      (current_time.tv_nsec - net_stats.prev_time.tv_nsec) / 1e9;
    if (time_diff > 0) {
        net_stats.rx_speed = (rx_bytes - net_stats.prev_rx_bytes) / time_diff / 1024.0;
        net_stats.tx_speed = (tx_bytes - net_stats.prev_tx_bytes) / time_diff / 1024.0;
    }
    net_stats.prev_rx_bytes = rx_bytes;
    net_stats.prev_tx_bytes = tx_bytes;
    net_stats.prev_time = current_time;
    return direction == 0 ? net_stats.rx_speed : net_stats.tx_speed;
}
