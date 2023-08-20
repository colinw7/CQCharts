#ifndef CQChartsPlotCustomControls_H
#define CQChartsPlotCustomControls_H

#include <CQChartsColor.h>
#include <CQChartsOptBool.h>

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
class CQChartsPlotOverview;

//---

class CQChartsPlotCustomControls : public QScrollArea {
  Q_OBJECT

  Q_PROPERTY(bool numericOnly READ isNumericOnly WRITE setNumericOnly)
  Q_PROPERTY(bool showTitle   READ isShowTitle   WRITE setShowTitle  )

  Q_PROPERTY(CQChartsOptBool showColorRange   READ isShowColorRange   WRITE setShowColorRange  )
  Q_PROPERTY(CQChartsOptBool showColorMapping READ isShowColorMapping WRITE setShowColorMapping)

 public:
  using Plot    = CQChartsPlot;
  using OptBool = CQChartsOptBool;

 public:
  CQChartsPlotCustomControls(CQCharts *charts, const QString &plotType);

  virtual ~CQChartsPlotCustomControls() = default;

  virtual void init() { }

  //---

  virtual void addWidgets() { }

  virtual void addColumnWidgets() { }

  virtual void addOptionsWidgets() { }

#ifdef CQCHARTS_MODULE_SHLIB
  void addModuleWidgets();
#endif

  //---

  virtual Plot *plot() const;
  virtual void setPlot(Plot *plot);

  bool isNumericOnly() const { return numericOnly_; }
  void setNumericOnly(bool b);

  bool isShowTitle() const { return showTitle_; }
  void setShowTitle(bool b);

  //---

  const OptBool &isShowColorRange() const { return showColorRange_; }
  void setShowColorRange(const OptBool &b) { showColorRange_ = b; updateWidgets(); }

  const OptBool &isShowColorMapping() const { return showColorMapping_; }
  void setShowColorMapping(const OptBool &b) { showColorMapping_ = b; updateWidgets(); }

  //---

  struct FrameData {
    CQGroupBox*  groupBox { nullptr };
    QFrame*      frame    { nullptr };
    QGridLayout* grid     { nullptr };
    QBoxLayout*  box      { nullptr };
    int          row      { 0 };
    int          col      { 0 };
  };

  struct ColumnControlGroupData {
    CQChartsColumnControlGroup* group          { nullptr };
    FrameData                   fixedFrame;
    FrameData                   columnFrame;
    QFrame*                     columnControls { nullptr };
  };

  struct FrameOpts {
    bool stretch { true };
    bool grid    { true };
    bool hbox    { false };
    bool vbox    { false };

    FrameOpts() { }

    static FrameOpts makeHBox() {
      FrameOpts opts;
      opts.stretch = false;
      opts.grid    = false;
      opts.hbox    = true;
      return opts;
    }

    static FrameOpts makeVBox() {
      FrameOpts opts;
      opts.stretch = false;
      opts.grid    = false;
      opts.vbox    = true;
      return opts;
    }

    static FrameOpts makeNoStretch() {
      FrameOpts opts;
      opts.stretch = false;
      return opts;
    }
  };

  FrameData createFrame(const QString &objName, const FrameOpts &frameOpts=FrameOpts());

  FrameData createGroupFrame(const QString &name, const QString &objName,
                             const FrameOpts &frameOpts=FrameOpts()) {
    return createGroupFrame(name, objName, "groupBox", frameOpts);
  }

  FrameData createGroupFrame(const QString &name, const QString &objName, const QString &groupName,
                             const FrameOpts &frameOpts=FrameOpts());

  ColumnControlGroupData createColumnControlGroup(const QString &name, const QString &title);

  void addNamedColumnWidgets(const QStringList &columnNames, FrameData &frameData);

  void showColumnWidgets(const QStringList &columnNames);

  void addColorColumnWidgets(const QString &title="Color");

  void addFrameWidget(FrameData &frameData, const QString &label, QWidget *w, bool nextRow=true);
  void addFrameWidget(FrameData &frameData, QWidget *w, bool nextRow=true);

  void addFrameSpacer(FrameData &frameData, bool nextRow=true);

  void addFrameColWidget(FrameData &frameData, QWidget *w, bool nextRow=false);

  void addFrameRowStretch(FrameData &frameData);
  void addFrameColStretch(FrameData &frameData);

  void addLayoutStretch();

  void setFrameWidgetVisible(QWidget *w, bool visible);
  void setFrameWidgetEnabled(QWidget *w, bool visible);

  //---

  virtual CQChartsColor getColorValue() { return color_; }
  virtual void setColorValue(const CQChartsColor &c) { color_ = c; }

