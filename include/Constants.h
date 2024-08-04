#ifndef Constants_H
#define Constants_H

#define BYTES_IN_KILOBYTE 1024.0
#define BYTES_IN_MEGABYTE 1048576.0
#define KILOBYTES_IN_MEGABYTE 1024.0
#define KILOBYTES_IN_GIGABYTE 1048576.0

// base path for network bandwidth
#define NETWORK_BASE_DIR "/sys/class/net/"

// file from which cpu utilization is being calculated
#define CPU_USAGE_FILE "/proc/stat"

// number of columns in process table
#define COLUMNS_NUM 5

// refresh interval
#define TIMEOUT_INTERVAL 1000

// default size for buffers
#define BUFSIZE 4096

// constant strings for labels
#define UNIT_BYTE     "B"
#define UNIT_KILOBYTE "KiB"
#define UNIT_MEGABYTE "MiB"
#define UNIT_GIGABYTE "GiB"

// constants used to sort processes
#define SORT_NONE 0
#define SORT_BY_NAME_ASCENDING 1
#define SORT_BY_PID_ASCENDING 2
#define SORT_BY_USER_ASCENDING 3
#define SORT_BY_CPU_ASCENDING 4
#define SORT_BY_MEM_ASCENDING 5
#define SORT_BY_NAME_DESCENDING 6
#define SORT_BY_PID_DESCENDING 7
#define SORT_BY_USER_DESCENDING 8
#define SORT_BY_CPU_DESCENDING 9
#define SORT_BY_MEM_DESCENDING 10

#endif // Constants_H
