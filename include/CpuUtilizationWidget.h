#include "AbstractGraph.h"

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
  int cpuCores;
  QVector<double> y1; // vector for temperature samples
  double cpuUtilization, cpuTemperature;
  long long lastCpuUtilizationSample;
  long cpuCorrectionFactor;
  void getCpuUtilization();
  void getCpuTemperature();
  void updateWidget();
  void initializeVectors(int n) override;
  void updateGraph(int oldFrameSize, int newFrameSize) override;
};
