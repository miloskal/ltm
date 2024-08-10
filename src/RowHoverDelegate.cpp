#include "RowHoverDelegate.h"

RowHoverDelegate::RowHoverDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
  hovered_row_ = -1;
}

void RowHoverDelegate::onHoverIndexChanged(const QModelIndex& item) {
  hovered_row_ = item.row();
}

void RowHoverDelegate::onLeaveTableEvent() {
  hovered_row_ = -1;
}

void RowHoverDelegate::paint(QPainter *painter,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const {
  QStyleOptionViewItem opt = option;
  if(index.row() == hovered_row_) {
    opt.state |= QStyle::State_MouseOver;
  } else {
    opt.state &= ~QStyle::State_MouseOver;
  }
  QStyledItemDelegate::paint(painter, opt, index);
}
