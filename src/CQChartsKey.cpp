#include <CQChartsKey.h>
#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQPropertyViewModel.h>
#include <CQChartsRoundedPolygon.h>
#include <QPainter>
#include <QRectF>

CQChartsKey::
CQChartsKey(CQChartsPlot *plot) :
 CQChartsBoxObj(plot)
{
  CQChartsPaletteColor themeFg(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  textColor_ = themeFg;

  setBorder(true);

  clearItems();
}

CQChartsKey::
~CQChartsKey()
{
  for (auto &item : items_)
    delete item;
}

//---

QString
CQChartsKey::
textColorStr() const
{
  return textColor_.colorStr();
}

void
CQChartsKey::
setTextColorStr(const QString &str)
{
  textColor_.setColorStr(str);
}

QColor
CQChartsKey::
interpTextColor(int i, int n) const
{
  return textColor_.interpColor(plot_, i, n);
}

//---

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

  redraw();
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
  switch (location_.location) {
    case LocationType::TOP_LEFT:      return "tl";
    case LocationType::TOP_CENTER:    return "tc";
    case LocationType::TOP_RIGHT:     return "tr";
    case LocationType::CENTER_LEFT:   return "cl";
    case LocationType::CENTER_CENTER: return "cc";
    case LocationType::CENTER_RIGHT:  return "cr";
    case LocationType::BOTTOM_LEFT:   return "bl";
    case LocationType::BOTTOM_CENTER: return "bc";
    case LocationType::BOTTOM_RIGHT:  return "br";
    case LocationType::ABSOLUTE:      return "abs";
    default:                          return "none";
  }
}

void
CQChartsKey::
setLocationStr(const QString &str)
{
  QString lstr = str.toLower();

  if      (lstr == "tl" ) location_.location = LocationType::TOP_LEFT;
  else if (lstr == "tc" ) location_.location = LocationType::TOP_CENTER;
  else if (lstr == "tr" ) location_.location = LocationType::TOP_RIGHT;
  else if (lstr == "cl" ) location_.location = LocationType::CENTER_LEFT;
  else if (lstr == "cc" ) location_.location = LocationType::CENTER_CENTER;
  else if (lstr == "cr" ) location_.location = LocationType::CENTER_RIGHT;
  else if (lstr == "bl" ) location_.location = LocationType::BOTTOM_LEFT;
  else if (lstr == "bc" ) location_.location = LocationType::BOTTOM_CENTER;
  else if (lstr == "br" ) location_.location = LocationType::BOTTOM_RIGHT;
  else if (lstr == "abs") location_.location = LocationType::ABSOLUTE;

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
updateLocation(const CQChartsGeom::BBox &bbox)
{
  // calc key size
  QSizeF ks = calcSize();

  LocationType location = this->location();

  double xm = plot_->pixelToWindowWidth (8);
  double ym = plot_->pixelToWindowHeight(8);

  double kx { 0.0 }, ky { 0.0 };

  if      (location == LocationType::TOP_LEFT ||
           location == LocationType::CENTER_LEFT ||
           location == LocationType::BOTTOM_LEFT) {
    if (isInsideX())
      kx = bbox.getXMin() + xm;
    else
      kx = bbox.getXMin() - ks.width() - xm;
  }
  else if (location == LocationType::TOP_CENTER ||
           location == LocationType::CENTER_CENTER ||
           location == LocationType::BOTTOM_CENTER) {
    kx = bbox.getXMid() - ks.width()/2;
  }
  else if (location == LocationType::TOP_RIGHT ||
           location == LocationType::CENTER_RIGHT ||
           location == LocationType::BOTTOM_RIGHT) {
    if (isInsideX())
      kx = bbox.getXMax() - ks.width() - xm;
    else
      kx = bbox.getXMax() + xm;
  }

  if      (location == LocationType::TOP_LEFT ||
           location == LocationType::TOP_CENTER ||
           location == LocationType::TOP_RIGHT) {
    if (isInsideY())
      ky = bbox.getYMax() - ym;
    else
      ky = bbox.getYMax() + ks.height() + ym;
  }
  else if (location == LocationType::CENTER_LEFT ||
           location == LocationType::CENTER_CENTER ||
           location == LocationType::CENTER_RIGHT) {
    ky = bbox.getYMid() - ks.height()/2;
  }
  else if (location == LocationType::BOTTOM_LEFT ||
           location == LocationType::BOTTOM_CENTER ||
           location == LocationType::BOTTOM_RIGHT) {
    if (isInsideY())
      ky = bbox.getYMin() + ks.height() + ym;
    else {
      ky = bbox.getYMin() - ym;

      CQChartsAxis *xAxis = plot_->xAxis();

      if (xAxis && xAxis->side() == CQChartsAxis::Side::BOTTOM_LEFT && xAxis->bbox().isSet())
        ky -= xAxis->bbox().getHeight();
    }
  }

  QPointF kp(kx, ky);

  if (location == LocationType::ABSOLUTE) {
    kp = absPlotPosition();
  }

  setPosition(kp);
}

void
CQChartsKey::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "visible"    );
  model->addProperty(path, this, "location"   );
  model->addProperty(path, this, "absPosition");
  model->addProperty(path, this, "insideX"    );
  model->addProperty(path, this, "insideY"    );
  model->addProperty(path, this, "spacing"    );
  model->addProperty(path, this, "horizontal" );
  model->addProperty(path, this, "above"      );
  model->addProperty(path, this, "flipped"    );

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

