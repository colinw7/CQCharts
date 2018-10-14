#ifndef CQChartsPlotDlg_H
#define CQChartsPlotDlg_H

#include <CQChartsPlotType.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <QDialog>
#include <QPointer>

class CQCharts;
class CQChartsPlotParameter;
class CQChartsView;
class CQChartsPlot;
class CQChartsColumnEdit;
class CQChartsModelView;
class CQChartsModelData;
class CQSummaryModel;
class CQIntegerSpin;
class CQRealSpin;

class QAbstractItemModel;
class QItemSelectionModel;
class QFrame;
class QGridLayout;
class QHBoxLayout;
class QComboBox;
class QStackedWidget;
class QTextEdit;
class QLineEdit;
class QCheckBox;
class QRadioButton;
class QToolButton;
class QLabel;

class CQChartsPlotDlg : public QDialog {
  Q_OBJECT

 public:
  using ModelP          = QSharedPointer<QAbstractItemModel>;
  using SelectionModelP = QPointer<QItemSelectionModel>;

 public:
  CQChartsPlotDlg(CQCharts *charts, CQChartsModelData *modelData);
 ~CQChartsPlotDlg();

  void setSelectionModel(QItemSelectionModel *sm);
  QItemSelectionModel *selectionModel() const;

  CQCharts *charts() const { return charts_; }

  QAbstractItemModel *model() const { return model_.data(); }

  void setViewName(const QString &viewName);

  int exec();

  CQChartsPlot *plot() const { return plot_; }

 private:
  struct MapEditData {
    QCheckBox  *mappedCheck { nullptr };
    CQRealSpin *mapMinSpin  { nullptr };
    CQRealSpin *mapMaxSpin  { nullptr };
  };

  struct FormatEditData {
    QLineEdit   *formatEdit;
    QToolButton *formatUpdate;
  };

  struct PlotData {
    using ColumnEdits = std::map<QString,CQChartsColumnEdit*>;
    using LineEdits   = std::map<QString,QLineEdit*>;
    using WidgetEdits = std::map<QString,QWidget*>;
    using FormatEdits = std::map<QString,FormatEditData>;
    using Combos      = std::map<QString,QComboBox*>;
    using CheckBoxes  = std::map<QString,QCheckBox*>;
    using MapEdits    = std::map<QString,MapEditData>;

    ColumnEdits  columnEdits;
    LineEdits    columnsEdits;
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

  QFrame *createTypeDataFrame();
  QFrame *createGeneralDataFrame();
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

  QLineEdit *addRealEdit(QLayout *grid, int &row, int &column, const QString &name,
                         const QString &objName, const QString &placeholderText) const;
  CQChartsColumnEdit *addColumnEdit(QLayout *grid, int &row, int &column, const QString &name,
                                    const QString &objName, const QString &placeholderText) const;
  QLineEdit *addStringEdit(QLayout *grid, int &row, int &column, const QString &name,
                           const QString &objName, const QString &placeholderText) const;

  bool parsePosition(double &xmin, double &ymin, double &xmax, double &ymax) const;

  bool parseParameterColumnEdit(CQChartsPlotParameter *parameter, const PlotData &plotData,
                                CQChartsColumn &column, QString &columnStr, QString &columnType,
                                MapValueData &mapValueData);
  bool parseParameterColumnsEdit(CQChartsPlotParameter *parameter,
                                 const PlotData &plotData, std::vector<CQChartsColumn> &columns,
                                 QStringList &columnStrs, QString &columnType);
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

  bool columnLineEditValue(CQChartsColumnEdit *le, CQChartsColumn &column, QString &columnStr,
                           QString &columnType,
                           const CQChartsColumn &defColumn=CQChartsColumn()) const;
  bool columnLineEditValue(QLineEdit *le, CQChartsColumn &column, QString &columnStr,
                           QString &columnType,
                           const CQChartsColumn &defColumn=CQChartsColumn()) const;
  bool columnTextValue(QString &str, CQChartsColumn &column, QString &columnStr,
                       QString &columnType,
                       const CQChartsColumn &defColumn=CQChartsColumn()) const;

  bool columnLineEditValues(QLineEdit *le, std::vector<CQChartsColumn> &columns,
                            QStringList &columnStrs, QString &columnType) const;

