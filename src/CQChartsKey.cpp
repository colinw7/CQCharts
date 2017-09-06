#include <CQChartsKey.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQPropertyTree.h>
#include <CQUtil.h>
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

void
CQChartsKey::
redraw()
{
  plot_->update();
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

  plot_->update();
}

void
CQChartsKey::
addProperties(CQPropertyTree *tree, const QString &path)
{
  tree->addProperty(path, this, "visible"  );
  tree->addProperty(path, this, "location" );
  tree->addProperty(path, this, "insideX"  );
  tree->addProperty(path, this, "insideY"  );
  tree->addProperty(path, this, "spacing"  );

  CQChartsBoxObj::addProperties(tree, path);

  QString textPath = path + "/text";

  tree->addProperty(textPath, this, "textColor", "color");
  tree->addProperty(textPath, this, "textFont" , "font" );
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
  typedef std::map<int,Items>    ColItems;
  typedef std::map<int,ColItems> RowColItems;

  RowColItems rowColItems;

  numRows_ = 0;
  numCols_ = 0;

  for (const auto &item : items_) {
    numRows_ = std::max(numRows_, item->row() + item->rowSpan());
    numCols_ = std::max(numCols_, item->col() + item->colSpan());

    rowColItems[item->row()][item->col()].push_back(item);
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
  typedef std::map<int,double> RowHeights;
  typedef std::map<int,double> ColWidths;

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

  QRectF rect(px1, py1, px2 - px1, py2 - py1);

  //---

  p->save();

  p->setClipRect(CQUtil::toQRect(plot_->calcPixelRect()), Qt::ReplaceClip);

  //---

  p->fillRect(rect, background());

  //---

  if (isBorder()) {
    QPen pen(borderColor());

    pen.setWidth(borderWidth());

    p->setPen  (pen);
    p->setBrush(Qt::NoBrush);

    p->drawRect(rect);
  }

  //---

  for (const auto &item : items_) {
    Cell &cell = rowColCell_[item->row()][item->col()];

    double x1 = cell.x;
    double y1 = cell.y;
    double w1 = cell.width;
    double h1 = cell.height;

    for (int c = 1; c < item->colSpan(); ++c) {
      Cell &cell1 = rowColCell_[item->row()][item->col() + c];

      w1 += cell1.width;
    }

    for (int r = 1; r < item->rowSpan(); ++r) {
      Cell &cell1 = rowColCell_[item->row() + r][item->col()];

      h1 += cell1.height;
    }

    CBBox2D bbox(x1 + x, y1 + y - h1, x1 + x + w1, y1 + y);

    item->setBBox(bbox);

    item->draw(p, bbox);
  }

  //---

  if (plot_->showBoxes()) {
    CBBox2D prect;

    plot_->windowToPixel(bbox_, prect);

    p->setPen(Qt::red);
    p->setBrush(Qt::NoBrush);

    p->drawRect(CQUtil::toQRect(prect));
  }

  //---

  p->restore();
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

  QFontMetrics fm(key_->textFont());

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

  QFontMetrics fm(p->font());

  p->setPen(textColor());

  double px, py;

  plot->windowToPixel(rect.getXMin(), rect.getYMin(), px, py);

  p->drawText(px + 2, py - fm.descent() - 2, text_);
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

  QFontMetrics fm(key_->textFont());

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

  p->setPen  (borderColor());
  p->setBrush(fillColor  ());

  p->drawRect(prect1);
}

QColor
CQChartsKeyColorBox::
fillColor() const
{
  CQChartsPlot *plot = key_->plot();

  return plot->paletteColor(i_, n_);
}

QColor
CQChartsKeyColorBox::
borderColor() const
{
  return Qt::black;
}
