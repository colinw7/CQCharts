#ifndef CQExprModelExpr_H
#define CQExprModelExpr_H

#include <CExpr.h>
#include <QString>

class CQExprModel;

class CQExprModelExpr : public CExpr {
 public:
  CQExprModelExpr(CQExprModel *model);

  QString replaceNumericColumns(const QString &expr, int row, int column) const;
};

#endif
