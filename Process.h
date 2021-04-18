#include <QString>

class Process
{
  public:
    Process(long pid, QString name, QString user, double cpuUsage, double memUsage);
    ~Process();
    long getPid() const;
    QString getUser() const;
    QString getName() const;
    double getCpuUsage() const;
    double getMemUsage() const;
  private:
    long pid;
    QString user, name;
    double cpuUsage, memUsage;
};
