#include "executeShellCommand.h"

std::string
executeShellCommand(const char* cmd)
{
  char buffer[256];
  std::string result = "";
  FILE* pipe = popen(cmd, "r");
  if (!pipe) throw std::runtime_error("popen() failed!");
  try
  {
    while (fgets(buffer, sizeof buffer, pipe) != NULL) {
      result += buffer;
    }
  }
  catch (...) {
      pclose(pipe);
      throw;
  }
  pclose(pipe);
  return result;
}
