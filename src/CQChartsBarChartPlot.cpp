#include <CQChartsBarChartPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsBoxObj.h>
#include <CQChartsFillObj.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRenderer.h>
#include <CQChartsRoundedPolygon.h>

CQChartsBarChartPlotType::
CQChartsBarChartPlotType()
{
  addParameters();
}

void
CQChartsBarChartPlotType::
addParameters()
{
  addColumnParameter ("category", "Category", "categoryColumn", "", 0);
  addColumnsParameter("value"   , "Value"   , "valueColumns"  , "", "1");
  addColumnParameter ("name"    , "Name"    , "nameColumn"    , "optional");
  addColumnParameter ("color"   , "Color"   , "colorColumn"   , "optional");
  addColumnParameter ("label"   , "Label"   , "labelColumn"   , "optional");

  addBoolParameter("stacked"   , "Stacked"   , "stacked"   , "optional");
  addBoolParameter("horizontal", "Horizontal", "horizontal", "optional");
}

CQChartsPlot *
CQChartsBarChartPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsBarChartPlot(view, model);
}

//---

CQChartsBarChartPlot::
CQChartsBarChartPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("barchart"), model), dataLabel_(this)
{
  borderObj_ = new CQChartsBoxObj(this);
  fillObj_   = new CQChartsFillObj(this);

  fillObj_->setVisible(true);

  //---

  setBorder(true);

  setLayerActive(Layer::FG, true);

  addAxes();

  addKey();

  addTitle();
}

CQChartsBarChartPlot::
~CQChartsBarChartPlot()
{
  delete borderObj_;
  delete fillObj_;
}

QString
CQChartsBarChartPlot::
valueColumnsStr() const
{
  return CQChartsUtil::toString(valueColumns());
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

  addProperty("columns", this, "categoryColumn", "category" );
  addProperty("columns", this, "valueColumn"   , "value"    );
  addProperty("columns", this, "valueColumns"  , "valuesSet");
  addProperty("columns", this, "nameColumn"    , "name"     );
  addProperty("columns", this, "colorColumn"   , "color"    );
  addProperty("columns", this, "labelColumn"   , "label"    );

  addProperty("", this, "stacked"   );
  addProperty("", this, "horizontal");
  addProperty("", this, "margin"    , "barMargin");
  addProperty("", this, "keySets"   );

  QString strokeStr = "stroke";
  QString fillStr   = "fill";

  addProperty(strokeStr, this, "border"          , "visible"   );
  addProperty(strokeStr, this, "borderColor"     , "color"     );
  addProperty(strokeStr, this, "borderWidth"     , "width"     );
  addProperty(strokeStr, this, "borderCornerSize", "cornerSize");

  addProperty(fillStr, this, "barFill"   , "visible");
  addProperty(fillStr, this, "barColor"  , "color"  );
  addProperty(fillStr, this, "barAlpha"  , "alpha"  );
  addProperty(fillStr, this, "barPattern", "pattern");

  dataLabel_.addProperties("dataLabel");

  addProperty("color", this, "colorMapEnabled", "mapEnabled" );
  addProperty("color", this, "colorMapMin"    , "mapMin"     );
  addProperty("color", this, "colorMapMax"    , "mapMax"     );
}

//---

bool
CQChartsBarChartPlot::
isBorder() const
{
  return borderObj_->isBorder();
}

void
CQChartsBarChartPlot::
setBorder(bool b)
{
  borderObj_->setBorder(b); update();
}

QString
CQChartsBarChartPlot::
borderColorStr() const
{
  return borderObj_->borderColorStr();
}

void
CQChartsBarChartPlot::
setBorderColorStr(const QString &s)
{
  borderObj_->setBorderColorStr(s);
}

QColor
CQChartsBarChartPlot::
interpBorderColor(int i, int n) const
{
  return borderObj_->interpBorderColor(i, n);
}

