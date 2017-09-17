#include <CQChartsBarChartPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQRotatedText.h>
#include <CQUtil.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsBarChartPlot::
CQChartsBarChartPlot(CQChartsView *view, QAbstractItemModel *model) :
 CQChartsPlot(view, model), dataLabel_(this)
{
  addAxes();

  addKey();

  addTitle();

  xAxis_->setIntegral(true);
//xAxis_->setDataLabels(true);
  xAxis_->setMinorTicksDisplayed(false);
}

void
CQChartsBarChartPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  QString strokeStr    = "stroke";
  QString fillStr      = "fill";
  QString dataLabelStr = "dataLabel";

  addProperty(""          , this       , "xColumn"                );
  addProperty(""          , this       , "yColumn"                );
  addProperty(""          , this       , "stacked"                );
  addProperty(""          , this       , "margin"                 );
  addProperty(""          , this       , "keySets"                );
  addProperty(strokeStr   , this       , "border"     , "visible" );
  addProperty(strokeStr   , this       , "borderColor", "color"   );
  addProperty(strokeStr   , this       , "borderWidth", "width"   );
  addProperty(fillStr     , this       , "fill"       , "visible" );
  addProperty(fillStr     , this       , "barColor"   , "color"   );

  dataLabel_.addProperties(dataLabelStr);
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

  dataRange_.updateRange(-0.5, 0);

  //---

  xAxis_->clearTickLabels();

  valueSets_.clear();

  int ns = numSets();

  int numVisible = 0;

  for (int i = 0; i < n; ++i) {
    bool ok1;

    QString name = CQChartsUtil::modelString(model_, i, xColumn_, ok1);

    double sum = 0.0;

    ValueSet *valueSet = getValueSet(name);

    if (ns > 1) {
      if (isKeySets()) {
        if (isSetHidden(i))
          continue;

        int numVisible1 = 0;

        for (int j = 0; j < ns; ++j) {
          int yColumn = getSetColumn(j);

          bool ok2;

          double value = CQChartsUtil::modelReal(model_, i, yColumn, ok2);

          if (! ok2) value = i;

          if (CQChartsUtil::isNaN(value))
            continue;

          valueSet->values.push_back(value);

          if (isStacked())
            dataRange_.updateRange(0, sum + value);
          else
            dataRange_.updateRange(0, value);

          sum += value;

          ++numVisible1;
        }

        if (numVisible1 == 0)
          continue;

        //---

        xAxis_->setTickLabel(numVisible, name);

        ++numVisible;
      }
      else {
        int numVisible1 = 0;

        for (int j = 0; j < ns; ++j) {
          if (isSetHidden(j))
            continue;

          int yColumn = getSetColumn(j);

          bool ok2;

          double value = CQChartsUtil::modelReal(model_, i, yColumn, ok2);

          if (! ok2) value = i;

          if (CQChartsUtil::isNaN(value))
            continue;

          valueSet->values.push_back(value);

          if (isStacked())
            dataRange_.updateRange(0, sum + value);
          else
            dataRange_.updateRange(0, value);

          sum += value;

          ++numVisible1;
        }

        if (numVisible1 == 0)
          continue;

        //---

        xAxis_->setTickLabel(numVisible, name);

        ++numVisible;
      }
    }
    else {
      if (isSetHidden(i))
        continue;

      int yColumn = getSetColumn(0);

      bool ok2;

      double value = CQChartsUtil::modelReal(model_, i, yColumn, ok2);

      if (! ok2) value = i;

      if (CQChartsUtil::isNaN(value))
        continue;

      valueSet->values.push_back(value);

      if (isStacked())
        dataRange_.updateRange(0, sum + value);
      else
        dataRange_.updateRange(0, value);

      sum += value;

      //---

      xAxis_->setTickLabel(numVisible, name);

      ++numVisible;
    }
  }

  //---

  valueNames_.clear();

  if (ns > 1) {
    for (int j = 0; j < ns; ++j) {
      int yColumn = getSetColumn(j);

      QString name = model_->headerData(yColumn, Qt::Horizontal).toString();

      valueNames_.push_back(name);
    }
  }

  //---

  if (numVisible > 0) {
    dataRange_.updateRange(numVisible - 0.5, dataRange_.ymin());
  }
  else {
    dataRange_.updateRange(0.5, 1.0);
  }

  //---

  xAxis_->setColumn(xColumn_);
  yAxis_->setColumn(yColumn_);

  QString xname = model_->headerData(xColumn_, Qt::Horizontal).toString();

  xAxis_->setLabel(xname);

  //---

  applyDataRange();
}

int
CQChartsBarChartPlot::
numSets() const
{
  if (yColumns_.empty())
    return 1;

  return yColumns_.size();
}

