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
class QLabel;

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

  void applySlot();

 private:
  struct ParamEdit {
    int                row   { 0 };
    QLabel*            label { nullptr };
    CQChartsParamEdit* edit  { nullptr };
  };

  using ParamEdits = std::vector<ParamEdit>;

  CQChartsModelData*       modelData_       { nullptr };
  QFrame*                  editFrame_       { nullptr };
  QGridLayout*             editLayout_      { nullptr };
  QFrame*                  paramFrame_      { nullptr };
  QGridLayout*             paramLayout_     { nullptr };
  CQChartsColumnCombo*     columnNumEdit_   { nullptr };
  CQChartsLineEdit*        nameEdit_        { nullptr };
  CQChartsColumnTypeCombo* typeCombo_       { nullptr };
  CQChartsColumnTypeCombo* headerTypeCombo_ { nullptr };
  int                      editRow_         { 0 };
  int                      paramRow_        { 0 };
  ParamEdits               paramEdits_;
};

#endif
