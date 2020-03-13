#include <CQChartsColumnEval.h>
#include <CQChartsVariant.h>
#include <CMathUtil.h>
#include <QColor>

//------

class CQChartsColumnEvalTcl : public CQTcl {
 public:
  CQChartsColumnEvalTcl(CQChartsColumnEval *eval) :
   eval_(eval) {
  }

  int row() const { return row_; }
  void setRow(int i) { row_ = i; }

  int column() const { return column_; }
  void setColumn(int i) { column_ = i; }

  void handleTrace(const char *name, int flags) override {
    if (flags & TCL_TRACE_READS) {
      eval_->setVar(name, row(), column());
    }
  }

 private:
  CQChartsColumnEval *eval_   { nullptr };
  int                 row_    { -1 };
  int                 column_ { -1 };
};

//------

CQChartsColumnEval *
CQChartsColumnEval::
instance()
{
  static CQChartsColumnEval *inst;

  if (! inst)
    inst = new CQChartsColumnEval;

  return inst;
}

CQChartsColumnEval::
CQChartsColumnEval()
{
  qtcl_ = new CQChartsColumnEvalTcl(this);

  addFunc("column", (CQTcl::ObjCmdProc) &CQChartsColumnEval::columnCmd);
  addFunc("color" , (CQTcl::ObjCmdProc) &CQChartsColumnEval::colorCmd );

  qtcl_->traceVar("row"   );
  qtcl_->traceVar("x"     );
  qtcl_->traceVar("column");
  qtcl_->traceVar("col"   );
  qtcl_->traceVar("PI"    );
  qtcl_->traceVar("NaN"   );
  qtcl_->traceVar("_"     );
}

CQChartsColumnEval::
~CQChartsColumnEval()
{
  delete qtcl_;
}

CQTcl *
CQChartsColumnEval::
qtcl() const
{
  return qtcl_;
}

void
CQChartsColumnEval::
addFunc(const QString &name, CQTcl::ObjCmdProc proc)
{
  assert(name.length());

  std::unique_lock<std::mutex> lock(mutex_);

  qtcl_->createExprCommand(name, proc, (CQTcl::ObjCmdData) this);
}

//---

bool
CQChartsColumnEval::
evaluateExpression(const QString &expr, QVariant &value)
{
  if (expr.length() == 0)
    return false;

  std::unique_lock<std::mutex> lock(mutex_);

  qtcl_->setRow(row());

  bool showError = isDebug();

  int rc = qtcl()->evalExpr(expr, showError);

  if (rc != TCL_OK) {
    if (qtcl_->isDomainError(rc)) {
      double x = CMathUtil::getNaN();

      value      = QVariant(x);
      lastValue_ = value;

      return true;
    }

    if (isDebug())
      std::cerr << qtcl_->errorInfo(rc).toStdString() << std::endl;

    return false;
  }

  if (! getTclResult(value))
    return false;

  lastValue_ = value;

  return true;
}

bool
CQChartsColumnEval::
setTclResult(const QVariant &rc)
{
  qtcl_->setResult(rc);

  return true;
}

bool
CQChartsColumnEval::
getTclResult(QVariant &var) const
{
  var = qtcl_->getResult();

  return true;
}

void
CQChartsColumnEval::
setVar(const QString &name, int row, int column)
{
  if      (name == "row" || name == "x") {
    qtcl_->createVar(name, row);
  }
  else if (name == "column" || name == "col") {
    qtcl_->createVar(name, column);
  }
  else if (name == "pi") {
    qtcl_->createVar(name, QVariant(M_PI));
  }
  else if (name == "NaN") {
    qtcl_->createVar(name, QVariant(CMathUtil::getNaN()));
  }
  else if (name == "_") {
    if (lastValue_.isValid())
      qtcl_->createVar(name, QVariant(lastValue_));
    else
      qtcl_->createVar(name, QVariant(0.0));
  }
}

int
CQChartsColumnEval::
columnCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *eval = static_cast<CQChartsColumnEval *>(clientData);

  Values values;

  eval->parseCmd(objc, objv, values);

  int ind = 0;

  int col = 0, row = 0;

  if (! eval->getColumnValue(values, ind, col))
    return TCL_ERROR;

  if (! eval->getRowValue(values, ind, row))
    row = eval->row();

  if (! eval->checkIndex(row, col))
    return TCL_ERROR;

  QVariant res = eval->getModelData(row, col);

  return (res.isValid() ? eval->setResult(res) : TCL_ERROR);
}

int
CQChartsColumnEval::
colorCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *eval = static_cast<CQChartsColumnEval *>(clientData);

  Values values;

  eval->parseCmd(objc, objv, values);

  QColor c;

  if      (values.size() == 1) {
    c = QColor(values[0].toString());
  }
  else if (values.size() == 3) {
    auto clamp = [](double x) { return CMathUtil::clamp(x, 0.0, 255.0); };

    bool ok;

    double r = CQChartsVariant::toReal(values[0], ok);
    double g = CQChartsVariant::toReal(values[1], ok);
    double b = CQChartsVariant::toReal(values[2], ok);

    c.setRgb(clamp(r), clamp(g), clamp(b));
  }

  QVariant res = c;

  return (res.isValid() ? eval->setResult(res) : TCL_ERROR);
}

void
CQChartsColumnEval::
parseCmd(int objc, const Tcl_Obj **objv, Values &values)
{
  for (int i = 1; i < objc; ++i) {
    const Tcl_Obj *obj = objv[i];

    values.push_back(qtcl()->variantFromObj(obj));
  }
}

int
CQChartsColumnEval::
setResult(const QVariant &res)
{
  qtcl()->setResult(res);

  return TCL_OK;
}

//---

bool
CQChartsColumnEval::
getColumnValue(const Values &values, int &ind, int &col) const
{
  if (ind >= int(values.size()))
    return false;

  bool ok;

  col = CQChartsVariant::toInt(values[ind], ok);
  if (! ok) return false;

  ++ind;

  return true;
}

bool
CQChartsColumnEval::
getRowValue(const Values &values, int &ind, int &row) const
{
  if (ind >= int(values.size()))
    return false;

  bool ok;

  row = CQChartsVariant::toInt(values[ind], ok);
  if (! ok) return false;

  ++ind;

  return true;
}

bool
CQChartsColumnEval::
checkIndex(int row, int col) const
{
  if (! model_)
    return false;

  if (row < 0 || row >= model_->rowCount   ()) return false;
  if (col < 0 || col >= model_->columnCount()) return false;

  return true;
}

QVariant
CQChartsColumnEval::
getModelData(int row, int col) const
{
  if (! model_)
    return QVariant();

  QModelIndex parent; // TODO

  QModelIndex ind = model_->index(row, col, parent);

  QVariant var = model_->data(ind, Qt::EditRole);

  if (! var.isValid())
    var = model_->data(ind, Qt::DisplayRole);

  return var;
}
