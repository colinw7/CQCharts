#ifndef CQChartsModelTable_H
#define CQChartsModelTable_H

#include <CQTableWidget.h>
#include <CQChartsWidgetIFace.h>

class CQCharts;
class CQChartsView;
class CQChartsModelTable;

class QPushButton;

class CQChartsModelTableControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  CQChartsModelTableControl(CQCharts *charts=nullptr);

  CQCharts *charts() const { return charts_; }
  void setCharts(CQCharts *charts) override;

  CQChartsView *view() const { return view_; }
  void setView(CQChartsView *view) override;

 private:
  void updateState();

 private slots:
  void modelsSelectionChangeSlot();

  void loadModelSlot();
  void editModelSlot();
  void removeModelSlot();
  void createPlotModelSlot();

 private:
  CQCharts*           charts_     { nullptr };
  CQChartsView*       view_       { nullptr };
  CQChartsModelTable* modelTable_ { nullptr };

  QPushButton *editButton_   { nullptr };
  QPushButton *removeButton_ { nullptr };
  QPushButton *plotButton_   { nullptr };
};

//------

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
