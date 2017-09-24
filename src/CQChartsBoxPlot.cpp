#include <CQChartsBoxPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsBoxPlotType::
CQChartsBoxPlotType()
{
  addColumnParameter("x", "X", "xColumn", "", 0);
  addColumnParameter("y", "Y", "yColumn", "", 1);
}

//---

CQChartsBoxPlot::
CQChartsBoxPlot(CQChartsView *view, QAbstractItemModel *model) :
 CQChartsPlot(view, view->charts()->plotType("box"), model)
{
  addAxes();

  addKey();

  addTitle();

  xAxis_->setIntegral(true);
}

void
CQChartsBoxPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("", this, "boxColor");
}

void
CQChartsBoxPlot::
updateRange()
{
  if (whiskers_.empty()) {
    int n = model_->rowCount(QModelIndex());

    for (int i = 0; i < n; ++i) {
      bool ok1, ok2;

      int    set   = CQChartsUtil::modelInteger(model_, i, xColumn_, ok1);
      double value = CQChartsUtil::modelReal   (model_, i, yColumn_, ok2);

      if (! ok1) set   = i;
      if (! ok2) value = i;

      if (CQChartsUtil::isNaN(value))
        continue;

      whiskers_[set].addValue(value);
    }

    for (auto &iwhisker : whiskers_)
      iwhisker.second.init();
  }

  //---

  int i = 0;

  for (const auto &iwhisker : whiskers_) {
    bool hidden = isSetHidden(i);

    if (! hidden) {
      double pos = iwhisker.first;

      const CBoxWhisker &whisker = iwhisker.second;

      double min = whisker.value(0);
      double max = whisker.value(whisker.numValues() - 1);

      dataRange_.updateRange(pos - 0.5, min);
      dataRange_.updateRange(pos + 0.5, max);
    }

    ++i;
  }

  //---

  xAxis_->setColumn(xColumn_);
  yAxis_->setColumn(yColumn_);

  QString xname = model_->headerData(xColumn_, Qt::Horizontal).toString();
  QString yname = model_->headerData(yColumn_, Qt::Horizontal).toString();

  xAxis_->setLabel(xname);
  yAxis_->setLabel(yname);

  //---

  applyDataRange();
}

void
CQChartsBoxPlot::
initObjs(bool force)
{
  if (force) {
    clearPlotObjects();

    dataRange_.reset();
  }

  //---

  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //---

  int i = 0;
  int n = whiskers_.size();

  for (const auto &iwhisker : whiskers_) {
    bool hidden = isSetHidden(i);

    if (! hidden) {
      double pos = iwhisker.first;

      const CBoxWhisker &whisker = iwhisker.second;

      //----

      CBBox2D rect(pos - 0.10, whisker.lower(), pos + 0.10, whisker.upper());

      CQChartsBoxPlotObj *boxObj = new CQChartsBoxPlotObj(this, rect, pos, whisker, i, n);

      boxObj->setId(QString("%1:%2:%3").arg(pos).arg(whisker.lower()).arg(whisker.upper()));

      addPlotObject(boxObj);
    }

    ++i;
  }

  //---

  keyObj_->clearItems();

  addKeyItems(keyObj_);
}

void
CQChartsBoxPlot::
addKeyItems(CQChartsKey *key)
{
  int i = 0;
  int n = whiskers_.size();

  for (const auto &iwhisker : whiskers_) {
    double pos = iwhisker.first;

    QString name = QString("%1").arg(pos);

    CQChartsBoxKeyColor *color = new CQChartsBoxKeyColor(this, i, n);
    CQChartsBoxKeyText  *text  = new CQChartsBoxKeyText (this, i, name);

    key->addItem(color, i, 0);
    key->addItem(text , i, 1);

    ++i;
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsBoxPlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawParts(p);
}

//------

CQChartsBoxPlotObj::
CQChartsBoxPlotObj(CQChartsBoxPlot *plot, const CBBox2D &rect, double pos,
                   const CBoxWhisker &whisker, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), pos_(pos), whisker_(whisker), i_(i), n_(n)
{
}

void
CQChartsBoxPlotObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  QFontMetrics fm(plot_->view()->font());

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

  QColor boxColor = plot_->objectColor(this, i_, n_, plot_->boxColor());

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

//------

CQChartsBoxKeyColor::
CQChartsBoxKeyColor(CQChartsBoxPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

bool
CQChartsBoxKeyColor::
mousePress(const CPoint2D &)
{
  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->initObjs(/*force*/true);

  plot->update();

  return true;
}

QColor
CQChartsBoxKeyColor::
fillColor() const
{
  QColor c = CQChartsKeyColorBox::fillColor();

  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->bgColor(), 0.5);

  return c;
}

//------

CQChartsBoxKeyText::
CQChartsBoxKeyText(CQChartsBoxPlot *plot, int i, const QString &text) :
 CQChartsKeyText(plot, text), i_(i)
{
}

QColor
CQChartsBoxKeyText::
textColor() const
{
  QColor c = CQChartsKeyText::textColor();

  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->bgColor(), 0.5);

  return c;
}
