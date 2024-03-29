#ifndef CQChartsObjectPropertiesWidget_H
#define CQChartsObjectPropertiesWidget_H

#include <QFrame>
#include <QPointer>

class CQChartsPlot;
class CQChartsPropertyViewTree;
class CQChartsPropertyViewTreeFilterEdit;

/*!
 * \brief View settings plot object properties widget
 * \ingroup Charts
 */
class CQChartsObjectPropertiesWidget : public QFrame {
  Q_OBJECT

 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsObjectPropertiesWidget();

  Plot *plot() const;

  CQChartsPropertyViewTree *propertyTree() const { return propertyTree_; }

 Q_SIGNALS:
  void propertyItemSelected(QObject *obj, const QString &path);

 private Q_SLOTS:
  void filterStateSlot(bool show, bool focus);

 private:
  using PlotP = QPointer<Plot>;

  PlotP                               plot_;
  CQChartsPropertyViewTree*           propertyTree_ { nullptr };
  CQChartsPropertyViewTreeFilterEdit* filterEdit_   { nullptr };
};

#endif
