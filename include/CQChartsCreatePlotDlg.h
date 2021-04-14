#ifndef CQChartsCreatePlotDlg_H
#define CQChartsCreatePlotDlg_H

#include <CQChartsPlotType.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <QDialog>
#include <QPointer>
#include <set>

class CQCharts;
class CQChartsPlotParameter;
class CQChartsView;
class CQChartsPlot;
class CQChartsColumnCombo;
class CQChartsColumnLineEdit;
class CQChartsColumnsLineEdit;
class CQChartsModelViewHolder;
class CQChartsModelData;
class CQChartsModelDetailsWidget;
class CQChartsModelChooser;
class CQChartsPreviewPlot;
class CQChartsColumnParameterEdit;
class CQChartsEnumParameterEdit;
class CQChartsEnumParameterCombo;
class CQChartsStringParameterEdit;
class CQChartsRealParameterEdit;
class CQChartsIntParameterEdit;
class CQChartsBoolParameterEdit;
class CQChartsLineEdit;
class CQChartsIconButton;

class CQSummaryModel;
class CQIntegerSpin;
class CQRealSpin;

class QAbstractItemModel;
class QFrame;
class QGridLayout;
class QHBoxLayout;
class QComboBox;
class QStackedWidget;
class QTextEdit;
class QCheckBox;
class QRadioButton;
class QToolButton;
class QLabel;

/*!
 * \brief dialog to create new plot
 * \ingroup Charts
 */
class CQChartsCreatePlotDlg : public QDialog {
  Q_OBJECT

 public:
  using Plot               = CQChartsPlot;
  using PlotType           = CQChartsPlotType;
  using ModelData          = CQChartsModelData;
  using Column             = CQChartsColumn;
  using Columns            = CQChartsColumns;
  using PlotParameter      = CQChartsPlotParameter;
  using PlotParameterGroup = CQChartsPlotParameterGroup;
  using ModelP             = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsCreatePlotDlg(CQCharts *charts, ModelData *modelData);
 ~CQChartsCreatePlotDlg();

  CQCharts *charts() const { return charts_; }

  ModelData *modelData() const { return modelData_; }

  QAbstractItemModel *model() const { return model_.data(); }

  void setViewName(const QString &viewName);

  int exec();

  Plot *plot() const { return plot_; }

 private:
  using ColumnLineEdit  = CQChartsColumnLineEdit;
  using ColumnsLineEdit = CQChartsColumnsLineEdit;
  using LineEdit        = CQChartsLineEdit;
  using ColumnCombo     = CQChartsColumnCombo;

  struct MapEditData {
    QCheckBox*  mappedCheck { nullptr };
    CQRealSpin* mapMinSpin  { nullptr };
    CQRealSpin* mapMaxSpin  { nullptr };
  };

  struct FormatEditData {
    LineEdit*           formatEdit   { nullptr };
    CQChartsIconButton* formatUpdate { nullptr };
  };

  struct PlotData {
    using Names        = std::set<QString>;
    using ColumnEdits  = std::map<QString, CQChartsColumnParameterEdit *>;
    using ColumnsEdits = std::map<QString, ColumnsLineEdit*>;
    using FormatEdits  = std::map<QString, FormatEditData>;
    using MapEdits     = std::map<QString, MapEditData>;
    using StringEdits  = std::map<QString, CQChartsStringParameterEdit *>;
    using RealEdits    = std::map<QString, CQChartsRealParameterEdit *>;
    using IntEdits     = std::map<QString, CQChartsIntParameterEdit *>;
    using EnumEdits    = std::map<QString, CQChartsEnumParameterEdit *>;
    using BoolEdits    = std::map<QString, CQChartsBoolParameterEdit *>;

    Names        names;
    ColumnEdits  columnEdits;
    ColumnsEdits columnsEdits;
    FormatEdits  formatEdits;
    MapEdits     mappedEdits;
    StringEdits  stringEdits;
    RealEdits    realEdits;
    IntEdits     intEdits;
    EnumEdits    enumEdits;
    BoolEdits    boolEdits;
    QPushButton* okButton { nullptr };
    int          ind      { -1 };
  };

  struct MapValueData {
    bool   mapped { false };
    double min    { 0.0 };
    double max    { 1.0 };
  };

 private:
  void init();

