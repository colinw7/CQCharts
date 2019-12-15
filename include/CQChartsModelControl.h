#ifndef CQChartsModelControl_H
#define CQChartsModelControl_H

#include <QFrame>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQCharts;
class CQChartsModelData;
class CQChartsParamEdit;

class CQPropertyViewModel;
class CQPropertyViewTree;
class CQLineEdit;

class QGridLayout;
class QRadioButton;
class QCheckBox;
class QComboBox;
class QLabel;

/*!
 * \brief Model Control Widget
 * \ingroup Charts
 */
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
  CQChartsModelControl(CQCharts *charts, CQChartsModelData *modelData=nullptr);
 ~CQChartsModelControl();

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData);

  void updateModel();

  void updateModelDetails();

 public slots:
  void updateCurrentModel();

 private slots:
  void setColumnData(int column);

  void expressionModeSlot();

  void exprApplySlot();

#ifdef CQCHARTS_FOLDED_MODEL
  void foldApplySlot();
  void foldClearSlot();
#endif

  void typeChangedSlot();
  void typeApplySlot();

 private:
  QFrame *addExprFrame();
#ifdef CQCHARTS_FOLDED_MODEL
  QFrame *addFoldFrame();
#endif
  QFrame *addColumnDataFrame();
  QFrame *addPropertiesFrame();

  //---

  CQLineEdit *addLineEdit(QGridLayout *grid, int &row, const QString &name,
                          const QString &objName) const;
  QComboBox *addComboBox(QGridLayout *grid, int &row, const QString &name,
                         const QString &objName) const;

 private:
  struct ParamEdit {
    int                row   { 0 };
    QLabel*            label { nullptr };
    CQChartsParamEdit* edit  { nullptr };
  };

  using ParamEdits = std::vector<ParamEdit>;

  struct ColumnEditData {
    QFrame*      editFrame  { nullptr };
    QGridLayout* editLayout { nullptr };
    CQLineEdit*  numEdit    { nullptr };
    CQLineEdit*  nameEdit   { nullptr };
#if 0
    CQLineEdit*  typeEdit   { nullptr };
#else
    QComboBox*   typeCombo  { nullptr };
#endif
    int          row        { 0 };
    ParamEdits   paramEdits;
  };

  struct ExprWidgets {
    QRadioButton* addRadio    { nullptr };
    QRadioButton* removeRadio { nullptr };
    QRadioButton* modifyRadio { nullptr };
    CQLineEdit*   valueEdit   { nullptr };
    QLabel*       columnLabel { nullptr };
    CQLineEdit*   columnEdit  { nullptr };
    CQLineEdit*   nameEdit    { nullptr };
    QLabel*       typeLabel   { nullptr };
    CQLineEdit*   typeEdit    { nullptr };
  };

#ifdef CQCHARTS_FOLDED_MODEL
  struct FoldWidgets {
    CQLineEdit* columnEdit    { nullptr };
    QComboBox*  typeCombo     { nullptr };
    QCheckBox*  autoCheck     { nullptr };
    CQLineEdit* deltaEdit     { nullptr };
    CQLineEdit* countEdit     { nullptr };
    CQLineEdit* separatorEdit { nullptr };
  };
#endif

  CQCharts*            charts_          { nullptr };
  CQChartsModelData*   modelData_       { nullptr };
  Mode                 exprMode_        { Mode::ADD };
  ExprWidgets          exprWidgets_;
#ifdef CQCHARTS_FOLDED_MODEL
  FoldWidgets          foldWidgets_;
#endif
  CQPropertyViewModel* propertyModel_   { nullptr };
  CQPropertyViewTree*  propertyTree_    { nullptr };
  ColumnEditData       columnEditData_;
};

#endif
