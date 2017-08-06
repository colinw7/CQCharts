#include <CQChartsXYPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsXYPlot::
CQChartsXYPlot(QAbstractItemModel *model) :
 CQChartsPlot(nullptr, model)
{
  addAxes();

  xAxis_->setColumn(xColumn_);
  yAxis_->setColumn(yColumn_);

  addProperty("columns"  , this, "xColumn"       , "x"    );
  addProperty("columns"  , this, "yColumn"       , "y"    );
  addProperty("columns"  , this, "nameColumn"    , "name" );
  addProperty("bivariate", this, "bivariate"              );
  addProperty("points"   , this, "points"        , "shown");
  addProperty("points"   , this, "pointsColor"   , "color");
  addProperty("lines"    , this, "lines"         , "shown");
  addProperty("lines"    , this, "linesColor"    , "color");
  addProperty("fillUnder", this, "fillUnder"     , "shown");
  addProperty("fillUnder", this, "fillUnderColor", "color");
}

void
CQChartsXYPlot::
updateRange()
{
  int n = model_->rowCount(QModelIndex());

  dataRange_.reset();

  for (int i = 0; i < n; ++i) {
    double x = CQChartsUtil::modelReal(model_, i, xColumn_);

    if (isBivariate()) {
      for (const auto &ycol : yColumns()) {
        double y = CQChartsUtil::modelReal(model_, i, ycol);

        dataRange_.updateRange(x, y);
      }
    }
    else {
      double y = CQChartsUtil::modelReal(model_, i, yColumn_);

      dataRange_.updateRange(x, y);
    }
  }

  if (! dataRange_.isSet()) {
    dataRange_.updateRange(0, 0);
    dataRange_.updateRange(1, 1);
  }

  displayRange_.setWindowRange(dataRange_.xmin(), dataRange_.ymin(),
                               dataRange_.xmax(), dataRange_.ymax());

  if (xAxis_) {
    xAxis_->setRange(dataRange_.xmin(), dataRange_.xmax());
    yAxis_->setRange(dataRange_.ymin(), dataRange_.ymax());
  }
}

void
CQChartsXYPlot::
initObjs()
{
  if (! plotObjs_.empty())
    return;

  double sw = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  double sh = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  int n = model_->rowCount(QModelIndex());

  if (isBivariate()) {
    for (int i = 0; i < n; ++i) {
      double x = CQChartsUtil::modelReal(model_, i, xColumn_);

      std::vector<double> yVals;

      for (const auto &ycol : yColumns()) {
        double y = CQChartsUtil::modelReal(model_, i, ycol);

        yVals.push_back(y);
      }

      if (yVals.size() < 2)
        continue;

      double y1 = yVals[0];
      double y2 = yVals[1];

      CBBox2D bbox(x - sw/2, y1 - sh/2, x + sw/2, y2 + sh/2);

      CQChartsXYLineObj *lineObj = new CQChartsXYLineObj(this, bbox, x, y1, y2, 2, i);

      if (nameColumn_ >= 0) {
        QString name = CQChartsUtil::modelString(model_, i, nameColumn_);

        lineObj->setId(name);
      }
      else
        lineObj->setId(QString("%1").arg(i));

      addPlotObject(lineObj);
    }
  }
  else {
    for (int j = 0; j < numSets(); ++j) {
      int yColumn = (! yColumns_.empty() ? yColumns_[j] : yColumn_);

      QPolygonF poly;

      for (int i = 0; i < n; ++i) {
        double x = CQChartsUtil::modelReal(model_, i, xColumn_);
        double y = CQChartsUtil::modelReal(model_, i, yColumn);

        QString name;

        if (nameColumn_ >= 0)
          name = CQChartsUtil::modelString(model_, i, nameColumn_);

        CBBox2D bbox(x - sw/2, y - sh/2, x + sw/2, y + sh/2);

        CQChartsXYPointObj *pointObj = new CQChartsXYPointObj(this, bbox, x, y, 2, i);

        if (name.length())
          pointObj->setId(name);
        else
          pointObj->setId(QString("%1:%2").arg(i).arg(j));

        addPlotObject(pointObj);

        //---

        if (i == 0)
          poly << QPointF(x, dataRange_.ymin());

        poly << QPointF(x, y);

        if (i == n - 1)
          poly << QPointF(x, dataRange_.ymin());
      }

      //---

      CBBox2D bbox = CQUtil::fromQRect(poly.boundingRect());

      CQChartsXYPolygonObj *polyObj = new CQChartsXYPolygonObj(this, bbox, poly, j);

      polyObj->setId(QString("%1").arg(j));

      addPlotObject(polyObj);
    }
  }
}

int
CQChartsXYPlot::
numSets() const
{
  if (yColumns_.empty())
    return 1;

  return yColumns_.size();
}

