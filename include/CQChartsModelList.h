#ifndef CQChartsModelList_H
#define CQChartsModelList_H

#include <QFrame>

class CQChartsModelDataWidget;
class CQChartsModelData;
class CQCharts;

class CQTabWidget;

//---

/*!
 * \brief Model List Widget
 * \ingroup Charts
 */
class CQChartsModelList : public QFrame {
  Q_OBJECT

 public:
  CQChartsModelList(CQCharts *charts);

 ~CQChartsModelList();

  void addModelData(CQChartsModelData *modelData);

  CQChartsModelData *currentModelData() const;

 Q_SIGNALS:
  void filterTextChanged(const QString &);

 private Q_SLOTS:
  void updateCurrentModel();

  void addModelData(int ind);

  void updateModelType(int ind);

 private:
  CQTabWidget *viewTab() const { return viewTab_; }

  CQChartsModelDataWidget *currentModelDataWidget() const;
  CQChartsModelDataWidget *modelDataWidget(int ind) const;

 private Q_SLOTS:
  void currentTabChanged(int);
  void closeTabSlot(int);

 private:
  CQCharts*    charts_  { nullptr };
  CQTabWidget* viewTab_ { nullptr };
};

#endif
