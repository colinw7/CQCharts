#ifndef CQChartsModelDataWidget_H
#define CQChartsModelDataWidget_H

#include <CQChartsWidgetIFace.h>
#include <QFrame>

class CQChartsModelDetailsWidget;
class CQChartsModelData;
#ifdef CQCHARTS_MODEL_VIEW
class CQChartsModelView;
#else
class CQChartsTable;
class CQChartsTree;
#endif
class CQCharts;

class QStackedWidget;

/*!
 * \brief Model Data Widget
 * \ingroup Charts
 */
class CQChartsModelDataWidget : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

  Q_PROPERTY(bool proxy READ isProxy WRITE setProxy)

 public:
  using ModelData = CQChartsModelData;

 public:
  CQChartsModelDataWidget(CQCharts *charts=nullptr, ModelData *modelData=nullptr);
 ~CQChartsModelDataWidget();

  ModelData *modelData() const { return modelData_; }
  void setModelData(ModelData *modelData) override;

  bool isProxy() const { return proxy_; }
  void setProxy(bool b) { proxy_ = b; }

  void reloadModel();

  void setDetails();

 private:
  void init();

 signals:
  void filterTextChanged(const QString &);

 private slots:
  void reloadModelSlot();

  void filterSlot();

  void columnClicked(int column);
  void selectionChanged();

 private:
  CQCharts*  charts_    { nullptr };
  ModelData* modelData_ { nullptr };
  bool       proxy_     { true };

#ifdef CQCHARTS_MODEL_VIEW
  CQChartsModelView*          view_          { nullptr };
#else
  QStackedWidget*             stack_         { nullptr };
  CQChartsTree*               tree_          { nullptr };
  CQChartsTable*              table_         { nullptr };
#endif
  CQChartsModelDetailsWidget* detailsWidget_ { nullptr };
};

#endif
