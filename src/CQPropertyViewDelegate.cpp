#include <CQPropertyViewDelegate.h>
#include <CQPropertyViewTree.h>
#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

#include <QApplication>
#include <QTreeWidget>
#include <QLineEdit>
#include <QPainter>
#include <QLayout>

#include <cassert>

/*! create tree view delegate
*/
CQPropertyViewDelegate::
CQPropertyViewDelegate(CQPropertyViewTree *view) :
 QItemDelegate(view), view_(view)
{
}

/*! create editor for view item
*/
QWidget *
CQPropertyViewDelegate::
createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
  if (index.column() != 1)
    return nullptr;

  CQPropertyViewItem *item = view_->getModelItem(index);
  assert(item);

  CQPropertyViewDelegate *th = const_cast<CQPropertyViewDelegate *>(this);

  QWidget *w = item->createEditor(parent);

  assert(w);

  w->updateGeometry();

  if (w->layout())
    w->layout()->invalidate();

  //w->setFixedSize(sizeHint(option, index));

  w->installEventFilter(th);

  return w;
}

/*! get data to display in view item
*/
void
CQPropertyViewDelegate::
setEditorData(QWidget *, const QModelIndex &index) const
{
  if (index.column() != 1)
    return;

  CQPropertyViewItem *item = view_->getModelItem(index);
  assert(item);

  QVariant var = item->data();

  if (var.isNull()) {
    //std::cerr << "Failed to get model data" << std::endl;
  }

  item->setEditorData(var);
}

/*! store displayed view item data in model
*/
void
CQPropertyViewDelegate::
setModelData(QWidget *, QAbstractItemModel *model, const QModelIndex &index) const
{
  if (index.column() != 1)
    return;

  CQPropertyViewItem *item = view_->getModelItem(index);
  assert(item);

  QVariant var = item->getEditorData();

  model->setData(index, var);
}

/*! update geometry
*/
void
CQPropertyViewDelegate::
updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const
{
  if (index.column() != 1)
    return;

  editor->setGeometry(option.rect);
}

/*! draw item
*/
void
CQPropertyViewDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  CQPropertyViewItem *item = view_->getModelItem(index);
  assert(item);

  //---

  QStyleOptionViewItem option1 = option;

  if (! item->isWritable()) {
    option1.font.setItalic(true);
  }

  bool inside = view_->isMouseInd(index);

  if      (index.column() == 0) {
    QString label = item->aliasName();

    drawString(painter, option, label, index, inside);

    //QItemDelegate::paint(painter, option1, index);
  }
  else if (index.column() == 1) {
    if (inside)
      item->setInside(true);

    if (! item->paint(this, painter, option1, index))
      QItemDelegate::paint(painter, option1, index);

    if (inside)
      item->setInside(false);
  }
}

/*! size hint
*/
QSize
CQPropertyViewDelegate::
sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QSize size = QItemDelegate::sizeHint(option, index);

  size.setHeight(size.height() + 2);

  return size;
}

/*! create edit widget
*/
QWidget *
CQPropertyViewDelegate::
createEdit(QWidget *parent, const QString &text) const
{
  QLineEdit *edit = new QLineEdit(parent);

  edit->setObjectName("edit");

  edit->setText(text);

  edit->setMouseTracking(false);
  edit->setFocusPolicy(Qt::NoFocus);
  edit->setAcceptDrops(false);
  edit->setFrame(false);
  edit->setReadOnly(true);

  // set background of preview text to window background
  QPalette plt = QApplication::palette();
  QColor bgColor = plt.color(QPalette::Window);
  QString styleStr;
  styleStr.sprintf("background: #%2x%2x%2x", bgColor.red(), bgColor.green(), bgColor.blue());
  edit->setStyleSheet(styleStr);

  return edit;
}

void
CQPropertyViewDelegate::
drawBackground(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index, bool /*inside*/) const
{
  QItemDelegate::drawBackground(painter, option, index);
}

void
CQPropertyViewDelegate::
drawCheck(QPainter *painter, const QStyleOptionViewItem &option,
          bool checked, const QModelIndex &index, bool inside) const
{
  drawBackground(painter, option, index, inside);

  Qt::CheckState checkState = (checked ? Qt::Checked : Qt::Unchecked);

  QRect rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(0, 1, -3, -2);

  QItemDelegate::drawCheck(painter, option, rect, checkState);

  QFontMetrics fm(painter->font());

  int x = rect.right() + 4;
//int y = rect.top() + fm.ascent();

  QRect rect1;

  rect1.setCoords(x, option.rect.top(), option.rect.right(), option.rect.bottom());

  //painter->drawText(x, y, (checked ? "true" : "false"));
  QItemDelegate::drawDisplay(painter, option, rect1, checked ? "true" : "false");
}

