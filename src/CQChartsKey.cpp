#include <CQChartsKey.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQPropertyViewModel.h>
#include <CQRoundedPolygon.h>
#include <QPainter>
#include <QRectF>

CQChartsKey::
CQChartsKey(CQChartsPlot *plot) :
 plot_(plot)
{
  setBackground(Qt::white);
  setBorder    (true);

  clearItems();
}

CQChartsKey::
~CQChartsKey()
{
  for (auto &item : items_)
    delete item;
}

void
CQChartsKey::
redrawBoxObj()
{
  redraw();
}

void
CQChartsKey::
redraw()
{
  plot_->update();
}

void
CQChartsKey::
updatePlotKey()
{
  plot_->resetKeyItems();
}

void
CQChartsKey::
updateLayout()
{
  invalidateLayout();

  redraw();
}

QString
CQChartsKey::
locationStr() const
{
  switch (location_) {
    case Location::TOP_LEFT:      return "tl";
    case Location::TOP_CENTER:    return "tc";
    case Location::TOP_RIGHT:     return "tr";
    case Location::CENTER_LEFT:   return "cl";
    case Location::CENTER_CENTER: return "cc";
    case Location::CENTER_RIGHT:  return "cr";
    case Location::BOTTOM_LEFT:   return "bl";
    case Location::BOTTOM_CENTER: return "bc";
    case Location::BOTTOM_RIGHT:  return "br";
    default:                      return "none";
  }
}

void
CQChartsKey::
setLocationStr(const QString &str)
{
  QString lstr = str.toLower();

  if      (lstr == "tl") location_ = Location::TOP_LEFT;
  else if (lstr == "tc") location_ = Location::TOP_CENTER;
  else if (lstr == "tr") location_ = Location::TOP_RIGHT;
  else if (lstr == "cl") location_ = Location::CENTER_LEFT;
  else if (lstr == "cc") location_ = Location::CENTER_CENTER;
  else if (lstr == "cr") location_ = Location::CENTER_RIGHT;
  else if (lstr == "bl") location_ = Location::BOTTOM_LEFT;
  else if (lstr == "bc") location_ = Location::BOTTOM_CENTER;
  else if (lstr == "br") location_ = Location::BOTTOM_RIGHT;

  updatePosition();
}

void
CQChartsKey::
updatePosition()
{
  plot_->updateKeyPosition();

  redraw();
}

void
CQChartsKey::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "visible"   );
  model->addProperty(path, this, "location"  );
  model->addProperty(path, this, "insideX"   );
  model->addProperty(path, this, "insideY"   );
  model->addProperty(path, this, "spacing"   );
  model->addProperty(path, this, "horizontal");
  model->addProperty(path, this, "above"     );
  model->addProperty(path, this, "flipped"   );

  CQChartsBoxObj::addProperties(model, path);

  QString textPath = path + "/text";

  model->addProperty(textPath, this, "textColor", "color");
  model->addProperty(textPath, this, "textFont" , "font" );
  model->addProperty(textPath, this, "textAlign", "align");
}

void
CQChartsKey::
invalidateLayout()
{
  needsLayout_ = true;
}

void
CQChartsKey::
clearItems()
{
  for (auto &item : items_)
    delete item;

  items_.clear();

  invalidateLayout();

  maxRow_ = 0;
  maxCol_ = 0;
}

void
CQChartsKey::
addItem(CQChartsKeyItem *item, int row, int col, int nrows, int ncols)
{
  item->setKey(this);

  item->setRow(row);
  item->setCol(col);

  item->setRowSpan(nrows);
  item->setColSpan(ncols);

  items_.push_back(item);

  invalidateLayout();

  maxRow_ = std::max(maxRow_, row + nrows);
  maxCol_ = std::max(maxCol_, col + ncols);
}

