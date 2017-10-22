#ifndef CQChartsPlotDlg_H
#define CQChartsPlotDlg_H

#include <QDialog>

class CQCharts;
class CQChartsPlotType;
class CQChartsPlotParameter;
class CQChartsPlot;

class QAbstractItemModel;
class QGridLayout;
class QHBoxLayout;
class QComboBox;
class QStackedWidget;
class QLineEdit;
class QCheckBox;

class CQChartsPlotDlg : public QDialog {
  Q_OBJECT

 public:
  typedef QSharedPointer<QAbstractItemModel> ModelP;

 public:
  CQChartsPlotDlg(CQCharts *charts, const ModelP &model);

  CQCharts *charts() const { return charts_; }

  int exec();

  CQChartsPlot *plot() const { return plot_; }

 signals:
  void plotCreated(CQChartsPlot *plot);

 private:
  struct PlotData {
    typedef std::map<QString, QLineEdit *> LineEdits;
    typedef std::map<QString, QCheckBox *> CheckBoxes;

    LineEdits    columnEdits;
    LineEdits    columnsEdits;
    LineEdits    formatEdits;
    CheckBoxes   boolEdits;
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

  QLineEdit *addLineEdit(QGridLayout *grid, int &row, int &column, const QString &name,
                         const QString &objName, const QString &placeholderText) const;

  bool parsePosition(double &xmin, double &ymin, double &xmax, double &ymax) const;

  bool parseParameterColumnEdit(const CQChartsPlotParameter &parameter, const PlotData &plotData,
                                int &column, QString &columnStr, QString &columnType);
  bool parseParameterColumnsEdit(const CQChartsPlotParameter &parameter,
                                 const PlotData &plotData, std::vector<int> &columns,
                                 QStringList &columnStrs, QString &columnType);
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
  typedef std::map<QString,PlotData> TypePlotData;
  typedef std::map<int,QString>      TabTypeName;

  CQCharts*       charts_         { nullptr };
  ModelP          model_;
  QComboBox*      combo_          { nullptr };
  QStackedWidget* stack_          { nullptr };
  QLineEdit*      viewEdit_       { nullptr };
  QLineEdit*      posEdit_        { nullptr };
  QLineEdit*      titleEdit_      { nullptr };
  QLineEdit*      xminEdit_       { nullptr };
  QLineEdit*      yminEdit_       { nullptr };
  QLineEdit*      xmaxEdit_       { nullptr };
  QLineEdit*      ymaxEdit_       { nullptr };
  QCheckBox*      xintegralCheck_ { nullptr };
  QCheckBox*      yintegralCheck_ { nullptr };
  TypePlotData    typePlotData_;
  TabTypeName     tabTypeName_;
  CQChartsPlot*   plot_           { nullptr };
};

#endif
