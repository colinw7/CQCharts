#include <CQChartsBarChartPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsPlotBarChart::
CQChartsPlotBarChart(QAbstractItemModel *model) :
 CQChartsPlot(nullptr, model)
{
  xAxis_ = new CQChartsAxis(this, CQChartsAxis::DIR_HORIZONTAL, 0, 1);
  yAxis_ = new CQChartsAxis(this, CQChartsAxis::DIR_VERTICAL  , 0, 1);
}

void
CQChartsPlotBarChart::
updateRange()
{
  QModelIndex ind;

  int n = model_->rowCount(ind);

  dataRange_.reset();

  for (int i = 0; i < n; ++i) {
    QModelIndex xind = model_->index(i, xColumn_);

    QString name = model_->data(xind).toString();

    double value = modelReal(i, yColumn_);

    ValueSet *valueSet = 0;

    for (size_t j = 0; j < valueSets_.size(); ++j) {
      if (valueSets_[j].name == name) {
        valueSet = &valueSets_[j];
        break;
      }
    }

    if (! valueSet) {
      valueSets_.emplace_back(name);

      valueSet = &valueSets_.back();
    }

    valueSet->values.push_back(value);

    dataRange_.updateRange(0, value);
  }

  dataRange_.updateRange(valueSets_.size(), dataRange_.ymin());

  displayRange_.setWindowRange(dataRange_.xmin(), dataRange_.ymin(),
                               dataRange_.xmax(), dataRange_.ymax());

  if (xAxis_) {
    xAxis_->setRange(dataRange_.xmin(), dataRange_.xmax());
    yAxis_->setRange(dataRange_.ymin(), dataRange_.ymax());
  }
}

void
CQChartsPlotBarChart::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QFontMetrics fm(font());

  p.fillRect(rect(), QBrush(QColor(255,255,255)));

  //---

#if 0
  int pxmin = getPixelRange().getXRange().min();
  int pymin = getPixelRange().getYRange().min();
  int pxmax = getPixelRange().getXRange().max();
  int pymax = getPixelRange().getYRange().max();

  p->fillRect(QRect(pxmin, pymin, pxmax - pxmin - 1, pymax - pymin - 1), QBrush(background()));
#endif

  double px1, py1, px2, py2;

  windowToPixel(0, 0.0, px1, py1);
  windowToPixel(1, 0.0, px2, py2);

  double pw = px2 - px1;
  double bw = std::max(1.0, pw - 2.0);

  //bars_.clear();

  QColor barColor = QColor(100,100,200);

  for (int j = 0; j <  int(valueSets_.size()); ++j) {
    const ValueSet &valueSet = valueSets_[j];

    QString setName = valueSet.name;

    windowToPixel(1, 0.0, px1, py1);

    double bx = px1 + 1 + j*(bw + 1);

    for (size_t i = 0; i < valueSet.values.size(); ++i) {
      double value = valueSet.values[i];

      windowToPixel(0.0, 0.0  , px1, py1);
      windowToPixel(0.0, value, px2, py2);

      QRectF rect(bx, py1, bw, py2 - py1);

      QBrush fill = barColor;

      p.fillRect(rect, fill);

      //QString tip = QString("%1: %2")..arg(setName).arg(value);

      //bars_[BarKey(i, j)] = Bar(rect, fill, tip);

      bx += pw;
    }
  }

  xAxis_->draw(this, &p);
  yAxis_->draw(this, &p);

#if 0
  for (auto legend : legends_)
    legend->draw(this, p);
#endif
}

double
CQChartsPlotBarChart::
modelReal(int row, int col) const
{
  QModelIndex ind = model_->index(row, col);

  return CQChartsUtil::toReal(model_->data(ind));
}
