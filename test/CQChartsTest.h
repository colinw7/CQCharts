#ifndef CQChartsTest_H
#define CQChartsTest_H

#include <CQAppWindow.h>

class CQCharts;
class CQChartsTable;
class CQChartsTree;
class CQChartsLoader;
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
  CQChartsTest();

  void loadCsv (const QString &filename);
  void loadTsv (const QString &filename);
  void loadJson(const QString &filename);
  void loadData(const QString &filename);

  void init(const QString &plot, const QString &xarg, const QString &yarg,
            const QString &zarg, const QString &format);

 private:
  void addMenus();

  void addPieTab     (QTabWidget *plotTab);
  void addXYTab      (QTabWidget *plotTab);
  void addSunburstTab(QTabWidget *plotTab);
  void addBarChartTab(QTabWidget *plotTab);
  void addBoxTab     (QTabWidget *plotTab);
  void addParallelTab(QTabWidget *plotTab);

  void setTableModel(QAbstractItemModel *model);
  void setTreeModel (QAbstractItemModel *model);

  QLineEdit *addLineEdit(QGridLayout *grid, int &row, const QString &name,
                         const QString &objName) const;

  bool lineEditValue(QLineEdit *le, int &i, int defi=0) const;

  QSize sizeHint() const;

 private slots:
  void loadSlot();

  void loadFileSlot(const QString &type, const QString &filename);

  void tableColumnClicked(int column);
  void typeOKSlot();

  void pieOKSlot();
  void xyOKSlot();
  void sunburstOKSlot();
  void barChartOKSlot();
  void boxOKSlot();
  void parallelOKSlot();

 private:
  struct PieChartData {
    QLineEdit*   labelEdit { nullptr };
    QLineEdit*   dataEdit  { nullptr };
    QPushButton* okButton  { nullptr };
  };

  struct XYPlotData {
    QLineEdit*   xEdit          { nullptr };
    QLineEdit*   yEdit          { nullptr };
    QLineEdit*   nameEdit       { nullptr };
    QCheckBox*   bivariateCheck { nullptr };
    QPushButton* okButton       { nullptr };
  };

  struct SunburstData {
    QLineEdit*   nameEdit  { nullptr };
    QLineEdit*   valueEdit { nullptr };
    QPushButton* okButton  { nullptr };
  };

  struct BarChartData {
    QLineEdit*   nameEdit  { nullptr };
    QLineEdit*   valueEdit { nullptr };
    QPushButton* okButton  { nullptr };
  };

  struct BoxPlotData {
    QLineEdit*   xEdit    { nullptr };
    QLineEdit*   yEdit    { nullptr };
    QPushButton* okButton { nullptr };
  };

  struct ParallelPlotData {
    QLineEdit*   xEdit    { nullptr };
    QLineEdit*   yEdit    { nullptr };
    QPushButton* okButton { nullptr };
  };

  CQCharts*                 charts_            { nullptr };
  QAbstractItemModel*       model_             { nullptr };
  PieChartData              pieChartData_;
  XYPlotData                xyPlotData_;
  SunburstData              sunburstData_;
  BarChartData              barChartData_;
  BoxPlotData               boxPlotData_;
  ParallelPlotData          parallelPlotData_;
  QTabWidget*               plotTab_           { nullptr };
  QLineEdit*                columnTypeEdit_    { nullptr };
  QStackedWidget*           stack_             { nullptr };
  CQChartsTable*            table_             { nullptr };
  CQChartsTree*             tree_              { nullptr };
  CQGradientPalette*        palettePlot_       { nullptr };
  CQGradientPaletteControl* paletteControl_    { nullptr };
  QGroupBox*                typeGroup_         { nullptr };
  int                       tableColumn_       { 0 };
  CQChartsLoader*           loader_            { nullptr };
};

#endif
