#ifndef CQChartsModelControl_H
#define CQChartsModelControl_H

#include <QFrame>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQChartsModelExprControl;
class CQChartsModelFoldControl;
class CQChartsModelColumnDataControl;

class CQCharts;
class CQChartsModelData;

class CQPropertyViewModel;
class CQPropertyViewTree;

/*!
 * \brief Model Control Widget
 * \ingroup Charts
 */
class CQChartsModelControl : public QFrame {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsModelControl(CQCharts *charts, CQChartsModelData *modelData=nullptr);
 ~CQChartsModelControl();

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData);

 public slots:
  void updateCurrentModel();

 private:
  CQChartsModelExprControl *addExprFrame();

#ifdef CQCHARTS_FOLDED_MODEL
  CQChartsModelFoldControl *addFoldFrame();
#endif

  CQChartsModelColumnDataControl *addColumnDataFrame();

  QFrame *addPropertiesFrame();

 private:
  CQCharts*                       charts_          { nullptr };
  CQChartsModelData*              modelData_       { nullptr };
  CQChartsModelExprControl*       exprFrame_       { nullptr };
#ifdef CQCHARTS_FOLDED_MODEL
  CQChartsModelFoldControl*       foldFrame_       { nullptr };
#endif
  CQChartsModelColumnDataControl* columnDataFrame_ { nullptr };
  CQPropertyViewModel*            propertyModel_   { nullptr };
  CQPropertyViewTree*             propertyTree_    { nullptr };
};

#endif