  QFrame *createDataFrame();
  QFrame *createTypeDataFrame();
  QFrame *createGeneralDataFrame();
  QFrame *createDetailsFrame();
  QFrame *createSummaryFrame();
  QFrame *createPreviewFrame();

  QFrame *createTypeCombo();

  void sortedPlotTypes(CQCharts::PlotTypes &sortedPlotTypes);

  void addPlotWidgets(PlotType *type, int ind);

  void addParameterEdits(PlotType *type, PlotData &plotData, QGridLayout *layout,
                         int &row, bool isBasic);
  void addParameterEdits(const PlotType::Parameters &parameters, PlotData &plotData,
                         QGridLayout *layout, int &row, bool isBasic);

  void addParameterEdit(PlotData &plotData, QGridLayout *layout, int &row,
                        PlotParameter *parameter, bool isBasic);
  void addParameterEdit(PlotData &plotData, QHBoxLayout *layout,
                        PlotParameter *parameter, bool isBasic);

  void addParameterColumnEdit(PlotData &plotData, QGridLayout *layout, int &row,
                              PlotParameter *parameter, bool isBasic);
  void addParameterColumnsEdit(PlotData &plotData, QGridLayout *layout, int &row,
                               PlotParameter *parameter, bool isBasic);

  void addParameterEnumEdit(PlotData &plotData, QHBoxLayout *layout, PlotParameter *parameter);
  void addParameterBoolEdit(PlotData &plotData, QHBoxLayout *layout, PlotParameter *parameter);
  void addParameterStringEdit(PlotData &plotData, QHBoxLayout *layout, PlotParameter *parameter);
  void addParameterRealEdit(PlotData &plotData, QHBoxLayout *layout, PlotParameter *parameter);
  void addParameterIntEdit(PlotData &plotData, QHBoxLayout *layout, PlotParameter *parameter);

  LineEdit *addRealEdit(QLayout *grid, int &row, int &column, const QString &name,
                        const QString &objName, const QString &placeholderText) const;
  LineEdit *addStringEdit(QLayout *grid, int &row, int &column, const QString &name,
                          const QString &objName, const QString &placeholderText) const;

  void addFormatEdit(PlotData &plotData, PlotParameter *parameter, QGridLayout *layout,
                     const QString &name, int &row, int &col);

  //---

  bool parsePosition(double &xmin, double &ymin, double &xmax, double &ymax) const;

  bool parseParameterColumnEdit(PlotParameter *parameter, const PlotData &plotData,
                                Column &column, QString &columnType, MapValueData &mapValueData);
  bool parseParameterColumnsEdit(PlotParameter *parameter, const PlotData &plotData,
                                 Columns &columns, QString &columnType);

  bool parseParameterStringEdit(PlotParameter *parameter, const PlotData &plotData, QString &str);
  bool parseParameterRealEdit(PlotParameter *parameter, const PlotData &plotData, double &r);
  bool parseParameterIntEdit(PlotParameter *parameter, const PlotData &plotData, int &i);
  bool parseParameterEnumEdit(PlotParameter *parameter, const PlotData &plotData, int &i);
  bool parseParameterBoolEdit(PlotParameter *parameter, const PlotData &plotData, bool &b);

  bool columnLineEditValue(CQChartsColumnParameterEdit *editData, Column &column,
                           const Column &defColumn=Column()) const;
  bool columnsLineEditValue(ColumnsLineEdit *le, Columns &columns,
                            const Columns &defColumns=Columns()) const;

  bool validate(QStringList &msgs);

  void updatePreviewPlot(bool valid);

  void setXYMin(const QString &id);

  void applyPlot(Plot *plot, bool preview=false);

  PlotType *getPlotType() const;

  bool isAutoAnalyzeModel() const { return autoAnalyzeModel_; }
  void setAutoAnalyzeModel(bool b) { autoAnalyzeModel_ = b; }

  bool isAdvanced() const { return advanced_; }

 private slots:
  void comboSlot(const QString &desc);

  void plotDataSlot(int);

  void autoAnalyzeSlot(int);

  void autoPlaceSlot(int);

  void xminSlot();
  void yminSlot();
  void xmaxSlot();
  void ymaxSlot();

  void modelChangeSlot();
  void updateModelData();

  void validateSlot();

  void updateFormatSlot();

  void previewEnabledSlot();

  void summaryEnabledSlot();
  void updateSummaryTypeSlot();
  void updatePreviewSlot();

