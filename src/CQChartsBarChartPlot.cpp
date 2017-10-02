#include <CQChartsBarChartPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQRoundedPolygon.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsBarChartPlotType::
CQChartsBarChartPlotType()
{
  addColumnParameter ("name" , "Name" , "nameColumn"  , "", 0);
  addColumnsParameter("value", "Value", "valueColumns", "", "1");

  addBoolParameter("stacked"   , "Stacked"   , "stacked"  , "optional");
  addBoolParameter("horizontal", "Horizontal", "horizontal", "optional");
}

//---

CQChartsBarChartPlot::
CQChartsBarChartPlot(CQChartsView *view, QAbstractItemModel *model) :
 CQChartsPlot(view, view->charts()->plotType("barchart"), model), dataLabel_(this)
{
  setLayerActive(Layer::FG, true);

  addAxes();

  addKey();

  addTitle();
}

QString
CQChartsBarChartPlot::
valueColumnsStr() const
{
  return CQChartsUtil::toString(valueColumns_);
}

bool
CQChartsBarChartPlot::
setValueColumnsStr(const QString &s)
{
  std::vector<int> valueColumns;

  if (! CQChartsUtil::fromString(s, valueColumns))
    return false;

  setValueColumns(valueColumns);

  return true;
}

void
CQChartsBarChartPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  QString strokeStr = "stroke";
  QString fillStr   = "fill";

  addProperty("columns", this, "nameColumn"  , "name"      );
  addProperty("columns", this, "valueColumn" , "value"     );
  addProperty("columns", this, "valueColumns", "valuesSet" );
  addProperty(""       , this, "stacked"                   );
  addProperty(""       , this, "horizontal"                );
  addProperty(""       , this, "margin"                    );
  addProperty(""       , this, "keySets"                   );
  addProperty(strokeStr, this, "border"      , "visible"   );
  addProperty(strokeStr, this, "borderColor" , "color"     );
  addProperty(strokeStr, this, "borderWidth" , "width"     );
  addProperty(strokeStr, this, "cornerSize"  , "cornerSize");
  addProperty(fillStr  , this, "fill"        , "visible"   );
  addProperty(fillStr  , this, "barColor"    , "color"     );

  dataLabel_.addProperties("dataLabel");
}

QString
CQChartsBarChartPlot::
barColorStr() const
{
  if (barColorPalette_)
    return "palette";

  return barColor_.name();
}

void
CQChartsBarChartPlot::
setBarColorStr(const QString &str)
{
  if (str == "palette") {
    barColorPalette_ = true;
  }
  else {
    barColorPalette_ = false;
    barColor_        = QColor(str);
  }
}

QColor
CQChartsBarChartPlot::
barColor(int i, int n) const
{
  if (barColorPalette_)
    return paletteColor(i, n);

  return barColor_;
}

