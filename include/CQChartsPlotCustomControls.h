#ifndef CQChartsPlotCustomControls_H
#define CQChartsPlotCustomControls_H

#include <CQChartsColor.h>

#include <QFrame>

class CQCharts;
class CQChartsPlot;
class CQChartsPlotType;
class CQChartsColumnParameterEdit;
class CQChartsColumnsParameterEdit;
class CQChartsEnumParameterEdit;
class CQChartsBoolParameterEdit;
class CQChartsColorLineEdit;
class CQChartsColorRangeSlider;
class CQChartsColumnCombo;
class CQChartsPaletteNameEdit;
class CQChartsMapKeyWidget;
class CQChartsColumnControlGroup;

//class CQTabSplit;
class QCheckBox;
class QLabel;
class QGridLayout;
class QVBoxLayout;

class CQChartsPlotCustomControls : public QFrame {
  Q_OBJECT

 public:
  CQChartsPlotCustomControls(CQCharts *charts, const QString &plotType);

  virtual ~CQChartsPlotCustomControls() { }

  virtual CQChartsPlot *plot() const { return plot_; }
  virtual void setPlot(CQChartsPlot *plot);

  //---

  struct FrameData {
    QFrame*      frame  { nullptr };
    QGridLayout* layout { nullptr };
    int          row    { 0 };
  };

  struct ColumnControlGroupData {
    CQChartsColumnControlGroup* group          { nullptr };
    FrameData                   fixedFrame;
    FrameData                   columnFrame;
    QFrame*                     columnControls { nullptr };
  };

  FrameData createFrame();
  FrameData createGroupFrame(const QString &name);

  ColumnControlGroupData createColumnControlGroup(const QString &name, const QString &title);

  void addColumnWidgets(const QStringList &columnNames, FrameData &frameData);

  void showColumnWidgets(const QStringList &columnNames);

  void addColorColumnWidgets(const QString &title="Color");

  void addFrameWidget(FrameData &frameData, const QString &label, QWidget *w);

  void addFrameRowStretch(FrameData &frameData);

  void addLayoutStretch();

  //---

  virtual CQChartsColor getColorValue() { return color_; }
  virtual void setColorValue(const CQChartsColor &c) { color_ = c; }

 public slots:
  virtual void updateWidgets();

 protected slots:
  void colorGroupChanged();

  void plotDrawnSlot();

  void colorDetailsSlot();

  void colorSlot();
  void colorColumnSlot();
  void colorRangeSlot();
  void colorPaletteSlot();

  void columnSlot();
  void columnsSlot();

  void numericOnlySlot(int state);

 protected:
  CQChartsPlotType *plotType() const;

  CQChartsBoolParameterEdit *createBoolEdit(const QString &name);
  CQChartsEnumParameterEdit *createEnumEdit(const QString &name);

  void connectSlots(bool b);

 protected:
  using ColumnEdits  = std::vector<CQChartsColumnParameterEdit  *>;
  using ColumnsEdits = std::vector<CQChartsColumnsParameterEdit *>;

  CQCharts*    charts_ { nullptr };
//CQTabSplit*  split_  { nullptr };
  QVBoxLayout* layout_ { nullptr };

  QString       plotType_;
  CQChartsPlot* plot_ { nullptr };

  QLabel*      titleWidget_  { nullptr };
  ColumnEdits  columnEdits_;
  ColumnsEdits columnsEdits_;
  QCheckBox*   numericCheck_ { nullptr };

  CQChartsColumnControlGroup* colorControlGroup_ { nullptr };
  CQChartsColorLineEdit*      colorEdit_         { nullptr };
  CQChartsColumnCombo*        colorColumnCombo_  { nullptr };
  CQChartsColorRangeSlider*   colorRange_        { nullptr };
  CQChartsPaletteNameEdit*    colorPaletteEdit_  { nullptr };
  CQChartsMapKeyWidget*       colorMapKey_       { nullptr };

  CQChartsColor color_; // dummy color for getColorValue/setColorValue virtual
};

#endif