void
CQChartsKey::
doLayout()
{
  if (! needsLayout_)
    return;

  needsLayout_ = false;

  //---

  // get items in each cell and dimension of grid
  using ColItems    = std::map<int,Items>;
  using RowColItems = std::map<int,ColItems>;

  RowColItems rowColItems;

  numRows_ = 0;
  numCols_ = 0;

  for (const auto &item : items_) {
    numRows_ = std::max(numRows_, item->row() + item->rowSpan());
    numCols_ = std::max(numCols_, item->col() + item->colSpan());
  }

  for (const auto &item : items_) {
    int col = item->col();

    if (isFlipped())
      col = numCols_ - 1 - col;

    rowColItems[item->row()][col].push_back(item);
  }

  //---

  // get size of each cell
  rowColCell_.clear();

  for (int r = 0; r < numRows_; ++r) {
    for (int c = 0; c < numCols_; ++c) {
      const Items &items = rowColItems[r][c];

      for (const auto &item : items) {
        QSizeF size = item->size();

        double width  = size.width ()/item->colSpan();
        double height = size.height()/item->rowSpan();

        rowColCell_[r][c].width  = std::max(rowColCell_[r][c].width , width );
        rowColCell_[r][c].height = std::max(rowColCell_[r][c].height, height);
      }
    }
  }

  //---

  double xs = plot_->pixelToWindowWidth (spacing());
  double ys = plot_->pixelToWindowHeight(spacing());

  double xm = plot_->pixelToWindowWidth (margin());
  double ym = plot_->pixelToWindowHeight(margin());

  //---

  // get size of each row and column
  using RowHeights = std::map<int,double>;
  using ColWidths  = std::map<int,double>;

  RowHeights rowHeights;
  ColWidths  colWidths;

  for (int r = 0; r < numRows_; ++r) {
    for (int c = 0; c < numCols_; ++c) {
      rowHeights[r] = std::max(rowHeights[r], rowColCell_[r][c].height);
      colWidths [c] = std::max(colWidths [c], rowColCell_[r][c].width );
    }
  }

  //----

  // update cell positions and sizes
  double y = -ym;

  for (int r = 0; r < numRows_; ++r) {
    double x = xm;

    double rh = rowHeights[r] + 2*ys;

    for (int c = 0; c < numCols_; ++c) {
      double cw = colWidths[c] + 2*xs;

      Cell &cell = rowColCell_[r][c];

      cell.x      = x;
      cell.y      = y;
      cell.width  = cw;
      cell.height = rh;

      x += cell.width;
    }

    y -= rh; // T->B
  }

  //----

  double w = 0, h = 0;

  for (int c = 0; c < numCols_; ++c) {
    Cell &cell = rowColCell_[0][c];

    w += cell.width;
  }

  w += 2*xm;

  for (int r = 0; r < numRows_; ++r) {
    Cell &cell = rowColCell_[r][0];

    h += cell.height;
  }

  h += 2*ym;

  size_ = QSizeF(w, h);
}

QSizeF
CQChartsKey::
calcSize()
{
  doLayout();

  return size_;
}

bool
CQChartsKey::
contains(const CPoint2D &p) const
{
  if (! isVisible())
    return false;

  return bbox().inside(p);
}

CQChartsKeyItem *
CQChartsKey::
getItemAt(const CPoint2D &p) const
{
  if (! isVisible())
    return nullptr;

  for (auto &item : items_) {
    if (item->bbox().inside(p))
      return item;
  }

  return nullptr;
}

bool
CQChartsKey::
setInside(CQChartsKeyItem *item)
{
  bool changed = false;

  for (auto &item1 : items_) {
    if (item1 == item) {
      if (! item1->isInside()) {
        item1->setInside(true);

        changed = true;
      }
    }
    else {
      if (item1->isInside()) {
        item1->setInside(false);

        changed = true;
      }
    }
  }

  return changed;
}

void
CQChartsKey::
setFlipped(bool b)
{
  if (b == flipped_)
    return;

  flipped_ = b;

  needsLayout_ = true;

  redraw();
}

void
CQChartsKey::
draw(QPainter *p)
{
  if (! isVisible())
    return;

  //---

  doLayout();

  //---

  double x = position_.x(); // left
  double y = position_.y(); // top

  double w = size_.width ();
  double h = size_.height();

  bbox_ = CBBox2D(x, y - h, x + w, y);

  //---

  double px1, py1, px2, py2;

  plot_->windowToPixel(x    , y    , px1, py2);
  plot_->windowToPixel(x + w, y - h, px2, py1);

  QRectF rect(px1, py2, px2 - px1, py1 - py2);

  //---

  p->save();

  QRectF dataRect = plot_->calcRect();
  QRectF clipRect = CQChartsUtil::toQRect(plot_->calcPixelRect());

  if (isInsideX()) {
    clipRect.setLeft (dataRect.left ());
    clipRect.setRight(dataRect.right());
  }

  if (isInsideY()) {
    clipRect.setTop   (dataRect.top   ());
    clipRect.setBottom(dataRect.bottom());
  }

  p->setClipRect(clipRect, Qt::ReplaceClip);

  //---

  CQChartsBoxObj::draw(p, rect);

  //---

  for (const auto &item : items_) {
    int col = item->col();

    if (isFlipped())
      col = numCols_ - 1 - col;

    Cell &cell = rowColCell_[item->row()][col];

    double x1 = cell.x;
    double y1 = cell.y;
    double w1 = cell.width;
    double h1 = cell.height;

    for (int c = 1; c < item->colSpan(); ++c) {
      Cell &cell1 = rowColCell_[item->row()][col + c];

      w1 += cell1.width;
    }

    for (int r = 1; r < item->rowSpan(); ++r) {
      Cell &cell1 = rowColCell_[item->row() + r][col];

      h1 += cell1.height;
    }

    CBBox2D bbox(x1 + x, y1 + y - h1, x1 + x + w1, y1 + y);

    item->setBBox(bbox);

    item->draw(p, bbox);
  }

  //---

  if (plot_->showBoxes())
    plot_->drawWindowRedBox(p, bbox_);

  //---

  p->restore();
}

