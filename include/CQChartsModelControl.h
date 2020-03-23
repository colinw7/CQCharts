#ifndef CQChartsModelControl_H
#define CQChartsModelControl_H

#include <QFrame>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQCharts;
class CQChartsModelData;
class CQChartsParamEdit;
class CQChartsLineEdit;

class CQPropertyViewModel;
class CQPropertyViewTree;

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
  void headerTypeChangedSlot();
  void typeApplySlot();

 private:
  QFrame *addExprFrame();
#ifdef CQCHARTS_FOLDED_MODEL
  QFrame *addFoldFrame();
#endif
  QFrame *addColumnDataFrame();
  QFrame *addPropertiesFrame();

  //---

  CQChartsLineEdit *addLineEdit(QGridLayout *grid, int &row, const QString &name,
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
    QFrame*           editFrame       { nullptr };
    QGridLayout*      editLayout      { nullptr };
    CQChartsLineEdit* numEdit         { nullptr };
    CQChartsLineEdit* nameEdit        { nullptr };
    QComboBox*        typeCombo       { nullptr };
    QComboBox*        headerTypeCombo { nullptr };
    int               row             { 0 };
    ParamEdits        paramEdits;
  };

  struct ExprWidgets {
    QRadioButton*     addRadio    { nullptr };
    QRadioButton*     removeRadio { nullptr };
    QRadioButton*     modifyRadio { nullptr };
    CQChartsLineEdit* valueEdit   { nullptr };
    QLabel*           columnLabel { nullptr };
    CQChartsLineEdit* columnEdit  { nullptr };
    CQChartsLineEdit* nameEdit    { nullptr };
    QLabel*           typeLabel   { nullptr };
    CQChartsLineEdit* typeEdit    { nullptr };
  };

#ifdef CQCHARTS_FOLDED_MODEL
  struct FoldWidgets {
    CQChartsLineEdit* columnEdit    { nullptr };
    QComboBox*        typeCombo     { nullptr };
    QCheckBox*        autoCheck     { nullptr };
    CQChartsLineEdit* deltaEdit     { nullptr };
    CQChartsLineEdit* countEdit     { nullptr };
    CQChartsLineEdit* separatorEdit { nullptr };
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
