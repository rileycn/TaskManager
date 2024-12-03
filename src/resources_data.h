#ifndef RESOURCES_DATA_H
#define RESOURCES_DATA_H

int get_cpu_count();
double get_cpu_usage(int cpu_num);
double get_memory_usage();
double get_swap_usage();
double get_network_usage();
double get_network_speed(int direction);

#endif // DATA_H
