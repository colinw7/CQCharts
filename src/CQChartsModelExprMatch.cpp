#include <CQChartsModelExprMatch.h>
#include <CQChartsUtil.h>
#include <COSNaN.h>
#include <CQStrParse.h>

#include <QAbstractItemModel>
#include <QVariant>

namespace {

bool varToInt(const QVariant &var, int &i) {
  bool ok;

  int i1 = var.toInt(&ok);

  if (! ok)
    return false;

  i = i1;

  return true;
}

}

//---

#ifdef CQChartsModelExprMatch_USE_CEXPR
#include <CExpr.h>

class CQChartsModelExprMatchExprFn : public CExprFunctionObj {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQChartsModelExprMatchExprFn(CQChartsModelExprMatch *match, const QString &name) :
   match_(match), name_(name) {
    CExpr *expr = match->expr();

    expr->addFunction(name_.toLatin1().constData(), "...", this);
  }

  virtual ~CQChartsModelExprMatchExprFn() { }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    expr_ = expr;

    Vars vars;

    for (auto &value : values)
      vars.push_back(valueToVariant(value));

    QVariant var = exec(vars);

    CExprValuePtr value;

    if (! variantToValue(var, value))
      return CExprValuePtr();

    return value;
  }

  virtual QVariant exec(const Vars &vars) = 0;

  bool checkColumn(int col) const { return match_->checkColumn(col); }

  bool checkIndex(int row, int col) const { return match_->checkIndex(row, col); }

  bool variantToValue(const QVariant &var, CExprValuePtr &value) {
    return variantToValue(expr_, var, value);
  }

  bool variantToValue(CExpr *expr, const QVariant &var, CExprValuePtr &value) {
    return match_->variantToValue(expr, var, value);
  }

  QVariant valueToVariant(const CExprValuePtr &value) const {
    return valueToVariant(expr_, value);
  }

  QVariant valueToVariant(CExpr *expr, const CExprValuePtr &value) const {
    return match_->valueToVariant(expr, value);
  }

 protected:
  CQChartsModelExprMatch *match_ { nullptr };
  QString                 name_;
  mutable CExpr*          expr_  { nullptr };
};
#endif

//---

#ifdef CQChartsModelExprMatch_USE_TCL
#include <CQTclUtil.h>

class CQChartsModelExprMatchTclFn {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQChartsModelExprMatchTclFn(CQChartsModelExprMatch *model, const QString &name) :
   model_(model), name_(name) {
    QString mathName = "tcl::mathfunc::" + name_;

    cmdId_ = model->qtcl()->createObjCommand(mathName,
               (CQTcl::ObjCmdProc) &CQChartsModelExprMatchTclFn::commandProc,
               (CQTcl::ObjCmdData) this);
  }

  virtual ~CQChartsModelExprMatchTclFn() { }

  static int commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv) {
    CQChartsModelExprMatchTclFn *command = (CQChartsModelExprMatchTclFn *) clientData;

    Vars vars;

    for (int i = 1; i < objc; ++i) {
      Tcl_Obj *obj = const_cast<Tcl_Obj *>(objv[i]);

      vars.push_back(CQTclUtil::variantFromObj(command->model_->qtcl()->interp(), obj));
    }

    QVariant var = command->exec(vars);

    command->model_->setTclResult(var);

    return TCL_OK;
  }

  virtual QVariant exec(const Vars &vars) = 0;

  bool checkColumn(int col) const { return model_->checkColumn(col); }

  bool checkIndex(int row, int col) const { return model_->checkIndex(row, col); }

 protected:
  CQChartsModelExprMatch* model_ { nullptr };
  QString                 name_;
  Tcl_Command             cmdId_ { nullptr };
};
#endif

//---

#ifdef CQChartsModelExprMatch_USE_CEXPR
class CQChartsModelExprMatchExprNameFn : public CQChartsModelExprMatchExprFn {
 public:
  CQChartsModelExprMatchExprNameFn(CQChartsModelExprMatch *match, const QString &name) :
   CQChartsModelExprMatchExprFn(match, name) {
  }

  QVariant exec(const Vars &vars) override { return match_->processCmd(name_, vars); }
};
#endif

