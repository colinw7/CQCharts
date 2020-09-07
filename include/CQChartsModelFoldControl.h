#ifndef CQChartsModelFoldControl_H
#define CQChartsModelFoldControl_H

#ifdef CQCHARTS_FOLDED_MODEL

#include <QFrame>
#include <QSharedPointer>

class CQChartsModelData;
class CQChartsLineEdit;

class QAbstractItemModel;
class QComboBox;
class QCheckBox;

/*!
 * \brief Widget to allow User to Fold Model Data by Column
 * \ingroup Charts
 */
class CQChartsModelFoldControl : public QFrame {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsModelFoldControl(QWidget *parent=nullptr);

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData);

 private slots:
  void applySlot();

  void clearSlot();

 private:
  CQChartsModelData* modelData_     { nullptr };
  CQChartsLineEdit*  columnEdit_    { nullptr };
  QComboBox*         typeCombo_     { nullptr };
  QCheckBox*         autoCheck_     { nullptr };
  CQChartsLineEdit*  deltaEdit_     { nullptr };
  CQChartsLineEdit*  countEdit_     { nullptr };
  CQChartsLineEdit*  separatorEdit_ { nullptr };
};

#endif

#endif
