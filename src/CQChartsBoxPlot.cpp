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
updateRange(bool apply)
{
  if (whiskers_.empty()) {
    QAbstractItemModel *model = this->model();

    if (! model)
      return;

    int nr = model->rowCount(QModelIndex());

    // determine x data type

    bool isInt = true, isReal = true;

    for (int r = 0; r < nr; ++r) {
      QModelIndex xind = model->index(r, xColumn());

      if (isInt) {
        bool ok1;

        (void) CQChartsUtil::modelInteger(model, xind, ok1);

        if (ok1)
          continue;

        isInt = false;
      }

      if (isReal) {
        bool ok1;

        (void) CQChartsUtil::modelReal(model, xind, ok1);

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

    for (int r = 0; r < nr; ++r) {
      QModelIndex xind = model->index(r, xColumn());

      QModelIndex xind1 = normalizeIndex(xind);

      //---

      int setId = r;

      if      (isInt) {
        bool ok1;

        setId = CQChartsUtil::modelInteger(model, xind, ok1);
      }
      else if (isReal) {
        bool ok1;

        double real = CQChartsUtil::modelReal(model, xind, ok1);

        if (CQChartsUtil::isNaN(real))
          continue;

        auto p = valueSet_.find(real);

        if (p == valueSet_.end()) {
          int setId = valueSet_.size() + 1;

          p = valueSet_.insert(p, ValueSet::value_type(real, setId));

          setValue_[setId] = real;
        }

        setId = (*p).second;
      }
      else {
        bool ok1;

        QString s = CQChartsUtil::modelString(model, xind, ok1);

        auto p = nameSet_.find(s);

        if (p == nameSet_.end()) {
          int setId = nameSet_.size() + 1;

          p = nameSet_.insert(p, NameSet::value_type(s, setId));

          setName_[setId] = s;
        }

        setId = (*p).second;
      }

      //---

      QModelIndex yind = model->index(r, yColumn());

      bool ok2;

      double value = CQChartsUtil::modelReal(model, yind, ok2);

      if (! ok2) value = r;

      if (CQChartsUtil::isNaN(value))
        continue;

      CQChartsBoxPlotValue wv(value, xind1);

      whiskers_[setId].addValue(wv);
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

      const CQChartsBoxPlotWhisker &whisker = iwhisker.second;

      double min = whisker.rvalue(0);
      double max = whisker.rvalue(whisker.numValues() - 1);

      dataRange_.updateRange(pos - 0.5, min);
      dataRange_.updateRange(pos + 0.5, max);
    }

    ++i;
  }

  //---

  xAxis_->setColumn(xColumn());
  yAxis_->setColumn(yColumn());

  QAbstractItemModel *model = this->model();

  if (model) {
    QString xname = model->headerData(xColumn(), Qt::Horizontal).toString();
    QString yname = model->headerData(yColumn(), Qt::Horizontal).toString();

    xAxis_->setLabel(xname);
    yAxis_->setLabel(yname);
  }

  //---

  if (apply)
    applyDataRange();
}

void
CQChartsBoxPlot::
initObjs()
{
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

      const CQChartsBoxPlotWhisker &whisker = iwhisker.second;

      //----

      CBBox2D rect(pos - 0.10, whisker.lower(), pos + 0.10, whisker.upper());

      CQChartsBoxPlotObj *boxObj = new CQChartsBoxPlotObj(this, rect, pos, whisker, i, n);

      boxObj->setId(QString("%1:%2:%3").arg(pos).arg(whisker.lower()).arg(whisker.upper()));

      addPlotObject(boxObj);
    }

    ++i;
  }

  //---

  resetKeyItems();
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
                   const CQChartsBoxPlotWhisker &whisker, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), pos_(pos), whisker_(whisker), i_(i), n_(n)
{
}

void
CQChartsBoxPlotObj::
mousePress(const CPoint2D &)
{
  plot_->beginSelect();

  for (auto value : whisker_.values()) {
    plot_->addSelectIndex(value.ind.row(), plot_->xColumn(), value.ind.parent());
    plot_->addSelectIndex(value.ind.row(), plot_->yColumn(), value.ind.parent());
  }

  plot_->endSelect();
}

void
CQChartsBoxPlotObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  QFontMetricsF fm(plot_->view()->font());

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

    plot_->windowToPixel(pos_, whisker_.rvalue(o), px1, py1);

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

  plot->updateObjs();

  return true;
}

QBrush
CQChartsBoxKeyColor::
fillBrush() const
{
  QColor c = CQChartsKeyColorBox::fillBrush().color();

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
