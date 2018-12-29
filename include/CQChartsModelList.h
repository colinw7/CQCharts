#ifndef CQChartsModelList_H
#define CQChartsModelList_H

#include <QFrame>

class CQCharts;
class CQChartsModelData;
class CQChartsModelDetails;
class CQChartsTable;
class CQChartsTree;
class CQTableWidget;
class QStackedWidget;
class QTabWidget;
class QPushButton;
class QLineEdit;
class QTextEdit;

//---

struct CQChartsViewWidgetData {
  int             ind           { -1 };
  int             tabInd        { -1 };
  QStackedWidget* stack         { nullptr };
  CQChartsTable*  table         { nullptr };
  CQChartsTree*   tree          { nullptr };
  QLineEdit*      filterEdit    { nullptr };
  QPushButton*    detailsUpdate { nullptr };
  QTextEdit*      detailsText   { nullptr };
  CQTableWidget*  detailsTable  { nullptr };
};

//---

class CQChartsModelList : public QFrame {
  Q_OBJECT

 public:
  using ViewWidgetDatas = std::map<int,CQChartsViewWidgetData*>;

 public:
  CQChartsModelList(CQCharts *charts);

 ~CQChartsModelList();

  int numModels() const { return viewWidgetDatas_.size(); }

  void addModelData(CQChartsModelData *modelData);

  void updateModel();

  CQChartsModelData *currentModelData() const;

  void setTabTitle(int ind, const QString &title);

  void redrawView();

  void reloadModel();

  void setDetails();

 private slots:
  void updateCurrentModel();

  void addModelData(int ind);

  void updateModelType(int ind);

  void invalidateDetails();

  void updateDetails();

 private:
  QTabWidget *viewTab() const { return viewTab_; }

  CQChartsViewWidgetData *currentViewWidgetData() const;

  CQChartsViewWidgetData *viewWidgetData(int ind) const;

  void addModelDataWidgets(CQChartsModelData *modelData);

 private slots:
  void currentTabChanged(int);

  void filterSlot();

  void treeColumnClicked(int column);
  void treeSelectionChanged();

  void tableColumnClicked(int column);
  void tableSelectionChanged();

 private:
  CQCharts*                   charts_                { nullptr };
  CQChartsModelData*          modelData_             { nullptr };
  QTabWidget*                 viewTab_               { nullptr };
  ViewWidgetDatas             viewWidgetDatas_;
  const CQChartsModelDetails* currentDetails_        { nullptr };
  CQChartsViewWidgetData*     currentViewWidgetData_ { nullptr };
};

#endif