  bool stringToColumn(const QString &str, CQChartsColumn &column) const;

  bool validate(QStringList &msgs);

  void updatePreviewPlot(bool valid);

  void setXYMin(const QString &id);

  void applyPlot(CQChartsPlot *plot, bool preview=false);

  CQChartsPlotType *getPlotType() const;

 private slots:
  void comboSlot(const QString &desc);

  void xminSlot();
  void yminSlot();
  void xmaxSlot();
  void ymaxSlot();

  void validateSlot();

  void updateFormatSlot();

  void previewEnabledSlot();
  void summaryEnabledSlot();
  void updatePreviewSlot();

  void okSlot();
  bool applySlot();
  void cancelSlot();

 private:
  using TypePlotData    = std::map<QString,PlotData>;
  using TabType         = std::map<int,CQChartsPlotType *>;
  using TypeInitialized = std::map<QString,bool>;

  CQCharts*          charts_                 { nullptr }; // parent charts
  CQChartsModelData* modelData_              { nullptr }; // model data
  ModelP             model_;                              // model
  SelectionModelP    selectionModel_;                     // selection model
  QComboBox*         combo_                  { nullptr }; // type combo
  QStackedWidget*    stack_                  { nullptr }; // widget stack
  QTextEdit*         descText_               { nullptr }; // type description
  QLineEdit*         whereEdit_              { nullptr }; // where edit
  QLineEdit*         viewEdit_               { nullptr }; // view name edit
  QLineEdit*         posEdit_                { nullptr }; // position edit
  QLineEdit*         titleEdit_              { nullptr }; // title edit
  QLineEdit*         xLabelEdit_             { nullptr }; // x label edit
  QLineEdit*         yLabelEdit_             { nullptr }; // y label edit
  QLineEdit*         xminEdit_               { nullptr }; // xmin edit
  QToolButton*       xminButton_             { nullptr }; // xmin load button
  QLineEdit*         yminEdit_               { nullptr }; // ymin edit
  QToolButton*       yminButton_             { nullptr }; // ymin load button
  QLineEdit*         xmaxEdit_               { nullptr }; // xmax edit
  QToolButton*       xmaxButton_             { nullptr }; // xmax load button
  QLineEdit*         ymaxEdit_               { nullptr }; // ymax edit
  QToolButton*       ymaxButton_             { nullptr }; // ymax load button
  QCheckBox*         xintegralCheck_         { nullptr }; // x integral check
  QCheckBox*         yintegralCheck_         { nullptr }; // y integral check
  QCheckBox*         xlogCheck_              { nullptr }; // x log check
  QCheckBox*         ylogCheck_              { nullptr }; // y log check
  TypePlotData       typePlotData_;                       // per type plot data
  QLabel*            msgLabel_               { nullptr }; // message label
  QPushButton*       okButton_               { nullptr }; // ok button
  QPushButton*       applyButton_            { nullptr }; // apply button
  TabType            tabType_;                            // tab type map
  QCheckBox*         previewEnabledCheck_    { nullptr }; // preview enabled checkbox
  QCheckBox*         summaryEnabledCheck_    { nullptr }; // summary enabled checkbox
  CQIntegerSpin*     previewMaxRows_         { nullptr }; // preview summary max rows
  QRadioButton*      previewNormalRadio_     { nullptr }; // preview summary normal type
  QRadioButton*      previewRandomRadio_     { nullptr }; // preview summary random type
  QRadioButton*      previewSortedRadio_     { nullptr }; // preview summary sorted type
  QRadioButton*      previewPagedRadio_      { nullptr }; // preview summary paged type
  CQIntegerSpin*     previewSortedColEdit_   { nullptr }; // preview summary column edit
  CQIntegerSpin*     previewPageSizeEdit_    { nullptr }; // preview summary page size edit
  CQIntegerSpin*     previewCurrentPageEdit_ { nullptr }; // preview summary current page edit
  CQChartsModelView* previewModelView_       { nullptr }; // preview model view
  CQChartsView*      previewView_            { nullptr }; // preview chart view
  CQChartsPlot*      previewPlot_            { nullptr }; // preview plot
  CQChartsPlot*      plot_                   { nullptr }; // last created plot (apply)
  bool               initialized_            { false   }; // is initialized
  TypeInitialized    typeInitialzed_;                     // is type initialized
};

#endif
