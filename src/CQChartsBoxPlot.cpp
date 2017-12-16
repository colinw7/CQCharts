#include <CQChartsBoxPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsRoundedPolygon.h>
#include <QPainter>

CQChartsBoxPlotType::
CQChartsBoxPlotType()
{
  addParameters();
}

void
CQChartsBoxPlotType::
addParameters()
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
  boxObj_  = new CQChartsBoxObj(this);
  textObj_ = new CQChartsTextBoxObj(this);

  CQChartsPaletteColor bg(CQChartsPaletteColor::Type::PALETTE);

  boxObj_->setBackgroundColor(bg);

  whiskerColor_ = CQChartsPaletteColor(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  addAxes();

  addKey();

  addTitle();

  xAxis_->setIntegral(true);
}

CQChartsBoxPlot::
~CQChartsBoxPlot()
{
  delete boxObj_;
  delete textObj_;
}

QString
CQChartsBoxPlot::
boxColorStr() const
{
  return boxObj_->backgroundColorStr();
}

void
CQChartsBoxPlot::
setBoxColorStr(const QString &s)
{
  boxObj_->setBackgroundColorStr(s);

  update();
}

QColor
CQChartsBoxPlot::
interpBoxColor(int i, int n) const
{
  return boxObj_->interpBackgroundColor(i, n);
}

QString
CQChartsBoxPlot::
borderColorStr() const
{
  return boxObj_->borderColorStr();
}

void
CQChartsBoxPlot::
setBorderColorStr(const QString &s)
{
  boxObj_->setBorderColorStr(s);

  update();
}

QColor
CQChartsBoxPlot::
interpBorderColor(int i, int n) const
{
  return boxObj_->interpBorderColor(i, n);
}

double
CQChartsBoxPlot::
borderAlpha() const
{
  return boxObj_->borderAlpha();
}

void
CQChartsBoxPlot::
setBorderAlpha(double r)
{
  boxObj_->setBorderAlpha(r);

  update();
}

double
CQChartsBoxPlot::
borderWidth() const
{
  return boxObj_->borderWidth();
}

void
CQChartsBoxPlot::
setBorderWidth(double r)
{
  boxObj_->setBorderWidth(r);

  update();
}

double
CQChartsBoxPlot::
cornerSize() const
{
  return boxObj_->borderCornerSize();
}

void
CQChartsBoxPlot::
setCornerSize(double r)
{
  boxObj_->setBorderCornerSize(r);

  update();
}

QString
CQChartsBoxPlot::
whiskerColorStr() const
{
  return whiskerColor_.colorStr();
}

void
CQChartsBoxPlot::
setWhiskerColorStr(const QString &s)
{
  whiskerColor_.setColorStr(s);

  update();
}

QColor
CQChartsBoxPlot::
interpWhiskerColor(int i, int n) const
{
  return whiskerColor_.interpColor(this, i, n);
}

QString
CQChartsBoxPlot::
textColorStr() const
{
  return textObj_->colorStr();
}

void
CQChartsBoxPlot::
setTextColorStr(const QString &s)
{
  textObj_->setColorStr(s);

  update();
}

QColor
CQChartsBoxPlot::
interpTextColor(int i, int n) const
{
  return textObj_->interpColor(i, n);
}

const QFont &
CQChartsBoxPlot::
font() const
{
  return textObj_->font();
}

void
CQChartsBoxPlot::
setFont(const QFont &f)
{
  textObj_->setFont(f);

  update();
}

//---

void
CQChartsBoxPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("", this, "boxColor"   );
  addProperty("", this, "borderColor");
  addProperty("", this, "borderAlpha");
  addProperty("", this, "borderWidth");
  addProperty("", this, "cornerSize" );
  addProperty("", this, "textColor"  );
  addProperty("", this, "font"       );
  addProperty("", this, "textMargin" );
  addProperty("", this, "symbolSize" );
}

void
CQChartsBoxPlot::
updateRange(bool apply)
{
  updateWhiskers();

  //---

  dataRange_.reset();

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
    bool ok;

    QString xname = CQChartsUtil::modelHeaderString(model, xColumn(), ok);
    QString yname = CQChartsUtil::modelHeaderString(model, yColumn(), ok);

    xAxis_->setLabel(xname);
    yAxis_->setLabel(yname);
  }

  //---

  if (apply)
    applyDataRange();
}

void
CQChartsBoxPlot::
updateWhiskers()
{
  whiskers_.clear();

  //---

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

    int setId;

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
        int setId1 = valueSet_.size() + 1;

        p = valueSet_.insert(p, ValueSet::value_type(real, setId1));

        setValue_[setId1] = real;
      }

      setId = (*p).second;
    }
    else {
      bool ok1;

      QString s = CQChartsUtil::modelString(model, xind, ok1);

      auto p = nameSet_.find(s);

      if (p == nameSet_.end()) {
        int setId1 = nameSet_.size() + 1;

        p = nameSet_.insert(p, NameSet::value_type(s, setId1));

        setName_[setId1] = s;
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

bool
CQChartsBoxPlot::
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return false;
  }

  //---

  if (! plotObjs_.empty())
    return false;

  //---

  int i = 0;
  int n = whiskers_.size();

  for (const auto &iwhisker : whiskers_) {
    bool hidden = isSetHidden(i);

    if (! hidden) {
      double pos = iwhisker.first;

      const CQChartsBoxPlotWhisker &whisker = iwhisker.second;

      //----

      CQChartsGeom::BBox rect(pos - 0.10, whisker.lower(), pos + 0.10, whisker.upper());

      CQChartsBoxPlotObj *boxObj = new CQChartsBoxPlotObj(this, rect, pos, whisker, i, n);

      addPlotObject(boxObj);
    }

    ++i;
  }

  //---

  resetKeyItems();

  //---

  return true;
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

bool
CQChartsBoxPlot::
probe(ProbeData &probeData) const
{
  if (probeData.x < dataRange_.xmin() + 0.5)
    probeData.x = dataRange_.xmin() + 0.5;

  if (probeData.x > dataRange_.xmax() - 0.5)
    probeData.x = dataRange_.xmax() - 0.5;

  probeData.x = std::round(probeData.x);

  return true;
}

void
CQChartsBoxPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

//------

CQChartsBoxPlotObj::
CQChartsBoxPlotObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect, double pos,
                   const CQChartsBoxPlotWhisker &whisker, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), pos_(pos), whisker_(whisker), i_(i), n_(n)
{
}

