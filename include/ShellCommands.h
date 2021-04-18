// initialy written for bash 5.0.17

#ifndef ShellCommands_H
#define ShellCommands_H

#define SHELLCMD_GET_NUMBER_OF_CPU_CORES "nproc"
#define SHELLCMD_GET_CPU_USAGE "cat \"/proc/stat\" | grep \"cpu \" | tr -s ' ' | cut -d ' ' -f2-4"
#define SHELLCMD_GET_CPU_TEMPERATURE "sensors | grep Tdie | tr -s ' ' | cut -d ' ' -f2 | tr -d '+\n'"
#define SHELLCMD_GET_PROCESSES_INFO "top -n 1 -b | tail -n+8 | tr -s ' '"
#define SHELLCMD_GET_TOTAL_MEMORY "free | grep Mem | tr -s ' ' | cut -d ' ' -f2"
#define SHELLCMD_GET_USED_MEMORY "free | grep Mem | tr -s ' ' | cut -d ' ' -f3"

#include "executeShellCommand.h"


#endif // ShellCommands_H
