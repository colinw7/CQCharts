#ifndef CQChartsCeilUtil_H
#define CQChartsCeilUtil_H

#include <CCeil.h>

namespace CQChartsCeilUtil {

using Vars = std::vector<QVariant>;

inline void init() {
  //ClParserInst->setDollarPrefix(true);

  ClLanguageMgrInst->init(nullptr, nullptr);
}

inline QVariant valueToVar(const ClParserValuePtr &value) {
  if (! value.isValid())
    return QVariant();

  if      (value->getType() == CL_PARSER_VALUE_TYPE_INTEGER) {
    long l;

    (void) value->integerValue(&l);

    return QVariant(int(l));
  }
  else if (value->getType() == CL_PARSER_VALUE_TYPE_REAL) {
    double r;

    (void) value->realValue(&r);

    return QVariant(r);
  }
  else if (value->getType() == CL_PARSER_VALUE_TYPE_STRING) {
    std::string s;

    (void) value->stringValue(s);

    return QVariant(s.c_str());
  }
  else {
    return QVariant(value->asString().c_str());
  }
}

inline ClParserValuePtr valueFromVar(const QVariant &var) {
  if      (var.type() == QVariant::Double) {
    return ClParserValueMgrInst->createValue(var.value<double>());
  }
  else if (var.type() == QVariant::Int) {
    return ClParserValueMgrInst->createValue((long) var.value<int>());
  }
  else if (var.type() == QVariant::Bool) {
    return ClParserValueMgrInst->createValue((long) var.value<bool>());
  }
  else {
    QString str = var.toString();

    return ClParserValueMgrInst->createValue(str.toStdString());
  }
}

inline QVariant varValue(const QString &name) {
  ClParserValuePtr value = ClParserInst->getVariableValue(name.toStdString());

  return valueToVar(value);
}

inline Vars varArrayValue(const QString &name) {
  ClParserValuePtr value = ClParserInst->getVariableValue(name.toStdString());
  if (! value.isValid()) Vars();

  ClParserValueArray values;

  value->toSubValues(values);

  int nv = values.size();

  Vars vars;

  vars.resize(nv);

  for (int i = 0; i < nv; ++i) {
    vars[i] = valueToVar(values[i]);
  }

  return vars;
}

inline void setResult(const QVariant &rc) {
  ClParserInst->setVariableValue("_rc", CQChartsCeilUtil::valueFromVar(rc));
}

}

#endif
