#ifndef CQChartsModelWidgets_H
#define CQChartsModelWidgets_H

#include <QFrame>

class CQCharts;
class CQChartsModelList;
class CQChartsModelControl;
class CQChartsModelData;

/*!
 * \brief model list and control widgets
 * \ingroup Charts
 */
class CQChartsModelWidgets : public QFrame {
  Q_OBJECT

 public:
  CQChartsModelWidgets(CQCharts *charts, QWidget *parent=nullptr);

  CQChartsModelList *modelList() const { return modelList_   ; }

  CQChartsModelControl *modelControl() const { return modelControl_; }

  void addModelData(CQChartsModelData *modelData);

 private:
  CQCharts*             charts_       { nullptr };
  CQChartsModelList*    modelList_    { nullptr };
  CQChartsModelControl* modelControl_ { nullptr };
};

#endif
