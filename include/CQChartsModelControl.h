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
class QLineEdit;
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

  void setColumnData(int num, const QString &headerStr, const QString &typeStr);

  void updateModel(CQChartsModelData *modelData);

  void updateModelDetails(const CQChartsModelData *modelData);

 private slots:
  void expressionModeSlot();

  void exprSlot();

  void foldSlot();

  void typeSetSlot();

 private:
  QLineEdit *addLineEdit(QGridLayout *grid, int &row, const QString &name,
                         const QString &objName) const;

 private:
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
  QLineEdit*         foldEdit_        { nullptr };
#endif
  QLineEdit*         columnNumEdit_   { nullptr };
  QLineEdit*         columnNameEdit_  { nullptr };
  QLineEdit*         columnTypeEdit_  { nullptr };
};

#endif
