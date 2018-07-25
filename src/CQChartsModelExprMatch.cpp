#include <CQChartsModelExprMatch.h>
#include <CQChartsUtil.h>
#include <COSNaN.h>
#include <CQStrParse.h>

#include <QAbstractItemModel>
#include <QVariant>

namespace {

bool varToInt(const QVariant &var, int &i) {
  bool ok;

  long i1 = CQChartsUtil::toInt(var, ok);
  if (! ok) return false;

  i = i1;

  return true;
}

}

//---

#ifdef CQCharts_USE_TCL
#include <CQTclUtil.h>

class CQChartsModelExprMatchFn {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQChartsModelExprMatchFn(CQChartsModelExprMatch *model, const QString &name) :
   model_(model), name_(name) {
    qtcl_ = model->qtcl();

    cmdId_ = qtcl()->createExprCommand(name_,
               (CQTcl::ObjCmdProc) &CQChartsModelExprMatchFn::commandProc,
               (CQTcl::ObjCmdData) this);
  }

  virtual ~CQChartsModelExprMatchFn() { }

  CQTcl *qtcl() const { return qtcl_; }

  static int commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv) {
    CQChartsModelExprMatchFn *command = (CQChartsModelExprMatchFn *) clientData;

    Vars vars;

    for (int i = 1; i < objc; ++i) {
      Tcl_Obj *obj = const_cast<Tcl_Obj *>(objv[i]);

      vars.push_back(CQTclUtil::variantFromObj(command->qtcl()->interp(), obj));
    }

    QVariant var = command->exec(vars);

    command->qtcl()->setResult(var);

    return TCL_OK;
  }

  QVariant exec(const Vars &vars) { return model_->processCmd(name_, vars); }

  bool checkColumn(int col) const { return model_->checkColumn(col); }

  bool checkIndex(int row, int col) const { return model_->checkIndex(row, col); }

 protected:
  CQChartsModelExprMatch* model_ { nullptr };
  QString                 name_;
  CQTcl*                  qtcl_  { nullptr };
  Tcl_Command             cmdId_ { nullptr };
};
#endif

//------

CQChartsModelExprMatch::
CQChartsModelExprMatch(QAbstractItemModel *model) :
 model_(model)
{
#ifdef CQCharts_USE_TCL
  qtcl_ = new CQTcl;
#endif

  addBuiltinFunctions();
}

CQChartsModelExprMatch::
~CQChartsModelExprMatch()
{
#ifdef CQCharts_USE_TCL
  for (auto &tclCmd : tclCmds_)
    delete tclCmd;

  delete qtcl_;
#endif
}

void
CQChartsModelExprMatch::
addBuiltinFunctions()
{
#ifdef CQCharts_USE_TCL
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
  assert(name.length());

#ifdef CQCharts_USE_TCL
  CQChartsModelExprMatchFn *fn = new CQChartsModelExprMatchFn(this, name);

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

  columnNames_.clear();

  int nc = model_->columnCount();

  for (int column = 0; column < nc; ++column) {
    QVariant var = model_->headerData(column, Qt::Horizontal);

    bool ok;

    QString name = CQChartsUtil::toString(var, ok);

    columnNames_[column] = name;
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

#ifdef CQCharts_USE_TCL
  qtcl_->createVar("row"   , currentRow_);
  qtcl_->createVar("column", currentCol_);

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
    qtcl_->createVar(columnNames_[column], var);
  }

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

bool
CQChartsModelExprMatch::
setTclResult(const QVariant &rc)
{
#ifdef CQCharts_USE_TCL
  qtcl_->setResult(rc);
  return true;
#endif
}

bool
CQChartsModelExprMatch::
getTclResult(QVariant &var) const
{
#ifdef CQCharts_USE_TCL
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

      if      (parse.isDigit()) {
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
      else if (parse.isChar('{')) {
        int pos = parse.getPos();

        parse.skipChar();

        while (! parse.eof() && ! parse.isChar('}'))
          parse.skipChar();

        QString str = parse.getBefore(pos + 1);

        if (parse.isChar('}'))
          parse.skipChar();

        CQChartsColumn c;

        if (CQChartsUtil::stringToColumn(model_, str, c))
          expr1 += QString("column(%1)").arg(c.column());
        else {
          parse.setPos(pos);

          expr1 += "@";
        }
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