QString
CQChartsBoxPlotObj::
calcId() const
{
  return QString("%1:%2:%3").arg(pos_).arg(whisker_.lower()).arg(whisker_.upper());
}

void
CQChartsBoxPlotObj::
addSelectIndex()
{
  for (auto value : whisker_.values()) {
    plot_->addSelectIndex(value.ind.row(), plot_->xColumn(), value.ind.parent());
    plot_->addSelectIndex(value.ind.row(), plot_->yColumn(), value.ind.parent());
  }
}

bool
CQChartsBoxPlotObj::
isIndex(const QModelIndex &ind) const
{
  for (auto value : whisker_.values()) {
    if (ind == value.ind)
      return true;
  }

  return false;
}

void
CQChartsBoxPlotObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  QFontMetricsF fm(plot_->font());

  double yf = (fm.ascent() - fm.descent())/2.0;

  //---

  double px1, py1, px2, py2, px3, py3, px4, py4, px5, py5;

  plot_->windowToPixel(pos_ - 0.10, whisker_.min   (), px1, py1);
  plot_->windowToPixel(pos_ - 0.10, whisker_.lower (), px2, py2);
  plot_->windowToPixel(pos_       , whisker_.median(), px3, py3);
  plot_->windowToPixel(pos_ + 0.10, whisker_.upper (), px4, py4);
  plot_->windowToPixel(pos_ + 0.10, whisker_.max   (), px5, py5);

  //---

  QColor whiskerColor = plot_->interpWhiskerColor(0, 1);
  double whiskerWidth = 0.0;

  //---

  // draw extent line
  painter->setPen(QPen(whiskerColor, whiskerWidth, Qt::SolidLine));

  painter->drawLine(QPointF(px3, py1), QPointF(px3, py5));

  //---

  // draw lower/upper horizontal lines
  painter->setPen(QPen(whiskerColor, whiskerWidth, Qt::SolidLine));

  painter->drawLine(QPointF(px2, py1), QPointF(px4, py1));
  painter->drawLine(QPointF(px2, py5), QPointF(px4, py5));

  //---

  // draw box
  QRectF rect(px2, py2, px4 - px2, py4 - py2);

  QColor boxColor = plot_->interpBoxColor   (i_, n_);
  QBrush brush(boxColor);

  QColor borderColor = plot_->interpBorderColor(i_, n_);

  borderColor.setAlphaF(plot_->borderAlpha());

  QPen pen(borderColor);

  pen.setWidthF(plot_->borderWidth());

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setBrush(brush);
  painter->setPen  (pen);

  CQChartsRoundedPolygon::draw(painter, rect, plot_->cornerSize());

  //---

  // draw median line
  painter->setPen(QPen(whiskerColor, whiskerWidth, Qt::SolidLine));

  painter->drawLine(QPointF(px2, py3), QPointF(px4, py3));

  //---

  // draw labels
  double margin = plot_->textMargin();

  painter->setFont(plot_->font());

  QColor textColor = plot_->interpTextColor(0, 1);

  painter->setPen(textColor);

  QString ustr = QString("%1").arg(whisker_.upper ());
  QString lstr = QString("%1").arg(whisker_.lower ());
  QString mstr = QString("%1").arg(whisker_.median());
  QString strl = QString("%1").arg(whisker_.min   ());
  QString strh = QString("%1").arg(whisker_.max   ());

  painter->drawText(QPointF(px2 - margin - fm.width(ustr), py4 + yf), ustr);
  painter->drawText(QPointF(px2 - margin - fm.width(lstr), py2 + yf), lstr);
  painter->drawText(QPointF(px4 + margin                 , py3 + yf), mstr);
  painter->drawText(QPointF(px4 + margin                 , py1 + yf), strl);
  painter->drawText(QPointF(px4 + margin                 , py5 + yf), strh);

  //---

  // draw whiskers
  painter->setPen(QPen(whiskerColor, whiskerWidth, Qt::SolidLine));

  painter->setBrush(brush);
  painter->setPen  (pen);

  double symbolSize = plot_->symbolSize();

  for (auto o : whisker_.outliers()) {
    double px1, py1;

    plot_->windowToPixel(pos_, whisker_.rvalue(o), px1, py1);

    QRectF rect(px1 - symbolSize, py1 - symbolSize, 2*symbolSize, 2*symbolSize);

    painter->drawEllipse(rect);
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
mousePress(const CQChartsGeom::Point &)
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
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

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
interpTextColor(int i, int n) const
{
  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  QColor c = CQChartsKeyText::interpTextColor(i, n);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}
