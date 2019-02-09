#include <CQChartsKey.h>
#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsView.h>
#include <CQChartsEditHandles.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRoundedPolygon.h>
#include <CQPropertyViewModel.h>

#include <QScrollBar>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionSlider>

CQChartsKey::
CQChartsKey(CQChartsView *view) :
 CQChartsBoxObj(view), CQChartsObjTextData<CQChartsKey>(this)
{
  setObjectName("key");

  setStateColoring(false);
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
      plot_->queueDrawObjs();
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
draw(QPainter *) const
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
updatePosition(bool /*queued*/)
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
  QFont font = view()->viewFont(textFont());

  QFontMetricsF fm(font);

  double bs = fm.height() + 4.0;

  //----

  double x = 0.0, y = 0.0;
//double dx = 0.0, dy = 0.0;

  if      (location().onLeft   ()) { x =   0.0; /*dx =  1.0; */ }
  else if (location().onHCenter()) { x =  50.0; /*dx =  0.0; */ }
  else if (location().onRight  ()) { x = 100.0; /*dx = -1.0; */ }

  if      (location().onTop    ()) { y = 100.0; /*dy =  1.0; */ }
  else if (location().onVCenter()) { y =  50.0; /*dy =  0.0; */ }
  else if (location().onBottom ()) { y =   0.0; /*dy = -1.0; */ }

  //----

  double px, py;

  view_->windowToPixel(x, y, px, py);

  //px += dx*bs;
  //py += dy*bs;

  //---

  double pw = 0.0;
  double ph = 0.0;

  int n = view_->numPlots();

  for (int i = 0; i < n; ++i) {
    CQChartsPlot *plot = view_->plot(i);

    QString name = plot->keyText();

    double tw = fm.width(name) + bs + margin();

    pw = std::max(pw, tw);

    ph += bs;
  }

  size_ = QSizeF(pw + 2*margin(), ph + 2*margin()  + (n - 1)*2);

  double pxr, pyr;

  if      (location().onLeft   ()) pxr = px                   + margin();
  else if (location().onHCenter()) pxr = px - size_.width()/2;
  else if (location().onRight  ()) pxr = px - size_.width()   - margin();

  if      (location().onTop    ()) pyr = py                    + margin();
  else if (location().onVCenter()) pyr = py - size_.height()/2;
  else if (location().onBottom ()) pyr = py - size_.height()   - margin();

  position_ = QPointF(pxr, pyr);
}

void
CQChartsViewKey::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "visible"      );
  model->addProperty(path, this, "selected"     );
  model->addProperty(path, this, "location"     );
  model->addProperty(path, this, "header"       );
  model->addProperty(path, this, "horizontal"   );
  model->addProperty(path, this, "autoHide"     );
  model->addProperty(path, this, "clipped"      );
  model->addProperty(path, this, "hiddenAlpha"  );
  model->addProperty(path, this, "pressBehavior");

  CQChartsBoxObj::addProperties(model, path);

  QString textPath = path + "/text";

  model->addProperty(textPath, this, "textColor", "color");
  model->addProperty(textPath, this, "textAlpha", "alpha");
  model->addProperty(textPath, this, "textFont" , "font" );
  model->addProperty(textPath, this, "textAlign", "align");
}

void
CQChartsViewKey::
draw(QPainter *painter) const
{
  if (! isVisible())
    return;

  //---

  CQChartsViewKey *th = const_cast<CQChartsViewKey *>(this);

  th->doLayout();

  //---

  // pixel position & size (TODO: using view/units)
  double px = position_.x(); // left
  double py = position_.y(); // top

  double pw = size_.width ();
  double ph = size_.height();

  //vbbox_ = CQChartsGeom::BBox(px, py - ph, px + pw, py);

  //---

  double x1, y1, x2, y2;

  view_->pixelToWindow(px     , py     , x1, y1);
  view_->pixelToWindow(px + pw, py + ph, x2, y2);

  pbbox_ = CQChartsGeom::BBox(x1, y2, x2, y1);

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

  double dth = (bs - fm.height())/2;

  for (int i = 0; i < n; ++i) {
    double py2 = py1 + bs + 2;

    CQChartsPlot *plot = view_->plot(i);

    bool checked = plot->isVisible();

    //---

    drawCheckBox(painter, px1, (py1 + py2)/2.0 - bs/2.0, bs, checked);

    //---

    painter->setPen(interpTextColor(0, 1));

    QString name = plot->keyText();

    double px2 = px1 + bs + margin();

    //double tw = fm.width(name);

    painter->drawText(px2, py1 + fm.ascent() + dth, name);

    //---

    // save view key item (plot) rect
    double x1, y1, x2, y2;

    view_->pixelToWindow(px     , py1, x1, y1);
    view_->pixelToWindow(px + pw, py2, x2, y2);

    CQChartsGeom::BBox prect(x1, y2, x2 - x1, y1 - y2);

    prects_.push_back(prect);

    //---

    py1 = py2;
  }
}

