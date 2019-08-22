#include <CQChartsPointPlot.h>
#include <CQChartsDataLabel.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelUtil.h>
#include <CQChartsVariant.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

CQChartsPointPlotType::
CQChartsPointPlotType() :
 CQChartsGroupPlotType()
{
}

void
CQChartsPointPlotType::
addMappingParameters()
{
  // custom columns/map
  startParameterGroup("Symbols");

  addColumnParameter("symbolType", "Symbol Type", "symbolTypeColumn").
    setTip("Custom Symbol Type").setMapped().
    setMapMinMax(CQChartsSymbol::minFillValue(), CQChartsSymbol::maxFillValue());

  addColumnParameter("symbolSize", "Symbol Size", "symbolSizeColumn").
    setTip("Custom Symbol Size").setMapped().
    setMapMinMax(CQChartsSymbolSize::minValue(), CQChartsSymbolSize::maxValue());

  addColumnParameter("fontSize", "Font Size", "fontSizeColumn").
    setTip("Custom Font Size for Label").setMapped().
    setMapMinMax(CQChartsFontSize::minValue(), CQChartsFontSize::maxValue());

  endParameterGroup();
}

//---

CQChartsPointPlot::
CQChartsPointPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model) :
 CQChartsGroupPlot(view, plotType, model)
{
  NoUpdate noUpdate(this);

  // create a data label (shared state for all data labels)
  dataLabel_ = new CQChartsDataLabel(this);

  dataLabel_->setSendSignal(true);

  connect(dataLabel_, SIGNAL(dataChanged()), this, SLOT(dataLabelChanged()));

  //---

  // set mapped range
  setSymbolTypeMapped(true);
  setSymbolTypeMapMin(CQChartsSymbol::minFillValue());
  setSymbolTypeMapMax(CQChartsSymbol::maxFillValue());

  setSymbolSizeMapped(true);
  setSymbolSizeMapMin(CQChartsSymbolSize::minValue());
  setSymbolSizeMapMax(CQChartsSymbolSize::maxValue());

  setFontSizeMapped(true);
  setFontSizeMapMin(CQChartsFontSize::minValue());
  setFontSizeMapMax(CQChartsFontSize::maxValue());
}

CQChartsPointPlot::
~CQChartsPointPlot()
{
  delete dataLabel_;
}

//---

void
CQChartsPointPlot::
addPointProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  // columns
  addProp("columns", "symbolTypeColumn", "symbolType", "Symbol type column");
  addProp("columns", "symbolSizeColumn", "symbolSize", "Symbol size column");
  addProp("columns", "fontSizeColumn"  , "fontSize"  , "Font size column");

  // mapping for columns
  addProp("mapping/symbolType", "symbolTypeMapped", "enabled", "Symbol type values mapped");
  addProp("mapping/symbolType", "symbolTypeMapMin", "min"    , "Symbol type map min value");
  addProp("mapping/symbolType", "symbolTypeMapMax", "max"    , "Symbol type map max value");

  addProp("mapping/symbolSize", "symbolSizeMapped"  , "enabled", "Symbol size values mapped");
  addProp("mapping/symbolSize", "symbolSizeMapMin"  , "min"    , "Symbol size map min value");
  addProp("mapping/symbolSize", "symbolSizeMapMax"  , "max"    , "Symbol size map max value");
  addProp("mapping/symbolSize", "symbolSizeMapUnits", "units"  , "Symbol size map units");

  addProp("mapping/fontSize", "fontSizeMapped"  , "enabled", "Font size value mapped");
  addProp("mapping/fontSize", "fontSizeMapMin"  , "min"    , "Font size map min value");
  addProp("mapping/fontSize", "fontSizeMapMax"  , "max"    , "Font size map max value");
  addProp("mapping/fontSize", "fontSizeMapUnits", "units"  , "Font size map units");
}

void
CQChartsPointPlot::
getPropertyNames(QStringList &names, bool hidden) const
{
  CQChartsPlot::getPropertyNames(names, hidden);

  propertyModel()->objectNames(dataLabel(), names, hidden);
}

//---

const CQChartsColumn &
CQChartsPointPlot::
symbolTypeColumn() const
{
  return symbolTypeData_.column;
}

void
CQChartsPointPlot::
setSymbolTypeColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(symbolTypeData_.column, c, [&]() { updateObjs(); } );
}

bool
CQChartsPointPlot::
isSymbolTypeMapped() const
{
  return symbolTypeData_.mapped;
}

