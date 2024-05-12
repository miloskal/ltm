#include "AbstractGraph.h"

class MemoryWidget: public AbstractGraph
{
public:
  MemoryWidget(QWidget* parent = nullptr);
  ~MemoryWidget();

private:
  QLabel *memLbl, *memVal;
  QFont *font;
  long long totalMemory;
  QString totalMemoryQString;

  long long getRAM();
  void updateGraph(int oldFrameSize, int newFrameSize) override;
  void initializeVectors(int frameSize) override;
private slots:
  void getMemoryStatus();
};