double
CQChartsBarChartPlot::
borderWidth() const
{
  return borderObj_->borderWidth();
}

void
CQChartsBarChartPlot::
setBorderWidth(double r)
{
  borderObj_->setBorderWidth(r); update();
}

double
CQChartsBarChartPlot::
borderCornerSize() const
{
  return borderObj_->borderCornerSize();
}

void
CQChartsBarChartPlot::
setBorderCornerSize(double r)
{
  borderObj_->setBorderCornerSize(r); update();
}

//---

bool
CQChartsBarChartPlot::
isBarFill() const
{
  return fillObj_->isVisible();
}

void
CQChartsBarChartPlot::
setBarFill(bool b)
{
  fillObj_->setVisible(b); update();
}

QString
CQChartsBarChartPlot::
barColorStr() const
{
  return fillObj_->colorStr();
}

void
CQChartsBarChartPlot::
setBarColorStr(const QString &str)
{
  fillObj_->setColorStr(str);

  update();
}

QColor
CQChartsBarChartPlot::
interpBarColor(int i, int n) const
{
  return fillObj_->interpColor(i, n);
}

double
CQChartsBarChartPlot::
barAlpha() const
{
  return fillObj_->alpha();
}

void
CQChartsBarChartPlot::
setBarAlpha(double a)
{
  fillObj_->setAlpha(a); update();
}

CQChartsBarChartPlot::Pattern
CQChartsBarChartPlot::
barPattern() const
{
  return (Pattern) fillObj_->pattern();
}

void
CQChartsBarChartPlot::
setBarPattern(Pattern pattern)
{
  fillObj_->setPattern((CQChartsFillObj::Pattern) pattern); update();
}

//---

void
CQChartsBarChartPlot::
initColorSet()
{
  colorSet_.clear();

  if (colorColumn() < 0)
    return;

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int nr = model->rowCount(QModelIndex());

  for (int i = 0; i < nr; ++i) {
    bool ok;

    QVariant value = CQChartsUtil::modelValue(model, i, colorColumn(), ok);

    colorSet_.addValue(value); // always add some value
  }
}

bool
CQChartsBarChartPlot::
colorSetColor(int i, OptColor &color)
{
  if (i < 0)
    return false;

  if (colorSet_.empty())
    return false;

  // color can be actual color value (string) or value used to map into palette
  // (map enabled or disabled)
  if (colorSet_.type() != CQChartsValueSet::Type::STRING) {
    double value = colorSet_.imap(i);

    color = CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE, value);
  }
  else {
    QVariant colorVar = colorSet_.value(i);

    color = QColor(colorVar.toString());
  }

  return true;
}

//---

