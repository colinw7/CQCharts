#ifndef CQChartsCreatePlotDlg_H
#define CQChartsCreatePlotDlg_H

#include <CQChartsPlotType.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <QDialog>
#include <QPointer>

class CQCharts;
class CQChartsPlotParameter;
class CQChartsView;
class CQChartsPlot;
class CQChartsColumnLineEdit;
class CQChartsColumnsLineEdit;
class CQChartsModelViewHolder;
class CQChartsModelData;
class CQSummaryModel;
class CQIntegerSpin;
class CQRealSpin;
class CQLineEdit;

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
    CQLineEdit*  formatEdit;
    QToolButton* formatUpdate;
  };

  struct PlotData {
    using ColumnEdits  = std::map<QString,CQChartsColumnLineEdit*>;
    using ColumnsEdits = std::map<QString,CQChartsColumnsLineEdit*>;
    using LineEdits    = std::map<QString,CQLineEdit*>;
    using WidgetEdits  = std::map<QString,QWidget*>;
    using FormatEdits  = std::map<QString,FormatEditData>;
    using Combos       = std::map<QString,QComboBox*>;
    using CheckBoxes   = std::map<QString,QCheckBox*>;
    using MapEdits     = std::map<QString,MapEditData>;

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
  QFrame *createSummaryFrame();
  QFrame *createPreviewFrame();

  void sortedPlotTypes(CQCharts::PlotTypes &sortedPlotTypes);

  void addPlotWidgets(CQChartsPlotType *type, int ind);

  void addParameterEdits(CQChartsPlotType *type, PlotData &plotData,
                         QGridLayout *layout, int &row);
  void addParameterEdits(const CQChartsPlotType::Parameters &parameters, PlotData &plotData,
                         QGridLayout *layout, int &row);

  void addParameterEdit(PlotData &plotData, QGridLayout *layout, int &row,
                        CQChartsPlotParameter *parameter);
  void addParameterEdit(PlotData &plotData, QHBoxLayout *layout,
                        CQChartsPlotParameter *parameter);

  void addParameterColumnEdit(PlotData &plotData, QGridLayout *layout, int &row,
                              CQChartsPlotParameter *parameter);

  void addParameterColumnsEdit(PlotData &plotData, QGridLayout *layout, int &row,
                               CQChartsPlotParameter *parameter);

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

  CQChartsColumnLineEdit *addColumnEdit(QLayout *grid, int &row, int &column,
                                        const QString &name, const QString &objName,
                                        const QString &placeholderText) const;
  CQChartsColumnsLineEdit *addColumnsEdit(QLayout *grid, int &row, int &column,
                                          const QString &name, const QString &objName,
                                          const QString &placeholderText) const;

  CQLineEdit *addRealEdit(QLayout *grid, int &row, int &column, const QString &name,
                          const QString &objName, const QString &placeholderText) const;
  CQLineEdit *addStringEdit(QLayout *grid, int &row, int &column, const QString &name,
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

  bool columnLineEditValue(CQChartsColumnLineEdit *le, CQChartsColumn &column,
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

 private slots:
  void comboSlot(const QString &desc);

  void autoAnalyzeSlot(int);

  void xminSlot();
  void yminSlot();
  void xmaxSlot();
  void ymaxSlot();

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

  CQCharts*                charts_                 { nullptr }; //!< parent charts
  CQChartsModelData*       modelData_              { nullptr }; //!< model data
  ModelP                   model_;                              //!< model
  QComboBox*               combo_                  { nullptr }; //!< type combo
  QStackedWidget*          stack_                  { nullptr }; //!< widget stack
  QTextEdit*               descText_               { nullptr }; //!< type description
  CQLineEdit*              whereEdit_              { nullptr }; //!< where edit
  CQLineEdit*              viewEdit_               { nullptr }; //!< view name edit
  CQLineEdit*              posEdit_                { nullptr }; //!< position edit
  CQLineEdit*              titleEdit_              { nullptr }; //!< title edit
  CQLineEdit*              xLabelEdit_             { nullptr }; //!< x label edit
  CQLineEdit*              yLabelEdit_             { nullptr }; //!< y label edit
  CQLineEdit*              xminEdit_               { nullptr }; //!< xmin edit
  QToolButton*             xminButton_             { nullptr }; //!< xmin load button
  CQLineEdit*              yminEdit_               { nullptr }; //!< ymin edit
  QToolButton*             yminButton_             { nullptr }; //!< ymin load button
  CQLineEdit*              xmaxEdit_               { nullptr }; //!< xmax edit
  QToolButton*             xmaxButton_             { nullptr }; //!< xmax load button
  CQLineEdit*              ymaxEdit_               { nullptr }; //!< ymax edit
  QToolButton*             ymaxButton_             { nullptr }; //!< ymax load button
  QCheckBox*               xintegralCheck_         { nullptr }; //!< x integral check
  QCheckBox*               yintegralCheck_         { nullptr }; //!< y integral check
  QCheckBox*               xlogCheck_              { nullptr }; //!< x log check
  QCheckBox*               ylogCheck_              { nullptr }; //!< y log check
  TypePlotData             typePlotData_;                       //!< per type plot data
  QLabel*                  msgLabel_               { nullptr }; //!< message label
  QPushButton*             okButton_               { nullptr }; //!< ok button
  QPushButton*             applyButton_            { nullptr }; //!< apply button
  TabType                  tabType_;                            //!< tab type map
  QCheckBox*               previewEnabledCheck_    { nullptr }; //!< preview enabled checkbox
  QCheckBox*               summaryEnabledCheck_    { nullptr }; //!< summary enabled checkbox
  CQIntegerSpin*           summaryMaxRows_         { nullptr }; //!< summary max rows
  QComboBox*               summaryTypeCombo_       { nullptr }; //!< summary type combo
  QStackedWidget*          summaryTypeStack_       { nullptr }; //!< summary type stack
  CQIntegerSpin*           summarySortedColEdit_   { nullptr }; //!< summary column edit
  CQIntegerSpin*           summaryPageSizeEdit_    { nullptr }; //!< summary page size edit
  CQIntegerSpin*           summaryCurrentPageEdit_ { nullptr }; //!< summary current page edit
  CQChartsModelViewHolder* summaryModelView_       { nullptr }; //!< summary model view
  CQChartsView*            previewView_            { nullptr }; //!< preview chart view
  CQChartsPlot*            previewPlot_            { nullptr }; //!< preview plot
  CQChartsPlot*            plot_                   { nullptr }; //!< last created plot (apply)
  bool                     initialized_            { false   }; //!< is initialized
  bool                     autoAnalyzeModel_       { true };    //!< auto analyze model
  TypeInitialized          typeInitialzed_;                     //!< is type initialized
};

#endif
