#include <CQChartsKey.h>
#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQPropertyViewModel.h>
#include <CQChartsRoundedPolygon.h>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOption>
#include <QRectF>

CQChartsKey::
CQChartsKey(CQChartsView *view) :
 CQChartsBoxObj(view), CQChartsObjTextData<CQChartsKey>(this)
{
  setObjectName("key");
}

CQChartsKey::
CQChartsKey(CQChartsPlot *plot) :
 CQChartsBoxObj(plot), CQChartsObjTextData<CQChartsKey>(this)
{
  setObjectName("key");

  setFilled(false);
}

QString
CQChartsKey::
id() const
{
  if      (view_)
    return view_->id();
  else if (plot_)
    return plot_->id();
  else
    return "";
}

void
CQChartsKey::
setSelected(bool b)
{
  CQChartsUtil::testAndSet(selected_, b, [&]() {
    if      (view_)
      view_->update();
    else if (plot_)
      plot_->invalidateLayers();
  } );
}

CQChartsKey::
~CQChartsKey()
{
}

void
CQChartsKey::
setLocation(const CQChartsKeyLocation &l)
{
  CQChartsUtil::testAndSet(location_, l, [&]() { updatePosition(); } );
}

//---

void
CQChartsKey::
draw(QPainter *)
{
}

//------

CQChartsViewKey::
CQChartsViewKey(CQChartsView *view) :
 CQChartsKey(view)
{
}

CQChartsViewKey::
~CQChartsViewKey()
{
}

void
CQChartsViewKey::
updatePosition()
{
  redraw();
}

void
CQChartsViewKey::
updateLayout()
{
  redraw();
}

void
CQChartsViewKey::
doLayout()
{
  double x  = 0.0, y  = 0.0;
  double dx = 0.0, dy = 0.0;

  if      (location().onLeft   ()) { x =   0.0; dx =  1.0; }
  else if (location().onHCenter()) { x =  50.0; dx =  0.0; }
  else if (location().onRight  ()) { x = 100.0; dx = -1.0; }

  if      (location().onTop    ()) { y = 100.0; dy =  1.0; }
  else if (location().onVCenter()) { y =  50.0; dy =  0.0; }
  else if (location().onBottom ()) { y =   0.0; dy = -1.0; }

  //----

  double px, py;

  view_->windowToPixel(x, y, px, py);

  px += dx*16.0;
  py += dy*16.0;

  //---

  QFont font = view()->viewFont(textFont());

  QFontMetricsF fm(font);

  double pw = 0.0;
  double ph = 0.0;

  int n = view_->numPlots();

  for (int i = 0; i < n; ++i) {
    CQChartsPlot *plot = view_->plot(i);

    QString name = plot->keyText();

    double tw = fm.width(name) + 16.0 + margin();

    pw = std::max(pw, tw);

    ph += fm.height();
  }

  if      (location().onLeft   ()) position_ = QPointF(px        +   margin(), py);
  else if (location().onHCenter()) position_ = QPointF(px - pw/2 -   margin(), py);
  else if (location().onRight  ()) position_ = QPointF(px - pw   - 2*margin(), py);

  size_ = QSizeF(pw + 2*margin(), ph + 2*margin());
}

void
CQChartsViewKey::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "visible"    );
  model->addProperty(path, this, "location"   );
  model->addProperty(path, this, "header"     );
  model->addProperty(path, this, "horizontal" );
  model->addProperty(path, this, "autoHide"   );
  model->addProperty(path, this, "clipped"    );
  model->addProperty(path, this, "hiddenAlpha");

  CQChartsBoxObj::addProperties(model, path);

  QString textPath = path + "/text";

  model->addProperty(textPath, this, "textColor", "color");
  model->addProperty(textPath, this, "textAlpha", "alpha");
  model->addProperty(textPath, this, "textFont" , "font" );
  model->addProperty(textPath, this, "textAlign", "align");
}

