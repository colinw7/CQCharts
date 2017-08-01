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
  xAxis_ = new CQChartsAxis(this, CQChartsAxis::DIR_HORIZONTAL, 0, 1);
  yAxis_ = new CQChartsAxis(this, CQChartsAxis::DIR_VERTICAL  , 0, 1);

  xAxis_->setColumn(xColumn_);
  yAxis_->setColumn(yColumn_);

  addProperty("columns"  , this, "xColumn"       , "x");
  addProperty("columns"  , this, "yColumn"       , "y");
  addProperty("bivariate", this, "bivariate"     );
  addProperty("points"   , this, "points"        , "shown");
  addProperty("points"   , this, "pointsColor"   , "color");
  addProperty("lines"    , this, "lines"         , "shown");
  addProperty("lines"    , this, "linesColor"    , "color");
  addProperty("fillUnder", this, "fillUnder"     , "shown");
  addProperty("fillUnder", this, "fillUnderColor", "color");

  Q_PROPERTY(QColor pointsColor    READ pointsColor    WRITE setPointsColor   )
  Q_PROPERTY(bool   lines          READ isLines        WRITE setLines         )
  Q_PROPERTY(QColor linesColor     READ linesColor     WRITE setLinesColor    )
  Q_PROPERTY(bool   fillUnder      READ isFillUnder    WRITE setFillUnder     )
  Q_PROPERTY(QColor fillUnderColor READ fillUnderColor WRITE setFillUnderColor)

}

void
CQChartsXYPlot::
updateRange()
{
  QModelIndex ind;

  int n = model_->rowCount(ind);

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
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  double s = 4;

  painter.fillRect(rect(), Qt::white);

  QModelIndex ind;

  int n = model_->rowCount(ind);

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

      // draw value range
      double y1 = yVals[0];
      double y2 = yVals[1];

      double px1, py1, px2, py2;

      windowToPixel(x, y1, px1, py1);
      windowToPixel(x, y2, px2, py2);

      painter.setBrush(CQUtil::toQBrush(fillUnderData_.brush));

      painter.setPen(painter.brush().color());

      painter.drawLine(px1, py1, px2, py2);
    }
  }
  else {
    double pxmin, pymin, pxmax, pymax;

    windowToPixel(dataRange_.xmin(), dataRange_.ymin(), pxmin, pymin);
    windowToPixel(dataRange_.xmax(), dataRange_.ymax(), pxmax, pymax);

    QPolygonF poly;

    double px1 = 0, py1 = 0;

    for (int i = 0; i < n; ++i) {
      double x = CQChartsUtil::modelReal(model_, i, xColumn_);
      double y = CQChartsUtil::modelReal(model_, i, yColumn_);

      double px2, py2;

      windowToPixel(x, y, px2, py2);

      //---

      if (isFillUnder()) {
        if (i == 0)
          poly << QPointF(px2, pymin);

        poly << QPointF(px2, py2);

        if (i == n - 1)
          poly << QPointF(px2, pymin);
      }

      // draw cross symbol
      if (isPoints()) {
        painter.setPen(CQUtil::toQPen(pointData_.pen));

        painter.drawLine(px2 - s, py2 - s, px2 + s, py2 + s);
        painter.drawLine(px2 - s, py2 + s, px2 + s, py2 - s);
      }

      if (i > 0) {
        if (isLines()) {
          // connect points
          painter.setPen(CQUtil::toQPen(lineData_.pen));

          painter.drawLine(px1, py1, px2, py2);
        }
      }

      //---

      px1 = px2;
      py1 = py2;
    }

    if (isFillUnder()) {
      painter.setPen(Qt::NoPen);
      painter.setBrush(CQUtil::toQBrush(fillUnderData_.brush));

      painter.drawPolygon(poly);
    }
  }

  xAxis_->draw(this, &painter);
  yAxis_->draw(this, &painter);
}
