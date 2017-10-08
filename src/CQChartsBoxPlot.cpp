#include <CQChartsBoxPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQRoundedPolygon.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsBoxPlotType::
CQChartsBoxPlotType()
{
  addColumnParameter("x", "X", "xColumn", "", 0);
  addColumnParameter("y", "Y", "yColumn", "", 1);
}

CQChartsPlot *
CQChartsBoxPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsBoxPlot(view, model);
}

//---

CQChartsBoxPlot::
CQChartsBoxPlot(CQChartsView *view, const ModelP &model) :
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

  addProperty("", this, "boxColor"    );
  addProperty("", this, "cornerRadius");
}

void
CQChartsBoxPlot::
updateRange()
{
  if (whiskers_.empty()) {
    QAbstractItemModel *model = this->model();

    if (! model)
      return;

    int n = model->rowCount(QModelIndex());

    // determine x data type

    bool isInt = true, isReal = true;

    for (int i = 0; i < n; ++i) {
      if (isInt) {
        bool ok1;

        (void) CQChartsUtil::modelInteger(model, i, xColumn_, ok1);

        if (ok1)
          continue;

        isInt = false;
      }

      if (isReal) {
        bool ok1;

        (void) CQChartsUtil::modelReal(model, i, xColumn_, ok1);

        if (ok1)
          continue;

        isReal = false;
      }

      break;
    }

    valueSet_.clear();
    setValue_.clear();
    nameSet_ .clear();
    setName_ .clear();

    for (int i = 0; i < n; ++i) {
      int setId = i;

      if      (isInt) {
        bool ok1;

        setId = CQChartsUtil::modelInteger(model, i, xColumn_, ok1);
      }
      else if (isReal) {
        bool ok1;

        double r = CQChartsUtil::modelReal(model, i, xColumn_, ok1);

        if (CQChartsUtil::isNaN(r))
          continue;

        auto p = valueSet_.find(r);

        if (p == valueSet_.end()) {
          int setId = valueSet_.size() + 1;

          p = valueSet_.insert(p, ValueSet::value_type(r, setId));

          setValue_[setId] = r;
        }

        setId = (*p).second;
      }
      else {
        bool ok1;

        QString s = CQChartsUtil::modelString(model, i, xColumn_, ok1);

        auto p = nameSet_.find(s);

        if (p == nameSet_.end()) {
          int setId = nameSet_.size() + 1;

          p = nameSet_.insert(p, NameSet::value_type(s, setId));

          setName_[setId] = s;
        }

        setId = (*p).second;
      }

      //---

      bool ok2;

      double value = CQChartsUtil::modelReal(model, i, yColumn_, ok2);

      if (! ok2) value = i;

      if (CQChartsUtil::isNaN(value))
        continue;

      whiskers_[setId].addValue(value);
    }

    for (auto &iwhisker : whiskers_)
      iwhisker.second.init();
  }

  //---

  xAxis_->clearTickLabels();

  int i = 0;

  for (const auto &iwhisker : whiskers_) {
    bool hidden = isSetHidden(i);

    if (! hidden) {
      int setId = iwhisker.first;

      if      (! setValue_.empty())
        xAxis_->setTickLabel(setId, QString("%1").arg(setValue_[setId]));
      else if (! setName_.empty())
        xAxis_->setTickLabel(setId, setName_[setId]);

      //---

      double pos = setId;

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

  QAbstractItemModel *model = this->model();

  if (model) {
    QString xname = model->headerData(xColumn_, Qt::Horizontal).toString();
    QString yname = model->headerData(yColumn_, Qt::Horizontal).toString();

    xAxis_->setLabel(xname);
    yAxis_->setLabel(yname);
  }

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

  p->setBrush(boxColor);

  CQRoundedPolygon::draw(p, rect, plot_->cornerRadius());

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
