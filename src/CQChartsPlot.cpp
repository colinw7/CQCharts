#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsPlotObj.h>
#include <CQPropertyTree.h>
#include <CQUtil.h>
#include <CGradientPalette.h>

#include <QPainter>
#include <QMouseEvent>

CQChartsPlot::
CQChartsPlot(QWidget *parent, QAbstractItemModel *model) :
 parent_(parent), model_(model)
{
  setMouseTracking(true);

  displayRange_.setWindowRange(0, 0, 1, 1);

  expander_ = new CQChartsPlotExpander(this);

  setContentsMargins(0, 0, expander_->width(), 0);

  propertyTree_ = new CQPropertyTree(this);

  addProperty("", this, "background");
}

void
CQChartsPlot::
addAxes()
{
  xAxis_ = new CQChartsAxis(this, CQChartsAxis::DIR_HORIZONTAL, 0, 1);
  yAxis_ = new CQChartsAxis(this, CQChartsAxis::DIR_VERTICAL  , 0, 1);

  xAxis_->addProperties(propertyTree_, "X Axis");
  yAxis_->addProperties(propertyTree_, "Y Axis");
}

void
CQChartsPlot::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  propertyTree_->addProperty(path, object, name, alias);
}

void
CQChartsPlot::
addPlotObject(CQChartsPlotObj *obj)
{
  plotObjTree_.add(obj);

  plotObjs_.push_back(obj);
}

void
CQChartsPlot::
mousePressEvent(QMouseEvent *me)
{
  CPoint2D p = CQUtil::fromQPoint(QPointF(me->pos()));

  CPoint2D w;

  pixelToWindow(p, w);

  PlotObjTree::DataList dataList;

  objsAtPoint(w, dataList);

  for (auto obj : dataList) {
    std::cerr << obj->id().toStdString() << std::endl;
  }
}

void
CQChartsPlot::
mouseMoveEvent(QMouseEvent *me)
{
  CPoint2D p = CQUtil::fromQPoint(QPointF(me->pos()));

  CPoint2D w;

  pixelToWindow(p, w);

  PlotObjTree::DataList dataList;

  objsAtPoint(w, dataList);

  bool changed = false;

  auto resetInside = [&] (CQChartsPlotObj *obj) -> void {
    for (const auto &obj1 : dataList) {
      if (obj1 == obj) {
        if (! obj->isInside()) {
          obj->setInside(true);

          changed = true;
        }

        return;
      }
    }

    if (obj->isInside()) {
      obj->setInside(false);

      changed = true;
    }
  };

  plotObjTree_.process(resetInside);

  if (changed)
    update();
}

void
CQChartsPlot::
objsAtPoint(const CPoint2D &p, PlotObjTree::DataList &dataList1) const
{
  PlotObjTree::DataList dataList;

  plotObjTree_.dataAtPoint(p.x, p.y, dataList);

  for (const auto &obj : dataList) {
    if (obj->inside(p))
      dataList1.push_back(obj);
  }
}

void
CQChartsPlot::
resizeEvent(QResizeEvent *)
{
  updateGeometry();
}

void
CQChartsPlot::
updateGeometry()
{
  double m = std::min(width(), height())/10;

  displayRange_.setPixelRange(m, m, width() - m, height() - m);

  propertyTree_->setVisible(expander_->isExpanded());

  if (expander_->isExpanded()) {
    propertyTree_->move  (width() - propertyTree_->width() - expander_->width(), 0);
    propertyTree_->resize(propertyTree_->width(), height());
  }

  expander_->setVisible(true);

  expander_->move  (width() - expander_->width(), 0);
  expander_->resize(expander_->width(), height());
}

void
CQChartsPlot::
drawAxes(QPainter *painter)
{
  if (xAxis_ && xAxis_->getVisible())
    xAxis_->draw(this, painter);

  if (yAxis_ && yAxis_->getVisible())
    yAxis_->draw(this, painter);
}

QColor
CQChartsPlot::
objectColor(CQChartsPlotObj *obj, int i, int n, const QColor &def) const
{
  QColor c = paletteColor(i, n, def);

  if (obj->isInside())
    c = CQUtil::blendColors(c, Qt::white, 0.8);

  return c;
}

QColor
CQChartsPlot::
paletteColor(int i, int n, const QColor &def) const
{
  QColor c = def;

  if (palette()) {
    CRGBA rgba = palette()->getColor((1.0*i + 1.0)/(n + 1)).rgba();

    c = CQUtil::toQColor(rgba);
  }

  return c;
}

void
CQChartsPlot::
windowToPixel(double wx, double wy, double &px, double &py)
{
  displayRange_.windowToPixel(wx, wy, &px, &py);
}

void
CQChartsPlot::
pixelToWindow(double px, double py, double &wx, double &wy)
{
  displayRange_.pixelToWindow(px, py, &wx, &wy);
}

void
CQChartsPlot::
windowToPixel(const CPoint2D &w, CPoint2D &p)
{
  windowToPixel(w.x, w.y, p.x, p.y);
}

void
CQChartsPlot::
pixelToWindow(const CPoint2D &p, CPoint2D &w)
{
  pixelToWindow(p.x, p.y, w.x, w.y);
}

void
CQChartsPlot::
windowToPixel(const CBBox2D &wrect, CBBox2D &prect)
{
  double px1, py1, px2, py2;

  windowToPixel(wrect.getXMin(), wrect.getYMin(), px1, py2);
  windowToPixel(wrect.getXMax(), wrect.getYMax(), px2, py1);

  prect = CBBox2D(px1, py1, px2, py2);
}

void
CQChartsPlot::
pixelToWindow(const CBBox2D &prect, CBBox2D &wrect)
{
  double wx1, wy1, wx2, wy2;

  pixelToWindow(prect.getXMin(), prect.getYMin(), wx1, wy2);
  pixelToWindow(prect.getXMax(), prect.getYMax(), wx2, wy1);

  wrect = CBBox2D(wx1, wy1, wx2, wy2);
}

QSize
CQChartsPlot::
sizeHint() const
{
  return QSize(1000, 1000);
}

//---

CQChartsPlotExpander::
CQChartsPlotExpander(CQChartsPlot *plot) :
 QFrame(plot), plot_(plot)
{
  setObjectName("expander");

  setAutoFillBackground(true);

  setFixedWidth(8);
}

void
CQChartsPlotExpander::
mousePressEvent(QMouseEvent *)
{
  setExpanded(! isExpanded());

  plot_->updateGeometry();
}

void
CQChartsPlotExpander::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  int ym = height()/2;

  p.setBrush(QColor(0,0,0));

  int s = width();

  QPolygonF poly;

  if (! expanded_) {
    poly << QPoint(0, ym    );
    poly << QPoint(s, ym - s);
    poly << QPoint(s, ym + s);
  }
  else {
    poly << QPoint(s, ym    );
    poly << QPoint(0, ym - s);
    poly << QPoint(0, ym + s);
  }

  p.drawPolygon(poly);
}
