#ifndef CQChartsModelList_H
#define CQChartsModelList_H

#include <QFrame>

class CQChartsTest;
class CQChartsModelData;
class CQChartsTable;
class CQChartsTree;
class QStackedWidget;
class QTabWidget;
class QLineEdit;
class QTextEdit;

//---

struct CQChartsViewWidgetData {
  int             ind          { -1 };
  int             tabInd       { -1 };
  QStackedWidget* stack        { nullptr };
  CQChartsTable*  table        { nullptr };
  CQChartsTree*   tree         { nullptr };
  QLineEdit*      filterEdit   { nullptr };
  QTextEdit*      detailsText  { nullptr };
};

//---

class CQChartsModelList : public QFrame {
  Q_OBJECT

 public:
  using ViewWidgetDatas = std::map<int,CQChartsViewWidgetData*>;

 public:
  CQChartsModelList(CQChartsTest *test);

 ~CQChartsModelList();

  int numModels() const { return viewWidgetDatas_.size(); }

  void addModelData(CQChartsModelData *modelData);

  void setTabTitle(int ind, const QString &title);

  void redrawView(const CQChartsModelData *modelData);

  void reloadModel(CQChartsModelData *modelData);

  void setDetailsText(const CQChartsModelData *modelData);

 private:
  QTabWidget *viewTab() const { return viewTab_; }

  CQChartsViewWidgetData *viewWidgetData(int ind) const;

 private slots:
  void currentTabChanged(int);

  void filterSlot();

  void tableColumnClicked(int column);

 private:
  CQChartsTest*   test_            { nullptr };
  QTabWidget*     viewTab_         { nullptr };
  ViewWidgetDatas viewWidgetDatas_;
};

#endif