void
CQChartsViewKey::
draw(QPainter *painter)
{
  if (! isVisible())
    return;

  //---

  doLayout();

  //---

  double px = position_.x(); // left
  double py = position_.y(); // top

  double pw = size_.width ();
  double ph = size_.height();

  //bbox_ = CQChartsGeom::BBox(x, y - h, x + w, y);

  //---

  double x1, y1, x2, y2;

  view_->pixelToWindow(px     , py     , x1, y1);
  view_->pixelToWindow(px + pw, py + ph, x2, y2);

  rect_ = QRectF(x1, y2, x2 - x1, y1 - y2);

  //---

  QRectF rect(px, py, pw, ph);

  //---

  CQChartsBoxObj::draw(painter, rect);

  //---

  view()->setPainterFont(painter, textFont());

  QFontMetricsF fm(painter->font());

  double px1 = px + margin();
  double py1 = py + margin();

  int n = view_->numPlots();

  double bs = fm.height() + 4.0;

  for (int i = 0; i < n; ++i) {
    double py2 = py1 + fm.height() + 2;

    CQChartsPlot *plot = view_->plot(i);

    bool checked = plot->isVisible();

    //---

    QRectF qrect(px1, (py1 + py2)/2.0 - bs/2.0, bs, bs);

    QStylePainter spainter(view_);

    spainter.setPen(interpTextColor(0, 1));

    QStyleOptionButton opt;

    opt.initFrom(view_);

    opt.rect = qrect.toRect();

    opt.state |= (checked ? QStyle::State_On : QStyle::State_Off);

    spainter.drawControl(QStyle::CE_CheckBox, opt);

    //painter->drawRect(qrect);

    //---

    painter->setPen(interpTextColor(0, 1));

    QString name = plot->keyText();

    double px2 = px1 + bs + margin();

    //double tw = fm.width(name);

    painter->drawText(px2, py1 + fm.ascent(), name);

    //---

    double x1, y1, x2, y2;

    view_->pixelToWindow(px     , py1, x1, y1);
    view_->pixelToWindow(px + pw, py2, x2, y2);

    QRectF prect(x1, y2, x2 - x1, y1 - y2);

    prects_.push_back(prect);

    //---

    py1 = py2;
  }
}

bool
CQChartsViewKey::
isInside(const CQChartsGeom::Point &w) const
{
  return rect_.contains(CQChartsUtil::toQPoint(w));
}

void
CQChartsViewKey::
selectPress(const CQChartsGeom::Point &w, CQChartsSelMod /*selMod*/)
{
  int n = std::min(view_->numPlots(), int(prects_.size()));

  for (int i = 0; i < n; ++i) {
    CQChartsPlot *plot = view_->plot(i);

    if (prects_[i].contains(CQChartsUtil::toQPoint(w))) {
      plot->setVisible(! plot->isVisible());
      break;
    }
  }

  redraw();
}

void
CQChartsViewKey::
redraw()
{
  view_->update();
}

//------

CQChartsPlotKey::
CQChartsPlotKey(CQChartsPlot *plot) :
 CQChartsKey(plot), editHandles_(plot, CQChartsEditHandles::Mode::MOVE)
{
  setBorder(true);

  clearItems();
}

CQChartsPlotKey::
~CQChartsPlotKey()
{
  for (auto &item : items_)
    delete item;
}

//---

void
CQChartsPlotKey::
redraw()
{
  plot_->invalidateLayer(CQChartsBuffer::Type::BACKGROUND);
  plot_->invalidateLayer(CQChartsBuffer::Type::FOREGROUND);
}

void
CQChartsPlotKey::
updateKeyItems()
{
  plot_->resetKeyItems();

  redraw();
}

void
CQChartsPlotKey::
updateLayout()
{
  invalidateLayout();

  redraw();
}

void
CQChartsPlotKey::
updatePosition()
{
  plot_->updateKeyPosition();

  redraw();
}

void
CQChartsPlotKey::
updateLocation(const CQChartsGeom::BBox &bbox)
{
  // calc key size
  QSizeF ks = calcSize();

  double xm = plot_->pixelToWindowWidth (8);
  double ym = plot_->pixelToWindowHeight(8);

  double kx { 0.0 }, ky { 0.0 };

  if      (location().onLeft()) {
    if (isInsideX())
      kx = bbox.getXMin() + xm;
    else
      kx = bbox.getXMin() - ks.width() - xm;
  }
  else if (location().onHCenter()) {
    kx = bbox.getXMid() - ks.width()/2;
  }
  else if (location().onRight()) {
    if (isInsideX())
      kx = bbox.getXMax() - ks.width() - xm;
    else
      kx = bbox.getXMax() + xm;
  }

  if      (location().onTop()) {
    if (isInsideY())
      ky = bbox.getYMax() - ym;
    else
      ky = bbox.getYMax() + ks.height() + ym;
  }
  else if (location().onVCenter()) {
    ky = bbox.getYMid() - ks.height()/2;
  }
  else if (location().onBottom()) {
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

  CQChartsKeyLocation::Type locationType = this->location().type();

  if (locationType == CQChartsKeyLocation::Type::ABS_POS) {
    kp = absPlotPosition();
  }

  setPosition(kp);
}

void
CQChartsPlotKey::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "visible"    );
  model->addProperty(path, this, "location"   );
  model->addProperty(path, this, "header"     );
  model->addProperty(path, this, "horizontal" );
  model->addProperty(path, this, "autoHide"   );
  model->addProperty(path, this, "clipped"    );
  model->addProperty(path, this, "hiddenAlpha");

  model->addProperty(path, this, "absPosition");
  model->addProperty(path, this, "insideX"    );
  model->addProperty(path, this, "insideY"    );
  model->addProperty(path, this, "spacing"    );
  model->addProperty(path, this, "above"      );
  model->addProperty(path, this, "flipped"    );

  CQChartsBoxObj::addProperties(model, path);

  QString textPath = path + "/text";

  model->addProperty(textPath, this, "textColor", "color");
  model->addProperty(textPath, this, "textAlpha", "alpha");
  model->addProperty(textPath, this, "textFont" , "font" );
  model->addProperty(textPath, this, "textAlign", "align");
}

