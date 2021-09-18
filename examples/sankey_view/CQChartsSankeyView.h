#include <QMainWindow>

class CQCharts;
class CQChartsSankeyPlot;

class MainWindow : public QMainWindow {
 public:
  MainWindow();
 ~MainWindow();

 private:
  CQCharts*           charts_     { nullptr };
  CQChartsSankeyPlot* sankeyPlot_ { nullptr };
};
