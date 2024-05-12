// initialy written for bash 5.0.17

#ifndef ShellCommands_H
#define ShellCommands_H

#define SHELLCMD_GET_CPU_TEMPERATURE "sensors | grep Tctl | tr -s ' ' | cut -d ' ' -f2 | tr -d '+\n'"
#define SHELLCMD_GET_PROCESSES_INFO "top -n 1 -b -w 250 | tail -n +8 | tr -s ' '"

#include "executeShellCommand.h"


#endif // ShellCommands_H
