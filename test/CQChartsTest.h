#ifndef CQChartsTest_H
#define CQChartsTest_H

#include <CQAppWindow.h>
#include <CBBox2D.h>
#include <QPointer>
#include <map>

#include <boost/optional.hpp>

class CQCharts;
class CQChartsTable;
class CQChartsTree;
class CQChartsLoader;
class CQChartsView;
class CQChartsPlot;
class CQChartsPlotType;
class CQChartsPlotParameter;
class CQChartsCsv;
class CQChartsTsv;

class QAbstractItemModel;
class QStackedWidget;
class QTabWidget;
class QGroupBox;
class QLineEdit;
class QCheckBox;
class QPushButton;
class QGridLayout;

class CQChartsTest : public CQAppWindow {
  Q_OBJECT

 public:
  typedef boost::optional<double> OptReal;

 public:
  struct InitData {
    typedef std::map<QString,QString> NameValues;
    typedef std::map<QString,bool>    NameBools;
    typedef std::vector<QString>      FileNames;

    bool       csv             { false };
    bool       tsv             { false };
    bool       json            { false };
    bool       data            { false };
    QString    plot;
    NameValues nameValues;
    NameBools  nameBools;
    QString    format;
    FileNames  filenames;
    bool       commentHeader   { false };
    bool       firstLineHeader { false };
    bool       xintegral       { false };
    bool       yintegral       { false };
    QString    title;
    OptReal    xmin, ymin, xmax, ymax;
    bool       y1y2            { false };
    bool       overlay         { false };
    int        nr              { 1 };
    int        nc              { 1 };
    double     dx              { 1000 };
    double     dy              { 1000 };

    QString nameValue(const QString &name) const {
      auto p = nameValues.find(name);

      return (p != nameValues.end() ? (*p).second : QString());
    }

    void setNameValue(const QString &name, const QString &value) {
      nameValues[name] = value;
    }

    bool nameBool(const QString &name) const {
      auto p = nameBools.find(name);

      return (p != nameBools.end() ? (*p).second : false);
    }

    void setNameBool(const QString &name, bool value) {
      nameBools[name] = value;
    }
  };

 private:
  struct PlotData {
    typedef std::map<QString, QLineEdit *> LineEdits;
    typedef std::map<QString, QCheckBox *> CheckBoxes;

    LineEdits    columnEdits;
    LineEdits    columnsEdits;
    CheckBoxes   boolEdits;
    QPushButton* okButton { nullptr };
    int          tabInd { -1 };
  };

 public:
  CQChartsTest();

 ~CQChartsTest();

  const QString &id() const { return id_; }
  void setId(const QString &v) { id_ = v; }

  const CBBox2D &bbox() const { return bbox_; }
  void setBBox(const CBBox2D &v) { bbox_ = v; }

  CQChartsView *view() const;

  void loadCsv (const QString &filename, bool commentHeader, bool firstLineHeader);
  void loadTsv (const QString &filename, bool commentHeader, bool firstLineHeader);
  void loadJson(const QString &filename);
  void loadData(const QString &filename, bool commentHeader, bool firstLineHeader);

  bool initPlot(const InitData &initData);

  CQChartsPlot *init(const InitData &initData, int i);

 private:
  void addMenus();

  void addPlotTab      (QTabWidget *plotTab, const QString &typeName);
  void addScatterTab   (QTabWidget *plotTab);
  void addSunburstTab  (QTabWidget *plotTab);
  void addBarChartTab  (QTabWidget *plotTab);
  void addBoxTab       (QTabWidget *plotTab);
  void addParallelTab  (QTabWidget *plotTab);
  void addGeometryTab  (QTabWidget *plotTab);
  void addDelaunayTab  (QTabWidget *plotTab);
  void addAdjacencyTab (QTabWidget *plotTab);
  void addBubbleTab    (QTabWidget *plotTab);
  void addHierBubbleTab(QTabWidget *plotTab);
  void addTreeMapTab   (QTabWidget *plotTab);

  void setTableModel(QAbstractItemModel *model);
  void setTreeModel (QAbstractItemModel *model);

  void addParameterEdits(CQChartsPlotType *type, PlotData &plotData,
                         QGridLayout *layout, int &row);

  void addParameterColumnEdit(PlotData &plotData, QGridLayout *layout, int &row,
                              const CQChartsPlotParameter &parameter);
  void addParameterColumnsEdit(PlotData &plotData, QGridLayout *layout, int &row,
                               const CQChartsPlotParameter &parameter);
  void addParameterBoolEdit(PlotData &plotData, QGridLayout *layout, int &row,
                            const CQChartsPlotParameter &parameter);

  QLineEdit *addLineEdit(QGridLayout *grid, int &row, const QString &name,
                         const QString &objName) const;

  bool parseParameterColumnEdit(const CQChartsPlotParameter &parameter,
                                const PlotData &plotData, int &column);
  bool parseParameterColumnsEdit(const CQChartsPlotParameter &parameter,
                                 const PlotData &plotData, std::vector<int> &columns);
  bool parseParameterBoolEdit(const CQChartsPlotParameter &parameter,
                              const PlotData &plotData, bool &b);

  bool lineEditValue(QLineEdit *le, int &i, int defi=0) const;

  bool lineEditValues(QLineEdit *le, std::vector<int> &ivals) const;

  void setEditsFromInitData(const PlotData &plotData, const InitData &initData);

  CQChartsView *getView(bool reuse=true);

  QSize sizeHint() const;

 private slots:
  void loadSlot();

  void loadFileSlot(const QString &type, const QString &filename);

  void filterSlot();
  void tableColumnClicked(int column);
  void typeOKSlot();

  CQChartsPlot *tabOKSlot(bool reuse=false);

 private:
  typedef std::vector<CQChartsPlot *> Plots;
  typedef std::map<QString,PlotData>  TypePlotData;
  typedef std::map<int,QString>       TabTypeName;
  typedef QPointer<CQChartsView>      ViewP;

  Plots               plots_;
  CQChartsPlot*       rootPlot_          { nullptr };
  CQCharts*           charts_            { nullptr };
  CQChartsCsv*        csv_               { nullptr };
  CQChartsTsv*        tsv_               { nullptr };
  QAbstractItemModel* model_             { nullptr };
  TypePlotData        typePlotData_;
  TabTypeName         tabTypeName_;
  QTabWidget*         plotTab_           { nullptr };
  QLineEdit*          columnTypeEdit_    { nullptr };
  QStackedWidget*     stack_             { nullptr };
  QLineEdit*          filterEdit_        { nullptr };
  CQChartsTable*      table_             { nullptr };
  CQChartsTree*       tree_              { nullptr };
  QGroupBox*          typeGroup_         { nullptr };
  int                 tableColumn_       { 0 };
  CQChartsLoader*     loader_            { nullptr };
  ViewP               view_;
  QString             id_;
  CBBox2D             bbox_;
};

#endif
