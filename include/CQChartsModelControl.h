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

  void foldApplySlot();
  void foldClearSlot();

  void typeChangedSlot();
  void typeApplySlot();

 private:
  QFrame *addExprFrame();
  QFrame *addFoldFrame();
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

  CQCharts*            charts_          { nullptr };
  CQChartsModelData*   modelData_       { nullptr };
  Mode                 exprMode_        { Mode::ADD };
  QRadioButton*        exprAddRadio_    { nullptr };
  QRadioButton*        exprRemoveRadio_ { nullptr };
  QRadioButton*        exprModifyRadio_ { nullptr };
  QLabel*              exprValueLabel_  { nullptr };
  CQLineEdit*          exprValueEdit_   { nullptr };
  QLabel*              exprColumnLabel_ { nullptr };
  CQLineEdit*          exprColumnEdit_  { nullptr };
  QLabel*              exprNameLabel_   { nullptr };
  CQLineEdit*          exprNameEdit_    { nullptr };
  QLabel*              exprTypeLabel_   { nullptr };
  CQLineEdit*          exprTypeEdit_    { nullptr };
#ifdef CQCHARTS_FOLDED_MODEL
  CQLineEdit*          foldColumnEdit_  { nullptr };
  QCheckBox*           foldAutoCheck_   { nullptr };
  CQLineEdit*          foldDeltaEdit_   { nullptr };
  CQLineEdit*          foldCountEdit_   { nullptr };
#endif
  CQPropertyViewModel* propertyModel_   { nullptr };
  CQPropertyViewTree*  propertyTree_    { nullptr };
  ColumnEditData       columnEditData_;
};

#endif
