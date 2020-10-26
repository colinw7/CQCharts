#ifndef CQChartsModelFlattenControl_H
#define CQChartsModelFlattenControl_H

#include <QFrame>
#include <QSharedPointer>

class CQChartsModelData;
class CQChartsLineEdit;

class QAbstractItemModel;

/*!
 * \brief Widget to allow User to Flatten Model Data by Column
 * \ingroup Charts
 */
class CQChartsModelFlattenControl : public QFrame {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsModelFlattenControl(QWidget *parent=nullptr);

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData);

 private slots:
  void applySlot();

 private:
  CQChartsModelData* modelData_     { nullptr };
  CQChartsLineEdit*  columnEdit_    { nullptr };
  CQChartsLineEdit*  opColumnsEdit_ { nullptr };
};

#endif
