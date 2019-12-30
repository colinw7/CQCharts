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
class CQChartsLineEdit;

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
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsCreatePlotDlg(CQCharts *charts, CQChartsModelData *modelData);
 ~CQChartsCreatePlotDlg();

  CQCharts *charts() const { return charts_; }

  CQChartsModelData *modelData() const { return modelData_; }

  QAbstractItemModel *model() const { return model_.data(); }

  void setViewName(const QString &viewName);

  int exec();

  CQChartsPlot *plot() const { return plot_; }

 private:
  struct MapEditData {
    QCheckBox*  mappedCheck { nullptr };
    CQRealSpin* mapMinSpin  { nullptr };
    CQRealSpin* mapMaxSpin  { nullptr };
  };

  struct FormatEditData {
    CQChartsLineEdit* formatEdit   { nullptr };
    QToolButton*      formatUpdate { nullptr };
  };

  struct ColumnEditData {
    CQChartsColumnCombo*    basicEdit    { nullptr };
    CQChartsColumnLineEdit* advancedEdit { nullptr };
  };

  struct PlotData {
    using Names        = std::set<QString>;
    using ColumnEdits  = std::map<QString,ColumnEditData>;
    using ColumnsEdits = std::map<QString,CQChartsColumnsLineEdit*>;
    using LineEdits    = std::map<QString,CQChartsLineEdit*>;
    using WidgetEdits  = std::map<QString,QWidget*>;
    using FormatEdits  = std::map<QString,FormatEditData>;
    using Combos       = std::map<QString,QComboBox*>;
    using CheckBoxes   = std::map<QString,QCheckBox*>;
    using MapEdits     = std::map<QString,MapEditData>;

    Names        names;
    ColumnEdits  columnEdits;
    ColumnsEdits columnsEdits;
    MapEdits     mappedEdits;
    FormatEdits  formatEdits;
    Combos       enumEdits;
    CheckBoxes   boolEdits;
    LineEdits    stringEdits;
    WidgetEdits  realEdits;
    WidgetEdits  intEdits;
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

  void addPlotWidgets(CQChartsPlotType *type, int ind);

  void addParameterEdits(CQChartsPlotType *type, PlotData &plotData,
                         QGridLayout *layout, int &row, bool isBasic);
  void addParameterEdits(const CQChartsPlotType::Parameters &parameters, PlotData &plotData,
                         QGridLayout *layout, int &row, bool isBasic);

  void addParameterEdit(PlotData &plotData, QGridLayout *layout, int &row,
                        CQChartsPlotParameter *parameter, bool isBasic);
  void addParameterEdit(PlotData &plotData, QHBoxLayout *layout,
                        CQChartsPlotParameter *parameter, bool isBasic);

  void addParameterBasicColumnEdit(PlotData &plotData, QGridLayout *layout, int &row,
                                   CQChartsPlotParameter *parameter);
  void addParameterColumnEdit(PlotData &plotData, QGridLayout *layout, int &row,
                              CQChartsPlotParameter *parameter);
  void addParameterColumnsEdit(PlotData &plotData, QGridLayout *layout, int &row,
                               CQChartsPlotParameter *parameter, bool isBasic);

  void addParameterEnumEdit(PlotData &plotData, QHBoxLayout *layout,
                            CQChartsPlotParameter *parameter);
  void addParameterBoolEdit(PlotData &plotData, QHBoxLayout *layout,
                            CQChartsPlotParameter *parameter);
  void addParameterStringEdit(PlotData &plotData, QHBoxLayout *layout,
                              CQChartsPlotParameter *parameter);
  void addParameterRealEdit(PlotData &plotData, QHBoxLayout *layout,
                            CQChartsPlotParameter *parameter);
  void addParameterIntEdit(PlotData &plotData, QHBoxLayout *layout,
                           CQChartsPlotParameter *parameter);

  CQChartsColumnLineEdit* addColumnEdit(QLayout *grid, int &row, int &column,
                                        const QString &name, const QString &objName,
                                        const QString &placeholderText) const;
  CQChartsColumnsLineEdit* addColumnsEdit(QLayout *grid, int &row, int &column,
                                          const QString &name, const QString &objName,
                                          const QString &placeholderText, bool isBasic) const;

  CQChartsLineEdit *addRealEdit(QLayout *grid, int &row, int &column, const QString &name,
                                const QString &objName, const QString &placeholderText) const;
  CQChartsLineEdit *addStringEdit(QLayout *grid, int &row, int &column, const QString &name,
                                  const QString &objName, const QString &placeholderText) const;

  bool parsePosition(double &xmin, double &ymin, double &xmax, double &ymax) const;

  bool parseParameterColumnEdit(CQChartsPlotParameter *parameter, const PlotData &plotData,
                                CQChartsColumn &column, QString &columnType,
                                MapValueData &mapValueData);
  bool parseParameterColumnsEdit(CQChartsPlotParameter *parameter, const PlotData &plotData,
                                 CQChartsColumns &columns, QString &columnType);

  bool parseParameterStringEdit(CQChartsPlotParameter *parameter,
                                const PlotData &plotData, QString &str);
  bool parseParameterRealEdit(CQChartsPlotParameter *parameter,
                              const PlotData &plotData, double &r);
  bool parseParameterIntEdit(CQChartsPlotParameter *parameter,
                             const PlotData &plotData, int &i);
  bool parseParameterEnumEdit(CQChartsPlotParameter *parameter,
                              const PlotData &plotData, int &i);
  bool parseParameterBoolEdit(CQChartsPlotParameter *parameter,
                              const PlotData &plotData, bool &b);

  bool columnLineEditValue(const ColumnEditData &editData, CQChartsColumn &column,
                           const CQChartsColumn &defColumn=CQChartsColumn()) const;
  bool columnsLineEditValue(CQChartsColumnsLineEdit *le, CQChartsColumns &columns,
                            const CQChartsColumns &defColumns=CQChartsColumns()) const;

  bool validate(QStringList &msgs);

  void updatePreviewPlot(bool valid);

  void setXYMin(const QString &id);

  void applyPlot(CQChartsPlot *plot, bool preview=false);

  CQChartsPlotType *getPlotType() const;

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
  using TypePlotData    = std::map<QString,PlotData>;
  using TabType         = std::map<int,CQChartsPlotType *>;
  using TypeInitialized = std::map<QString,bool>;

  struct RangeEditData {
    CQChartsLineEdit* xminEdit   { nullptr }; //!< xmin edit
    QToolButton*      xminButton { nullptr }; //!< xmin load button
    CQChartsLineEdit* yminEdit   { nullptr }; //!< ymin edit
    QToolButton*      yminButton { nullptr }; //!< ymin load button
    CQChartsLineEdit* xmaxEdit   { nullptr }; //!< xmax edit
    QToolButton*      xmaxButton { nullptr }; //!< xmax load button
    CQChartsLineEdit* ymaxEdit   { nullptr }; //!< ymax edit
    QToolButton*      ymaxButton { nullptr }; //!< ymax load button
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

  CQCharts*                charts_                 { nullptr }; //!< parent charts
  CQChartsModelData*       modelData_              { nullptr }; //!< model data
  ModelP                   model_;                              //!< model

  // plot type widgets
  QStackedWidget*          plotDataStack_          { nullptr }; //!< plot widget stack
  QStackedWidget*          basicPlotDataStack_     { nullptr }; //!< basic plot widget stack
  QStackedWidget*          advancedPlotDataStack_  { nullptr }; //!< advanced plot widget stack
  TypePlotData             basicTypePlotData_;                  //!< per type plot data
  TypePlotData             advancedTypePlotData_;               //!< per type plot data
  bool                     advanced_               { false };   //!< is advanced
  TabType                  stackIndexPlotType_;                 //!< stacked index for plot type

  // filter edit
  QFrame*                  whereFrame_             { nullptr }; //!< where frame
  CQChartsLineEdit*        whereEdit_              { nullptr }; //!< where edit

  // general widgets
  CQChartsLineEdit*        viewEdit_               { nullptr }; //!< view name edit
  CQChartsLineEdit*        posEdit_                { nullptr }; //!< position edit
  QCheckBox*               autoRangeEdit_          { nullptr }; //!< auto range check
  CQChartsLineEdit*        titleEdit_              { nullptr }; //!< title edit
  CQChartsLineEdit*        xLabelEdit_             { nullptr }; //!< x label edit
  CQChartsLineEdit*        yLabelEdit_             { nullptr }; //!< y label edit
  RangeEditData            rangeEditData_;
  QCheckBox*               xintegralCheck_         { nullptr }; //!< x integral check
  QCheckBox*               yintegralCheck_         { nullptr }; //!< y integral check
  QCheckBox*               xlogCheck_              { nullptr }; //!< x log check
  QCheckBox*               ylogCheck_              { nullptr }; //!< y log check

  // description text
  QTextEdit*               descText_               { nullptr }; //!< type description

  QLabel*                  msgLabel_               { nullptr }; //!< message label
  QPushButton*             okButton_               { nullptr }; //!< ok button
  QPushButton*             applyButton_            { nullptr }; //!< apply button

  // summary widgets
  SummaryEditData          summaryEditData_;

  CQChartsModelDetailsWidget* detailsWidget_ { nullptr };

  // preview widgets
  QCheckBox*               previewEnabledCheck_    { nullptr }; //!< preview enabled checkbox
  CQChartsView*            previewView_            { nullptr }; //!< preview chart view
  CQChartsPlot*            previewPlot_            { nullptr }; //!< preview plot

  CQChartsPlot*            plot_                   { nullptr }; //!< last created plot (apply)
  bool                     initialized_            { false   }; //!< is initialized

  // auto analyze
  bool                     autoAnalyzeModel_       { true };    //!< auto analyze model
  TypeInitialized          basicTypeInitialzed_;                //!< is type initialized
  TypeInitialized          advancedTypeInitialzed_;             //!< is type initialized
};

#endif
