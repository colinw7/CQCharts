#ifndef CQChartsTest_H
#define CQChartsTest_H

#include <CQAppWindow.h>
#include <CBBox2D.h>

class CQCharts;
class CQChartsTable;
class CQChartsTree;
class CQChartsLoader;
class CQChartsWindow;

class CQGradientPalette;
class CQGradientPaletteControl;

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
  struct InitData {
    typedef std::map<int,QString> ArgMap;

    bool                 csv  { false };
    bool                 tsv  { false };
    bool                 json { false };
    bool                 data { false };
    QString              plot;
    ArgMap               argMap;
    QString              format;
    bool                 bivariate { false };
    bool                 stacked   { false };
    std::vector<QString> filenames;
    bool                 commentHeader   { false };
    bool                 firstLineHeader { false };

    QString arg(int i) const {
      auto p = argMap.find(i);

      return (p != argMap.end() ? (*p).second : QString());
    }

    void setArg(int i, const QString &arg) {
      argMap[i] = arg;
    }
  };

 public:
  CQChartsTest();

  const QString &id() const { return id_; }
  void setId(const QString &v) { id_ = v; }

  const CBBox2D &bbox() const { return bbox_; }
  void setBBox(const CBBox2D &v) { bbox_ = v; }

  CQChartsWindow *window() const { return window_; }

  void loadCsv (const QString &filename);
  void loadTsv (const QString &filename, bool commentHeader, bool firstLineHeader);
  void loadJson(const QString &filename);
  void loadData(const QString &filename);

  void init(const InitData &initData);

 private:
  void addMenus();

  void addPieTab      (QTabWidget *plotTab);
  void addXYTab       (QTabWidget *plotTab);
  void addScatterTab  (QTabWidget *plotTab);
  void addSunburstTab (QTabWidget *plotTab);
  void addBarChartTab (QTabWidget *plotTab);
  void addBoxTab      (QTabWidget *plotTab);
  void addParallelTab (QTabWidget *plotTab);
  void addGeometryTab (QTabWidget *plotTab);
  void addDelaunayTab (QTabWidget *plotTab);
  void addAdjacencyTab(QTabWidget *plotTab);

  void setTableModel(QAbstractItemModel *model);
  void setTreeModel (QAbstractItemModel *model);

  QLineEdit *addLineEdit(QGridLayout *grid, int &row, const QString &name,
                         const QString &objName) const;

  bool lineEditValue(QLineEdit *le, int &i, int defi=0) const;

  CQChartsWindow *getWindow();

  QSize sizeHint() const;

 private slots:
  void loadSlot();

  void loadFileSlot(const QString &type, const QString &filename);

  void filterSlot();
  void tableColumnClicked(int column);
  void typeOKSlot();

  void pieOKSlot();
  void xyOKSlot();
  void scatterOKSlot();
  void sunburstOKSlot();
  void barChartOKSlot();
  void boxOKSlot();
  void parallelOKSlot();
  void geometryOKSlot();
  void delaunayOKSlot();
  void adjacencyOKSlot();

 private:
  struct PlotData {
    QPushButton* okButton { nullptr };
  };

  struct PieChartData : public PlotData {
    QLineEdit* labelEdit { nullptr };
    QLineEdit* dataEdit  { nullptr };
  };

  struct XYPlotData : public PlotData {
    QLineEdit* xEdit          { nullptr };
    QLineEdit* yEdit          { nullptr };
    QLineEdit* nameEdit       { nullptr };
    QCheckBox* bivariateCheck { nullptr };
    QCheckBox* stackedCheck   { nullptr };
  };

  struct ScatterPlotData : public PlotData {
    QLineEdit* nameEdit { nullptr };
    QLineEdit* xEdit    { nullptr };
    QLineEdit* yEdit    { nullptr };
  };

  struct SunburstData : public PlotData {
    QLineEdit* nameEdit  { nullptr };
    QLineEdit* valueEdit { nullptr };
  };

  struct BarChartData : public PlotData {
    QLineEdit* nameEdit     { nullptr };
    QLineEdit* valueEdit    { nullptr };
    QCheckBox* stackedCheck { nullptr };
  };

  struct BoxPlotData : public PlotData {
    QLineEdit* xEdit { nullptr };
    QLineEdit* yEdit { nullptr };
  };

  struct ParallelPlotData : public PlotData {
    QLineEdit* xEdit { nullptr };
    QLineEdit* yEdit { nullptr };
  };

  struct GeometryPlotData : public PlotData {
    QLineEdit* nameEdit     { nullptr };
    QLineEdit* geometryEdit { nullptr };
    QLineEdit* valueEdit    { nullptr };
  };

  struct DelaunayPlotData : public PlotData {
    QLineEdit* xEdit    { nullptr };
    QLineEdit* yEdit    { nullptr };
    QLineEdit* nameEdit { nullptr };
  };

  struct AdjacencyPlotData : public PlotData {
    QLineEdit* nodeEdit        { nullptr };
    QLineEdit* connectionsEdit { nullptr };
    QLineEdit* nameEdit        { nullptr };
    QLineEdit* groupEdit       { nullptr };
  };

  CQCharts*                 charts_            { nullptr };
  QAbstractItemModel*       model_             { nullptr };
  PieChartData              pieChartData_;
  XYPlotData                xyPlotData_;
  ScatterPlotData           scatterPlotData_;
  SunburstData              sunburstData_;
  BarChartData              barChartData_;
  BoxPlotData               boxPlotData_;
  ParallelPlotData          parallelPlotData_;
  GeometryPlotData          geometryPlotData_;
  DelaunayPlotData          delaunayPlotData_;
  AdjacencyPlotData         adjacencyPlotData_;
  QTabWidget*               plotTab_           { nullptr };
  QLineEdit*                columnTypeEdit_    { nullptr };
  QStackedWidget*           stack_             { nullptr };
  QLineEdit*                filterEdit_        { nullptr };
  CQChartsTable*            table_             { nullptr };
  CQChartsTree*             tree_              { nullptr };
  CQGradientPalette*        palettePlot_       { nullptr };
  CQGradientPaletteControl* paletteControl_    { nullptr };
  QGroupBox*                typeGroup_         { nullptr };
  int                       tableColumn_       { 0 };
  CQChartsLoader*           loader_            { nullptr };
  CQChartsWindow*           window_            { nullptr };
  QString                   id_;
  CBBox2D                   bbox_;
};

#endif
