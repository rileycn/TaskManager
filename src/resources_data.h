#ifndef RESOURCES_DATA_H
#define RESOURCES_DATA_H

// Get the number of CPU cores
int get_cpu_count();

// Get CPU usage for a specific CPU (cpu_num = -1 for total, 0+ for specific CPU)
double get_cpu_usage(int cpu_num);

double get_memory_usage();
double get_swap_usage();
double get_network_usage();
double get_network_speed(int direction);
#endif // DATA_H