  //---

  virtual void handlePlotDrawn();

  void updateColorKeyVisible();

 Q_SIGNALS:
  void numericOnlyChanged();

 public Q_SLOTS:
  virtual void updateWidgets();

 protected Q_SLOTS:
  void resetPlot();

  void colorGroupChanged();

  void plotDrawnSlot();

  void colorDetailsSlot();

  void keyVisibleSlot();

  void overviewChanged();

  void colorSlot();
  void colorColumnSlot();
  void colorRangeSlot();
  void colorPaletteSlot();
  void colorMappingSlot();

  void columnSlot();
  void columnsSlot();

  void numericOnlySlot(bool state);

  void showColorKeySlot(bool b);
  void showPlotKeySlot(bool b);
  void showKeyListSlot(bool b);

  void showOverviewSlot(bool b);

#ifdef CQCHARTS_MODULE_SHLIB
  void moduleEditSlot();
  void moduleApplySlot();
#endif

 protected:
  CQChartsPlotType *plotType() const;

  CQChartsBoolParameterEdit *createBoolEdit(const QString &name, bool choice=true);
  CQChartsEnumParameterEdit *createEnumEdit(const QString &name);

  //QCheckBox *makeOptionCheck(const QString &param);

  void addKeyList();

  void addOverview();

  virtual void connectSlots(bool b);

  void updateNumericOnly();

 protected:
  using PlotP        = QPointer<Plot>;
  using ColumnEdits  = std::vector<CQChartsColumnParameterEdit  *>;
  using ColumnsEdits = std::vector<CQChartsColumnsParameterEdit *>;

  CQCharts* charts_ { nullptr }; //!< charts

  QFrame*      frame_  { nullptr }; //!< frame
  QVBoxLayout* layout_ { nullptr }; //!< frame layout

  QString plotType_; //!< plot type
  PlotP   plot_;     //!< plot

  bool numericOnly_ { false }; //!< support numeric only columns
  bool showTitle_   { true };  //!< show title

  OptBool showColorRange_;
  OptBool showColorMapping_;

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
  QLineEdit*                  colorMappingEdit_  { nullptr }; //!< color mapping edit

  bool connected_ { false }; //!< is connected

  CQChartsColor color_; //!< dummy color for getColorValue/setColorValue virtual

  QCheckBox*             plotKeyCheck_ { nullptr }; //!< plot key check
  QCheckBox*             keyListCheck_ { nullptr }; //!< key list check
  CQChartsPlotCustomKey *keyList_      { nullptr }; //!< key list

  QCheckBox*            overviewCheck_ { nullptr }; //!< overview check
  CQChartsPlotOverview *overview_      { nullptr }; //!< overview widget

#ifdef CQCHARTS_MODULE_SHLIB
  using WidgetP     = QPointer<QWidget>;
  using ModuleEdits = std::map<QString, WidgetP>;

  FrameData   moduleFrame_;
  ModuleEdits moduleEdits_;
#endif
};

//---

class CQTableWidget;
class CQChartsKeyItem;

class CQChartsPlotCustomKey : public QFrame {
  Q_OBJECT

 public:
  using Plot  = CQChartsPlot;
  using PlotP = QPointer<Plot>;

 public:
  CQChartsPlotCustomKey(Plot *plot=nullptr);

  Plot *plot() const;
  void setPlot(Plot *plot);

  void updateWidgets();

  QSize sizeHint() const override;

 private:
  void addItems(const CQChartsKeyItem *item, int &row);

 private Q_SLOTS:
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

  PlotP          plot_;              //!< plot
  CQTableWidget* table_ { nullptr }; //!< table
  RowColItemData itemData_;          //!< item data
};

//---

#include <CQChartsColumn.h>

class CQChartsPlotColumnChooser : public QFrame {
  Q_OBJECT

 public:
  using Plot  = CQChartsPlot;
  using PlotP = QPointer<Plot>;

 public:
  CQChartsPlotColumnChooser(Plot *plot=nullptr);

  Plot *plot() const;
  void setPlot(Plot *plot);

  void updateWidgets();

  virtual const CQChartsColumns &getColumns() const = 0;

  virtual bool isColumnVisible(int ic) const = 0;
  virtual void setColumnVisible(int ic, bool visible) = 0;

  QSize sizeHint() const override;

 private Q_SLOTS:
  void columnClickSlot(int row, int column, bool b);

 private:
  PlotP          plot_;                   //!< plot
  CQTableWidget* columnList_ { nullptr }; //!< column list
};

#endif