void
CQChartsBarChartPlot::
updateRange()
{
  int n = model_->rowCount(QModelIndex());

  dataRange_.reset();

  if (! isHorizontal())
    dataRange_.updateRange(-0.5, 0);
  else
    dataRange_.updateRange(0, -0.5);

  //---

  xAxis_->clearTickLabels();
  yAxis_->clearTickLabels();

  valueSets_.clear();

  int ns = numSets();

  int numVisible = 0;

  for (int i = 0; i < n; ++i) {
    bool ok1;

    QString name = CQChartsUtil::modelString(model_, i, nameColumn_, ok1);

    double sum = 0.0;

    ValueSet *valueSet = getValueSet(name);

    if (ns > 1) {
      if (isKeySets()) {
        if (isSetHidden(i))
          continue;

        int numVisible1 = 0;

        for (int j = 0; j < ns; ++j) {
          int valueColumn = getSetColumn(j);

          bool ok2;

          double value = CQChartsUtil::modelReal(model_, i, valueColumn, ok2);

          if (! ok2) value = i;

          if (CQChartsUtil::isNaN(value))
            continue;

          valueSet->values.push_back(value);

          if (! isHorizontal()) {
            if (isStacked())
              dataRange_.updateRange(0, sum + value);
            else
              dataRange_.updateRange(0, value);
          }
          else {
            if (isStacked())
              dataRange_.updateRange(sum + value, 0);
            else
              dataRange_.updateRange(value, 0);
          }

          sum += value;

          ++numVisible1;
        }

        if (numVisible1 == 0)
          continue;

        //---

        if (! isHorizontal())
          xAxis_->setTickLabel(numVisible, name);
        else
          yAxis_->setTickLabel(numVisible, name);

        ++numVisible;
      }
      else {
        int numVisible1 = 0;

        for (int j = 0; j < ns; ++j) {
          if (isSetHidden(j))
            continue;

          int valueColumn = getSetColumn(j);

          bool ok2;

          double value = CQChartsUtil::modelReal(model_, i, valueColumn, ok2);

          if (! ok2) value = i;

          if (CQChartsUtil::isNaN(value))
            continue;

          valueSet->values.push_back(value);

          if (! isHorizontal()) {
            if (isStacked())
              dataRange_.updateRange(0, sum + value);
            else
              dataRange_.updateRange(0, value);
          }
          else {
            if (isStacked())
              dataRange_.updateRange(sum + value, 0);
            else
              dataRange_.updateRange(value, 0);
          }

          sum += value;

          ++numVisible1;
        }

        if (numVisible1 == 0)
          continue;

        //---

        if (! isHorizontal())
          xAxis_->setTickLabel(numVisible, name);
        else
          yAxis_->setTickLabel(numVisible, name);

        ++numVisible;
      }
    }
    else {
      if (isSetHidden(i))
        continue;

      int valueColumn = getSetColumn(0);

      bool ok2;

      double value = CQChartsUtil::modelReal(model_, i, valueColumn, ok2);

      if (! ok2) value = i;

      if (CQChartsUtil::isNaN(value))
        continue;

      valueSet->values.push_back(value);

      if (! isHorizontal()) {
        if (isStacked())
          dataRange_.updateRange(0, sum + value);
        else
          dataRange_.updateRange(0, value);
      }
      else {
        if (isStacked())
          dataRange_.updateRange(sum + value, 0);
        else
          dataRange_.updateRange(value, 0);
      }

      sum += value;

      //---

      if (! isHorizontal())
        xAxis_->setTickLabel(numVisible, name);
      else
        yAxis_->setTickLabel(numVisible, name);

      ++numVisible;
    }
  }

  //---

  valueNames_.clear();

  if (ns > 1) {
    for (int j = 0; j < ns; ++j) {
      int valueColumn = getSetColumn(j);

      QString name = model_->headerData(valueColumn, Qt::Horizontal).toString();

      valueNames_.push_back(name);
    }
  }

  //---

  if (! isHorizontal()) {
    if (numVisible > 0)
      dataRange_.updateRange(numVisible - 0.5, dataRange_.ymin());
    else
      dataRange_.updateRange(0.5, 1.0);
  }
  else {
    if (numVisible > 0)
      dataRange_.updateRange(dataRange_.xmin(), numVisible - 0.5);
    else
      dataRange_.updateRange(1.0, 0.5);
  }

  //---

  setXValueColumn(nameColumn_);
  setYValueColumn(valueColumn_);

  //---

  if (! isHorizontal())
    xAxis_->setColumn(nameColumn_);
  else
    yAxis_->setColumn(nameColumn_);

  QString xname = model_->headerData(nameColumn_ , Qt::Horizontal).toString();

  if (! isHorizontal())
    xAxis_->setLabel(xname);
  else
    yAxis_->setLabel(xname);

  //---

  if (! isHorizontal())
    yAxis_->setColumn(valueColumn_);
  else
    xAxis_->setColumn(valueColumn_);

  if (valueColumns_.size() <= 1) {
    QString yname = model_->headerData(valueColumn_, Qt::Horizontal).toString();

    if (! isHorizontal())
      yAxis_->setLabel(yname);
    else
      xAxis_->setLabel(yname);
  }
  else {
    if (! isHorizontal())
      yAxis_->setLabel("");
    else
      xAxis_->setLabel("");
  }

  //---

  applyDataRange();
}

int
CQChartsBarChartPlot::
numSets() const
{
  if (valueColumns_.empty())
    return 1;

  return valueColumns_.size();
}

int
CQChartsBarChartPlot::
getSetColumn(int i) const
{
  if (! valueColumns_.empty())
    return valueColumns_[i];
  else
    return valueColumn_;
}

CQChartsBarChartPlot::ValueSet *
CQChartsBarChartPlot::
getValueSet(const QString &name)
{
  int nv = numValueSets();

  for (int j = 0; j < nv; ++j) {
    if (valueSets_[j].name == name)
      return &valueSets_[j];
  }

  valueSets_.emplace_back(name);

  return &valueSets_.back();
}