QPointF
CQChartsKey::
absPlotPosition() const
{
  double wx, wy;

  plot_->viewToWindow(absPosition().x(), absPosition().y(), wx, wy);

  return QPointF(wx, wy);
}

void
CQChartsKey::
setAbsPlotPosition(const QPointF &p)
{
  double vx, vy;

  plot_->windowToView(p.x(), p.y(), vx, vy);

  setAbsPosition(QPointF(vx, vy));
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
contains(const CQChartsGeom::Point &p) const
{
  if (! isVisible())
    return false;

  return bbox().inside(p);
}

CQChartsKeyItem *
CQChartsKey::
getItemAt(const CQChartsGeom::Point &p) const
{
  if (! isVisible())
    return nullptr;

  for (auto &item : items_) {
    if (item->bbox().inside(p))
      return item;
  }

  return nullptr;
}

//------

bool
CQChartsKey::
mouseMove(const CQChartsGeom::Point &w)
{
  bool changed = false;

  if (contains(w)) {
    CQChartsKeyItem *item = getItemAt(w);

    bool handled = false;

    if (item) {
      changed = setInside(item);

      handled = item->mouseMove(w);
    }

    if (changed)
      plot_->update();

    if (handled)
      return true;
  }

  changed = setInside(nullptr);

  if (changed)
    plot_->update();

  return false;
}

//------

bool
CQChartsKey::
mouseDragPress(const CQChartsGeom::Point &p)
{
  dragPos_ = p;

  location_.location = LocationType::ABSOLUTE;

  setAbsPlotPosition(position_);

  return true;
}

bool
CQChartsKey::
mouseDragMove(const CQChartsGeom::Point &p)
{
  double dx = p.x - dragPos_.x;
  double dy = p.y - dragPos_.y;

  location_.location = LocationType::ABSOLUTE;

  setAbsPlotPosition(absPlotPosition() + QPointF(dx, dy));

  dragPos_ = p;

  updatePosition();

  return true;
}

void
CQChartsKey::
mouseDragRelease(const CQChartsGeom::Point &)
{
}

//------

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

//------

void
CQChartsKey::
draw(QPainter *painter)
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

  bbox_ = CQChartsGeom::BBox(x, y - h, x + w, y);

  //---

  double px1, py1, px2, py2;

  plot_->windowToPixel(x    , y    , px1, py2);
  plot_->windowToPixel(x + w, y - h, px2, py1);

  QRectF rect(px1, py2, px2 - px1, py1 - py2);

  //---

  painter->save();

  QRectF dataRect = CQChartsUtil::toQRect(plot_->calcDataPixelRect());
  QRectF clipRect = CQChartsUtil::toQRect(plot_->calcPixelRect());

  if (location_.location != LocationType::ABSOLUTE) {
    if (isInsideX()) {
      clipRect.setLeft (dataRect.left ());
      clipRect.setRight(dataRect.right());
    }

    if (isInsideY()) {
      clipRect.setTop   (dataRect.top   ());
      clipRect.setBottom(dataRect.bottom());
    }
  }

  painter->setClipRect(clipRect);

  //---

  CQChartsBoxObj::draw(painter, rect);

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

    CQChartsGeom::BBox bbox(x1 + x, y1 + y - h1, x1 + x + w1, y1 + y);

    item->setBBox(bbox);

    item->draw(painter, bbox);
  }

  //---

  if (plot_->showBoxes())
    plot_->drawWindowRedBox(painter, bbox_);

  //---

  painter->restore();
}

