#ifndef CQChartsPlotCustomControls_H
#define CQChartsPlotCustomControls_H

#include <CQChartsColor.h>

#include <QScrollArea>
#include <QPointer>

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

class CQChartsPlotCustomKey;

class CQChartsPlotCustomControls : public QScrollArea {
  Q_OBJECT

  Q_PROPERTY(bool numericOnly READ isNumericOnly WRITE setNumericOnly)
  Q_PROPERTY(bool showTitle   READ isShowTitle   WRITE setShowTitle  )

 public:
  CQChartsPlotCustomControls(CQCharts *charts, const QString &plotType);

  virtual ~CQChartsPlotCustomControls() { }

  virtual void init() { }

  virtual void addWidgets() { }

  //---

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

  void addFrameWidget(FrameData &frameData, const QString &label, QWidget *w, bool nextRow=true);
  void addFrameWidget(FrameData &frameData, QWidget *w, bool nextRow=true);

  void addFrameSpacer(FrameData &frameData, bool nextRow=true);

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

 signals:
  void numericOnlyChanged();

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
  void showKeyListSlot(bool b);

 protected:
  CQChartsPlotType *plotType() const;

  CQChartsBoolParameterEdit *createBoolEdit(const QString &name, bool choice=true);
  CQChartsEnumParameterEdit *createEnumEdit(const QString &name);

  QCheckBox *makeOptionCheck(const QString &param);

  void addKeyList();

  virtual void connectSlots(bool b);

  void updateNumericOnly();

 protected:
  using ColumnEdits  = std::vector<CQChartsColumnParameterEdit  *>;
  using ColumnsEdits = std::vector<CQChartsColumnsParameterEdit *>;

  CQCharts* charts_ { nullptr }; //!< charts

  QFrame*      frame_  { nullptr }; //!< frame
  QVBoxLayout* layout_ { nullptr }; //!< frame layout

  QString       plotType_;         //!< plot type
  CQChartsPlot* plot_ { nullptr }; //!< plot

  bool numericOnly_ { false }; //!< support numeric only columns
  bool showTitle_   { true };  //!< show title

  QFrame* titleFrame_ { nullptr }; //!< title frame
  QLabel* titleLabel_ { nullptr }; //!< title label

  ColumnEdits   columnEdits_;             //!< column edits
  ColumnsEdits  columnsEdits_;            //!< columns edits
  CQIconButton* numericIcon_ { nullptr }; //!< is numeric icon button

  CQChartsColumnControlGroup* colorControlGroup_ { nullptr }; //!< color control group
  CQChartsColorLineEdit*      colorEdit_         { nullptr }; //!< color line edit
  CQChartsColumnCombo*        colorColumnCombo_  { nullptr }; //!< color column combo
  CQChartsColorRangeSlider*   colorRange_        { nullptr }; //!< color range slider
  CQChartsPaletteNameEdit*    colorPaletteEdit_  { nullptr }; //!< color palette edit
  CQChartsMapKeyWidget*       colorMapKeyWidget_ { nullptr }; //!< color map key widget

  bool connected_ { false }; //!< is connected

  CQChartsColor color_; //!< dummy color for getColorValue/setColorValue virtual

  CQGroupBox*            keyGroup_ { nullptr }; //!< key group
  CQChartsPlotCustomKey *keyList_  { nullptr }; //!< key list
};

//---

class CQTableWidget;
class CQChartsKeyItem;

class CQChartsPlotCustomKey : public QFrame {
  Q_OBJECT

 public:
  CQChartsPlotCustomKey(CQChartsPlot *plot=nullptr);

  CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *plot) { plot_ = plot; }

  void updateWidgets();

  QSize sizeHint() const override;

 private:
  void addItems(const CQChartsKeyItem *item, int &row);

 private slots:
  void boolClickSlot(int row, int column, bool b);

 private:
  using ItemP = QPointer<CQChartsKeyItem>;

  struct ItemData {
    ItemP    item;
    QVariant value;
    bool     clickable { false };
  };

  using ColItems = std::map<int, ItemData>;

  struct RowData {
    ColItems colItems;
    bool     clickable { false };
  };

  using RowItems = std::map<int, RowData>;

  struct RowColItemData {
    RowItems rowItems;
    bool     clickable { false };
  };

  CQChartsPlot*  plot_  { nullptr }; //!< plot
  CQTableWidget* table_ { nullptr }; //!< table
  RowColItemData itemData_;          //!< item data
};

//---

#include <CQChartsColumn.h>

class CQChartsPlotColumnChooser : public QFrame {
  Q_OBJECT

 public:
  CQChartsPlotColumnChooser(CQChartsPlot *plot=nullptr);

  CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *plot) { plot_ = plot; }

  void updateWidgets();

  virtual const CQChartsColumns &getColumns() const = 0;

  virtual bool isColumnVisible(int ic) const = 0;
  virtual void setColumnVisible(int ic, bool visible) = 0;

  QSize sizeHint() const override;

 private slots:
  void columnClickSlot(int row, int column, bool b);

 private:
  CQChartsPlot*  plot_       { nullptr }; //!< plot
  CQTableWidget* columnList_ { nullptr }; //!< column list
};

#endif