void
CQChartsPlotKey::
invalidateLayout()
{
  needsLayout_ = true;
}

void
CQChartsPlotKey::
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
CQChartsPlotKey::
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
CQChartsPlotKey::
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

  double tw = 0, th = 0;

  if (headerStr().length()) {
    QFont font = view()->plotFont(plot_, textFont());

    QFontMetricsF fm(font);

    double ptw = fm.width(headerStr());
    double pth = fm.height();

    tw = plot_->pixelToWindowWidth (ptw) + 2*xm;
    th = plot_->pixelToWindowHeight(pth) + 2*ym;
  }

  //---

  // update cell positions and sizes
  double y = -ym;

  y -= th;

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

  h += 2*ym + th;

  w = std::max(w, tw);

  size_ = QSizeF(w, h);
}

QPointF
CQChartsPlotKey::
absPlotPosition() const
{
  double wx, wy;

  plot_->viewToWindow(absPosition().x(), absPosition().y(), wx, wy);

  return QPointF(wx, wy);
}

void
CQChartsPlotKey::
setAbsPlotPosition(const QPointF &p)
{
  double vx, vy;

  plot_->windowToView(p.x(), p.y(), vx, vy);

  setAbsPosition(QPointF(vx, vy));
}

QSizeF
CQChartsPlotKey::
calcSize()
{
  doLayout();

  return size_;
}

bool
CQChartsPlotKey::
contains(const CQChartsGeom::Point &p) const
{
  if (! isVisible() || isEmpty())
    return false;

  return bbox().inside(p);
}

CQChartsKeyItem *
CQChartsPlotKey::
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

bool
CQChartsPlotKey::
isEmpty() const
{
  return items_.empty();
}

//------

bool
CQChartsPlotKey::
selectMove(const CQChartsGeom::Point &w)
{
  bool changed = false;

  if (contains(w)) {
    CQChartsKeyItem *item = getItemAt(w);

    bool handled = false;

    if (item) {
      changed = setInside(item);

      handled = item->selectMove(w);
    }

    if (changed)
      redraw();

    if (handled)
      return true;
  }

  changed = setInside(nullptr);

  if (changed)
    redraw();

  return false;
}

//------

bool
CQChartsPlotKey::
editPress(const CQChartsGeom::Point &p)
{
  editHandles_.setDragPos(p);

  location_ = CQChartsKeyLocation(CQChartsKeyLocation::Type::ABS_POS);

  setAbsPlotPosition(position_);

  return true;
}

bool
CQChartsPlotKey::
editMove(const CQChartsGeom::Point &p)
{
  const CQChartsGeom::Point &dragPos = editHandles_.dragPos();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  location_ = CQChartsKeyLocation::Type::ABS_POS;

  setAbsPlotPosition(absPlotPosition() + QPointF(dx, dy));

  editHandles_.setDragPos(p);

  updatePosition();

  return true;
}

bool
CQChartsPlotKey::
editMotion(const CQChartsGeom::Point &p)
{
  return editHandles_.selectInside(p);
}

bool
CQChartsPlotKey::
editRelease(const CQChartsGeom::Point &)
{
  return true;
}

void
CQChartsPlotKey::
editMoveBy(const QPointF &f)
{
  location_ = CQChartsKeyLocation::Type::ABS_POS;

  setAbsPlotPosition(position_ + f);

  updatePosition();
}

//------

bool
CQChartsPlotKey::
tipText(const CQChartsGeom::Point &p, QString &tip) const
{
  bool rc = false;

  CQChartsKeyItem *item = getItemAt(p);

  if (item) {
    QString tip1;

    if (item->tipText(p, tip1)) {
      if (! tip.length())
        tip += "\n";

      tip += tip1;

      rc = true;
    }
  }

  return rc;
}

//------

