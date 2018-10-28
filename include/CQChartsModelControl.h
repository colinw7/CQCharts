#ifndef CQChartsModelControl_H
#define CQChartsModelControl_H

#include <QFrame>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQCharts;
class CQChartsModelList;
class CQChartsModelData;

class QGridLayout;
class QRadioButton;
class QCheckBox;
class QLineEdit;
class QComboBox;
class QLabel;

class CQChartsModelControl : public QFrame {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

  enum class Mode {
    ADD,
    REMOVE,
    MODIFY
  };

 public:
  CQChartsModelControl(CQCharts *charts);

  void setModelList(CQChartsModelList *modelList) { modelList_ = modelList; }

  void setColumnData(int column);

  void updateModel(CQChartsModelData *modelData);

  void updateModelDetails(const CQChartsModelData *modelData);

 private slots:
  void expressionModeSlot();

  void exprApplySlot();

  void foldApplySlot();
  void foldClearSlot();

  void typeApplySlot();

 private:
  QLineEdit *addLineEdit(QGridLayout *grid, int &row, const QString &name,
                         const QString &objName) const;
  QComboBox *addComboBox(QGridLayout *grid, int &row, const QString &name,
                         const QString &objName) const;

 private:
  struct ParamEdit {
    int        row { 0 };
    QLabel    *label;
    QLineEdit *edit;
  };

  using ParamEdits = std::vector<ParamEdit>;

  struct ColumnEditData {
    QFrame*      editFrame  { nullptr };
    QGridLayout* editLayout { nullptr };
    QLineEdit*   numEdit    { nullptr };
    QLineEdit*   nameEdit   { nullptr };
#if 0
    QLineEdit*   typeEdit   { nullptr };
#else
    QComboBox*   typeCombo  { nullptr };
#endif
    int          row        { 0 };
    ParamEdits   paramEdits;
  };

  CQCharts*          charts_          { nullptr };
  CQChartsModelList* modelList_       { nullptr };
  Mode               mode_            { Mode::ADD };
  QRadioButton*      exprAddRadio_    { nullptr };
  QRadioButton*      exprRemoveRadio_ { nullptr };
  QRadioButton*      exprModifyRadio_ { nullptr };
  QLabel*            exprValueLabel_  { nullptr };
  QLineEdit*         exprValueEdit_   { nullptr };
  QLabel*            exprColumnLabel_ { nullptr };
  QLineEdit*         exprColumnEdit_  { nullptr };
  QLabel*            exprNameLabel_   { nullptr };
  QLineEdit*         exprNameEdit_    { nullptr };
  QLabel*            exprTypeLabel_   { nullptr };
  QLineEdit*         exprTypeEdit_    { nullptr };
#ifdef CQCHARTS_FOLDED_MODEL
  QLineEdit*         foldColumnEdit_  { nullptr };
  QCheckBox*         foldAutoCheck_   { nullptr };
  QLineEdit*         foldDeltaEdit_   { nullptr };
  QLineEdit*         foldCountEdit_   { nullptr };
#endif
  ColumnEditData     columnEditData_;
};

#endif
