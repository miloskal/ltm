#include "Process.h"

Process::Process(long pid_, QString name_, QString user_, double cpuUsage_, double memUsage_)
{
  pid = pid_;
  name = name_;
  user = user_;
  cpuUsage = cpuUsage_;
  memUsage = memUsage_;
}

Process::~Process()
{
}

long Process::getPid() const
{
  return pid;
}

QString Process::getName() const
{
  return name;
}

QString Process::getUser() const
{
  return user;
}

double Process::getCpuUsage() const
{
  return cpuUsage;
}

double Process::getMemUsage() const
{
  return memUsage;
}




























