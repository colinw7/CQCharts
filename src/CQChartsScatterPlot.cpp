#include <CQChartsScatterPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsScatterPlot::
CQChartsScatterPlot(QAbstractItemModel *model) :
 CQChartsPlot(nullptr, model)
{
  addAxes();

  addKey();

  xAxis_->setColumn(xColumn_);
  yAxis_->setColumn(yColumn_);

  QString xname = model_->headerData(xColumn_, Qt::Horizontal).toString();
  QString yname = model_->headerData(yColumn_, Qt::Horizontal).toString();

  xAxis_->setLabel(xname);
  yAxis_->setLabel(yname);

  addProperty("columns", this, "nameColumn", "name");
  addProperty("columns", this, "xColumn"   , "x"   );
  addProperty("columns", this, "yColumn"   , "y"   );
}

void
CQChartsScatterPlot::
updateRange()
{
  int n = numRows();

  dataRange_.reset();

  for (int i = 0; i < n; ++i) {
    double x = CQChartsUtil::modelReal(model_, i, xColumn_);
    double y = CQChartsUtil::modelReal(model_, i, yColumn_);

    dataRange_.updateRange(x, y);
  }

  applyDataRange();
}

int
CQChartsScatterPlot::
numRows() const
{
  return model_->rowCount(QModelIndex());
}

int
CQChartsScatterPlot::
nameIndex(const QString &name) const
{
  int i = 0;

  for (const auto &nv : nameValues_) {
    if (nv.first == name)
      return i;

    ++i;
  }

  return 0;
}

void
CQChartsScatterPlot::
initObjs()
{
  if (! plotObjs_.empty())
    return;

  double sw = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  double sh = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  double s = 4;

  int n = numRows();

  for (int i = 0; i < n; ++i) {
    QString name = CQChartsUtil::modelString(model_, i, nameColumn_);
    double  x    = CQChartsUtil::modelReal  (model_, i, xColumn_   );
    double  y    = CQChartsUtil::modelReal  (model_, i, yColumn_   );

    nameValues_[name].push_back(QPointF(x, y));

    int ni = nameValues_[name].size() - 1;

    CBBox2D bbox(x - sw/2, y - sh/2, x + sw/2, y + sh/2);

    CQChartsScatterPointObj *pointObj =
      new CQChartsScatterPointObj(this, bbox, x, y, s, name, ni);

    pointObj->setId(QString("%1:%2:%3").arg(name).arg(x).arg(y));

    addPlotObject(pointObj);
  }

  //---

  key_->clearItems();

  int nv = nameValues_.size();

  int i = 0;

  for (const auto &nameValue: nameValues_) {
    const QString &name = nameValue.first;

    CQChartsKeyColorBox *color = new CQChartsKeyColorBox(this, i, nv);
    CQChartsKeyText     *text  = new CQChartsKeyText    (this, name);

    key_->addItem(color, i, 0);
    key_->addItem(text , i, 1);

    ++i;
  }
}

void
CQChartsScatterPlot::
paintEvent(QPaintEvent *)
{
  initObjs();

  //---

  QPainter p(this);

  p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  drawBackground(&p);

  //---

  for (const auto &plotObj : plotObjs_)
    plotObj->draw(&p);

  drawAxes(&p);

  //---

  drawKey(&p);
}

//------

CQChartsScatterPointObj::
CQChartsScatterPointObj(CQChartsScatterPlot *plot, const CBBox2D &rect, double x,
                        double y, double s, const QString &name, int ind) :
 CQChartsPlotObj(rect), plot_(plot), x_(x), y_(y), s_(s), name_(name), ind_(ind)
{
}

bool
CQChartsScatterPointObj::
inside(const CPoint2D &p) const
{
  double px, py;

  plot_->windowToPixel(x_, y_, px, py);

  CBBox2D pbbox(px - s_, py - s_, px + s_, py + s_);

  CPoint2D pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsScatterPointObj::
draw(QPainter *p)
{
  double px, py;

  plot_->windowToPixel(x_, y_, px, py);

  int i = plot_->nameIndex(name_);
  int n = plot_->nameValues().size();

  QColor color = plot_->objectColor(this, i, n, Qt::blue);

  p->setPen  (Qt::black);
  p->setBrush(color);

  QRectF erect(px - s_, py - s_, 2*s_, 2*s_);

  p->drawEllipse(erect);
}
