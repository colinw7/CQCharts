#ifndef CQChartsPlotDlg_H
#define CQChartsPlotDlg_H

#include <CQChartsModelP.h>
#include <CQChartsColumn.h>
#include <QDialog>

class CQCharts;
class CQChartsPlotType;
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
class QGridLayout;
class QHBoxLayout;
class QComboBox;
class QStackedWidget;
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
  CQChartsPlotDlg(CQCharts *charts, const CQChartsModelP &model);
 ~CQChartsPlotDlg();

  void setSelectionModel(QItemSelectionModel *sm);
  QItemSelectionModel *selectionModel() const;

  CQCharts *charts() const { return charts_; }

  QAbstractItemModel *model() const { return model_.data(); }

  void setViewName(const QString &viewName);

  int exec();

  CQChartsPlot *plot() const { return plot_; }

 signals:
  void plotCreated(CQChartsPlot *plot);

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
    using FormatEdits = std::map<QString,FormatEditData>;
    using CheckBoxes  = std::map<QString,QCheckBox*>;
    using MapEdits    = std::map<QString,MapEditData>;

    ColumnEdits  columnEdits;
    LineEdits    columnsEdits;
    MapEdits     mappedEdits;
    FormatEdits  formatEdits;
    CheckBoxes   boolEdits;
    LineEdits    stringEdits;
    LineEdits    realEdits;
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

  void addPlotWidgets(const QString &typeName, int ind);

  void addParameterEdits(CQChartsPlotType *type, PlotData &plotData,
                         QGridLayout *layout, int &row);

  void addParameterColumnEdit(PlotData &plotData, QGridLayout *layout, int &row,
                              const CQChartsPlotParameter &parameter);

  void addParameterColumnsEdit(PlotData &plotData, QGridLayout *layout, int &row,
                               const CQChartsPlotParameter &parameter);

  void addParameterBoolEdit(PlotData &plotData, QHBoxLayout *layout,
                            const CQChartsPlotParameter &parameter);

  void addParameterStringEdit(PlotData &plotData, QHBoxLayout *layout,
                              const CQChartsPlotParameter &parameter);

  void addParameterRealEdit(PlotData &plotData, QHBoxLayout *layout,
                            const CQChartsPlotParameter &parameter);

  QLineEdit *addStringEdit(QGridLayout *grid, int &row, int &column, const QString &name,
                           const QString &objName, const QString &placeholderText) const;
  QLineEdit *addRealEdit(QGridLayout *grid, int &row, int &column, const QString &name,
                         const QString &objName, const QString &placeholderText) const;
  CQChartsColumnEdit *addColumnEdit(QGridLayout *grid, int &row, int &column, const QString &name,
                                    const QString &objName, const QString &placeholderText) const;

  bool parsePosition(double &xmin, double &ymin, double &xmax, double &ymax) const;

  bool parseParameterColumnEdit(const CQChartsPlotParameter &parameter, const PlotData &plotData,
                                CQChartsColumn &column, QString &columnStr, QString &columnType,
                                MapValueData &mapValueData);
  bool parseParameterColumnsEdit(const CQChartsPlotParameter &parameter,
                                 const PlotData &plotData, std::vector<CQChartsColumn> &columns,
                                 QStringList &columnStrs, QString &columnType);
  bool parseParameterStringEdit(const CQChartsPlotParameter &parameter,
                                const PlotData &plotData, QString &str);
  bool parseParameterRealEdit(const CQChartsPlotParameter &parameter,
                              const PlotData &plotData, double &rtr);
  bool parseParameterBoolEdit(const CQChartsPlotParameter &parameter,
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

  void updatePreviewPlot();

  void applyPlot(CQChartsPlot *plot, bool preview=false);

 private slots:
  void comboSlot(int ind);

  void validateSlot();

  void updateFormatSlot();

  void previewEnabledSlot();
  void updatePreviewSlot();

  void okSlot();
  bool applySlot();
  void cancelSlot();

 private:
  using TypePlotData = std::map<QString,PlotData>;
  using TabTypeName  = std::map<int,QString>;

  CQCharts*          charts_              { nullptr }; // parent charts
  CQChartsModelP     model_;                           // model
  SelectionModelP    selectionModel_;                  // selection model
  CQSummaryModel*    summaryModel_        { nullptr }; // summary model
  ModelP             summaryModelP_;
  CQChartsModelData* summaryModelData_    { nullptr };
  QComboBox*         combo_               { nullptr }; // type combo
  QStackedWidget*    stack_               { nullptr }; // widget stack
  QLineEdit*         whereEdit_           { nullptr }; // where edit
  QLineEdit*         viewEdit_            { nullptr }; // view name edit
  QLineEdit*         posEdit_             { nullptr }; // position edit
  QLineEdit*         titleEdit_           { nullptr }; // title edit
  QLineEdit*         xminEdit_            { nullptr }; // xmin edit
  QLineEdit*         yminEdit_            { nullptr }; // ymin edit
  QLineEdit*         xmaxEdit_            { nullptr }; // xmax edit
  QLineEdit*         ymaxEdit_            { nullptr }; // ymax edit
  QCheckBox*         xintegralCheck_      { nullptr }; // x integral check
  QCheckBox*         yintegralCheck_      { nullptr }; // y integral check
  QCheckBox*         xlogCheck_           { nullptr }; // x log check
  QCheckBox*         ylogCheck_           { nullptr }; // y log check
  TypePlotData       typePlotData_;                    // per type plot data
  QLabel*            msgLabel_            { nullptr }; // message label
  QPushButton*       okButton_            { nullptr }; // ok button
  QPushButton*       applyButton_         { nullptr }; // apply button
  TabTypeName        tabTypeName_;                     // tab type name map
  QCheckBox*         previewEnabledCheck_ { nullptr };
  CQIntegerSpin*     previewMaxRows_      { nullptr };
  QRadioButton*      previewNormalRadio_  { nullptr };
  QRadioButton*      previewRandomRadio_  { nullptr };
  QRadioButton*      previewSortedRadio_  { nullptr };
  CQIntegerSpin*     previewSortedColumnEdit_ { nullptr };
  CQChartsModelView* previewTable_        { nullptr };
  CQChartsView*      previewView_         { nullptr };
  CQChartsPlot*      previewPlot_         { nullptr };
  CQChartsPlot*      plot_                { nullptr }; // created plot
  bool               initialized_         { false   };
};

#endif
