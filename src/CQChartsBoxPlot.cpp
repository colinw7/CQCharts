#include <CQChartsBoxPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsBoxPlot::
CQChartsBoxPlot(QAbstractItemModel *model) :
 CQChartsPlot(nullptr, model)
{
  addAxes();

  xAxis_->setColumn(xColumn_);
  yAxis_->setColumn(yColumn_);

  xAxis_->setIntegral(true);

  addProperty("", this, "boxColor");
}

void
CQChartsBoxPlot::
init()
{
  whiskers_.clear();

  QModelIndex ind;

  int n = model_->rowCount(ind);

  for (int i = 0; i < n; ++i) {
    int    set   = CQChartsUtil::modelInteger(model_, i, xColumn_);
    double value = CQChartsUtil::modelReal   (model_, i, yColumn_);

    whiskers_[set].addValue(value);

    dataRange_.updateRange(set - 0.5, value);
    dataRange_.updateRange(set + 0.5, value);
  }

  if (! dataRange_.isSet()) {
    dataRange_.updateRange(0, 0);
    dataRange_.updateRange(1, 1);
  }

  displayRange_.setWindowRange(dataRange_.xmin(), dataRange_.ymin(),
                               dataRange_.xmax(), dataRange_.ymax());

  xAxis_->setRange(dataRange_.xmin(), dataRange_.xmax());
  yAxis_->setRange(dataRange_.ymin(), dataRange_.ymax());
}

void
CQChartsBoxPlot::
initObjs()
{
  if (! plotObjs_.empty())
    return;

  int i = 0;

  for (const auto &iwhisker : whiskers_) {
    double pos = iwhisker.first;

    const CBoxWhisker &whisker = iwhisker.second;

    //----

    CBBox2D rect(pos - 0.10, whisker.lower(), pos + 0.10, whisker.upper());

    CQChartsBoxObj *boxObj = new CQChartsBoxObj(this, rect, pos, whisker, i);

    boxObj->setId(QString("%1").arg(i));

    addPlotObject(boxObj);

    ++i;
  }
}

int
CQChartsBoxPlot::
numObjs() const
{
  return whiskers_.size();
}

void
CQChartsBoxPlot::
paintEvent(QPaintEvent *)
{
  initObjs();

  //---

  QPainter p(this);

  p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  p.fillRect(rect(), background());

  //---

  for (const auto &plotObj : plotObjs_)
    plotObj->draw(&p);

  drawAxes(&p);
}

//------

CQChartsBoxObj::
CQChartsBoxObj(CQChartsBoxPlot *plot, const CBBox2D &rect, double pos,
               const CBoxWhisker &whisker, int ind) :
 CQChartsPlotObj(rect), plot_(plot), pos_(pos), whisker_(whisker), ind_(ind)
{
}

void
CQChartsBoxObj::draw(QPainter *p)
{
  QFontMetrics fm(plot_->font());

  double yf = (fm.ascent() - fm.descent())/2.0;

  //---

  double px1, py1, px2, py2, px3, py3, px4, py4, px5, py5;

  plot_->windowToPixel(pos_ - 0.10, whisker_.min   (), px1, py1);
  plot_->windowToPixel(pos_ - 0.10, whisker_.lower (), px2, py2);
  plot_->windowToPixel(pos_       , whisker_.median(), px3, py3);
  plot_->windowToPixel(pos_ + 0.10, whisker_.upper (), px4, py4);
  plot_->windowToPixel(pos_ + 0.10, whisker_.max   (), px5, py5);

  p->setPen(QPen(QColor(0,0,0), 0.0, Qt::DashLine));

  p->drawLine(px3, py1, px3, py5);

  p->setPen(QPen(QColor(0,0,0), 0.0, Qt::SolidLine));

  p->drawLine(px2, py1, px4, py1);
  p->drawLine(px2, py5, px4, py5);

  QRectF rect(px2, py2, px4 - px2, py4 - py2);

  QColor boxColor = plot_->objectColor(this, ind_, plot_->numObjs(), plot_->boxColor());

  p->fillRect(rect, QBrush(boxColor));

  p->drawRect(rect);

  p->drawLine(px2, py3, px4, py3);

  QString ustr = QString("%1").arg(whisker_.upper ());
  QString lstr = QString("%1").arg(whisker_.lower ());
  QString mstr = QString("%1").arg(whisker_.median());
  QString strl = QString("%1").arg(whisker_.min   ());
  QString strh = QString("%1").arg(whisker_.max   ());

  p->drawText(px2 - 2 - fm.width(ustr), py4 + yf, ustr);
  p->drawText(px2 - 2 - fm.width(lstr), py2 + yf, lstr);
  p->drawText(px4 + 2                 , py3 + yf, mstr);
  p->drawText(px4 + 2                 , py1 + yf, strl);
  p->drawText(px4 + 2                 , py5 + yf, strh);

  for (auto o : whisker_.outliers()) {
    double px1, py1;

    plot_->windowToPixel(pos_, whisker_.value(o), px1, py1);

    QRectF rect(px1 - 4, py1 - 4, 8, 8);

    p->drawEllipse(rect);
  }
}
