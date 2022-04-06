#ifndef CQChartsColumnExprEdit_H
#define CQChartsColumnExprEdit_H

#include <CQChartsLineEdit.h>
#include <CQChartsExprModel.h>
#include <CQChartsColumn.h>

class CQChartsModelData;

class CQChartsColumnExprEdit : public CQChartsLineEdit {
  Q_OBJECT

  Q_PROPERTY(bool allowPrefix READ allowPrefix WRITE setAllowPrefix)

 public:
  CQChartsColumnExprEdit(QWidget *parent=nullptr);

  void setModelData(CQChartsModelData *modelData);

  bool allowPrefix() const { return allowPrefix_; }
  void setAllowPrefix(bool b);

  const QString &expr() const { return expr_; }
  void setExpr(const QString &expr);

  const CQChartsExprModel::Function &function() const { return function_; }

  const CQChartsColumn &column() const { return column_; }

  const QString &subExpr() const { return subExpr_; }

 signals:
  void exprChanged();

 private:
  void updateTip();

 private slots:
  void updateSlot();

 private:
  CQChartsModelData*          modelData_   { nullptr };
  bool                        allowPrefix_ { false };
  QString                     expr_;
  CQChartsExprModel::Function function_;
  CQChartsColumn              column_;
  QString                     subExpr_;
};

#endif
