#include <QStyledItemDelegate>
#include <iostream>

class RowHoverDelegate: public QStyledItemDelegate
{
  public:
    RowHoverDelegate(QObject *parent = nullptr);
  public slots:
    void onHoverIndexChanged(const QModelIndex &item);
    void onLeaveTableEvent();
  private:
    int hovered_row_; 
    void paint(QPainter *painter,const QStyleOptionViewItem &option, const QModelIndex &index) const;

};
