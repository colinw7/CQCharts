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
  using View = CQChartsView;

 public:
  CQChartsModelTableControl(CQCharts *charts=nullptr);

  CQCharts *charts() const { return charts_; }
  void setCharts(CQCharts *charts) override;

  View *view() const;
  void setView(View *view) override;

 private:
  void updateState();

 private Q_SLOTS:
  void modelsSelectionChangeSlot();

  void loadModelSlot();
  void saveModelSlot();
  void editModelSlot();
  void removeModelSlot();
  void createPlotModelSlot();

 private:
  using ViewP = QPointer<View>;

  CQCharts*           charts_     { nullptr };
  ViewP               view_;
  CQChartsModelTable* modelTable_ { nullptr };

  QPushButton *saveButton_   { nullptr };
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

  CQChartsModelData *selectedModelData() const;
  int selectedModel() const;

  QSize sizeHint() const override;

 protected Q_SLOTS:
  void updateModels();

 private:
  CQCharts*   charts_ { nullptr };
  QStringList headerNames_;
};

#endif