QColor
CQChartsKey::
interpBgColor() const
{
  if (isBackground())
    return interpBackgroundColor(0, 1);

  if (location_.location != LocationType::ABSOLUTE) {
    if      (isInsideX() && isInsideY()) {
      if (plot_->isDataBackground())
        return plot_->interpDataBackgroundColor(0, 1);
    }
    else if (isInsideX()) {
      if (location_.location == CENTER_LEFT ||
          location_.location == CENTER_CENTER ||
          location_.location == CENTER_RIGHT) {
        if (plot_->isDataBackground())
          return plot_->interpDataBackgroundColor(0, 1);
      }
    }
    else if (isInsideY()) {
      if (location_.location == TOP_CENTER ||
          location_.location == CENTER_CENTER ||
          location_.location == BOTTOM_CENTER) {
        if (plot_->isDataBackground())
          return plot_->interpDataBackgroundColor(0, 1);
      }
    }
  }

  if (plot_->isBackground())
    return plot_->interpBackgroundColor(0, 1);

  return plot_->interpThemeColor(0);
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

QColor
CQChartsKeyText::
interpTextColor(int i, int n) const
{
  return key_->interpTextColor(i, n);
}

void
CQChartsKeyText::
draw(QPainter *painter, const CQChartsGeom::BBox &rect)
{
  CQChartsPlot *plot = key_->plot();

  painter->setFont(key_->textFont());

  QFontMetricsF fm(painter->font());

  painter->setPen(interpTextColor(0, 1));

  double px1, px2, py;

  plot->windowToPixel(rect.getXMin(), rect.getYMin(), px1, py);
  plot->windowToPixel(rect.getXMax(), rect.getYMin(), px2, py);

  if (px1 > px2)
    std::swap(px1, px2);

  double px = px1 + 2;

  if (key_->textAlign() & Qt::AlignRight)
    px = px2 - 2 - fm.width(text_);

  if (! plot->isInvertY())
    painter->drawText(QPointF(px, py - fm.descent() - 2), text_);
  else
    painter->drawText(QPointF(px, py + fm.ascent() + 2), text_);
}

//------

CQChartsKeyColorBox::
CQChartsKeyColorBox(CQChartsPlot *plot, int i, int n) :
 CQChartsKeyItem(plot->key()), plot_(plot), i_(i), n_(n)
{
  borderColor_ = CQChartsPaletteColor(CQChartsPaletteColor::Type::THEME_VALUE, 1.0);
}

bool
CQChartsKeyColorBox::
mousePress(const CQChartsGeom::Point &)
{
  if (isClickHide()) {
    plot_->setSetHidden(i_, ! plot_->isSetHidden(i_));

    plot_->hiddenChanged();
  }

  return true;
}

QColor
CQChartsKeyColorBox::
interpBorderColor(int i, int n) const
{
  return borderColor_.interpColor(plot_, i, n);
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
draw(QPainter *painter, const CQChartsGeom::BBox &rect)
{
  CQChartsPlot *plot = key_->plot();

  CQChartsGeom::BBox prect;

  plot->windowToPixel(rect, prect);

  QRectF prect1(QPointF(prect.getXMin() + 2, prect.getYMin() + 2),
                QPointF(prect.getXMax() - 2, prect.getYMax() - 2));

  QColor bc    = interpBorderColor(0, 1);
  QBrush brush = fillBrush();

  if (isInside())
    brush.setColor(plot->insideColor(brush.color()));

  painter->setPen  (bc);
  painter->setBrush(brush);

  CQChartsRoundedPolygon::draw(painter, prect1, cornerRadius());
}

QBrush
CQChartsKeyColorBox::
fillBrush() const
{
  CQChartsPlot *plot = key_->plot();

  QColor c = plot->interpPaletteColor(i_, n_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}
