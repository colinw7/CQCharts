#define QT_KEYPAD_NAVIGATION 1

#include <CQExcelDelegate.h>
#include <CQBaseModelTypes.h>

#include <QAbstractItemView>
#include <QLineEdit>
#include <QLayout>
#include <QPainter>
#include <QEvent>

#include <cassert>

#include <svg/edit_item_svg.h>

CQExcelDelegate::
CQExcelDelegate(QAbstractItemView *view) :
 view_(view)
{
  //setClipping(false);
}

void
CQExcelDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  isEditable_  = (index.flags() & Qt::ItemIsEditable) &&
                 (option.state & QStyle::State_HasEditFocus);
  isMouseOver_ = (option.state & QStyle::State_MouseOver);

  if (! drawType(painter, option, index))
    QItemDelegate::paint(painter, option, index);
}

bool
CQExcelDelegate::
drawType(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  auto *model = view_->model();
  if (! model) return false;

  auto value = model->data(index);
  if (! value.isValid()) return false;

  //---

  if (value.type() == QVariant::Color)
    drawColor(painter, option, value.value<QColor>(), index);
  else
    return false;

  return true;
}

void
CQExcelDelegate::
drawColor(QPainter *painter, const QStyleOptionViewItem &option,
          const QColor &color, const QModelIndex &index) const
{
  QItemDelegate::drawBackground(painter, option, index);

  auto rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(0, 1, -3, -2);

  painter->fillRect(rect, QBrush(color));

  painter->setPen(QColor(0,0,0)); // TODO: contrast border

  painter->drawRect(rect);

  int x = rect.right() + 2;
//int y = rect.top() + fm.ascent();

  QRect rect1;

  rect1.setCoords(x, option.rect.top(), option.rect.right(), option.rect.bottom());

  QItemDelegate::drawDisplay(painter, option, rect1, color.name());
}

QSize
CQExcelDelegate::
sizeHint(const QStyleOptionViewItem &option, const QModelIndex &ind) const
{
  QSize size;

  if (! typeSizeHint(option, ind, size))
    return QItemDelegate::sizeHint(option, ind);

  return size;
}

bool
CQExcelDelegate::
typeSizeHint(const QStyleOptionViewItem &option, const QModelIndex &ind, QSize &size) const
{
  auto *model = view_->model();
  if (! model) return false;

  auto value = model->data(ind);
  if (! value.isValid()) return false;

  if (value.type() == QVariant::Color) {
    auto size1 = QItemDelegate::sizeHint(option, ind);

    int w = option.rect.height();

    size = QSize(size1.width() + w, size1.height());
  }
  else
    return false;

  return true;
}
