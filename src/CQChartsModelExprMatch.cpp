#include <CQChartsModelExprMatch.h>
#include <CQChartsExprCmdValues.h>
#include <CQChartsModelUtil.h>
#include <CQChartsVariant.h>

#include <COSNaN.h>

#include <QAbstractItemModel>
#include <QVariant>

#ifdef CQCharts_USE_TCL
#include <CQTclUtil.h>
#endif

//------

#ifdef CQCharts_USE_TCL
class CQChartsModelExprTcl : public CQTcl {
 public:
  CQChartsModelExprTcl(CQChartsModelExprMatch *match) :
   match_(match), row_(-1) {
  }

  int row() const { return row_; }
  void setRow(int i) { row_ = i; }

  void handleTrace(const char *name, int flags) override {
    if (flags & TCL_TRACE_READS) {
      match_->setVar(name, row());
    }
  }

 private:
  CQChartsModelExprMatch *match_ { nullptr };
  int                     row_   { -1 };
};
#endif

//------

#ifdef CQCharts_USE_TCL
class CQChartsModelExprMatchFn {
 public:
  using Values = std::vector<QVariant>;

 public:
  CQChartsModelExprMatchFn(CQChartsModelExprMatch *model, const QString &name) :
   model_(model), name_(name) {
    qtcl_ = model->qtcl();

    cmdId_ = qtcl()->createExprCommand(name_,
               (CQChartsModelExprTcl::ObjCmdProc) &CQChartsModelExprMatchFn::commandProc,
               (CQChartsModelExprTcl::ObjCmdData) this);
  }

  virtual ~CQChartsModelExprMatchFn() { }

  CQChartsModelExprTcl *qtcl() const { return qtcl_; }

  static int commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv) {
    CQChartsModelExprMatchFn *command = (CQChartsModelExprMatchFn *) clientData;

    Values values;

    for (int i = 1; i < objc; ++i) {
      const Tcl_Obj *obj = objv[i];

      values.push_back(command->qtcl()->variantFromObj(obj));
    }

    QVariant var = command->exec(values);

    command->qtcl()->setResult(var);

    return TCL_OK;
  }

  QVariant exec(const Values &values) { return model_->processCmd(name_, values); }

  bool checkColumn(int col) const { return model_->checkColumn(col); }

  bool checkIndex(int row, int col) const { return model_->checkIndex(row, col); }

 protected:
  CQChartsModelExprMatch* model_ { nullptr };
  QString                 name_;
  CQChartsModelExprTcl*   qtcl_  { nullptr };
  Tcl_Command             cmdId_ { nullptr };
};
#endif

//------

CQChartsModelExprMatch::
CQChartsModelExprMatch(QAbstractItemModel *model) :
 model_(model)
{
#ifdef CQCharts_USE_TCL
  qtcl_ = new CQChartsModelExprTcl(this);
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
  addFunction("isnan" );
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
  nr_ = (model_ ? model_->rowCount   () : 0);
  nc_ = (model_ ? model_->columnCount() : 0);

  matchExpr_ = replaceExprColumns(expr, QModelIndex());
}

