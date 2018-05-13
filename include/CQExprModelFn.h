#ifndef CQExprModelFn_H
#define CQExprModelFn_H

#include <CExpr.h>
#include <CQExprModel.h>
#include <QVariant>

class CQExprModelFn : public CExprFunctionObj {
 public:
  CQExprModelFn(CQExprModel *model) :
   model_(model) {
  }

  bool checkColumn(int col) const {
    if (col < 0 || col >= model_->columnCount()) return false;

    return true;
  }

  bool checkIndex(int row, int col) const {
    if (row < 0 || row >= model_->rowCount   ()) return false;
    if (col < 0 || col >= model_->columnCount()) return false;

    return true;
  }

  CExprValuePtr variantToValue(CExpr *expr, const QVariant &var) const {
    if (! var.isValid())
      return CExprValuePtr();

    if      (var.type() == QVariant::Double)
      return expr->createRealValue(var.toDouble());
    else if (var.type() == QVariant::Int)
      return expr->createIntegerValue((long) var.toInt());
    else
      return expr->createStringValue(var.toString().toStdString());
  }

  QVariant valueToVariant(CExpr *, const CExprValuePtr &value) const {
    if      (value->isRealValue()) {
      double r = 0.0;
      value->getRealValue(r);
      return QVariant(r);
    }
    else if (value->isIntegerValue()) {
      long i = 0;
      value->getIntegerValue(i);
      return QVariant((int) i);
    }
    else {
      std::string s;
      value->getStringValue(s);
      return QVariant(s.c_str());
    }

    return QVariant();
  }

 protected:
  CQExprModel *model_ { nullptr };
};

#endif
