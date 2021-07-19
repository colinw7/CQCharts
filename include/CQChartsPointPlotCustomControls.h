#ifndef CQChartsPointPlotCustomControls_H
#define CQChartsPointPlotCustomControls_H

#include <CQChartsGroupPlotCustomControls.h>

class CQChartsPointPlot;
class CQChartsLengthEdit;
class CQChartsSymbolLineEdit;
class CQChartsSymbolSetEdit;
class CQChartsSymbolTypeRangeSlider;
class CQChartsSymbolSizeRangeSlider;
class CQChartsMapKeyWidget;

class CQChartsPointPlotCustomControls : public CQChartsGroupPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsPointPlotCustomControls(CQCharts *charts, const QString &plotType);

  void setPlot(CQChartsPlot *plot) override;

  void addSymbolSizeWidgets();

  void handlePlotDrawn() override;

  void updateSymbolSizeMapKeyVisible();
  void updateSymbolTypeMapKeyVisible();

 protected slots:
  void plotDrawnSlot();

  void symbolSizeGroupChanged();
  void symbolSizeDetailsSlot();
  void symbolSizeLengthSlot();
  void symbolSizeColumnSlot();
  void symbolSizeRangeSlot(double min, double max);

  void symbolTypeGroupChanged();
  void symbolTypeDetailsSlot();
  void symbolTypeSlot();
  void symbolTypeColumnSlot();
  void symbolTypeRangeSlot(int min, int max);
  void symbolTypeSetSlot(const QString &name);

 public slots:
  void updateWidgets() override;

 protected slots:
  void showSymbolSizeMapKeySlot(bool b);
  void showSymbolTypeMapKeySlot(bool b);

 protected:
  void connectSlots(bool b) override;

 private:
  using ColumnEdits = std::vector<CQChartsColumnParameterEdit *>;

  CQChartsPointPlot* plot_ { nullptr };

  CQChartsColumnControlGroup*    symbolSizeControlGroup_ { nullptr };
  CQChartsLengthEdit*            symbolSizeLengthEdit_   { nullptr };
  CQChartsColumnCombo*           symbolSizeColumnCombo_  { nullptr };
  CQChartsSymbolSizeRangeSlider* symbolSizeRange_        { nullptr };
  CQChartsMapKeyWidget*          symbolSizeMapKey_       { nullptr };

  CQChartsColumnControlGroup*    symbolTypeControlGroup_ { nullptr };
  CQChartsSymbolLineEdit*        symbolTypeEdit_         { nullptr };
  CQChartsColumnCombo*           symbolTypeColumnCombo_  { nullptr };
  CQChartsSymbolTypeRangeSlider* symbolTypeRange_        { nullptr };
  CQChartsSymbolSetEdit*         symbolTypeSetEdit_      { nullptr };
  CQChartsMapKeyWidget*          symbolTypeMapKey_       { nullptr };
};

#endif
