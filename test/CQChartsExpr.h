#ifndef CQChartsExpr_H
#define CQChartsExpr_H

#include <CExpr.h>
#include <CQStrParse.h>

namespace CQChartsExpr {

// variable assignment (<var> = <value>)
inline bool processAssignExpression(CExpr *expr, const QString &exprStr, CExprValuePtr &value) {
  CQStrParse parse(exprStr);

  parse.skipSpace();

  QString identifier;

  while (! parse.eof()) {
    QChar c = parse.getCharAt();

    if (! identifier.length()) {
      if (! c.isLetter())
        break;
    }
    else {
      if (! c.isLetterOrNumber() && c != '_')
        break;
    }

    identifier += parse.getChar();
  }

  parse.skipSpace();

  if (identifier == "" || ! parse.isChar('='))
    return false;

  parse.skipChar();

  parse.skipSpace();

  if (! expr->evaluateExpression(parse.getAt(parse.getPos()).toStdString(), value))
    return false;

  if (! value.isValid())
    return false;

  expr->createVariable(identifier.toStdString(), value);

  return true;
}

inline bool processExpression(CExpr *expr, const QString &exprStr, CExprValuePtr &value) {
  if (! expr->evaluateExpression(exprStr.toStdString(), value))
    return false;

  return true;
}

inline bool processBoolExpression(CExpr *expr, const QString &exprStr, bool &b) {
  b = false;

  CExprValuePtr value;

  if (! expr->evaluateExpression(exprStr.toStdString(), value))
    return false;

  if (! value.isValid())
    return false;

  if (! value->getBooleanValue(b))
    return false;

  return true;
}

inline QString replaceStringVariables(CExpr *expr, const QString &str) {
  CQStrParse line(str);

  while (! line.eof()) {
    if (line.isChar('$'))
      break;

    line.skipChar();
  }

  if (line.eof())
    return str;

  QString str1 = line.getAt(0, line.getPos());

  while (! line.eof()) {
    if      (line.isChar('$')) {
      line.skipChar();

      if (line.isChar('{')) {
        line.skipChar();

        QString name;

        while (! line.eof()) {
          if (line.isChar('}'))
            break;

          name += line.getChar();
        }

        CExprValuePtr value;

        if (processExpression(expr, name, value)) {
          std::string s;

          if (value->getStringValue(s))
            str1 += s.c_str();
        }

        if (! line.eof())
          line.skipChar();
      }
      else
        str1 += "$";
    }
    else if (line.isChar('\\')) {
      line.skipChar();

      if (! line.eof())
        str1 += line.getChar();
    }
    else
      str1 += line.getChar();
  }

  return str1;
}

}

#endif
