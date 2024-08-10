#include "../include/executeShellCommand.h"

std::string
executeShellCommand(const char* cmd)
{
  char buffer[256];
  std::string result = "";
  FILE* pipe = popen(cmd, "re");
  if (!pipe) throw std::runtime_error("popen() failed!");
  try
  {
    while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
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
