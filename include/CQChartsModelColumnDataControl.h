#ifndef CQChartsModelColumnDataControl_H
#define CQChartsModelColumnDataControl_H

#include <QFrame>
#include <QSharedPointer>

class CQChartsModelData;
class CQChartsParamEdit;
class CQChartsColumnCombo;
class CQChartsColumnTypeCombo;
class CQChartsLineEdit;

class QAbstractItemModel;
class QGridLayout;
class QVBoxLayout;
class QLabel;

/*!
 * \brief Widget to allow User to change data of Columns of Model
 * \ingroup Charts
 */
class CQChartsModelColumnDataControl : public QFrame {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsModelColumnDataControl(QWidget *parent=nullptr);

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData);

  void init();

 private slots:
  void setColumnData(int column);

 private slots:
  void columnNumChanged();

  void typeChangedSlot();
  void headerTypeChangedSlot();

  void generalApplySlot();
  void paramApplySlot();
  void nullValueApplySlot();

 private:
  struct ParamEdit {
    int                row   { 0 };
    QLabel*            label { nullptr };
    CQChartsParamEdit* edit  { nullptr };
  };

  using ParamEdits = std::vector<ParamEdit>;

  CQChartsModelData*       modelData_       { nullptr };
  QFrame*                  generalFrame_    { nullptr };
  QGridLayout*             generalLayout_   { nullptr };
  QFrame*                  paramFrame_      { nullptr };
  QVBoxLayout*             paramLayout_     { nullptr };
  QFrame*                  paramSubFrame_   { nullptr };
  QGridLayout*             paramSubLayout_  { nullptr };
  QFrame*                  nullValueFrame_  { nullptr };
  QGridLayout*             nullValueLayout_ { nullptr };
  CQChartsColumnCombo*     columnNumEdit_   { nullptr };
  CQChartsLineEdit*        nameEdit_        { nullptr };
  CQChartsColumnTypeCombo* typeCombo_       { nullptr };
  CQChartsColumnTypeCombo* headerTypeCombo_ { nullptr };
  int                      paramRow_        { 0 };
  ParamEdits               paramEdits_;
  CQChartsLineEdit*        nullValueEdit_   { nullptr };
};

#endif
