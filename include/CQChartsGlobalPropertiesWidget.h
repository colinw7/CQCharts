#ifndef CQChartsGlobalPropertiesWidget_H
#define CQChartsGlobalPropertiesWidget_H

#include <CQChartsWidgetIFace.h>

#include <QFrame>

class CQCharts;
class CQChartsPropertyViewTree;
class CQChartsPropertyViewTreeFilterEdit;

/*!
 * \brief global properties widget
 * \ingroup Charts
 */
class CQChartsGlobalPropertiesWidget : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  CQChartsGlobalPropertiesWidget(CQCharts *charts=nullptr);

  CQCharts *charts() const { return charts_; }
  void setCharts(CQCharts *charts) override;

  CQChartsPropertyViewTree *propertyTree() const { return propertyTree_; }

 Q_SIGNALS:
  void propertyItemSelected(QObject *obj, const QString &path);

 private Q_SLOTS:
  void filterStateSlot(bool show, bool focus);

 private:
  CQCharts*                           charts_       { nullptr };
  CQChartsPropertyViewTree*           propertyTree_ { nullptr };
  CQChartsPropertyViewTreeFilterEdit* filterEdit_   { nullptr };
};

#endif
