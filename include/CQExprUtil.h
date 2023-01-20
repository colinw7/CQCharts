#ifndef CQExprUtil_H
#define CQExprUtil_H

#include <CExpr.h>
#include <QVariant>

namespace CQExprUtil {

inline bool variantToValue(CExpr *expr, const QVariant &var, CExprValuePtr &value) {
  if (! var.isValid()) {
    value = CExprValuePtr();
    return false;
  }

  if      (var.type() == QVariant::Double)
    value = expr->createRealValue(var.toDouble());
  else if (var.type() == QVariant::Int)
    value = expr->createIntegerValue(static_cast<long>(var.toInt()));
  else if (var.type() == QVariant::LongLong)
    value = expr->createIntegerValue(var.toLongLong());
  else if (var.type() == QVariant::Bool)
    value = expr->createBooleanValue(var.toBool());
  else
    value = expr->createStringValue(var.toString().toStdString());

  return true;
}

inline QVariant valueToVariant(CExpr *, const CExprValuePtr &value) {
  if (! value)
    return QVariant();

  if      (value->isRealValue()) {
    double r = 0.0;
    value->getRealValue(r);
    return QVariant(r);
  }
  else if (value->isIntegerValue()) {
    long i = 0;
    value->getIntegerValue(i);
    return QVariant(static_cast<qlonglong>(i));
  }
  else {
    std::string s;
    value->getStringValue(s);
    return QVariant(s.c_str());
  }

  return QVariant();
}

}

#endif