void
CQChartsViewKey::
drawCheckBox(QPainter *painter, double x, double y, int bs, bool checked) const
{
  QImage cimage(QSize(bs, bs), QImage::Format_ARGB32);

  cimage.fill(QColor(0,0,0,0));

  //QRectF qrect(x, y, bs, bs);
  QRectF qrect(0, 0, bs, bs);

  QStylePainter spainter(&cimage, view_);

  spainter.setPen(interpTextColor(0, 1));

  QStyleOptionButton opt;

  opt.initFrom(view_);

  opt.rect = qrect.toRect();

  opt.state |= (checked ? QStyle::State_On : QStyle::State_Off);

  spainter.drawControl(QStyle::CE_CheckBox, opt);

  painter->drawImage(x, y, cimage);

  //painter->drawRect(qrect);
}

bool
CQChartsViewKey::
isInside(const CQChartsGeom::Point &w) const
{
  return pbbox_.inside(w);
}

void
CQChartsViewKey::
selectPress(const CQChartsGeom::Point &w, CQChartsSelMod selMod)
{
  int n = std::min(view_->numPlots(), int(prects_.size()));

  for (int i = 0; i < n; ++i) {
    if (! prects_[i].inside(w))
      continue;

    if      (pressBehavior() == CQChartsKey::PressBehavior::SHOW)
      doShow(i, selMod);
    else if (pressBehavior() == CQChartsKey::PressBehavior::SELECT)
      doSelect(i, selMod);

    break;
  }

  redraw();
}

void
CQChartsViewKey::
doShow(int i, CQChartsSelMod selMod)
{
  CQChartsPlot *plot = view_->plot(i);

  if      (selMod == CQChartsSelMod::REPLACE) {
    CQChartsView::Plots plots;

    view_->getPlots(plots);

    for (auto &plot1 : plots)
      plot->setVisible(plot1 == plot);
  }
  else if (selMod == CQChartsSelMod::ADD)
    plot->setVisible(true);
  else if (selMod == CQChartsSelMod::REMOVE)
    plot->setVisible(false);
  else if (selMod == CQChartsSelMod::TOGGLE)
    plot->setVisible(! plot->isVisible());
}

void
CQChartsViewKey::
doSelect(int, CQChartsSelMod)
{
}

void
CQChartsViewKey::
redraw(bool /*queued*/)
{
  view_->update();
}

//------

CQChartsPlotKey::
CQChartsPlotKey(CQChartsPlot *plot) :
 CQChartsKey(plot)
{
  editHandles_ = new CQChartsEditHandles(plot, CQChartsEditHandles::Mode::MOVE);

  setBorder(true);

  clearItems();

  scrollData_.bar = new QScrollBar(Qt::Vertical, plot->view());

  scrollData_.bar->hide();

  connect(scrollData_.bar, SIGNAL(valueChanged(int)), this, SLOT(scrollSlot(int)));
}

CQChartsPlotKey::
~CQChartsPlotKey()
{
  delete editHandles_;

  for (auto &item : items_)
    delete item;
}

//---

void
CQChartsPlotKey::
scrollSlot(int v)
{
  scrollData_.pos = v;

  redraw(/*queued*/ false);
}

