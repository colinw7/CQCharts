#ifndef CQChartsModelFilterControl_H
#define CQChartsModelFilterControl_H

#include <QFrame>
#include <QSharedPointer>

class CQChartsModelData;
class CQChartsLineEdit;

class QAbstractItemModel;

/*!
 * \brief Widget to allow User to Filter Model Data by Column
 * \ingroup Charts
 */
class CQChartsModelFilterControl : public QFrame {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsModelFilterControl(QWidget *parent=nullptr);

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData);

  void setFilterText(const QString &text);

 private slots:
  void applySlot();

 private:
  CQChartsModelData* modelData_  { nullptr };
  CQChartsLineEdit*  filterEdit_ { nullptr };
};

#endif
