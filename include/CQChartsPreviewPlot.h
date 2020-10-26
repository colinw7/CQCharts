#ifndef CQChartsPreviewPlot_H
#define CQChartsPreviewPlot_H

#include <QFrame>
#include <QAbstractItemModel>
#include <QPointer>

class CQCharts;
class CQChartsPlotType;
class CQChartsPlot;
class CQChartsView;

class QCheckBox;

class CQChartsPreviewPlot : public QFrame {
  Q_OBJECT

 public:
  using PlotType = CQChartsPlotType;
  using Plot     = CQChartsPlot;
  using View     = CQChartsView;
  using ModelP   = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsPreviewPlot(CQCharts *charts);

  bool isEnabled() const;

  void updatePlot(const ModelP &model, const PlotType *type, const QString &filter);

  void resetPlot();

  Plot *plot() const { return plot_; }

 signals:
  void enableStateChanged();

 private slots:
  void enabledSlot();
  void fitSlot();

 private:
  CQCharts*  charts_       { nullptr }; //!< charts
  QCheckBox* enabledCheck_ { nullptr }; //!< preview enabled checkbox
  View*      view_         { nullptr }; //!< preview chart view
  Plot*      plot_         { nullptr }; //!< preview plot
};

#endif