  void okSlot();
  bool applySlot();
  void cancelSlot();

 private:
  void connectSlots(bool b);

 private:
  using TypePlotData    = std::map<QString, PlotData>;
  using TabType         = std::map<int, PlotType *>;
  using TypeInitialized = std::map<QString, bool>;

  struct RangeEditData {
    LineEdit*    xminEdit   { nullptr }; //!< xmin edit
    QToolButton* xminButton { nullptr }; //!< xmin load button
    LineEdit*    yminEdit   { nullptr }; //!< ymin edit
    QToolButton* yminButton { nullptr }; //!< ymin load button
    LineEdit*    xmaxEdit   { nullptr }; //!< xmax edit
    QToolButton* xmaxButton { nullptr }; //!< xmax load button
    LineEdit*    ymaxEdit   { nullptr }; //!< ymax edit
    QToolButton* ymaxButton { nullptr }; //!< ymax load button
  };

  struct SummaryEditData {
    QCheckBox*               enabledCheck    { nullptr }; //!< summary enabled checkbox
    CQIntegerSpin*           maxRows         { nullptr }; //!< summary max rows
    QComboBox*               typeCombo       { nullptr }; //!< summary type combo
    QStackedWidget*          typeStack       { nullptr }; //!< summary type stack
    CQIntegerSpin*           sortedColEdit   { nullptr }; //!< summary column edit
    CQIntegerSpin*           pageSizeEdit    { nullptr }; //!< summary page size edit
    CQIntegerSpin*           currentPageEdit { nullptr }; //!< summary current page edit
    CQChartsModelViewHolder* modelView       { nullptr }; //!< summary model view
  };

  CQCharts*  charts_    { nullptr }; //!< parent charts
  ModelData* modelData_ { nullptr }; //!< model data
  ModelP     model_;                 //!< model

  CQChartsModelChooser* modelChooser_ { nullptr }; //!< model chooser

  // plot type widgets
  QStackedWidget* plotDataStack_         { nullptr }; //!< plot widget stack
  QStackedWidget* basicPlotDataStack_    { nullptr }; //!< basic plot widget stack
  QStackedWidget* advancedPlotDataStack_ { nullptr }; //!< advanced plot widget stack
  TypePlotData    basicTypePlotData_;                 //!< per type plot data
  TypePlotData    advancedTypePlotData_;              //!< per type plot data
  bool            advanced_              { false };   //!< is advanced
  TabType         stackIndexPlotType_;                //!< stacked index for plot type

  // filter edit
  QFrame*   whereFrame_ { nullptr }; //!< where frame
  LineEdit* whereEdit_  { nullptr }; //!< where edit

  // general widgets
  LineEdit*     viewEdit_       { nullptr }; //!< view name edit
  LineEdit*     posEdit_        { nullptr }; //!< position edit
  QCheckBox*    autoRangeEdit_  { nullptr }; //!< auto range check
  LineEdit*     titleEdit_      { nullptr }; //!< title edit
  LineEdit*     xLabelEdit_     { nullptr }; //!< x label edit
  LineEdit*     yLabelEdit_     { nullptr }; //!< y label edit
  RangeEditData rangeEditData_;
  QCheckBox*    xintegralCheck_ { nullptr }; //!< x integral check
  QCheckBox*    yintegralCheck_ { nullptr }; //!< y integral check
  QCheckBox*    xlogCheck_      { nullptr }; //!< x log check
  QCheckBox*    ylogCheck_      { nullptr }; //!< y log check

  // description text
  QTextEdit* descText_ { nullptr }; //!< type description

  QLabel*      msgLabel_    { nullptr }; //!< message label
  QPushButton* okButton_    { nullptr }; //!< ok button
  QPushButton* applyButton_ { nullptr }; //!< apply button

  // summary widgets
  SummaryEditData summaryEditData_;

  CQChartsModelDetailsWidget* detailsWidget_ { nullptr };

  //! preview
  CQChartsPreviewPlot *previewPlot_ { nullptr };

  //! misc
  Plot* plot_        { nullptr }; //!< last created plot (apply)
  bool  initialized_ { false   }; //!< is initialized

  // auto analyze
  bool            autoAnalyzeModel_       { true }; //!< auto analyze model
  TypeInitialized basicTypeInitialzed_;             //!< is type initialized
  TypeInitialized advancedTypeInitialzed_;          //!< is type initialized
};

#endif