#ifdef CQChartsModelExprMatch_USE_TCL
class CQChartsModelExprMatchTclNameFn : public CQChartsModelExprMatchTclFn {
 public:
  CQChartsModelExprMatchTclNameFn(CQChartsModelExprMatch *model, const QString &name) :
   CQChartsModelExprMatchTclFn(model, name) {
  }

  QVariant exec(const Vars &vars) override { return model_->processCmd(name_, vars); }
};
#endif

//------

CQChartsModelExprMatch::
CQChartsModelExprMatch(QAbstractItemModel *model) :
 model_(model)
{
#ifdef CQChartsModelExprMatch_USE_CEXPR
  expr_ = new CExpr;
#endif

#ifdef CQChartsModelExprMatch_USE_TCL
  qtcl_ = new CQTcl;
#endif

  addBuiltinFunctions();
}

CQChartsModelExprMatch::
~CQChartsModelExprMatch()
{
#ifdef CQChartsModelExprMatch_USE_CEXPR
  //for (auto &exprCmd : exprCmds_)
  //  delete exprCmd;

  delete expr_;
#endif

#ifdef CQChartsModelExprMatch_USE_TCL
  for (auto &tclCmd : tclCmds_)
    delete tclCmd;

  delete qtcl_;
#endif
}

void
CQChartsModelExprMatch::
addBuiltinFunctions()
{
#ifdef CQChartsModelExprMatch_USE_CEXPR
  expr_->createRealVariable("pi" , M_PI);
  expr_->createRealVariable("NaN", COSNaN::get_nan());
#endif

#ifdef CQChartsModelExprMatch_USE_TCL
  qtcl_->createVar("pi" , QVariant(M_PI));
  qtcl_->createVar("NaN", QVariant(COSNaN::get_nan()));
#endif

  addFunction("column");
  addFunction("row"   );
  addFunction("cell"  );
  addFunction("header");
}

void
CQChartsModelExprMatch::
addFunction(const QString &name)
{
#ifdef CQChartsModelExprMatch_USE_CEXPR
  addExprFunction(name, new CQChartsModelExprMatchExprNameFn(this, name));
#endif
#ifdef CQChartsModelExprMatch_USE_TCL
  addTclFunction (name, new CQChartsModelExprMatchTclNameFn (this, name));
#endif
}

void
CQChartsModelExprMatch::
addExprFunction(const QString &name, CQChartsModelExprMatchExprFn *fn)
{
  assert(name.length());

#ifdef CQChartsModelExprMatch_USE_CEXPR
  exprCmds_.push_back(fn);
#endif
}

void
CQChartsModelExprMatch::
addTclFunction(const QString &name, CQChartsModelExprMatchTclFn *fn)
{
  assert(name.length());

#ifdef CQChartsModelExprMatch_USE_TCL
  tclCmds_.push_back(fn);
#endif
}

void
CQChartsModelExprMatch::
initMatch(const QString &expr)
{
  matchExpr_ = replaceNumericColumns(expr, QModelIndex());
}

void
CQChartsModelExprMatch::
initColumns()
{
  assert(model_);

  if (exprType_ == ExprType::EXPR) {
#ifdef CQChartsModelExprMatch_USE_CEXPR
    rowValue_ = expr_->createIntegerValue(-1);
    colValue_ = expr_->createIntegerValue(-1);

    rowVar_ = expr_->createVariable("row", rowValue_);
    colVar_ = expr_->createVariable("col", colValue_);
#endif
  }

  columnNames_.clear();
  columnVars_ .clear();

  int nc = model_->columnCount();

  for (int column = 0; column < nc; ++column) {
    QVariant var = model_->headerData(column, Qt::Horizontal);

    bool ok;

    QString name = CQChartsUtil::toString(var, ok);

    columnNames_[column] = name;

    if      (exprType_ == ExprType::EXPR) {
#ifdef CQChartsModelExprMatch_USE_CEXPR
      CExprVariablePtr evar;

      if (expr_->isValidVariableName(name.toStdString()))
        evar = expr_->createVariable(name.toStdString(), CExprValuePtr());

      columnVars_[column] = evar;
#endif
    }
  }
}

bool
CQChartsModelExprMatch::
match(const QString &expr, const QModelIndex &ind, bool &ok)
{
  ok = true;

  QVariant value;

  if (! evaluateExpression(expr, ind, value, /*replace*/ true)) {
    ok = false;
    return true;
  }

  bool rc = value.toBool();

  return rc;
}

