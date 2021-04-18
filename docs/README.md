# ltm

ltm is lightweight gui-like qt5-based linux task manager.

![alt text] (processes.png)

## Prerequisites

- lm-sensors. Can be installed by:
```bash
sudo apt-get install lm-sensors
```
- c++ compiler like g++. Can be installed by:
```bash
sudo apt-get install g++
```
- qt5. Can be installed by:
```bash
sudo apt-get install qt5-default
```
- Some GNU tools like `grep`, `cat`, `tr`, `cut`, `free`, `tail` which basically come
preinstalled on vast majority of Linux distros.

## Build and Install

Open terminal and type 
```bash
qmake
make
```
Then, `lsb` executable should be created.
Eventually, you may need to enable execution of `lsb` binary by `sudo chmod +x lsb`.

## Note

Application is initially written and tested on Ryzen 5 3500u CPU, Kubuntu 20.04.
Hence, minimal changes of SHELLCMD_ macros may be needed in `include/ShellCommands.h`

## Under the hood

- CPU utilization is calculated from `/proc/stat` file
- CPU temperature is obtained from `sensors`(`lm-sensors` package) output
- Memory Utilization is obtained by `free` command in bash
- Network bandwidth is calculated by iterating through all network interfaces in
`/sys/class/net` directory
- Processes in processes tab are parsed from `top` output
- Plotting in performance tab is done using `QCustomPlot` objects to gain maximum performance
