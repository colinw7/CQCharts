#include <CQChartsExprTcl.h>

CQChartsExprTcl::
CQChartsExprTcl(QAbstractItemModel *model) :
 model_(model)
{
}

int
CQChartsExprTcl::
nameColumn(const QString &name) const
{
  auto p = nameColumns_.find(name);

  return (p != nameColumns_.end() ? (*p).second : -1);
}

void
CQChartsExprTcl::
setNameColumn(const QString &name, int column)
{
  nameColumns_[name] = column;

  traceVar(name);
}

void
CQChartsExprTcl::
resetColumns()
{
  nameColumns_.clear();
  columnRoles_.clear();
}

QString
CQChartsExprTcl::
encodeColumnName(const QString &name)
{
  assert(name.length());

  QString name1;

  for (int i = 0; i < name.length(); ++i) {
    const auto &c = name[i];

    if (i == 0) {
      if (c.isLetter() || c == '_')
        name1 += c;
      else {
        name1 += "C";

        if (c.isLetter() || c.isNumber() || c == '_')
          name1 += c;
        else
          name1 += '_';
      }
    }
    else {
      if (c.isLetter() || c.isNumber() || c == '_')
        name1 += c;
      else
        name1 += '_';
    }
  }

  return name1;
}

void
CQChartsExprTcl::
handleTrace(const char *name, int flags)
{
  if (flags & TCL_TRACE_READS)
    setVar(name, row(), column());
}

void
CQChartsExprTcl::
initVars()
{
  // TODO: x optional ?
  traceVar("row"   );
  traceVar("x"     );
  traceVar("column");
  traceVar("col"   );
  traceVar("PI"    );
  traceVar("NaN"   );
  traceVar("_"     );
}

void
CQChartsExprTcl::
initFunctions()
{
  createExprCommand("column",
    reinterpret_cast<CQTcl::ObjCmdProc>(&CQChartsExprTcl::columnCmd), this);
}

void
CQChartsExprTcl::
defineProc(const QString &name, const QString &args, const QString &body)
{
  eval(QString("proc ::tcl::mathfunc::%1 {%2} {%3}").arg(name).arg(args).arg(body));
}

bool
CQChartsExprTcl::
evaluateExpression(const QString &expr, QVariant &value, bool showError) const
{
  ErrorData errorData;

  errorData.showError = showError;

  return evaluateExpression(expr, value, errorData);
}

bool
CQChartsExprTcl::
evaluateExpression(const QString &expr, QVariant &value, ErrorData &errorData) const
{
  auto *th = const_cast<CQChartsExprTcl *>(this);

  CQTcl::EvalData evalData;

  int rc = th->evalExpr(expr, evalData);

  if (rc != TCL_OK) {
    if (isDomainError(rc)) {
      double x = CMathUtil::getNaN();

      value = CQChartsVariant::fromReal(x);

      th->setLastValue(value);

      return true;
    }

    if (errorData.showError)
      std::cerr << evalData.errMsg.toStdString() << std::endl;

    errorData.messages.push_back(evalData.errMsg);

    incErrorCount();

    return false;
  }

  value = getResult();

  th->setLastValue(value);

  return true;
}

void
CQChartsExprTcl::
setVar(const QString &name, int row, int column)
{
  int nameCol = (model() ? nameColumn(name) : -1);

  if      (nameCol >= 0) {
    // get model value
    QModelIndex parent; // TODO

    auto ind = model()->index(row, nameCol, parent);

    QVariant var;

    auto pr = columnRoles_.find(column);

    if (pr != columnRoles_.end())
      var = model()->data(ind, (*pr).second);

    if (! var.isValid())
      var = getModelData(ind);

    // store value in column variable
    if (! var.isValid()) {
      incErrorCount();
      return;
    }

    createVar(name, var);
  }
  else if (name == "row" || name == "x") {
    createVar(name, row + 1); // rows are 1->N
  }
  else if (name == "column" || name == "col") {
    createVar(name, column + 1); // columns are 1->N
  }
  else if (name == "PI") {
    createVar(name, CQChartsVariant::fromReal(M_PI));
  }
  else if (name == "NaN") {
    createVar(name, CQChartsVariant::fromReal(CMathUtil::getNaN()));
  }
  else if (name == "_") {
    if (hasLastValue())
      createVar(name, lastValue());
    else
      createVar(name, CQChartsVariant::fromReal(0.0));
  }
}

int
CQChartsExprTcl::
columnCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *tcl = static_cast<CQChartsExprTcl *>(clientData);

  Values values;

  tcl->parseCmd(objc, objv, values);

  int ind = 0;

  int col = 0;

  if (! tcl->getColumnValue(values, ind, col)) {
    tcl->incErrorCount();
    return TCL_ERROR;
  }

  if (! tcl->checkColumn(col)) {
    tcl->incErrorCount();
    return TCL_ERROR;
  }

  auto res = tcl->getModelData(tcl->row(), col);

  if (! res.isValid()) {
    tcl->incErrorCount();
    return TCL_ERROR;
  }

  tcl->setResult(res);

  return TCL_OK;
}

bool
CQChartsExprTcl::
getColumnValue(const Values &values, int &ind, int &col) const
{
  if (ind >= int(values.size()))
    return false;

  bool ok;

  col = int(CQChartsVariant::toInt(values[size_t(ind)], ok));
  if (! ok) return false;

  ++ind;

  return true;
}

bool
CQChartsExprTcl::
getRowValue(const Values &values, int &ind, int &row) const
{
  if (ind >= int(values.size()))
    return false;

  bool ok;

  row = int(CQChartsVariant::toInt(values[size_t(ind)], ok));
  if (! ok) return false;

  ++ind;

  return true;
}

bool
CQChartsExprTcl::
checkIndex(int row, int col) const
{
  if (! model())
    return false;

  if (row < 0 || row >= model()->rowCount   ()) return false;
  if (col < 0 || col >= model()->columnCount()) return false;

  return true;
}

bool
CQChartsExprTcl::
checkColumn(int col) const
{
  if (! model())
    return false;

  if (col < 0 || col >= model()->columnCount()) return false;

  return true;
}

QVariant
CQChartsExprTcl::
getModelData(int row, int col) const
{
  if (! model())
    return QVariant();

  QModelIndex parent; // TODO

  auto ind = model()->index(row, col, parent);

  return getModelData(ind);
}

QVariant
CQChartsExprTcl::
getModelData(const QModelIndex &ind) const
{
  auto var = model()->data(ind, Qt::EditRole);

  if (! var.isValid())
    var = model()->data(ind, Qt::DisplayRole);

  return var;
}

void
CQChartsExprTcl::
parseCmd(int objc, const Tcl_Obj **objv, Values &values)
{
  for (int i = 1; i < objc; ++i) {
    const auto *obj = objv[i];

    values.push_back(variantFromObj(obj));
  }
}