void
CQChartsXYPlot::
paintEvent(QPaintEvent *)
{
  initObjs();

  //---

  QPainter p(this);

  p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  p.fillRect(rect(), background());

  //---

  int n = model_->rowCount(QModelIndex());

  if (isBivariate()) {
    for (const auto &plotObj : plotObjs_)
      plotObj->draw(&p);
  }
  else {
    if (isLines()) {
      double px1 = 0, py1 = 0, px2 = 0, py2;

      for (int j = 0; j < numSets(); ++j) {
        int yColumn = (! yColumns_.empty() ? yColumns_[j] : yColumn_);

        for (int i = 0; i < n; ++i) {
          double x = CQChartsUtil::modelReal(model_, i, xColumn_);
          double y = CQChartsUtil::modelReal(model_, i, yColumn );

          windowToPixel(x, y, px2, py2);

          if (i > 0) {
            p.setPen(CQUtil::toQPen(lineData_.pen));

            p.drawLine(px1, py1, px2, py2);
          }

          px1 = px2;
          py1 = py2;
        }
      }
    }

    //---

    for (const auto &plotObj : plotObjs_)
      plotObj->draw(&p);
  }

  drawAxes(&p);
}

//------

CQChartsXYLineObj::
CQChartsXYLineObj(CQChartsXYPlot *plot, const CBBox2D &rect, double x,
                  double y1, double y2, double s, int ind) :
 CQChartsPlotObj(rect), plot_(plot), x_(x), y1_(y1), y2_(y2), s_(s), ind_(ind)
{
}

bool
CQChartsXYLineObj::
inside(const CPoint2D &p) const
{
  double px, py1, py2;

  plot_->windowToPixel(x_, y1_, px, py1);
  plot_->windowToPixel(x_, y2_, px, py2);

  CBBox2D pbbox(px - s_, py1 - s_, px + s_, py2 + s_);

  CPoint2D pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsXYLineObj::
draw(QPainter *p)
{
  if (plot_->isLines()) {
    double px, py1, py2;

    plot_->windowToPixel(x_, y1_, px, py1);
    plot_->windowToPixel(x_, y2_, px, py2);

    p->setBrush(CQUtil::toQBrush(plot_->fillUnderBrush()));

    if (isInside())
      p->setPen(Qt::red);
    else
      p->setPen(p->brush().color());

    p->drawLine(px, py1, px, py2);
  }

  if (plot_->isPoints()) {
    double px, py1, py2;

    plot_->windowToPixel(x_, y1_, px, py1);
    plot_->windowToPixel(x_, y2_, px, py2);

    if (isInside())
      p->setPen(Qt::red);
    else
      p->setPen(CQUtil::toQPen(plot_->pointsPen()));

    p->drawLine(px - s_, py1 - s_, px + s_, py1 + s_);
    p->drawLine(px - s_, py1 + s_, px + s_, py1 - s_);

    p->drawLine(px - s_, py2 - s_, px + s_, py2 + s_);
    p->drawLine(px - s_, py2 + s_, px + s_, py2 - s_);
  }
}

//------

CQChartsXYPointObj::
CQChartsXYPointObj(CQChartsXYPlot *plot, const CBBox2D &rect, double x,
                   double y, double s, int ind) :
 CQChartsPlotObj(rect), plot_(plot), x_(x), y_(y), s_(s), ind_(ind)
{
}

bool
CQChartsXYPointObj::
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
CQChartsXYPointObj::
draw(QPainter *p)
{
  if (plot_->isPoints()) {
    double px, py;

    plot_->windowToPixel(x_, y_, px, py);

    if (isInside())
      p->setPen(Qt::red);
    else
      p->setPen(CQUtil::toQPen(plot_->pointsPen()));

    p->drawLine(px - s_, py - s_, px + s_, py + s_);
    p->drawLine(px + s_, py - s_, px - s_, py + s_);
  }
}

//------

CQChartsXYPolygonObj::
CQChartsXYPolygonObj(CQChartsXYPlot *plot, const CBBox2D &rect, const QPolygonF &poly, int ind) :
 CQChartsPlotObj(rect), plot_(plot), poly_(poly), ind_(ind)
{
}

bool
CQChartsXYPolygonObj::
inside(const CPoint2D &p) const
{
  return poly_.containsPoint(CQUtil::toQPoint(p), Qt::OddEvenFill);
}

void
CQChartsXYPolygonObj::
draw(QPainter *p)
{
  if (plot_->isFillUnder()) {
    p->setPen(Qt::NoPen);

    QBrush fillBrush = CQUtil::toQBrush(plot_->fillUnderBrush());
    QColor fillColor = fillBrush.color();

    fillColor = plot_->objectColor(this, ind_, plot_->numSets(), fillColor);

    fillColor.setAlpha(128);

    fillBrush.setColor(fillColor);

    p->setBrush(fillBrush);

    QPolygonF poly;

    for (int i = 0; i < poly_.length(); ++i) {
      double px, py;

      plot_->windowToPixel(poly_[i].x(), poly_[i].y(), px, py);

      poly.push_back(QPointF(px, py));
    }

    p->drawPolygon(poly);
  }
}
