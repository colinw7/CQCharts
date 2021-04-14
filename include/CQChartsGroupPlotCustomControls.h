#ifndef CQChartsGroupPlotCustomControls_H
#define CQChartsGroupPlotCustomControls_H

#include <CQChartsPlotCustomControls.h>

class CQChartsGroupPlot;

class CQChartsGroupPlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsGroupPlotCustomControls(CQCharts *charts, const QString &plotType);

  void setPlot(CQChartsPlot *plot) override;

  void addGroupColumnWidgets();

  FrameData &groupFrame() { return groupFrame_; }

 protected:
  void connectSlots(bool b);

 public slots:
  void updateWidgets() override;

 protected slots:
  void groupColumnSlot();

 private:
  CQChartsGroupPlot*   plot_             { nullptr };
  FrameData            groupFrame_;
  CQChartsColumnCombo* groupColumnCombo_ { nullptr };
};

#endif
