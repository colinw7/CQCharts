#ifndef CQChartsModelTable_H
#define CQChartsModelTable_H

#include <CQTableWidget.h>
#include <CQChartsWidgetIFace.h>

class CQCharts;

class CQChartsModelTable : public CQTableWidget, public CQChartsWidgetIFace {
  Q_OBJECT

  Q_PROPERTY(int selectedModel READ selectedModel)

 public:
  CQChartsModelTable(CQCharts *charts=nullptr);

  void setCharts(CQCharts *charts) override;

  int selectedModel() const;

  QSize sizeHint() const override;

 protected slots:
  void updateModels();

 private:
  CQCharts*   charts_ { nullptr };
  QStringList headerNames_;
};

#endif
