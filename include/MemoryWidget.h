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

  void updateGraph(int oldFrameSize, int newFrameSize);
  void initializeVectors(int frameSize); 
private slots:
  void getMemoryStatus();
};