QColor
CQChartsKey::
bgColor() const
{
  if (isBackground())
    return backgroundColor();

  if      (isInsideX() && isInsideY()) {
    if (plot_->isDataBackground())
      return plot_->dataBackgroundColor();
  }
  else if (isInsideX()) {
    if (location_ == CENTER_LEFT || location_ == CENTER_CENTER || location_ == CENTER_RIGHT) {
      if (plot_->isDataBackground())
        return plot_->dataBackgroundColor();
    }
  }
  else if (isInsideY()) {
    if (location_ == TOP_CENTER || location_ == CENTER_CENTER || location_ == BOTTOM_CENTER) {
      if (plot_->isDataBackground())
        return plot_->dataBackgroundColor();
    }
  }

  if (plot_->isBackground())
    return plot_->backgroundColor();

  return Qt::white;
}

//------

CQChartsKeyItem::
CQChartsKeyItem(CQChartsKey *key) :
 key_(key)
{
}

//------

CQChartsKeyText::
CQChartsKeyText(CQChartsPlot *plot, const QString &text) :
 CQChartsKeyItem(plot->key()), plot_(plot), text_(text)
{
}

QSizeF
CQChartsKeyText::
size() const
{
  CQChartsPlot *plot = key_->plot();

  QFontMetricsF fm(key_->textFont());

  double w = fm.width(text_);
  double h = fm.height();

  double ww = plot->pixelToWindowWidth (w + 4);
  double wh = plot->pixelToWindowHeight(h + 4);

  return QSizeF(ww, wh);
}

void
CQChartsKeyText::
draw(QPainter *p, const CBBox2D &rect)
{
  CQChartsPlot *plot = key_->plot();

  p->setFont(key_->textFont());

  QFontMetricsF fm(p->font());

  p->setPen(textColor());

  double px1, px2, py;

  plot->windowToPixel(rect.getXMin(), rect.getYMin(), px1, py);
  plot->windowToPixel(rect.getXMax(), rect.getYMin(), px2, py);

  double px = px1 + 2;

  if (key_->textAlign() & Qt::AlignRight)
    px = px2 - 2 - fm.width(text_);

  p->drawText(px, py - fm.descent() - 2, text_);
}

QColor
CQChartsKeyText::
textColor() const
{
  return key_->textColor();
}

//------

CQChartsKeyColorBox::
CQChartsKeyColorBox(CQChartsPlot *plot, int i, int n) :
 CQChartsKeyItem(plot->key()), plot_(plot), i_(i), n_(n)
{
}

QSizeF
CQChartsKeyColorBox::
size() const
{
  CQChartsPlot *plot = key_->plot();

  QFontMetricsF fm(key_->textFont());

  double h = fm.height();

  double ww = plot->pixelToWindowWidth (h + 2);
  double wh = plot->pixelToWindowHeight(h + 2);

  return QSizeF(ww, wh);
}

void
CQChartsKeyColorBox::
draw(QPainter *p, const CBBox2D &rect)
{
  CQChartsPlot *plot = key_->plot();

  CBBox2D prect;

  plot->windowToPixel(rect, prect);

  QRectF prect1(QPointF(prect.getXMin() + 2, prect.getYMin() + 2),
                QPointF(prect.getXMax() - 2, prect.getYMax() - 2));

  QColor bc    = borderColor();
  QBrush brush = fillBrush();

  if (isInside())
    brush.setColor(plot->insideColor(brush.color()));

  p->setPen  (bc);
  p->setBrush(brush);

  CQRoundedPolygon::draw(p, prect1, cornerRadius());
}

QBrush
CQChartsKeyColorBox::
fillBrush() const
{
  CQChartsPlot *plot = key_->plot();

  return plot->paletteColor(i_, n_);
}