void
CQChartsBarChartPlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int nr = model->rowCount(QModelIndex());

  dataRange_.reset();

  if (! isHorizontal())
    dataRange_.updateRange(-0.5, 0);
  else
    dataRange_.updateRange(0, -0.5);

  //---

  xAxis_->clearTickLabels();
  yAxis_->clearTickLabels();

  valueSets_   .clear();
  valueNameInd_.clear();

  int ns = numSets();

  int numVisible = 0;

  //---

  for (int r = 0; r < nr; ++r) {
    // required category name
    QModelIndex categoryInd = model->index(r, categoryColumn());

    QModelIndex categoryInd1 = normalizeIndex(categoryInd);

    bool ok1;

    QString category = CQChartsUtil::modelString(model, categoryInd, ok1);

    //---

    // optional name for category value
    QString name;

    if (nameColumn() >= 0) {
      QModelIndex nameInd = model->index(r, nameColumn());

      bool ok3;

      name = CQChartsUtil::modelString(model, nameInd, ok3);

      if (! ok3) name = "";
    }

    //---

    // optional data label
    QString label;

    if (labelColumn() >= 0) {
      QModelIndex labelInd = model->index(r, labelColumn());

      bool ok3;

      label = CQChartsUtil::modelString(model, labelInd, ok3);

      if (! ok3) label = "";
    }

    //---

    double sum = 0.0;

    ValueSet *valueSet = getValueSet(category);

    if (ns > 1) {
      if (isKeySets()) {
        if (isSetHidden(r))
          continue;

        int numVisible1 = 0;

        for (int j = 0; j < ns; ++j) {
          int valueColumn = getSetColumn(j);

          QModelIndex valueInd = model->index(r, valueColumn);

          if (! valueInd.isValid())
            continue;

          bool ok2;

          double value = CQChartsUtil::modelReal(model, valueInd, ok2);

          if (! ok2) value = r;

          if (CQChartsUtil::isNaN(value))
            continue;

          valueSet->values.emplace_back(value, "", "", categoryInd1);

          //---

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

        QString axisLabel = (name != "" ? name : category);

        if (! isHorizontal())
          xAxis_->setTickLabel(numVisible, axisLabel);
        else
          yAxis_->setTickLabel(numVisible, axisLabel);

        ++numVisible;
      }
      else {
        int numVisible1 = 0;

        for (int j = 0; j < ns; ++j) {
          if (isSetHidden(j))
            continue;

          //---

          int valueColumn = getSetColumn(j);

          QModelIndex valueInd = model->index(r, valueColumn);

          if (! valueInd.isValid())
            continue;

          bool ok2;

          double value = CQChartsUtil::modelReal(model, valueInd, ok2);

          if (! ok2) value = r;

          if (CQChartsUtil::isNaN(value))
            continue;

          valueSet->values.emplace_back(value, "", "", categoryInd1);

          //---

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

        QString axisLabel = (name != "" ? name : category);

        if (! isHorizontal())
          xAxis_->setTickLabel(numVisible, axisLabel);
        else
          yAxis_->setTickLabel(numVisible, axisLabel);

        ++numVisible;
      }
    }
    else {
      if (isSetHidden(valueSet->ind))
        continue;

      //---

      int valueColumn = getSetColumn(0);

      QModelIndex valueInd = model->index(r, valueColumn);

      if (! valueInd.isValid())
        continue;

      bool ok2;

      double value = CQChartsUtil::modelReal(model, valueInd, ok2);

      if (! ok2) value = r;

      if (CQChartsUtil::isNaN(value))
        continue;

      //---

      bool isNew = valueSet->values.empty();

      valueSet->values.emplace_back(value, name, label, categoryInd1);

      double posSum, negSum;

      valueSet->calcSums(posSum, negSum);

      if (! isHorizontal()) {
        if (isStacked()) {
          dataRange_.updateRange(0, posSum);
          dataRange_.updateRange(0, negSum);
        }
        else {
          dataRange_.updateRange(0, value);
        }
      }
      else {
        if (isStacked()) {
          dataRange_.updateRange(posSum, 0);
          dataRange_.updateRange(negSum, 0);
        }
        else {
          dataRange_.updateRange(value, 0);
        }
      }

      //---

      if (isNew) {
        QString axisLabel = (name != "" ? name : category);

        if (! isHorizontal())
          xAxis_->setTickLabel(numVisible, axisLabel);
        else
          yAxis_->setTickLabel(numVisible, axisLabel);

        ++numVisible;
      }
    }
  }

  //---

  valueNames_.clear();

  if (ns > 1) {
    for (int j = 0; j < ns; ++j) {
      int valueColumn = getSetColumn(j);

      bool ok;

      QString valueName = CQChartsUtil::modelHeaderString(model, valueColumn, ok);

      valueNames_.push_back(valueName);
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

  adjustDataRange();

  //---

  setXValueColumn(categoryColumn());
  setYValueColumn(valueColumn());

  //---

  if (! isHorizontal())
    xAxis_->setColumn(categoryColumn());
  else
    yAxis_->setColumn(categoryColumn());

  bool ok;

  QString xname = CQChartsUtil::modelHeaderString(model, categoryColumn(), ok);

  if (! isHorizontal())
    xAxis_->setLabel(xname);
  else
    yAxis_->setLabel(xname);

  //---

  if (! isHorizontal())
    yAxis_->setColumn(valueColumn());
  else
    xAxis_->setColumn(valueColumn());

  if (valueColumns().size() <= 1) {
    bool ok;

    QString yname = CQChartsUtil::modelHeaderString(model, valueColumn(), ok);

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

  if (apply)
    applyDataRange();
}

int
CQChartsBarChartPlot::
numSets() const
{
  if (valueColumns().empty())
    return 1;

  return valueColumns().size();
}

int
CQChartsBarChartPlot::
getSetColumn(int i) const
{
  if (! valueColumns().empty())
    return valueColumns()[i];
  else
    return valueColumn();
}

CQChartsBarChartPlot::ValueSet *
CQChartsBarChartPlot::
getValueSet(const QString &name)
{
  auto p = valueNameInd_.find(name);

  if (p != valueNameInd_.end())
    return &valueSets_[(*p).second];

  int nv = numValueSets();

  valueSets_.emplace_back(name, nv);

  valueNameInd_[name] = nv;

  return &valueSets_.back();

#if 0
  int nv = numValueSets();

  for (int j = 0; j < nv; ++j) {
    if (valueSets_[j].name == name)
      return &valueSets_[j];
  }

  valueSets_.emplace_back(name, nv);

  return &valueSets_.back();
#endif
}

void
CQChartsBarChartPlot::
updateObjs()
{
  colorSet_.clear();

  CQChartsPlot::updateObjs();
}

bool
CQChartsBarChartPlot::
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

  // init color value set
  if (colorSet_.empty())
    initColorSet();

  //---

  xAxis_->setIntegral           (! isHorizontal());
//xAxis_->setDataLabels         (! isHorizontal());
  xAxis_->setMinorTicksDisplayed(  isHorizontal());

  yAxis_->setIntegral           (  isHorizontal());
//yAxis_->setDataLabels         (  isHorizontal());
  yAxis_->setMinorTicksDisplayed(! isHorizontal());

  //---

  // start at px1 - bar width
  double bx = -0.5;

  int ns = numSets();

  int nv = numValueSets();

  for (int j = 0; j < nv; ++j) {
    const ValueSet &valueSet = valueSets_[j];

    //const QString &setName = valueSet.name;

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
      if (isSetHidden(valueSet.ind))
        continue;

      numVisible = valueSet.values.size();
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
          const Value &ivalue = valueSet.values[numVisible1];

          double             value = ivalue.value;
          const QModelIndex &ind   = ivalue.ind;

          double value1 = value + sum;

          OptColor color;

          (void) colorSetColor(ind.row(), color);

          //---

          // create bar rect
          CQChartsGeom::BBox brect;

          if (! isHorizontal()) {
            if (isStacked())
              brect = CQChartsGeom::BBox(bx, sum, bx + 1.0, value1);
            else
              brect = CQChartsGeom::BBox(bx1, 0.0, bx1 + bw1, value);
          }
          else {
            if (isStacked())
              brect = CQChartsGeom::BBox(sum, bx, value1, bx + 1.0);
            else
              brect = CQChartsGeom::BBox(0.0, bx1, value, bx1 + bw1);
          }

          CQChartsBarChartObj *barObj =
            new CQChartsBarChartObj(this, brect, i, ns, j, nv, 0, 1, value, ind);

          if (color)
            barObj->setColor(*color);

          if (ivalue.label.length())
            barObj->setLabel(ivalue.label);

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

          const Value &ivalue = valueSet.values[numVisible1];

          double             value = ivalue.value;
          const QModelIndex &ind   = ivalue.ind;

          double value1 = value + sum;

          OptColor color;

          (void) colorSetColor(ind.row(), color);

          //---

          // create bar rect
          CQChartsGeom::BBox brect;

          if (! isHorizontal()) {
            if (isStacked())
              brect = CQChartsGeom::BBox(bx, sum, bx + 1.0, value1);
            else
              brect = CQChartsGeom::BBox(bx1, 0.0, bx1 + bw1, value);
          }
          else {
            if (isStacked())
              brect = CQChartsGeom::BBox(sum, bx, value1, bx + 1.0);
            else
              brect = CQChartsGeom::BBox(0.0, bx1, value, bx1 + bw1);
          }

          CQChartsBarChartObj *barObj =
            new CQChartsBarChartObj(this, brect, i, ns, j, nv, 0, 1, value, ind);

          if (color)
            barObj->setColor(*color);

          if (ivalue.label.length())
            barObj->setLabel(ivalue.label);

          addPlotObject(barObj);

          //---

          bx1 += bw1;
          sum += value;

          ++numVisible1;
        }
      }
    }
    else {
      if (isSetHidden(valueSet.ind))
        continue;

      if (valueSet.values.empty())
        continue;

      double lastPosValue = 0.0, lastNegValue = 0.0;

      int nvs = valueSet.values.size();

      for (int i = 0; i < nvs; ++i) {
        const Value &ivalue = valueSet.values[i];

        double             value     = ivalue.value;
//      const QString     &valueName = ivalue.name;
        const QModelIndex &ind       = ivalue.ind;

        OptColor color;

        (void) colorSetColor(ind.row(), color);

        //---

        // create bar rect
        CQChartsGeom::BBox brect;

        if (value >= 0) {
          if (! isHorizontal()) {
            if (isStacked())
              brect = CQChartsGeom::BBox(bx, lastPosValue, bx + 1.0, lastPosValue + value);
            else
              brect = CQChartsGeom::BBox(bx1, 0.0, bx1 + bw1, value);
          }
          else {
            if (isStacked())
              brect = CQChartsGeom::BBox(lastPosValue, bx, lastPosValue + value, bx + 1.0);
            else
              brect = CQChartsGeom::BBox(0.0, bx1, value, bx1 + bw1);
          }
        }
        else {
          if (! isHorizontal()) {
            if (isStacked())
              brect = CQChartsGeom::BBox(bx, lastNegValue + value, bx + 1.0, lastNegValue);
            else
              brect = CQChartsGeom::BBox(bx1, 0.0, bx1 + bw1, value);
          }
          else {
            if (isStacked())
              brect = CQChartsGeom::BBox(lastNegValue + value, bx, lastNegValue, bx + 1.0);
            else
              brect = CQChartsGeom::BBox(0.0, bx1, value, bx1 + bw1);
          }
        }

        CQChartsBarChartObj *barObj =
          new CQChartsBarChartObj(this, brect, 0, 1, j, nv, i, nvs, value, ind);

        if (color)
          barObj->setColor(*color);

        if (ivalue.label.length())
          barObj->setLabel(ivalue.label);

        addPlotObject(barObj);

        //---

        if (value >= 0) {
          lastPosValue = lastPosValue + value;
        }
        else {
          lastNegValue = lastNegValue + value;
        }

        bx1 += bw1;
      }
    }

    bx += 1.0;
  }

  //----

  resetKeyItems();

  //----

  return true;
}

