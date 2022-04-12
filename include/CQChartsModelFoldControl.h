#ifndef CQChartsModelFoldControl_H
#define CQChartsModelFoldControl_H

#ifdef CQCHARTS_FOLDED_MODEL

#include <CQChartsWidgetIFace.h>

#include <QFrame>
#include <QSharedPointer>

class CQChartsModelData;
//class CQChartsColumnCombo;
class CQChartsLineEdit;

class QAbstractItemModel;
class QComboBox;
class QCheckBox;

/*!
 * \brief Widget to allow User to Fold Model Data by Column
 * \ingroup Charts
 */
class CQChartsModelFoldControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsModelFoldControl(QWidget *parent=nullptr);

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData) override;

 private slots:
  void applySlot();

  void clearSlot();

 private:
  CQChartsModelData*   modelData_     { nullptr };
//CQChartsColumnCombo* columnEdit_    { nullptr };
  CQChartsLineEdit*    columnEdit_    { nullptr };
  QComboBox*           typeCombo_     { nullptr };
  QCheckBox*           autoCheck_     { nullptr };
  CQChartsLineEdit*    deltaEdit_     { nullptr };
  CQChartsLineEdit*    countEdit_     { nullptr };
  CQChartsLineEdit*    separatorEdit_ { nullptr };
};

#endif

#endif