void
CQChartsPlotKey::
redraw(bool queued)
{
  if (queued) {
    plot_->queueDrawBackground();
    plot_->queueDrawForeground();
  }
  else {
    plot_->invalidateLayer(CQChartsBuffer::Type::BACKGROUND);
    plot_->invalidateLayer(CQChartsBuffer::Type::FOREGROUND);
  }
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
updatePosition(bool queued)
{
  plot_->updateKeyPosition();

  redraw(queued);
}

void
CQChartsPlotKey::
updateLocation(const CQChartsGeom::BBox &bbox)
{
  // calc key size
  QSizeF ks = calcSize();

  double xm = plot_->pixelToWindowWidth (margin());
  double ym = plot_->pixelToWindowHeight(margin());

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
  model->addProperty(path, this, "visible"      );
  model->addProperty(path, this, "selected"     );
  model->addProperty(path, this, "location"     );
  model->addProperty(path, this, "header"       );
  model->addProperty(path, this, "horizontal"   );
  model->addProperty(path, this, "autoHide"     );
  model->addProperty(path, this, "clipped"      );
  model->addProperty(path, this, "hiddenAlpha"  );
  model->addProperty(path, this, "pressBehavior");
  model->addProperty(path, this, "scrollHeight" );

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

  // get max number of rows
  int numRows = numRows_;

  // limit rows if height (and this scrolled) not defined
  if (! scrollData_.height.isSet())
    numRows = std::min(numRows, maxRows());

  //---

  // get size of each cell
  rowColCell_.clear();

  for (int r = 0; r < numRows; ++r) {
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

  // get spacing and margin in plot coords
  xs_ = plot_->pixelToWindowWidth (spacing());
  ys_ = plot_->pixelToWindowHeight(spacing());

  xm_ = plot_->pixelToWindowWidth (margin());
  ym_ = plot_->pixelToWindowHeight(margin());

  //---

  // get size of each row and column
  rowHeights_.clear();
  colWidths_ .clear();

  for (int r = 0; r < numRows; ++r) {
    for (int c = 0; c < numCols_; ++c) {
      rowHeights_[r] = std::max(rowHeights_[r], rowColCell_[r][c].height);
      colWidths_ [c] = std::max(colWidths_ [c], rowColCell_[r][c].width );
    }
  }

  //----

  // get header text size
  double tw = 0, th = 0;

  if (headerStr().length()) {
    QFont font = view()->plotFont(plot_, textFont());

    QFontMetricsF fm(font);

    double ptw = fm.width(headerStr());
    double pth = fm.height();

    tw = plot_->pixelToWindowWidth (ptw) + 2*xs_;
    th = plot_->pixelToWindowHeight(pth) + 2*ys_;
  }

  //---

  // update cell positions and sizes
  double y = -ym_;

  y -= th;

  for (int r = 0; r < numRows; ++r) {
    double x = xm_;

    double rh = rowHeights_[r] + 2*ys_;

    for (int c = 0; c < numCols_; ++c) {
      double cw = colWidths_[c] + 2*xs_;

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

  // calc size
  double w = 0, h = 0;

  for (int c = 0; c < numCols_; ++c) {
    Cell &cell = rowColCell_[0][c];

    w += cell.width;
  }

  w += 2*xm_;

  for (int r = 0; r < numRows; ++r) {
    Cell &cell = rowColCell_[r][0];

    h += cell.height;
  }

  h += 2*ym_ + th;

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
  editHandles_->setDragPos(p);

  location_ = CQChartsKeyLocation(CQChartsKeyLocation::Type::ABS_POS);

  setAbsPlotPosition(position_);

  return true;
}

bool
CQChartsPlotKey::
editMove(const CQChartsGeom::Point &p)
{
  const CQChartsGeom::Point &dragPos = editHandles_->dragPos();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  location_ = CQChartsKeyLocation::Type::ABS_POS;

  setAbsPlotPosition(absPlotPosition() + QPointF(dx, dy));

  editHandles_->setDragPos(p);

  updatePosition(/*queued*/false);

  return true;
}

bool
CQChartsPlotKey::
editMotion(const CQChartsGeom::Point &p)
{
  return editHandles_->selectInside(p);
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
draw(QPainter *painter) const
{
  if (! isVisible() || isEmpty()) {
    scrollData_.bar->hide();
    return;
  }

  //---

  CQChartsPlotKey *th = const_cast<CQChartsPlotKey *>(this);

  th->doLayout();

  //---

  // calc plot bounding box
  double x = position_.x(); // left
  double y = position_.y(); // top

  double w = size_.width ();
  double h = size_.height();

  //---

  double psw = 13.0;

  double sw = plot_->pixelToWindowWidth(psw);
  double sh = h;

  if (scrollData_.height.isSet()) {
    sh = plot_->lengthPlotHeight(scrollData_.height.length());

    scrollData_.scrolled = (sh < h);

    if (scrollData_.scrolled) {
      psw = 13.0;

      double scrollHeight = sh - 2*ym_;

      for (int i = 0; i < numRows_; ++i) {
        scrollHeight -= rowHeights_[i] + 2*ys_;

        if (scrollHeight <= 0)
          break;
      }

      sh -= scrollHeight;
    }
  }
  else
    scrollData_.scrolled = false;

  wbbox_ = CQChartsGeom::BBox(x, y - sh, x + w + sw, y);

  //---

  // calc pixel bounding box
  double px1, py1, px2, py2;

  plot_->windowToPixel(x         , y     , px1, py2);
  plot_->windowToPixel(x + w + sw, y - sh, px2, py1);

  QRectF rect(px1, py2, px2 - px1, py1 - py2);

  double psh = py1 - py2;

  //---

  CQChartsGeom::BBox plotPixelRect = plot_->calcPlotPixelRect();

  pixelWidthExceeded_  = (rect.width () > plotPixelRect.getWidth ()*0.8);
  pixelHeightExceeded_ = (rect.height() > plotPixelRect.getHeight()*0.8);

  if (isAutoHide()) {
    if (pixelWidthExceeded_ || pixelHeightExceeded_)
      return;
  }

  //---

  CQChartsGeom::BBox dataPixelRect = plot_->calcDataPixelRect();

  QRectF dataRect = CQChartsUtil::toQRect(dataPixelRect);
  QRectF clipRect = CQChartsUtil::toQRect(plotPixelRect);

  painter->save();

  sy_ = 0.0;

  if (scrollData_.scrolled) {
    scrollData_.bar->show();

    scrollData_.bar->move(px2 - psw, py2);
    scrollData_.bar->resize(psw - 1, psh - 1);

    //---

    // count number of rows in height
    int    scrollRows   = 0;
    double scrollHeight = sh - 2*ym_;

    for (int i = 0; i < numRows_; ++i) {
      ++scrollRows;

      scrollHeight -= rowHeights_[i] + 2*ys_;

      if (scrollHeight <= 0)
        break;
    }

    //---

    // update scroll bar
    int smax = std::max(numRows_ - scrollRows, 1);

    if (scrollData_.bar->maximum() != smax)
      scrollData_.bar->setRange(0, smax);

    if (scrollData_.bar->pageStep() != scrollRows)
      scrollData_.bar->setPageStep(scrollRows);

    if (scrollData_.bar->value() != scrollData_.pos)
      scrollData_.bar->setValue(scrollData_.pos);

    for (int i = 0; i < scrollData_.pos; ++i)
      sy_ += rowHeights_[i] + 2*ys_;

    wbbox_ = CQChartsGeom::BBox(x, y - sh, x + w, y);

    painter->setClipRect(QRect(px1, py2, px2 - px1 - psw, psh));
  }
  else {
    if (scrollData_.bar)
      scrollData_.bar->hide();

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
  }

  //---

  CQChartsBoxObj::draw(painter, rect);

  //---

  // draw items
  for (const auto &item : items_) {
    int col = item->col();

    if (isFlipped())
      col = numCols_ - 1 - col;

    Cell &cell = rowColCell_[item->row()][col];

    double x1 = cell.x;
    double y1 = cell.y + sy_;
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

    if (wbbox_.inside(bbox)) {
      item->draw(painter, bbox);

      if (plot_->showBoxes())
        plot_->drawWindowColorBox(painter, bbox);
    }
  }

  //---

  // draw header
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

  // draw box
  if (plot_->showBoxes())
    plot_->drawWindowColorBox(painter, wbbox_);

  //---

  painter->restore();
}

void
CQChartsPlotKey::
drawEditHandles(QPainter *painter) const
{
  assert(plot_->view()->mode() == CQChartsView::Mode::EDIT || isSelected());

  const_cast<CQChartsPlotKey *>(this)->editHandles_->setBBox(this->bbox());

  editHandles_->draw(painter);
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
CQChartsKeyItem(CQChartsPlotKey *key, int i, int n) :
 key_(key), i_(i), n_(n)
{
}

bool
CQChartsKeyItem::
selectPress(const CQChartsGeom::Point &, CQChartsSelMod selMod)
{
  if (isClickable()) {
    if      (key_->pressBehavior() == CQChartsKey::PressBehavior::SHOW)
      doShow(selMod);
    else if (key_->pressBehavior() == CQChartsKey::PressBehavior::SELECT)
      doSelect(selMod);
  }

  return true;
}

bool
CQChartsKeyItem::
selectMove(const CQChartsGeom::Point &)
{
  return isClickable();
}

void
CQChartsKeyItem::
doShow(CQChartsSelMod selMod)
{
  CQChartsPlot *plot = key_->plot();

  if      (selMod == CQChartsSelMod::REPLACE) {
    for (int i = 0; i < n_; ++i)
      plot->setSetHidden(i, i != i_);
  }
  else if (selMod == CQChartsSelMod::ADD)
    plot->setSetHidden(i_, false);
  else if (selMod == CQChartsSelMod::REMOVE)
    plot->setSetHidden(i_, true);
  else if (selMod == CQChartsSelMod::TOGGLE)
    plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->queueUpdateObjs();
}

void
CQChartsKeyItem::
doSelect(CQChartsSelMod)
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
 CQChartsKeyItem(plot->key(), i, n), plot_(plot), text_(text)
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
draw(QPainter *painter, const CQChartsGeom::BBox &rect) const
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
 CQChartsKeyItem(plot->key(), i, n), plot_(plot)
{
  setClickable(true);
}

QColor
CQChartsKeyColorBox::
interpBorderColor(int i, int n) const
{
  return plot_->charts()->interpColor(borderColor(), i, n);
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
draw(QPainter *painter, const CQChartsGeom::BBox &rect) const
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