void
CQPropertyViewDelegate::
drawColor(QPainter *painter, const QStyleOptionViewItem &option,
          const QColor &c, const QModelIndex &index, bool inside) const
{
  drawBackground(painter, option, index, inside);

  QRect rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(0, 1, -3, -2);

  painter->fillRect(rect, QBrush(c));

  painter->setPen(QColor(0,0,0));
  painter->drawRect(rect);

  QFontMetrics fm(painter->font());

  int x = rect.right() + 2;
//int y = rect.top() + fm.ascent();

  QRect rect1;

  rect1.setCoords(x, option.rect.top(), option.rect.right(), option.rect.bottom());

//painter->drawText(x, y, c.name());
  QItemDelegate::drawDisplay(painter, option, rect1, c.name());
}

void
CQPropertyViewDelegate::
drawFont(QPainter *painter, const QStyleOptionViewItem &option,
         const QFont &f, const QModelIndex &index, bool inside) const
{
  drawBackground(painter, option, index, inside);

  QRect rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(0, 1, -3, -2);

  QFont f1 = f;
  QFont f2 = painter->font();

  QFontMetrics fm1(f1);
  QFontMetrics fm2(f2);

  int fw = fm1.width("Abc");
  int fh = fm1.height();

  if (fh > rect.height()) {
    f1.setPixelSize(rect.height());

    fm1 = QFontMetrics(f1);

    fw = fm1.width("Abc");
  }

  int x1 = rect.left();
  int y1 = rect.top() + fm1.ascent();

  painter->save();

  painter->setFont(f1);
  painter->setPen(QColor(0,0,0));

  painter->drawText(x1, y1, "Abc");

  painter->restore();

  int x2 = x1 + fw + 4;
//int y2 = rect.top() + fm2.ascent();

  QRect rect1;

  rect1.setCoords(x2, option.rect.top(), option.rect.right(), option.rect.bottom());

//painter->drawText(x2, y2, f.toString());
  QItemDelegate::drawDisplay(painter, option, rect1, f.toString());
}

void
CQPropertyViewDelegate::
drawPoint(QPainter *painter, const QStyleOptionViewItem &option,
         const QPointF &p, const QModelIndex &index, bool inside) const
{
  drawBackground(painter, option, index, inside);

  QRect rect = option.rect;

  //rect.setWidth(option.rect.height());

  //QFontMetrics fm(painter->font());

  QString str = QString("(%1, %2)").arg(p.x()).arg(p.y());

  QItemDelegate::drawDisplay(painter, option, rect, str);
}

void
CQPropertyViewDelegate::
drawSize(QPainter *painter, const QStyleOptionViewItem &option,
         const QSizeF &s, const QModelIndex &index, bool inside) const
{
  drawBackground(painter, option, index, inside);

  QRect rect = option.rect;

  //rect.setWidth(option.rect.height());

  //QFontMetrics fm(painter->font());

  QString str = QString("(%1, %2)").arg(s.width()).arg(s.height());

  QItemDelegate::drawDisplay(painter, option, rect, str);
}

void
CQPropertyViewDelegate::
drawRect(QPainter *painter, const QStyleOptionViewItem &option,
         const QRectF &r, const QModelIndex &index, bool inside) const
{
  drawBackground(painter, option, index, inside);

  QRect rect = option.rect;

  //rect.setWidth(option.rect.height());

  //QFontMetrics fm(painter->font());

  QString str = QString("(%1, %2) (%3 %4)").arg(r.left ()).arg(r.top   ()).
                                            arg(r.right()).arg(r.bottom());

  QItemDelegate::drawDisplay(painter, option, rect, str);
}

#if 0
void
CQPropertyViewDelegate::
drawAngle(QPainter *painter, const QStyleOptionViewItem &option,
         const CAngle &a, const QModelIndex &index, bool inside) const
{
  drawBackground(painter, option, index, inside);

  QRect rect = option.rect;

  //rect.setWidth(option.rect.height());

  //QFontMetrics fm(painter->font());

  QString str = QString("%1").arg(a.degrees());

  QItemDelegate::drawDisplay(painter, option, rect, str);
}
#endif

void
CQPropertyViewDelegate::
drawString(QPainter *painter, const QStyleOptionViewItem &option, const QString &str,
           const QModelIndex &index, bool inside) const
{
  drawBackground(painter, option, index, inside);

  QRect rect = option.rect;

  if (inside) {
    QStyleOptionViewItem option1 = option;

    QColor c = QColor(100, 100, 200);

    option1.palette.setColor(QPalette::WindowText, c);
    option1.palette.setColor(QPalette::Text      , c);

    QItemDelegate::drawDisplay(painter, option1, rect, str);
  }
  else
    QItemDelegate::drawDisplay(painter, option, rect, str);

  //painter->fillRect(option.rect, Qt::red);
}