void
CQChartsModelExprMatch::
initColumns()
{
  assert(model_);

  columnNames_.clear();
  nameColumns_.clear();

  nr_ = (model_ ? model_->rowCount   () : 0);
  nc_ = (model_ ? model_->columnCount() : 0);

  for (int column = 0; column < nc_; ++column) {
    QVariant var = model_->headerData(column, Qt::Horizontal);

    bool ok;

    QString name = CQChartsVariant::toString(var, ok);

    columnNames_[column] = name;
    nameColumns_[name  ] = column;

    qtcl_->traceVar(name);
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
processCmd(const QString &name, const Values &values)
{
  if      (name == "column") return columnCmd(values);
  else if (name == "row"   ) return rowCmd   (values);
  else if (name == "cell"  ) return cellCmd  (values);
  else if (name == "header") return headerCmd(values);
  else if (name == "isnan" ) return isnanCmd (values);
  else                       return QVariant(false);
}

//------

// column(), column(col), column(col,defVal) : get column value
QVariant
CQChartsModelExprMatch::
columnCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  int row = currentRow();
  int col = currentCol();

  if (! cmdValues.hasValues())
    return col;

  if (! cmdValues.getInt(col))
    return QVariant();

  //---

  if (! checkIndex(row, col)) {
    QString defStr;

    if (! cmdValues.getStr(defStr))
      return QVariant();

    return QVariant(defStr);
  }

  //---

  return getCmdData(row, col);
}

//---

// row(), row(row) : get row value
QVariant
CQChartsModelExprMatch::
rowCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  int row = currentRow();
  int col = currentCol();

  if (! cmdValues.hasValues())
    return row;

  if (! cmdValues.getInt(row))
    return QVariant();

  //---

  if (! checkIndex(row, col)) {
    QString defStr;

    if (! cmdValues.getStr(defStr))
      return QVariant();

    return QVariant(defStr);
  }

  //---

  return getCmdData(row, col);
}

//---

// cell(), cell(row,column), cell(row,column,defVal) : get cell value
QVariant
CQChartsModelExprMatch::
cellCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  int row = currentRow();
  int col = currentCol();

  if (cmdValues.numValues() < 2) {
    // TODO: row and column string value ?
    return QVariant();
  }

  if (! cmdValues.getInt(row))
    return QVariant();

  if (! cmdValues.getInt(col))
    return QVariant();

  //---

  if (! checkIndex(row, col)) {
    QString defStr;

    if (! cmdValues.getStr(defStr))
      return QVariant();

    return QVariant(defStr);
  }

  //---

  return getCmdData(row, col);
}

//---

// header(), header(col)
QVariant
CQChartsModelExprMatch::
headerCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  int col = currentCol();

  (void) cmdValues.getInt(col);

  //---

  if (! checkColumn(col))
    return QVariant();

  //---

  return model()->headerData(col, Qt::Horizontal, Qt::DisplayRole);
}

//---

// isnan()
QVariant
CQChartsModelExprMatch::
isnanCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  double r { 0.0 };

  (void) cmdValues.getReal(r);

  //---

  return CMathUtil::isNaN(r);
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

  QString expr1 = (replace ? replaceExprColumns(expr, ind) : expr);

  qtcl_->setRow(ind.row());

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

void
CQChartsModelExprMatch::
setVar(const QString &name, int row)
{
#ifdef CQCharts_USE_TCL
  auto p = nameColumns_.find(name);

  if (p == nameColumns_.end())
    return;

  int col = (*p).second;

  // get model value
  QVariant var = getCmdData(row, col);

  // store value in column variable
  qtcl_->createVar(name, var);
#endif
}

void
CQChartsModelExprMatch::
setVar(const QModelIndex &ind)
{
#ifdef CQCharts_USE_TCL
  // get model value
  QVariant var = getCmdData(ind);

  // store value in column variable
  qtcl_->createVar(columnNames_[ind.column()], var);
#endif
}

QVariant
CQChartsModelExprMatch::
getCmdData(int row, int col) const
{
  QModelIndex parent; // TODO

  QModelIndex ind = model()->index(row, col, parent);

  return getCmdData(ind);
}

QVariant
CQChartsModelExprMatch::
getCmdData(const QModelIndex &ind) const
{
  QVariant var = model()->data(ind, Qt::EditRole);

  if (! var.isValid())
    var = model()->data(ind, Qt::DisplayRole);

  return var;
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
replaceExprColumns(const QString &expr, const QModelIndex &ind) const
{
  return CQChartsModelUtil::replaceModelExprVars(expr, model_, ind, nr_, nc_);
}

bool
CQChartsModelExprMatch::
checkColumn(int col) const
{
  if (col < 0 || col >= nc_) return false;

  return true;
}

bool
CQChartsModelExprMatch::
checkIndex(int row, int col) const
{
  if (row < 0 || row >= nr_) return false;
  if (col < 0 || col >= nc_) return false;

  return true;
}
