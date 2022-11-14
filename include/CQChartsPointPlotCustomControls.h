#ifndef CQChartsPointPlotCustomControls_H
#define CQChartsPointPlotCustomControls_H

#include <CQChartsGroupPlotCustomControls.h>
#include <CQChartsOptBool.h>

class CQChartsPointPlot;
class CQChartsLengthEdit;
class CQChartsSymbolLineEdit;
class CQChartsSymbolSetEdit;
class CQChartsSymbolTypeRangeSlider;
class CQChartsSymbolSizeRangeSlider;
class CQChartsMapKeyWidget;

class CQChartsPointPlotCustomControls : public CQChartsGroupPlotCustomControls {
  Q_OBJECT

  Q_PROPERTY(CQChartsOptBool showSizeRange   READ isShowSizeRange   WRITE setShowSizeRange  )
  Q_PROPERTY(CQChartsOptBool showSizeMapping READ isShowSizeMapping WRITE setShowSizeMapping)

  Q_PROPERTY(CQChartsOptBool showSymbolRange   READ isShowSymbolRange   WRITE setShowSymbolRange  )
  Q_PROPERTY(CQChartsOptBool showSymbolSet     READ isShowSymbolSet     WRITE setShowSymbolSet    )
  Q_PROPERTY(CQChartsOptBool showSymbolMapping READ isShowSymbolMapping WRITE setShowSymbolMapping)

 public:
  using OptBool = CQChartsOptBool;

 public:
  CQChartsPointPlotCustomControls(CQCharts *charts, const QString &plotType);

  void setPlot(Plot *plot) override;

  //---

  // point symbol size
  const OptBool &isShowSizeRange() const { return showSizeRange_; }
  void setShowSizeRange(const OptBool &b) { showSizeRange_ = b; updateWidgets(); }

  const OptBool &isShowSizeMapping() const { return showSizeMapping_; }
  void setShowSizeMapping(const OptBool &b) { showSizeMapping_ = b; updateWidgets(); }

  // point symbol type
  const OptBool &isShowSymbolRange() const { return showSymbolRange_; }
  void setShowSymbolRange(const OptBool &b) { showSymbolRange_ = b; updateWidgets(); }

  const OptBool &isShowSymbolSet() const { return showSymbolSet_; }
  void setShowSymbolSet(const OptBool &b) { showSymbolSet_ = b; updateWidgets(); }

  const OptBool &isShowSymbolMapping() const { return showSymbolMapping_; }
  void setShowSymbolMapping(const OptBool &b) { showSymbolMapping_ = b; updateWidgets(); }

  //---

  void addSymbolSizeWidgets();

  void handlePlotDrawn() override;

  void updateSymbolSizeMapKeyVisible();
  void updateSymbolTypeMapKeyVisible();

 protected Q_SLOTS:
  void plotDrawnSlot();

  void symbolSizeGroupChanged();
  void symbolSizeDetailsSlot();
  void symbolSizeLengthSlot();
  void symbolSizeColumnSlot();
  void symbolSizeRangeSlot(double min, double max);
  void symbolSizeMappingSlot();

  void symbolTypeGroupChanged();
  void symbolTypeDetailsSlot();
  void symbolTypeSlot();
  void symbolTypeColumnSlot();
  void symbolTypeRangeSlot(int min, int max);
  void symbolTypeSetSlot(const QString &name);
  void symbolTypeMappingSlot();

 public Q_SLOTS:
  void updateWidgets() override;

 protected Q_SLOTS:
  void showSymbolSizeMapKeySlot(bool b);
  void showSymbolTypeMapKeySlot(bool b);

 protected:
  void connectSlots(bool b) override;

 private:
  using ColumnEdits = std::vector<CQChartsColumnParameterEdit *>;

  CQChartsPointPlot* pointPlot_ { nullptr };

  OptBool showSizeRange_;
  OptBool showSizeMapping_;
  OptBool showSymbolRange_;
  OptBool showSymbolSet_;
  OptBool showSymbolMapping_;

  // point symbol size
  CQChartsColumnControlGroup*    symbolSizeControlGroup_ { nullptr };
  CQChartsLengthEdit*            symbolSizeLengthEdit_   { nullptr };
  CQChartsColumnCombo*           symbolSizeColumnCombo_  { nullptr };
  CQChartsSymbolSizeRangeSlider* symbolSizeRange_        { nullptr };
  CQChartsMapKeyWidget*          symbolSizeMapKey_       { nullptr };
  QLineEdit*                     symbolSizeMappingEdit_  { nullptr };

  // point symbol type
  CQChartsColumnControlGroup*    symbolTypeControlGroup_ { nullptr };
  CQChartsSymbolLineEdit*        symbolTypeEdit_         { nullptr };
  CQChartsColumnCombo*           symbolTypeColumnCombo_  { nullptr };
  CQChartsSymbolTypeRangeSlider* symbolTypeRange_        { nullptr };
  CQChartsSymbolSetEdit*         symbolTypeSetEdit_      { nullptr };
  CQChartsMapKeyWidget*          symbolTypeMapKey_       { nullptr };
  QLineEdit*                     symbolTypeMappingEdit_  { nullptr };
};

#endif
