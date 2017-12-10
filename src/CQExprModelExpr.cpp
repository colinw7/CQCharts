#include <CQExprModelExpr.h>
#include <CQExprModelFn.h>
#include <COSNaN.h>
#include <CQStrParse.h>

CQExprModelExpr::
CQExprModelExpr(CQExprModel *model)
{
  createRealVariable("pi" , M_PI);
  createRealVariable("NaN", COSNaN::get_nan());

  addFunction("column"   , "...", new CQExprModelColumnFn   (model));
  addFunction("row"      , "...", new CQExprModelRowFn      (model));
  addFunction("cell"     , "...", new CQExprModelCellFn     (model));
  addFunction("setColumn", "...", new CQExprModelSetColumnFn(model));
  addFunction("setRow"   , "...", new CQExprModelSetRowFn   (model));
  addFunction("setCell"  , "...", new CQExprModelSetCellFn  (model));
  addFunction("header"   , "...", new CQExprModelHeaderFn   (model));
  addFunction("setHeader", "...", new CQExprModelSetHeaderFn(model));
  addFunction("type"     , "...", new CQExprModelTypeFn     (model));
  addFunction("setType"  , "...", new CQExprModelSetTypeFn  (model));
  addFunction("map"      , "...", new CQExprModelMapFn      (model));
  addFunction("bucket"   , "...", new CQExprModelBucketFn   (model));
  addFunction("norm"     , "...", new CQExprModelNormFn     (model));
  addFunction("key"      , "...", new CQExprModelKeyFn      (model));
  addFunction("rand"     , "...", new CQExprModelRandFn     (model));
}

QString
CQExprModelExpr::
replaceNumericColumns(const QString &expr, int row, int column) const
{
  CQStrParse parse(expr);

  QString expr1;

  while (! parse.eof()) {
    // @<n> get column value (current row)
    if (parse.isChar('@')) {
      parse.skipChar();

      if (parse.isDigit()) {
        int pos = parse.getPos();

        while (parse.isDigit())
          parse.skipChar();

        QString str = parse.getBefore(pos);

        int column1 = str.toInt();

        expr1 += QString("column(%1)").arg(column1);
      }
      else if (parse.isChar('c')) {
        parse.skipChar();

        expr1 += QString("%1").arg(column);
      }
      else if (parse.isChar('r')) {
        parse.skipChar();

        expr1 += QString("%1").arg(row);
      }
      else
        expr1 += "@";
    }
    else
      expr1 += parse.getChar();
  }

  return expr1;
}
