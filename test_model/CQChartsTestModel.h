#ifndef CQChartsTestModel_H
#define CQChartsTestModel_H

#include <CQDataModel.h>
#include <CHRTime.h>

#include <QFrame>

class CQChartsTestModel;

class CQCharts;
class CQChartsView;
class CQChartsWindow;
class CQChartsPlot;

class QSortFilterProxyModel;

class CQChartsTestPlot : public QFrame {
  Q_OBJECT

 public:
  CQChartsTestPlot(QWidget *parent=nullptr);

 private Q_SLOTS:
  void updateDataSlot();

 private:
  CQCharts*          charts_ { nullptr };
  CQChartsView*      view_   { nullptr };
  CQChartsWindow*    window_ { nullptr };
  CQChartsTestModel* model_  { nullptr };
  CQChartsPlot*      plot_   { nullptr };
  CHRTime            lastTime_;
};

class CQChartsTestModel : public CQDataModel {
 public:
  CQChartsTestModel();

  QSortFilterProxyModel *proxyModel() const { return proxyModel_; }

  void updateData();

 private:
  static const int NUM_STARS = 2000;

  struct Star {
    double x { 0.0 };
    double y { 0.0 };
    double r { 0.0 };
    QColor c;
  };

  using Stars = std::vector<Star>;

  QSortFilterProxyModel *proxyModel_ { nullptr };

  double  t_  { 0.0 };
  double  dt_ { 1.0/60.0 };
  Stars   stars_;
};

#endif