void
CQChartsPointPlot::
setSymbolTypeMapped(bool b)
{
  CQChartsUtil::testAndSet(symbolTypeData_.mapped, b, [&]() { updateObjs(); } );
}

int
CQChartsPointPlot::
symbolTypeMapMin() const
{
  return symbolTypeData_.map_min;
}

void
CQChartsPointPlot::
setSymbolTypeMapMin(int i)
{
  CQChartsUtil::testAndSet(symbolTypeData_.map_min, i, [&]() { updateObjs(); } );
}

int
CQChartsPointPlot::
symbolTypeMapMax() const
{
  return symbolTypeData_.map_max;
}

void
CQChartsPointPlot::
setSymbolTypeMapMax(int i)
{
  CQChartsUtil::testAndSet(symbolTypeData_.map_max, i, [&]() { updateObjs(); } );
}

//---

const CQChartsColumn &
CQChartsPointPlot::
symbolSizeColumn() const
{
  return symbolSizeData_.column;
}

void
CQChartsPointPlot::
setSymbolSizeColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(symbolSizeData_.column, c, [&]() { updateRangeAndObjs(); } );
}

bool
CQChartsPointPlot::
isSymbolSizeMapped() const
{
  return symbolSizeData_.mapped;
}

void
CQChartsPointPlot::
setSymbolSizeMapped(bool b)
{
  CQChartsUtil::testAndSet(symbolSizeData_.mapped, b, [&]() { updateRangeAndObjs(); } );
}

double
CQChartsPointPlot::
symbolSizeMapMin() const
{
  return symbolSizeData_.map_min;
}

void
CQChartsPointPlot::
setSymbolSizeMapMin(double r)
{
  CQChartsUtil::testAndSet(symbolSizeData_.map_min, r, [&]() { updateRangeAndObjs(); } );
}

double
CQChartsPointPlot::
symbolSizeMapMax() const
{
  return symbolSizeData_.map_max;
}

void
CQChartsPointPlot::
setSymbolSizeMapMax(double r)
{
  CQChartsUtil::testAndSet(symbolSizeData_.map_max, r, [&]() { updateRangeAndObjs(); } );
}

const QString &
CQChartsPointPlot::
symbolSizeMapUnits() const
{
  return symbolSizeData_.units;
}

void
CQChartsPointPlot::
setSymbolSizeMapUnits(const QString &s)
{
  CQChartsUtil::testAndSet(symbolSizeData_.units, s, [&]() { updateRangeAndObjs(); } );
}

//---

const CQChartsColumn &
CQChartsPointPlot::
fontSizeColumn() const
{
  return fontSizeData_.column;
}

void
CQChartsPointPlot::
setFontSizeColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(fontSizeData_.column, c, [&]() { updateRangeAndObjs(); } );
}

bool
CQChartsPointPlot::
isFontSizeMapped() const
{
  return fontSizeData_.mapped;
}

void
CQChartsPointPlot::
setFontSizeMapped(bool b)
{
  CQChartsUtil::testAndSet(fontSizeData_.mapped, b, [&]() { updateRangeAndObjs(); } );
}

double
CQChartsPointPlot::
fontSizeMapMin() const
{
  return fontSizeData_.map_min;
}

void
CQChartsPointPlot::
setFontSizeMapMin(double r)
{
  CQChartsUtil::testAndSet(fontSizeData_.map_min, r, [&]() { updateRangeAndObjs(); } );
}

double
CQChartsPointPlot::
fontSizeMapMax() const
{
  return fontSizeData_.map_max;
}

void
CQChartsPointPlot::
setFontSizeMapMax(double r)
{
  CQChartsUtil::testAndSet(fontSizeData_.map_max, r, [&]() { updateRangeAndObjs(); } );
}

const QString &
CQChartsPointPlot::
fontSizeMapUnits() const
{
  return fontSizeData_.units;
}