QString
CQChartsBarChartPlot::
valueStr(double v) const
{
  if (! isHorizontal())
    return yAxis_->valueStr(v);
  else
    return xAxis_->valueStr(v);
}

void
CQChartsBarChartPlot::
addKeyItems(CQChartsKey *key)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  int row = key->maxRow();

  int ns = numSets();

  if (ns > 1) {
    if (isKeySets()) {
      int nv = numValueSets();

      for (int i = 0; i < nv; ++i) {
        const ValueSet &valueSet = valueSets_[i];

        CQChartsBarKeyColor *color = new CQChartsBarKeyColor(this, i, nv);
        CQChartsBarKeyText  *text  = new CQChartsBarKeyText (this, i, valueSet.name);

        key->addItem(color, row, 0);
        key->addItem(text , row, 1);

        ++row;
      }
    }
    else {
      for (int i = 0; i < ns; ++i) {
        CQChartsBarKeyColor *color = new CQChartsBarKeyColor(this, i, ns);
        CQChartsBarKeyText  *text  = new CQChartsBarKeyText (this, i, valueName(i));

        key->addItem(color, row, 0);
        key->addItem(text , row, 1);

        ++row;
      }
    }
  }
  else {
    if (! isKeySets()) {
      int nv = numValueSets();

      for (int i = 0; i < nv; ++i) {
        const ValueSet &valueSet = valueSets_[i];

        CQChartsBarKeyColor *keyColor = new CQChartsBarKeyColor(this, i, nv);
        CQChartsBarKeyText  *keyText  = new CQChartsBarKeyText (this, i, valueSet.name);

        if (valueSet.values.size() == 1) {
          const Value &ivalue = valueSet.values[0];

          const QModelIndex &ind = ivalue.ind;

          OptColor color;

          if (colorSetColor(ind.row(), color))
            keyColor->setColor((*color).interpColor(0, 1));
        }

        key->addItem(keyColor, row, 0);
        key->addItem(keyText , row, 1);

        ++row;
      }
    }
    else {
      QString title = this->title();

      if (! title.length()) {
        bool ok;

        QString yname = CQChartsUtil::modelHeaderString(model, valueColumn(), ok);

        title = yname;
      }

      CQChartsBarKeyColor *color = new CQChartsBarKeyColor(this, 0, 1);
      CQChartsBarKeyText  *text  = new CQChartsBarKeyText (this, 0, title);

      key->addItem(color, row, 0);
      key->addItem(text , row, 1);

      ++row;
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

  return 0;
}

bool
CQChartsBarChartPlot::
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
CQChartsBarChartPlot::
draw(CQChartsRenderer *renderer)
{
  initPlotObjs();

  //---

  drawParts(renderer);
}

void
CQChartsBarChartPlot::
drawDataLabel(CQChartsRenderer *renderer, const QRectF &qrect, const QString &ystr)
{
  dataLabel_.draw(renderer, qrect, ystr);
}

//------

CQChartsBarChartObj::
CQChartsBarChartObj(CQChartsBarChartPlot *plot, const CQChartsGeom::BBox &rect,
                    int iset, int nset, int ival, int nval, int isval, int nsval,
                    double value, const QModelIndex &ind) :
 CQChartsPlotObj(plot, rect), plot_(plot), iset_(iset), nset_(nset), ival_(ival), nval_(nval),
 isval_(isval), nsval_(nsval), value_(value), ind_(ind)
{
}

QString
CQChartsBarChartObj::
calcId() const
{
  const QString &setName = plot_->valueSetName(ival_);

  QString valueName;

  if (nset_ > 1)
    valueName = plot_->valueName(iset_);
  else
    valueName = plot_->valueSetValueName(ival_, isval_);

  QString valueStr = plot_->valueStr(value_);

  if (valueName.length())
    return QString("%1:%2=%3").arg(setName).arg(valueName).arg(valueStr);
  else
    return QString("%1:%2").arg(setName).arg(valueStr);
}

void
CQChartsBarChartObj::
addSelectIndex()
{
  int yColumn = plot_->getSetColumn(iset_);

  plot_->addSelectIndex(ind_.row(), plot_->categoryColumn(), ind_.parent());
  plot_->addSelectIndex(ind_.row(), yColumn                , ind_.parent());
}

bool
CQChartsBarChartObj::
isIndex(const QModelIndex &ind) const
{
  return (ind == ind_);
}

void
CQChartsBarChartObj::
draw(CQChartsRenderer *renderer, const CQChartsPlot::Layer &layer)
{
  static double minBarSize = 1.0/64.0;

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

  renderer->save();

  CQChartsGeom::BBox prect;

  plot_->windowToPixel(rect(), prect);

  QRectF qrect = CQChartsUtil::toQRect(prect);

  if (layer == CQChartsPlot::Layer::MID) {
    double m = plot_->margin();

    if (prect.getWidth() > 3*m) {
      prect.setXMin(prect.getXMin() + m);
      prect.setXMax(prect.getXMax() - m);

      qrect = CQChartsUtil::toQRect(prect);
    }

    //---

    // calc pen (stroke)
    QPen pen;

    if (plot_->isBorder()) {
      pen.setColor (plot_->interpBorderColor(0, 1));
      pen.setWidthF(plot_->borderWidth());
    }
    else {
      pen.setStyle(Qt::NoPen);
    }

    // calc brush (fill)
    QBrush barBrush;

    if (plot_->isBarFill()) {
      QColor barColor;

      if (nset_ > 1) {
        if (plot_->isKeySets())
          barColor = plot_->interpBarColor(ival_, nval_);
        else
          barColor = plot_->interpBarColor(iset_, nset_);
      }
      else {
        if (! color_) {
          if (nsval_ > 1) {
            QColor barColor1 = plot_->interpBarColor(ival_    , nval_ + 1);
            QColor barColor2 = plot_->interpBarColor(ival_ + 1, nval_ + 1);

            double f = (1.0*isval_)/nsval_;

            barColor = CQChartsUtil::blendColors(barColor1, barColor2, f);
          }
          else
            barColor = plot_->interpBarColor(ival_, nval_);
        }
        else {
          barColor = (*color_).interpColor(plot_, 0.0);
        }
      }

      barColor.setAlphaF(plot_->barAlpha());

      barBrush.setColor(barColor);
      barBrush.setStyle(CQChartsFillObj::patternToStyle(
        (CQChartsFillObj::Pattern) plot_->barPattern()));
    }
    else {
      barBrush.setStyle(Qt::NoBrush);
    }

    plot_->updateObjPenBrushState(this, pen, barBrush);

    //---

    // draw rect
    if (qrect.width() > minBarSize && qrect.height() > minBarSize) {
      renderer->setPen(pen);
      renderer->setBrush(barBrush);

      CQChartsRoundedPolygon::draw(renderer, qrect, plot_->borderCornerSize());
    }
    else {
      if (! plot_->isBorder())
        renderer->setPen(barBrush.color());

      renderer->drawLine(QPointF(qrect.left (), qrect.bottom()),
                         QPointF(qrect.right(), qrect.top   ()));
    }
  }
  else {
    QString label = label_;

    if (plot_->labelColumn() < 0)
      label = plot_->valueStr(value_);

    plot_->drawDataLabel(renderer, qrect, label);
  }

  renderer->restore();
}

//------

CQChartsBarKeyColor::
CQChartsBarKeyColor(CQChartsBarChartPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

bool
CQChartsBarKeyColor::
mousePress(const CQChartsGeom::Point &)
{
  CQChartsBarChartPlot *plot = qobject_cast<CQChartsBarChartPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->updateRangeAndObjs();

  return true;
}

QBrush
CQChartsBarKeyColor::
fillBrush() const
{
  CQChartsBarChartPlot *plot = qobject_cast<CQChartsBarChartPlot *>(plot_);

  QColor c;

  if (color_)
    c = (*color_).interpColor(plot, 0.0);
  else
    c = plot->interpBarColor(i_, n_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

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
interpTextColor(int i, int n) const
{
  CQChartsBarChartPlot *plot = qobject_cast<CQChartsBarChartPlot *>(plot_);

  QColor c = CQChartsKeyText::interpTextColor(i, n);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, CQChartsUtil::bwColor(c), 0.5);

  return c;
}
