#ifndef CQChartsPlotDlg_H
#define CQChartsPlotDlg_H

#include <CQChartsModelP.h>
#include <QDialog>

class CQCharts;
class CQChartsPlotType;
class CQChartsPlotParameter;
class CQChartsPlot;

class QAbstractItemModel;
class QItemSelectionModel;
class QGridLayout;
class QHBoxLayout;
class QComboBox;
class QStackedWidget;
class QLineEdit;
class QCheckBox;

class CQChartsPlotDlg : public QDialog {
  Q_OBJECT

 public:
  using SelectionModelP = QPointer<QItemSelectionModel>;

 public:
  CQChartsPlotDlg(CQCharts *charts, const CQChartsModelP &model);

  void setSelectionModel(QItemSelectionModel *sm);
  QItemSelectionModel *selectionModel() const;

  CQCharts *charts() const { return charts_; }

  QAbstractItemModel *model() const { return model_.data(); }

  int exec();

  CQChartsPlot *plot() const { return plot_; }

 signals:
  void plotCreated(CQChartsPlot *plot);

 private:
  struct PlotData {
    using LineEdits  = std::map<QString,QLineEdit*>;
    using CheckBoxes = std::map<QString,QCheckBox*>;

    LineEdits    columnEdits;
    LineEdits    columnsEdits;
    LineEdits    formatEdits;
    CheckBoxes   boolEdits;
    LineEdits    stringEdits;
    LineEdits    realEdits;
    QPushButton* okButton { nullptr };
    int          ind      { -1 };
  };

 private:
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

  QLineEdit *addLineEdit(QGridLayout *grid, int &row, int &column, const QString &name,
                         const QString &objName, const QString &placeholderText) const;

  bool parsePosition(double &xmin, double &ymin, double &xmax, double &ymax) const;

  bool parseParameterColumnEdit(const CQChartsPlotParameter &parameter, const PlotData &plotData,
                                int &column, QString &columnStr, QString &columnType);
  bool parseParameterColumnsEdit(const CQChartsPlotParameter &parameter,
                                 const PlotData &plotData, std::vector<int> &columns,
                                 QStringList &columnStrs, QString &columnType);
  bool parseParameterStringEdit(const CQChartsPlotParameter &parameter,
                                const PlotData &plotData, QString &str);
  bool parseParameterRealEdit(const CQChartsPlotParameter &parameter,
                              const PlotData &plotData, double &rtr);
  bool parseParameterBoolEdit(const CQChartsPlotParameter &parameter,
                              const PlotData &plotData, bool &b);

  bool lineEditValue(QLineEdit *le, int &column, QString &columnStr,
                     QString &columnType, int defi=0) const;

  bool lineEditValues(QLineEdit *le, std::vector<int> &columns,
                      QStringList &columnStrs, QString &columnType) const;

  bool stringToColumn(const QString &str, int &column) const;

 private slots:
  void comboSlot(int ind);

  bool applySlot();
  void okSlot();

 private:
  using TypePlotData = std::map<QString,PlotData>;
  using TabTypeName  = std::map<int,QString>;

  CQCharts*       charts_         { nullptr }; // parent charts
  CQChartsModelP  model_;                      // model
  SelectionModelP selectionModel_;             // selection model
  QComboBox*      combo_          { nullptr }; // type combo
  QStackedWidget* stack_          { nullptr }; // widget stack
  QLineEdit*      viewEdit_       { nullptr }; // view name edit
  QLineEdit*      posEdit_        { nullptr }; // position edit
  QLineEdit*      titleEdit_      { nullptr }; // title edit
  QLineEdit*      xminEdit_       { nullptr }; // xmin edit
  QLineEdit*      yminEdit_       { nullptr }; // ymin edit
  QLineEdit*      xmaxEdit_       { nullptr }; // xmax edit
  QLineEdit*      ymaxEdit_       { nullptr }; // ymax edit
  QCheckBox*      xintegralCheck_ { nullptr }; // x integral check
  QCheckBox*      yintegralCheck_ { nullptr }; // y integral check
  TypePlotData    typePlotData_;               // per type plot data
  TabTypeName     tabTypeName_;                // tab type name map
  CQChartsPlot*   plot_           { nullptr }; // created plot
};

#endif
