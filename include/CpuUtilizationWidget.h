#include "AbstractGraph.h"
#include "Constants.h"

class CpuUtilizationWidget: public AbstractGraph
{
public:
  CpuUtilizationWidget(QWidget *parent = nullptr);
  ~CpuUtilizationWidget();


private:
  QLabel *cpuUtilizationLbl,
         *cpuUtilizationVal,
         *cpuTemperatureLbl,
         *cpuTemperatureVal;
  QFont *font;
  QPen *conPen;
  int cpuCores;
  QVector<double> y1; // vector for temperature samples
  double cpuUtilization, cpuTemperature;
  char cpuTemperatureFile[BUFSIZE];
  long long lastCpuUtilizationSample;
  long cpuCorrectionFactor;
  void getCpuUtilization();
  void getCpuTemperaturePath(char *path);
#if defined(__aarch64__) || defined(_M_ARM64)
  void getCpuTemperaturePath(char *hwmonDir, char *path);
#endif
  void getCpuTemperature();
  void updateWidget();
  void initializeVectors(int n) override;
  void updateGraph(int oldFrameSize, int newFrameSize) override;
};