void
CQChartsBarChartPlot::
initObjs(bool force)
{
  if (force) {
    clearPlotObjects();

    dataRange_.reset();
  }

  //---

  if (! dataRange_.isSet()) {
    xAxis_->setIntegral           (! isHorizontal());
  //xAxis_->setDataLabels         (! isHorizontal());
    xAxis_->setMinorTicksDisplayed(  isHorizontal());

    yAxis_->setIntegral           (  isHorizontal());
  //yAxis_->setDataLabels         (  isHorizontal());
    yAxis_->setMinorTicksDisplayed(! isHorizontal());

    //---

    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //---

  // start at px1 - bar width
  double bx = -0.5;

  int ns = numSets();

  int nv = numValueSets();

  for (int j = 0; j < nv; ++j) {
    const ValueSet &valueSet = valueSets_[j];

    QString setName = valueSet.name;

    //---

    int numVisible = 0;

    if (ns > 1) {
      if (isKeySets()) {
        if (isSetHidden(j))
          continue;

        numVisible = ns;
      }
      else {
        for (int i = 0; i < ns; ++i) {
          if (isSetHidden(i))
            continue;

          ++numVisible;
        }

        if (! numVisible)
          continue;
      }
    }
    else {
      if (isSetHidden(j))
        continue;

      numVisible = 1;
    }

    //---

    double bx1 = bx;
    double bw1 = (isStacked() ? 1.0 : 1.0/numVisible);

    double sum = 0.0;

    if (ns > 1) {
      if (isKeySets()) {
        if (isSetHidden(j))
          continue;

        int numVisible1 = 0;

        for (int i = 0; i < ns; ++i) {
          double value = valueSet.values[numVisible1];

          double value1 = value + sum;

          //---

          // create bar rect
          CBBox2D brect;

          if (! isHorizontal()) {
            if (isStacked())
              brect = CBBox2D(bx, sum, bx + 1.0, value1);
            else
              brect = CBBox2D(bx1, 0.0, bx1 + bw1, value);
          }
          else {
            if (isStacked())
              brect = CBBox2D(sum, bx, value1, bx + 1.0);
            else
              brect = CBBox2D(0.0, bx1, value, bx1 + bw1);
          }

          CQChartsBarChartObj *barObj = new CQChartsBarChartObj(this, brect, i, ns, j, nv, value);

          QString valueName = valueNames_[i];

          QString valueStr = this->valueStr(value);

          barObj->setId(QString("%1:%2:%3").arg(setName).arg(valueName).arg(valueStr));

          addPlotObject(barObj);

          //---

          bx1 += bw1;
          sum += value;

          ++numVisible1;
        }
      }
      else {
        int numVisible1 = 0;

        for (int i = 0; i < ns; ++i) {
          if (isSetHidden(i))
            continue;

          //---

          double value = valueSet.values[numVisible1];

          double value1 = value + sum;

          //---

          // create bar rect
          CBBox2D brect;

          if (! isHorizontal()) {
            if (isStacked())
              brect = CBBox2D(bx, sum, bx + 1.0, value1);
            else
              brect = CBBox2D(bx1, 0.0, bx1 + bw1, value);
          }
          else {
            if (isStacked())
              brect = CBBox2D(sum, bx, value1, bx + 1.0);
            else
              brect = CBBox2D(0.0, bx1, value, bx1 + bw1);
          }

          CQChartsBarChartObj *barObj = new CQChartsBarChartObj(this, brect, i, ns, j, nv, value);

          QString valueName = valueNames_[i];

          QString valueStr = this->valueStr(value);

          barObj->setId(QString("%1:%2:%3").arg(setName).arg(valueName).arg(valueStr));

          addPlotObject(barObj);

          //---

          bx1 += bw1;
          sum += value;

          ++numVisible1;
        }
      }
    }
    else {
      if (isSetHidden(j))
        continue;

      if (valueSet.values.empty())
        continue;

      double value = valueSet.values[0];

      double value1 = value + sum;

      //---

      // create bar rect
      CBBox2D brect;

      if (! isHorizontal()) {
        if (isStacked())
          brect = CBBox2D(bx, sum, bx + 1.0, value1);
        else
          brect = CBBox2D(bx1, 0.0, bx1 + bw1, value);
      }
      else {
        if (isStacked())
          brect = CBBox2D(sum, bx, value1, bx + 1.0);
        else
          brect = CBBox2D(0.0, bx1, value, bx1 + bw1);
      }

      CQChartsBarChartObj *barObj = new CQChartsBarChartObj(this, brect, 0, ns, j, nv, value);

      QString valueName = valueSet.name;

      QString valueStr = this->valueStr(value);

      barObj->setId(QString("%1:%2").arg(valueName).arg(valueStr));

      addPlotObject(barObj);

      //---

      bx1 += bw1;
      sum += value;
    }

    bx += 1.0;
  }

  //----

  keyObj_->clearItems();

  addKeyItems(keyObj_);
}

QString
CQChartsBarChartPlot::
valueStr(double v) const
{
  if (! isHorizontal())
    return yAxis_->getValueStr(v);
  else
    return xAxis_->getValueStr(v);
}

void
CQChartsBarChartPlot::
addKeyItems(CQChartsKey *key)
{
  int ns = numSets();

  if (ns > 1) {
    if (isKeySets()) {
      int nv = numValueSets();

      for (int i = 0; i < nv; ++i) {
        const ValueSet &valueSet = valueSets_[i];

        CQChartsBarKeyColor *color = new CQChartsBarKeyColor(this, i, nv);
        CQChartsBarKeyText  *text  = new CQChartsBarKeyText (this, i, valueSet.name);

        key->addItem(color, i, 0);
        key->addItem(text , i, 1);
      }
    }
    else {
      for (int i = 0; i < ns; ++i) {
        CQChartsBarKeyColor *color = new CQChartsBarKeyColor(this, i, ns);
        CQChartsBarKeyText  *text  = new CQChartsBarKeyText (this, i, valueNames_[i]);

        key->addItem(color, i, 0);
        key->addItem(text , i, 1);
      }
    }
  }
  else {
    int nv = numValueSets();

    for (int i = 0; i < nv; ++i) {
      const ValueSet &valueSet = valueSets_[i];

      CQChartsBarKeyColor *color = new CQChartsBarKeyColor(this, i, nv);
      CQChartsBarKeyText  *text  = new CQChartsBarKeyText (this, i, valueSet.name);

      key->addItem(color, i, 0);
      key->addItem(text , i, 1);
    }
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

int
CQChartsBarChartPlot::
numValueSets() const
{
  return valueSets_.size();
}

int
CQChartsBarChartPlot::
numSetValues() const
{
  if (! valueSets_.empty())
    return valueSets_[0].values.size();
  else
    return 0;
}

void
CQChartsBarChartPlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawParts(p);
}

void
CQChartsBarChartPlot::
drawDataLabel(QPainter *p, const QRectF &qrect, const QString &ystr)
{
  dataLabel_.draw(p, qrect, ystr);
}

//------

CQChartsBarChartObj::
CQChartsBarChartObj(CQChartsBarChartPlot *plot, const CBBox2D &rect,
                    int iset, int nset, int ival, int nval, double value) :
 CQChartsPlotObj(rect), plot_(plot), iset_(iset), nset_(nset), ival_(ival), nval_(nval),
 value_(value)
{
}

void
CQChartsBarChartObj::
draw(QPainter *p, const CQChartsPlot::Layer &layer)
{
  bool hidden = false;

  if (nset_ > 1) {
    if (plot_->isKeySets())
      hidden = plot_->isSetHidden(ival_);
    else
      hidden = plot_->isSetHidden(iset_);
  }
  else
    hidden = plot_->isSetHidden(ival_);

  if (hidden)
    return;

  //---

  p->save();

  CBBox2D prect;

  plot_->windowToPixel(rect(), prect);

  QRectF qrect = CQChartsUtil::toQRect(prect);

  if (layer == CQChartsPlot::Layer::MID) {
    double m = plot_->margin();

    if (prect.getWidth() > 3*m) {
      prect.setXMin(prect.getXMin() + m);
      prect.setXMax(prect.getXMax() - m);
    }

    // set pen
    if (plot_->isBorder()) {
      QPen pen(plot_->borderColor());

      pen.setWidth(plot_->borderWidth());

      p->setPen(pen);
    }
    else
      p->setPen(Qt::NoPen);

    // set fill
    if (plot_->isFill()) {
      QColor barColor;

      if (nset_ > 1) {
        if (plot_->isKeySets())
          barColor = plot_->objectStateColor(this, plot_->barColor(ival_, nval_));
        else
          barColor = plot_->objectStateColor(this, plot_->barColor(iset_, nset_));
      }
      else
        barColor = plot_->objectStateColor(this, plot_->barColor(ival_, nval_));

      p->setBrush(barColor);
    }
    else
      p->setBrush(Qt::NoBrush);

    //---

    CQRoundedPolygon::draw(p, qrect, plot_->cornerSize());
  }
  else {
    QString ystr = plot_->valueStr(value_);

    plot_->drawDataLabel(p, qrect, ystr);
  }

  p->restore();
}

//------

CQChartsBarKeyColor::
CQChartsBarKeyColor(CQChartsBarChartPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

bool
CQChartsBarKeyColor::
mousePress(const CPoint2D &)
{
  CQChartsBarChartPlot *plot = qobject_cast<CQChartsBarChartPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->initObjs(/*force*/true);

  plot->update();

  return true;
}

QColor
CQChartsBarKeyColor::
fillColor() const
{
  QColor c = CQChartsKeyColorBox::fillColor();

  CQChartsBarChartPlot *plot = qobject_cast<CQChartsBarChartPlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->bgColor(), 0.5);

  return c;
}

//------

CQChartsBarKeyText::
CQChartsBarKeyText(CQChartsBarChartPlot *plot, int i, const QString &text) :
 CQChartsKeyText(plot, text), i_(i)
{
}

QColor
CQChartsBarKeyText::
textColor() const
{
  QColor c = CQChartsKeyText::textColor();

  CQChartsBarChartPlot *plot = qobject_cast<CQChartsBarChartPlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, Qt::white, 0.5);

  return c;
}
