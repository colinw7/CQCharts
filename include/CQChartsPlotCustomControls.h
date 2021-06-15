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

class CQIconButton;
class CQGroupBox;

class QCheckBox;
class QLabel;
class QGridLayout;
class QVBoxLayout;

class CQChartsPlotCustomControls : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool numericOnly READ isNumericOnly WRITE setNumericOnly)
  Q_PROPERTY(bool showTitle   READ isShowTitle   WRITE setShowTitle  )

 public:
  CQChartsPlotCustomControls(CQCharts *charts, const QString &plotType);

  virtual ~CQChartsPlotCustomControls() { }

  virtual CQChartsPlot *plot() const { return plot_; }
  virtual void setPlot(CQChartsPlot *plot);

  bool isNumericOnly() const { return numericOnly_; }
  void setNumericOnly(bool b);

  bool isShowTitle() const { return showTitle_; }
  void setShowTitle(bool b);

  //---

  struct FrameData {
    CQGroupBox*  groupBox { nullptr };
    QFrame*      frame    { nullptr };
    QGridLayout* layout   { nullptr };
    int          row      { 0 };
    int          col      { 0 };
  };

  struct ColumnControlGroupData {
    CQChartsColumnControlGroup* group          { nullptr };
    FrameData                   fixedFrame;
    FrameData                   columnFrame;
    QFrame*                     columnControls { nullptr };
  };

  FrameData createFrame(const QString &objName, bool stretch=true);
  FrameData createGroupFrame(const QString &name, const QString &objName, bool stretch=true);

  ColumnControlGroupData createColumnControlGroup(const QString &name, const QString &title);

  void addColumnWidgets(const QStringList &columnNames, FrameData &frameData);

  void showColumnWidgets(const QStringList &columnNames);

  void addColorColumnWidgets(const QString &title="Color");

  void addFrameWidget(FrameData &frameData, const QString &label, QWidget *w);
  void addFrameWidget(FrameData &frameData, QWidget *w);

  void addFrameColWidget(FrameData &frameData, QWidget *w, bool nextRow=false);

  void addFrameRowStretch(FrameData &frameData);

  void addLayoutStretch();

  void setFrameWidgetVisible(QWidget *w, bool visible);

  //---

  virtual CQChartsColor getColorValue() { return color_; }
  virtual void setColorValue(const CQChartsColor &c) { color_ = c; }

  //---

  virtual void handlePlotDrawn();

  void updateColorKeyVisible();

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

  void numericOnlySlot(bool state);

  void showColorKeySlot(bool b);

 protected:
  CQChartsPlotType *plotType() const;

  CQChartsBoolParameterEdit *createBoolEdit(const QString &name, bool choice=true);
  CQChartsEnumParameterEdit *createEnumEdit(const QString &name);

  void connectSlots(bool b);

  void updateNumericOnly();

 protected:
  using ColumnEdits  = std::vector<CQChartsColumnParameterEdit  *>;
  using ColumnsEdits = std::vector<CQChartsColumnsParameterEdit *>;

  CQCharts*    charts_ { nullptr };
  QVBoxLayout* layout_ { nullptr };

  QString       plotType_;
  CQChartsPlot* plot_ { nullptr };

  bool numericOnly_ { false };
  bool showTitle_   { false };

  QLabel* titleWidget_ { nullptr };

  ColumnEdits   columnEdits_;
  ColumnsEdits  columnsEdits_;
  CQIconButton* numericCheck_ { nullptr };

  CQChartsColumnControlGroup* colorControlGroup_ { nullptr };
  CQChartsColorLineEdit*      colorEdit_         { nullptr };
  CQChartsColumnCombo*        colorColumnCombo_  { nullptr };
  CQChartsColorRangeSlider*   colorRange_        { nullptr };
  CQChartsPaletteNameEdit*    colorPaletteEdit_  { nullptr };
  CQChartsMapKeyWidget*       colorMapKeyWidget_ { nullptr };

  CQChartsColor color_; // dummy color for getColorValue/setColorValue virtual
};

#endif