void
CQChartsPointPlot::
setFontSizeMapUnits(const QString &s)
{
  CQChartsUtil::testAndSet(fontSizeData_.units, s, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPointPlot::
initSymbolTypeData() const
{
  CQChartsPointPlot *th = const_cast<CQChartsPointPlot *>(this);

  th->symbolTypeData_.valid = false;

  if (! symbolTypeColumn().isValid())
    return;

  CQChartsModelColumnDetails *columnDetails = this->columnDetails(symbolTypeColumn());
  if (! columnDetails) return;

  if (symbolTypeColumn().isGroup()) {
    th->symbolTypeData_.data_min = 0.0;
    th->symbolTypeData_.data_max = std::max(numGroups() - 1, 0);
  }
  else {
    if (symbolTypeData_.mapped) {
      QVariant minVar = columnDetails->minValue();
      QVariant maxVar = columnDetails->maxValue();

      bool ok;

      th->symbolTypeData_.data_min = CQChartsVariant::toReal(minVar, ok);
      if (! ok) th->symbolTypeData_.data_min = 0.0;

      th->symbolTypeData_.data_max = CQChartsVariant::toReal(maxVar, ok);
      if (! ok) th->symbolTypeData_.data_max = 1.0;
    }

    CQBaseModelType    columnType;
    CQBaseModelType    columnBaseType;
    CQChartsNameValues nameValues;

    (void) CQChartsModelUtil::columnValueType(charts(), model().data(), symbolTypeColumn(),
                                              columnType, columnBaseType, nameValues);

    if (columnType == CQBaseModelType::SYMBOL) {
      CQChartsColumnTypeMgr *columnTypeMgr = charts()->columnTypeMgr();

      const CQChartsColumnSymbolTypeType *symbolTypeType =
        dynamic_cast<const CQChartsColumnSymbolTypeType *>(columnTypeMgr->getType(columnType));
      assert(symbolTypeType);

      symbolTypeType->getMapData(charts(), model().data(), symbolTypeColumn(), nameValues,
                                 th->symbolTypeData_.mapped,
                                 th->symbolTypeData_.map_min, th->symbolTypeData_.map_max,
                                 th->symbolTypeData_.data_min, th->symbolTypeData_.data_max);
    }
  }

  th->symbolTypeData_.valid = true;
}

bool
CQChartsPointPlot::
columnSymbolType(int row, const QModelIndex &parent, CQChartsSymbol &symbolType) const
{
  if (! symbolTypeData_.valid)
    return false;

  bool ok;

  QVariant var = modelValue(row, symbolTypeColumn(), parent, ok);
  if (! ok || ! var.isValid()) return false;

  if (symbolTypeData_.mapped) {
    if (CQChartsVariant::isNumeric(var)) {
      int i = CQChartsVariant::toInt(var, ok);
      if (! ok) return false;

      int i1 = CMathUtil::map(i, symbolTypeData_.data_min, symbolTypeData_.data_max,
                              symbolTypeData_.map_min, symbolTypeData_.map_max);

      symbolType = CQChartsSymbol::outlineFromInt(i1);
    }
    else {
      if (CQChartsVariant::isSymbol(var)) {
        symbolType = CQChartsVariant::toSymbol(var, ok);
      }
      else {
        CQChartsModelColumnDetails *columnDetails = this->columnDetails(symbolTypeColumn());
        if (! columnDetails) return false;

        // use unique index/count of edit values (which may have been converted)
        // not same as CQChartsColumnColorType::userData
        int n = columnDetails->numUnique();
        int i = columnDetails->valueInd(var);

        double r = (n > 1 ? double(i)/(n - 1) : 0.0);

        symbolType = CQChartsSymbol::interpOutline(r);
      }
    }
  }
  else {
    if (CQChartsVariant::isSymbol(var)) {
      symbolType = CQChartsVariant::toSymbol(var, ok);
    }
    else {
      QString str = CQChartsVariant::toString(var, ok);

      symbolType = CQChartsSymbol(str);
    }
  }

  return symbolType.isValid();
}

//------

void
CQChartsPointPlot::
initSymbolSizeData() const
{
  CQChartsPointPlot *th = const_cast<CQChartsPointPlot *>(this);

  th->symbolSizeData_.valid = false;

  if (! symbolSizeColumn().isValid())
    return;

  CQChartsModelColumnDetails *columnDetails = this->columnDetails(symbolSizeColumn());
  if (! columnDetails) return;

  if (symbolSizeColumn().isGroup()) {
    th->symbolSizeData_.data_min  = 0.0;
    th->symbolSizeData_.data_max  = std::max(numGroups() - 1, 0);
    th->symbolSizeData_.data_mean = symbolSizeData_.data_max/2.0;
  }
  else {
    if (symbolSizeData_.mapped) {
      QVariant minVar  = columnDetails->minValue();
      QVariant maxVar  = columnDetails->maxValue();
      QVariant meanVar = columnDetails->meanValue();

      bool ok;

      th->symbolSizeData_.data_min = CQChartsVariant::toReal(minVar, ok);
      if (! ok) th->symbolSizeData_.data_min = 0.0;

      th->symbolSizeData_.data_max = CQChartsVariant::toReal(maxVar, ok);
      if (! ok) th->symbolSizeData_.data_max = 1.0;

      th->symbolSizeData_.data_mean = CQChartsVariant::toReal(meanVar, ok);
      if (! ok) th->symbolSizeData_.data_mean =
                  CMathUtil::avg(symbolSizeData_.data_min, symbolSizeData_.data_max);
    }

    CQBaseModelType    columnType;
    CQBaseModelType    columnBaseType;
    CQChartsNameValues nameValues;

    (void) CQChartsModelUtil::columnValueType(charts(), model().data(), symbolSizeColumn(),
                                              columnType, columnBaseType, nameValues);

    if (columnType == CQBaseModelType::SYMBOL_SIZE) {
      CQChartsColumnTypeMgr *columnTypeMgr = charts()->columnTypeMgr();

      const CQChartsColumnSymbolSizeType *symbolSizeType =
        dynamic_cast<const CQChartsColumnSymbolSizeType *>(columnTypeMgr->getType(columnType));
      assert(symbolSizeType);

      symbolSizeType->getMapData(charts(), model().data(), symbolSizeColumn(), nameValues,
                                 th->symbolSizeData_.mapped,
                                 th->symbolSizeData_.map_min, th->symbolSizeData_.map_max,
                                 th->symbolSizeData_.data_min, th->symbolSizeData_.data_max);
    }
  }

  th->symbolSizeData_.valid = true;
}

bool
CQChartsPointPlot::
columnSymbolSize(int row, const QModelIndex &parent, CQChartsLength &symbolSize) const
{
  if (! symbolSizeData_.valid)
    return false;

  CQChartsUnits units = CQChartsUnits::PIXEL;

  (void) CQChartsUtil::decodeUnits(symbolSizeMapUnits(), units);

  bool ok;

  QVariant var = modelValue(row, symbolSizeColumn(), parent, ok);
  if (! ok || ! var.isValid()) return false;

  if (symbolSizeData_.mapped) {
    if (CQChartsVariant::isNumeric(var)) {
      double r = CQChartsVariant::toReal(var, ok);
      if (! ok) return false;

      double r1 = CMathUtil::map(r, symbolSizeData_.data_min, symbolSizeData_.data_max,
                                 symbolSizeData_.map_min, symbolSizeData_.map_max);

      symbolSize = CQChartsLength(r1, units);
    }
    else {
      if (CQChartsVariant::isLength(var)) {
        symbolSize = CQChartsVariant::toLength(var, ok);
      }
      else {
        CQChartsModelColumnDetails *columnDetails = this->columnDetails(symbolSizeColumn());
        if (! columnDetails) return false;

        // use unique index/count of edit values (which may have been converted)
        // not same as CQChartsColumnColorSize::userData
        int n = columnDetails->numUnique();
        int i = columnDetails->valueInd(var);

        double r = (n > 1 ? double(i)/(n - 1) : 0.0);

        double r1 = CMathUtil::map(r, 0.0, 1.0,
                                   CQChartsSymbolSize::minValue(),
                                   CQChartsSymbolSize::maxValue());

        symbolSize = CQChartsLength(r1, units);
      }
    }
  }
  else {
    if (CQChartsVariant::isLength(var)) {
      symbolSize = CQChartsVariant::toLength(var, ok);
    }
    else {
      QString str = CQChartsVariant::toString(var, ok);

      symbolSize = CQChartsLength(str, units);
    }
  }

  return symbolSize.isValid();
}

//------

void
CQChartsPointPlot::
initFontSizeData() const
{
  CQChartsPointPlot *th = const_cast<CQChartsPointPlot *>(this);

  th->fontSizeData_.valid = false;

  if (! fontSizeColumn().isValid())
    return;

  CQChartsModelColumnDetails *columnDetails = this->columnDetails(fontSizeColumn());
  if (! columnDetails) return;

  if (fontSizeColumn().isGroup()) {
    th->fontSizeData_.data_min = 0.0;
    th->fontSizeData_.data_max = std::max(numGroups() - 1, 0);
  }
  else {
    if (fontSizeData_.mapped) {
      QVariant minVar = columnDetails->minValue();
      QVariant maxVar = columnDetails->maxValue();

      bool ok;

      th->fontSizeData_.data_min = CQChartsVariant::toReal(minVar, ok);
      if (! ok) th->fontSizeData_.data_min = 0.0;

      th->fontSizeData_.data_max = CQChartsVariant::toReal(maxVar, ok);
      if (! ok) th->fontSizeData_.data_max = 1.0;
    }

    CQBaseModelType    columnType;
    CQBaseModelType    columnBaseType;
    CQChartsNameValues nameValues;

    (void) CQChartsModelUtil::columnValueType(charts(), model().data(), fontSizeColumn(),
                                              columnType, columnBaseType, nameValues);

    if (columnType == CQBaseModelType::FONT_SIZE) {
      CQChartsColumnTypeMgr *columnTypeMgr = charts()->columnTypeMgr();

      const CQChartsColumnFontSizeType *fontSizeType =
        dynamic_cast<const CQChartsColumnFontSizeType *>(columnTypeMgr->getType(columnType));
      assert(fontSizeType);

      fontSizeType->getMapData(charts(), model().data(), fontSizeColumn(), nameValues,
                               th->fontSizeData_.mapped,
                               th->fontSizeData_.map_min, th->fontSizeData_.map_max,
                               th->fontSizeData_.data_min, th->fontSizeData_.data_max);
    }
  }

  th->fontSizeData_.valid = true;
}

bool
CQChartsPointPlot::
columnFontSize(int row, const QModelIndex &parent, CQChartsLength &fontSize) const
{
  if (! fontSizeData_.valid)
    return false;

  CQChartsUnits units = CQChartsUnits::PIXEL;

  (void) CQChartsUtil::decodeUnits(fontSizeMapUnits(), units);

  bool ok;

  QVariant var = modelValue(row, fontSizeColumn(), parent, ok);
  if (! ok || ! var.isValid()) return false;

  if (fontSizeData_.mapped) {
    if (CQChartsVariant::isNumeric(var)) {
      double r = CQChartsVariant::toReal(var, ok);
      if (! ok) return false;

      double r1 = CMathUtil::map(r, fontSizeData_.data_min, fontSizeData_.data_max,
                                 fontSizeData_.map_min, fontSizeData_.map_max);

      fontSize = CQChartsLength(r1, units);
    }
    else {
      if (CQChartsVariant::isLength(var)) {
        fontSize = CQChartsVariant::toLength(var, ok);
      }
      else {
        CQChartsModelColumnDetails *columnDetails = this->columnDetails(fontSizeColumn());
        if (! columnDetails) return false;

        // use unique index/count of edit values (which may have been converted)
        // not same as CQChartsColumnColorSize::userData
        int n = columnDetails->numUnique();
        int i = columnDetails->valueInd(var);

        double r = (n > 1 ? double(i)/(n - 1) : 0.0);

        double r1 = CMathUtil::map(r, 0.0, 1.0,
                                   CQChartsFontSize::minValue(),
                                   CQChartsFontSize::maxValue());

        fontSize = CQChartsLength(r1, units);
      }
    }
  }
  else {
    if (CQChartsVariant::isLength(var)) {
      fontSize = CQChartsVariant::toLength(var, ok);
    }
    else {
      QString str = CQChartsVariant::toString(var, ok);

      fontSize = CQChartsLength(str, units);
    }
  }

  return fontSize.isValid();
}

//---

bool
CQChartsPointPlot::
isPointLabels() const
{
  return dataLabel()->isVisible();
}

void
CQChartsPointPlot::
setPointLabels(bool b)
{
  if (b != isPointLabels()) { dataLabel()->setVisible(b); drawObjs(); }
}

void
CQChartsPointPlot::
setDataLabelFont(const CQChartsFont &font)
{
  NoUpdate noUpdate(this);

  CQChartsDataLabel *dataLabel = this->dataLabel();

  disconnect(dataLabel, SIGNAL(dataChanged()), this, SLOT(dataLabelChanged()));

  dataLabel->setTextFont(font);

  connect(dataLabel, SIGNAL(dataChanged()), this, SLOT(dataLabelChanged()));
}

//---

void
CQChartsPointPlot::
dataLabelChanged()
{
  // TODO: not enough info to optimize behavior so reload all objects
  updateObjs();
}

//---

void
CQChartsPointPlot::
write(std::ostream &os, const QString &varName, const QString &modelName) const
{
  CQChartsPlot::write(os, varName, modelName);

  dataLabel_->write(os, varName);
}
