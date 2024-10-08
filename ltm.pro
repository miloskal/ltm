######################################################################
# Automatically generated by qmake (3.1) Sun Apr 18 21:08:30 2021
######################################################################

TEMPLATE = app
TARGET = ltm
INCLUDEPATH += include

CONFIG += optimize_full

QT += widgets printsupport
# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += include/AbstractGraph.h \
           include/Constants.h \
           include/CpuUtilizationWidget.h \
           include/executeShellCommand.h \
           include/ltm.h \
           include/MemoryWidget.h \
           include/NetworkWidget.h \
           include/Process.h \
           include/ProcessesTab.h \
           include/ProcessTable.h \
           include/qcustomplot.h \
           include/RowHoverDelegate.h \
           include/ShellCommands.h
SOURCES += src/AbstractGraph.cpp \
           src/CpuUtilizationWidget.cpp \
           src/executeShellCommand.cpp \
           src/ltm.cpp \
           src/main.cpp \
           src/MemoryWidget.cpp \
           src/NetworkWidget.cpp \
           src/Process.cpp \
           src/ProcessesTab.cpp \
           src/ProcessTable.cpp \
           src/qcustomplot.cpp \
           src/RowHoverDelegate.cpp