bool
CQChartsModelExprMatch::
match(const QModelIndex &ind, bool &ok)
{
  ok = true;

  QVariant value;

  if (! evaluateExpression(matchExpr_, ind, value, /*replace*/ false)) {
    ok = false;
    return true;
  }

  bool rc = value.toBool();

  return rc;
}

//------

QVariant
CQChartsModelExprMatch::
processCmd(const QString &name, const Vars &vars)
{
  if      (name == "column") return columnCmd(vars);
  else if (name == "row"   ) return rowCmd   (vars);
  else if (name == "cell"  ) return cellCmd  (vars);
  else if (name == "header") return headerCmd(vars);
  else                       return QVariant(false);
}

// column(), column(col) : get column value
QVariant
CQChartsModelExprMatch::
columnCmd(const Vars &vars) const
{
  int row = currentRow();
  int col = currentCol();

  if (vars.size() == 0)
    return col;

  if (! varToInt(vars[0], col))
    return QVariant();

  int extraInd = -1;

  if (vars.size() > 1)
    extraInd = 1;

  //---

  if (! checkIndex(row, col)) {
    if (extraInd >= 0)
      return vars[extraInd];

    return QVariant();
  }

  //---

  QModelIndex parent; // TODO

  QModelIndex ind = model()->index(row, col, parent);

  QVariant var = model()->data(ind, Qt::EditRole);

  return var;
}

// row(), row(row) : get row value
QVariant
CQChartsModelExprMatch::
rowCmd(const Vars &vars) const
{
  int row = currentRow();
  int col = currentCol();

  if (vars.size() == 0)
    return row;

  if (! varToInt(vars[0], row))
    return QVariant();

  int extraInd = -1;

  if (vars.size() > 1)
    extraInd = 1;

  //---

  if (! checkIndex(row, col)) {
    if (extraInd >= 0)
      return vars[extraInd];

    return QVariant();
  }

  //---

  QModelIndex parent; // TODO

  QModelIndex ind = model()->index(row, col, parent);

  QVariant var = model()->data(ind, Qt::EditRole);

  return var;
}

// cell(), cell(row,column) : get cell value
QVariant
CQChartsModelExprMatch::
cellCmd(const Vars &vars) const
{
  int row = currentRow();
  int col = currentCol();

  if (vars.size() < 2) {
    // TODO: row and column string value ?
    return QVariant();
  }

  if (! varToInt(vars[0], row))
    return QVariant();

  if (! varToInt(vars[1], col))
    return QVariant();

  int extraInd = -1;

  if (vars.size() > 2)
    extraInd = 2;

  //---

  if (! checkIndex(row, col)) {
    if (extraInd >= 0)
      return vars[extraInd];

    return QVariant();
  }

  //---

  QModelIndex parent; // TODO

  QModelIndex ind = model()->index(row, col, parent);

  QVariant var = model()->data(ind, Qt::EditRole);

  return var;
}

// header(), header(col)
QVariant
CQChartsModelExprMatch::
headerCmd(const Vars &vars) const
{
  int col = currentCol();

  if      (vars.size() == 0) {
  }
  else if (vars.size() == 1) {
    if (! varToInt(vars[0], col))
      return QVariant();
  }
  else {
    return QVariant();
  }

  //---

  if (! checkColumn(col))
    return QVariant();

  QVariant var = model()->headerData(col, Qt::Horizontal, Qt::DisplayRole);

  return var;
}

//------

