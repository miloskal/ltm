# ltm

ltm is lightweight gui-like qt5-based linux task manager.

<img src="processes.png" width="400"> <img src="performance.png" width="400">

## Prerequisites

- C++ compiler, lm-sensors and Qt5 development files:
```bash
sudo apt-get install g++ lm-sensors qtbase5-dev
```

- GNU tools like `top`, `grep`, `cat`, `tr`, `cut`, `free`, `tail` which basically come
preinstalled on vast majority of Linux distros.

## Build and Install
Eighter:

1) Open terminal and type 
```bash
qmake
make
```
or

2) Build project using Qt Creator.

## Running

After build process finishes, `lsb` executable should be created.
Eventually, you may need to enable execution of `lsb` binary by `sudo chmod +x lsb`.

## Note

Application is tested on:
- Ryzen 5 3500u CPU, Kubuntu 20.04 
- Ryzen 7 4700u CPU, Kubuntu 22.04

For some other system configurations, minimal changes of SHELLCMD_ macros may be needed in `include/ShellCommands.h` in order for application to work properly.

## Under the hood

- CPU utilization is calculated from `/proc/stat` file
- CPU temperature is obtained from `sensors`(`lm-sensors` package) output
- Memory Utilization is obtained by `free` command in bash
- Network bandwidth is calculated by iterating through all network interfaces in
`/sys/class/net` directory
- Processes in processes tab are parsed from `top` output
- Plotting in performance tab is done using `QCustomPlot` objects to gain maximum performance

## Licence

[GPL v3] (https://choosealicense.com/licenses/gpl-3.0/)
