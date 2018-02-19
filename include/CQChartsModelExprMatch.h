#ifndef CQChartsModelExprMatch_H
#define CQChartsModelExprMatch_H

#include <CExpr.h>
#include <QVariant>
#include <QString>

class QAbstractItemModel;

class CQChartsModelExprMatch : public CExpr {
 public:
  CQChartsModelExprMatch(QAbstractItemModel *model=0);

  QAbstractItemModel *model() const { return model_; }
  void setModel(QAbstractItemModel *model) { model_ = model; }

  void initMatch(const QString &expr);

  void initColumns();

  bool match(const QString &expr, const QModelIndex &ind, bool &ok);
  bool match(const QModelIndex &ind, bool &ok);

  int currentRow() const;
  int currentCol() const;

  static CExprValuePtr variantToValue(CExpr *expr, const QVariant &var);
  static QVariant valueToVariant(CExpr *, const CExprValuePtr &value);

 private:
  bool evaluateExpression(const QString &expr, const QModelIndex &ind,
                          CExprValuePtr &value, bool replace);

  QString replaceNumericColumns(const QString &expr, const QModelIndex &ind) const;

 private:
  using Variables = std::vector<CExprVariablePtr>;

  QAbstractItemModel *model_      { nullptr };
  Variables           columnVars_;
  CExprValuePtr       rowValue_;
  CExprValuePtr       colValue_;
  CExprVariablePtr    rowVar_;
  CExprVariablePtr    colVar_;
  QString             matchExpr_;
};

#endif