int
CQChartsBarChartPlot::
getSetColumn(int i) const
{
  if (! yColumns_.empty())
    return yColumns_[i];
  else
    return yColumn_;
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

  //--

  if (! dataRange_.isSet()) {
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

          if (isStacked())
            brect = CBBox2D(bx, sum, bx + 1.0, value1);
          else
            brect = CBBox2D(bx1, 0.0, bx1 + bw1, value);

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

          if (isStacked())
            brect = CBBox2D(bx, sum, bx + 1.0, value1);
          else
            brect = CBBox2D(bx1, 0.0, bx1 + bw1, value);

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

      if (isStacked())
        brect = CBBox2D(bx, sum, bx + 1.0, value1);
      else
        brect = CBBox2D(bx1, 0.0, bx1 + bw1, value);

      CQChartsBarChartObj *barObj = new CQChartsBarChartObj(this, brect, 0, ns, j, nv, value);

      QString valueName = valueSet.name;

      QString valueStr = this->valueStr(value);

      barObj->setId(QString("%1:%2:%3").arg(setName).arg(valueName).arg(valueStr));

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
  return yAxis_->getValueStr(v);
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
draw(QPainter *p)
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

  int m = plot_->margin();

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

  QRectF qrect = CQUtil::toQRect(prect);

  p->drawRect(qrect);

  //---

  QString ystr = plot_->valueStr(value_);

  plot_->drawDataLabel(p, qrect, ystr);

  //---

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
    c = CQUtil::blendColors(c, key_->bgColor(), 0.5);

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
    c = CQUtil::blendColors(c, Qt::white, 0.5);

  return c;
}

//------

CQChartsBarDataLabel::
CQChartsBarDataLabel(CQChartsBarChartPlot *plot) :
 plot_(plot)
{
}

void
CQChartsBarDataLabel::
addProperties(const QString &path)
{
  plot_->addProperty(path, this, "visible" );
  plot_->addProperty(path, this, "position");
  plot_->addProperty(path, this, "clip"    );
  plot_->addProperty(path, this, "angle"   );
  plot_->addProperty(path, this, "font"    );
  plot_->addProperty(path, this, "color"   );

  QString boxPath = plot_->id() + "/" + path + "/box";

  CQChartsBoxObj::addProperties(plot_->propertyView(), boxPath);
}

void
CQChartsBarDataLabel::
update()
{
  plot_->update();
}

void
CQChartsBarDataLabel::
draw(QPainter *p, const QRectF &qrect, const QString &ystr)
{
  if (! isVisible())
    return;

  p->save();

  p->setFont(font());

  QFontMetrics fm(p->font());

  double ym = 2;

  if (fabs(angle()) <= 1E-3) {
    int tw = fm.width(ystr);

    double x = 0.0, y = 0.0;

    if      (position() == Position::TOP_INSIDE) {
      x = qrect.center().x();
      y = qrect.top() + fm.ascent() + ym;
    }
    else if (position() == Position::TOP_OUTSIDE) {
      x = qrect.center().x();
      y = qrect.top() - fm.descent() - ym;
    }
    else if (position() == Position::BOTTOM_INSIDE) {
      x = qrect.center().x();
      y = qrect.bottom() - fm.descent() - ym;
    }
    else if (position() == Position::BOTTOM_OUTSIDE) {
      x = qrect.center().x();
      y = qrect.bottom() + fm.ascent() + ym;
    }

    bool clipped = false;

    if (isClip()) {
      if (tw >= qrect.width())
        clipped = true;
    }

    if (isClip()) {
      if (position() == Position::TOP_INSIDE ||
          position() == Position::BOTTOM_INSIDE) {
        p->setClipRect(qrect, Qt::ReplaceClip);
      }
    }

    int b1 = CQChartsBoxObj::margin();
    int b2 = CQChartsBoxObj::padding();

    int b = b1 + b2;

    QRectF brect(x - tw/2 - b, y - fm.ascent() - b, tw + 2*b, fm.ascent() + fm.descent() + 2*b);

    CQChartsBoxObj::draw(p, brect);

    if (! clipped) {
      p->setPen(color());

      p->drawText(x - tw/2, y, ystr);
    }
  }
  else {
    double x = 0.0, y = 0.0;

    if      (position() == Position::TOP_INSIDE) {
      x = qrect.center().x();
      y = qrect.top();
    }
    else if (position() == Position::TOP_OUTSIDE) {
      x = qrect.center().x();
      y = qrect.top();
    }
    else if (position() == Position::BOTTOM_INSIDE) {
      x = qrect.center().x();
      y = qrect.bottom();
    }
    else if (position() == Position::BOTTOM_OUTSIDE) {
      x = qrect.center().x();
      y = qrect.bottom();
    }

    QRectF bbox = CQRotatedText::bbox(x, y, ystr, p->font(), angle());

    double dx = (bbox.center().x() - x);
    double dy = 0.0;

    if      (position() == Position::TOP_INSIDE) {
      dy = (bbox.top() - y) + ym;
    }
    else if (position() == Position::TOP_OUTSIDE) {
      dy = (bbox.bottom() - y) - ym;
    }
    else if (position() == Position::BOTTOM_INSIDE) {
      dy = (bbox.bottom() - y) - ym;
    }
    else if (position() == Position::BOTTOM_OUTSIDE) {
      dy = (bbox.top() - y) + ym;
    }

    x -= dx;
    y -= dy;

    p->setPen(color());

    CQChartsBoxObj::draw(p, bbox);

    CQRotatedText::drawRotatedText(p, x, y, ystr, -angle());

    //p->setPen(Qt::red);
    //p->setBrush(Qt::NoBrush);

    //p->drawRect(bbox);
  }

  p->restore();
}