bool
CQChartsModelExprMatch::
evaluateExpression(const QString &expr, const QModelIndex &ind, QVariant &value, bool replace)
{
  if (expr.length() == 0)
    return false;

  currentRow_ = ind.row   ();
  currentCol_ = ind.column();

  if      (exprType_ == ExprType::EXPR) {
#ifdef CQChartsModelExprMatch_USE_CEXPR
    if (rowValue_.isValid())
      rowValue_->setIntegerValue(currentRow_);

    if (colValue_.isValid())
      colValue_->setIntegerValue(currentCol_);
#endif
  }
  else if (exprType_ == ExprType::TCL) {
#ifdef CQChartsModelExprMatch_USE_TCL
    qtcl_->createVar("row"   , currentRow_);
    qtcl_->createVar("column", currentCol_);
#endif
  }

  //---

  QString expr1 = (replace ? replaceNumericColumns(expr, ind) : expr);

  // TODO: optimize, get header variable names once (checked for valid chars)
  // and only add column value for columns which are found (possibly) in expression.

  int nc = (model_ ? model_->columnCount() : 0);

  for (int column = 0; column < nc; ++column) {
    // get model value
    QModelIndex ind1 = model_->index(ind.row(), column, ind.parent());

    QVariant var = model_->data(ind1, Qt::EditRole);

    // store value in column variable
    if      (exprType_ == ExprType::EXPR) {
#ifdef CQChartsModelExprMatch_USE_CEXPR
      CExprVariablePtr columnVar = columnVars_[column];

      if (! columnVar.isValid())
        continue;

      CExprValuePtr value;

      if (variantToValue(expr_, var, value))
        columnVar->setValue(value);
#endif
    }
    else if (exprType_ == ExprType::TCL) {
#ifdef CQChartsModelExprMatch_USE_TCL
      qtcl_->createVar(columnNames_[column], var);
#endif
    }
  }

  if      (exprType_ == ExprType::EXPR) {
#ifdef CQChartsModelExprMatch_USE_CEXPR
    CExprValuePtr evalue;

    if (! expr_->evaluateExpression(expr1.toStdString(), evalue))
      return false;

    if (! evalue.isValid())
      return false;

    value = valueToVariant(expr_, evalue);
#else
    return false;
#endif
  }
  else if (exprType_ == ExprType::TCL) {
#ifdef CQChartsModelExprMatch_USE_TCL
    int rc = qtcl_->evalExpr(expr1);

    if (rc != TCL_OK) {
      std::cerr << qtcl_->errorInfo(rc).toStdString() << std::endl;
      return false;
    }

    return getTclResult(value);
#else
    return false;
#endif
  }
  else {
    return false;
  }

  return true;
}

bool
CQChartsModelExprMatch::
setTclResult(const QVariant &rc)
{
#ifdef CQChartsModelExprMatch_USE_CEXPR
  qtcl_->setResult(rc);
  return true;
#endif
}

bool
CQChartsModelExprMatch::
getTclResult(QVariant &var) const
{
#ifdef CQChartsModelExprMatch_USE_TCL
  var = qtcl_->getResult();
  return true;
#endif
}

QString
CQChartsModelExprMatch::
replaceNumericColumns(const QString &expr, const QModelIndex &ind) const
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
      else if (parse.isChar('c') && ind.column() > 0) {
        parse.skipChar();

        expr1 += QString("%1").arg(ind.column());
      }
      else if (parse.isChar('r') && ind.row() > 0) {
        parse.skipChar();

        expr1 += QString("%1").arg(ind.row());
      }
      else
        expr1 += "@";
    }
    else
      expr1 += parse.getChar();
  }

  return expr1;
}

bool
CQChartsModelExprMatch::
checkColumn(int col) const
{
  if (col < 0 || col >= model_->columnCount()) return false;

  return true;
}

bool
CQChartsModelExprMatch::
checkIndex(int row, int col) const
{
  if (row < 0 || row >= model_->rowCount   ()) return false;
  if (col < 0 || col >= model_->columnCount()) return false;

  return true;
}

bool
CQChartsModelExprMatch::
variantToValue(CExpr *expr, const QVariant &var, CExprValuePtr &value)
{
#ifdef CQChartsModelExprMatch_USE_CEXPR
  if (! var.isValid())
    return false;

  if      (var.type() == QVariant::Double)
    value = expr->createRealValue(var.toDouble());
  else if (var.type() == QVariant::Int)
    value = expr->createIntegerValue((long) var.toInt());
  else {
    bool ok;

    QString str = CQChartsUtil::toString(var, ok);

    value = expr->createStringValue(str.toStdString());
  }

  return true;
#else
  assert(false && expr && &var && &value);
  return false;
#endif
}

QVariant
CQChartsModelExprMatch::
valueToVariant(CExpr *, const CExprValuePtr &value)
{
#ifdef CQChartsModelExprMatch_USE_CEXPR
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
#else
  assert(false && &value);
  return QVariant();
#endif
}