bool
CQChartsPlotKey::
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
CQChartsPlotKey::
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
CQChartsPlotKey::
draw(QPainter *painter)
{
  if (! isVisible() || isEmpty())
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

  CQChartsGeom::BBox pixelRect = plot_->calcPlotPixelRect();

  pixelWidthExceeded_  = (rect.width() > pixelRect.getWidth());
  pixelHeightExceeded_ = rect.height() > pixelRect.getHeight();

  if (isAutoHide()) {
    if (pixelWidthExceeded_ || pixelHeightExceeded_)
      return;
  }

  //---

  painter->save();

  QRectF dataRect = CQChartsUtil::toQRect(plot_->calcDataPixelRect());
  QRectF clipRect = CQChartsUtil::toQRect(plot_->calcPlotPixelRect());

  CQChartsKeyLocation::Type locationType = this->location().type();

  if (locationType != CQChartsKeyLocation::Type::ABS_POS) {
    if (isInsideX()) {
      clipRect.setLeft (dataRect.left ());
      clipRect.setRight(dataRect.right());
    }

    if (isInsideY()) {
      clipRect.setTop   (dataRect.top   ());
      clipRect.setBottom(dataRect.bottom());
    }
  }

  if (isClipped())
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

  if (headerStr().length()) {
    QPointF p = plot_->windowToPixel(QPointF(x, y)); // top left

    double pw = plot_->windowToPixelWidth(w);

    double xm = margin();
    double ym = margin();

    QFont font = view()->plotFont(plot_, textFont());

    QFontMetricsF fm(font);

    double fa = fm.ascent ();
//  double fd = fm.descent();

    double tw = fm.width(headerStr()) + 2*xm;
//  double th = fa + fd + 2*ym;

    double dx = (pw - tw + xm)/2.0;

    painter->drawText(p.x() + dx, p.y() + fa + ym, headerStr());
  }

  //---

  if (plot_->showBoxes())
    plot_->drawWindowColorBox(painter, bbox_);

  //---

  painter->restore();
}

void
CQChartsPlotKey::
drawEditHandles(QPainter *painter)
{
  assert(plot_->view()->mode() == CQChartsView::Mode::EDIT || isSelected());

  editHandles_.setBBox(this->bbox());

  editHandles_.draw(painter);
}

QColor
CQChartsPlotKey::
interpBgColor() const
{
  if (isFilled())
    return interpFillColor(0, 1);

  CQChartsKeyLocation::Type locationType = this->location().type();

  if (locationType != CQChartsKeyLocation::Type::ABS_POS) {
    if      (isInsideX() && isInsideY()) {
      if (plot_->isDataFilled())
        return plot_->interpDataFillColor(0, 1);
    }
    else if (isInsideX()) {
      if (locationType == CQChartsKeyLocation::Type::CENTER_LEFT ||
          locationType == CQChartsKeyLocation::Type::CENTER_CENTER ||
          locationType == CQChartsKeyLocation::Type::CENTER_RIGHT) {
        if (plot_->isDataFilled())
          return plot_->interpDataFillColor(0, 1);
      }
    }
    else if (isInsideY()) {
      if (locationType == CQChartsKeyLocation::Type::TOP_CENTER ||
          locationType == CQChartsKeyLocation::Type::CENTER_CENTER ||
          locationType == CQChartsKeyLocation::Type::BOTTOM_CENTER) {
        if (plot_->isDataFilled())
          return plot_->interpDataFillColor(0, 1);
      }
    }
  }

  if (plot_->isPlotFilled())
    return plot_->interpPlotFillColor(0, 1);

  return plot_->interpThemeColor(0);
}

//------

CQChartsKeyItem::
CQChartsKeyItem(CQChartsPlotKey *key) :
 key_(key)
{
}

bool
CQChartsKeyItem::
tipText(const CQChartsGeom::Point &, QString &) const
{
  return false;
}

//------

CQChartsKeyText::
CQChartsKeyText(CQChartsPlot *plot, const QString &text, int i, int n) :
 CQChartsKeyItem(plot->key()), plot_(plot), text_(text), i_(i), n_(n)
{
}

QSizeF
CQChartsKeyText::
size() const
{
  CQChartsPlot *plot = key_->plot();

  QFont font = plot->view()->plotFont(plot, key_->textFont());

  QFontMetricsF fm(font);

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

  plot->view()->setPlotPainterFont(plot, painter, key_->textFont());

  QFontMetricsF fm(painter->font());

  QColor tc = interpTextColor(0, 1);

  painter->setPen(tc);

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
}

bool
CQChartsKeyColorBox::
selectPress(const CQChartsGeom::Point &, CQChartsSelMod)
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
  return borderColor().interpColor(plot_->charts(), i, n);
}

QSizeF
CQChartsKeyColorBox::
size() const
{
  CQChartsPlot *plot = key_->plot();

  QFont font = plot->view()->plotFont(plot, key_->textFont());

  QFontMetricsF fm(font);

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

  double cxs = plot->lengthPixelWidth (cornerRadius());
  double cys = plot->lengthPixelHeight(cornerRadius());

  CQChartsRoundedPolygon::draw(painter, prect1, cxs, cys);
}

QBrush
CQChartsKeyColorBox::
fillBrush() const
{
  CQChartsPlot *plot = key_->plot();

  QColor c = plot->interpPaletteColor(i_, n_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}
