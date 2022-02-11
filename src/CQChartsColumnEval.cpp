#include <CQChartsColumnEval.h>
#include <CQChartsExprTcl.h>
#include <CQChartsVariant.h>
#include <CMathUtil.h>
#include <QColor>

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
  qtcl_ = std::make_shared<CQChartsExprTcl>();

  // add column, color expressions
  addFunc("column", reinterpret_cast<CQTcl::ObjCmdProc>(&CQChartsColumnEval::columnCmd));
  addFunc("color" , reinterpret_cast<CQTcl::ObjCmdProc>(&CQChartsColumnEval::colorCmd ));

  qtcl_->initVars();
}

CQChartsColumnEval::
~CQChartsColumnEval()
{
}

CQTcl *
CQChartsColumnEval::
qtcl() const
{
  return qtcl_.get();
}

const QAbstractItemModel *
CQChartsColumnEval::
model() const
{
  return qtcl_->model();
}

void
CQChartsColumnEval::
setModel(const QAbstractItemModel *model)
{
  qtcl_->setModel(model);
}

int
CQChartsColumnEval::
row() const
{
  return qtcl_->row();
}

void
CQChartsColumnEval::
setRow(int r)
{
  qtcl_->setRow(r);
}

void
CQChartsColumnEval::
addFunc(const QString &name, CQTcl::ObjCmdProc proc)
{
  assert(name.length());

  std::unique_lock<std::mutex> lock(mutex_);

  qtcl_->createExprCommand(name, proc, static_cast<CQTcl::ObjCmdData>(this));
}

//---

bool
CQChartsColumnEval::
evaluateExpression(const QString &expr, QVariant &value, bool showError)
{
  if (expr.length() == 0)
    return false;

  std::unique_lock<std::mutex> lock(mutex_);

  // set current model
  qtcl_->setModel(const_cast<QAbstractItemModel *>(model()));

  // set current row
  qtcl_->setRow(row());

  return qtcl_->evaluateExpression(expr, value, (showError || isDebug()));
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

  auto res = eval->getModelData(row, col);

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
    c = CQChartsUtil::stringToColor(values[0].toString());
  }
  else if (values.size() == 3) {
    auto clamp = [](double x) { return int(CMathUtil::clamp(x, 0.0, 255.0)); };

    bool ok;

    double r = CQChartsVariant::toReal(values[0], ok);
    double g = CQChartsVariant::toReal(values[1], ok);
    double b = CQChartsVariant::toReal(values[2], ok);

    c.setRgb(clamp(r), clamp(g), clamp(b));
  }

  auto res = c;

  return (res.isValid() ? eval->setResult(res) : TCL_ERROR);
}

void
CQChartsColumnEval::
parseCmd(int objc, const Tcl_Obj **objv, Values &values)
{
  return qtcl_->parseCmd(objc, objv, values);
}

int
CQChartsColumnEval::
setResult(const QVariant &res)
{
  qtcl_->setResult(res);

  return TCL_OK;
}

//---

bool
CQChartsColumnEval::
getColumnValue(const Values &values, int &ind, int &col) const
{
  return qtcl_->getColumnValue(values, ind, col);
}

bool
CQChartsColumnEval::
getRowValue(const Values &values, int &ind, int &row) const
{
  return qtcl_->getRowValue(values, ind, row);
}

bool
CQChartsColumnEval::
checkIndex(int row, int col) const
{
  return qtcl_->checkIndex(row, col);
}

QVariant
CQChartsColumnEval::
getModelData(int row, int col) const
{
  return qtcl_->getModelData(row, col);
}
