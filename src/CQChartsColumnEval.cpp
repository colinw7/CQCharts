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
  qtcl_ = new CQChartsExprTcl();

  addFunc("column", (CQTcl::ObjCmdProc) &CQChartsColumnEval::columnCmd);
  addFunc("color" , (CQTcl::ObjCmdProc) &CQChartsColumnEval::colorCmd );

  qtcl_->initVars();
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

  qtcl_->setModel(const_cast<QAbstractItemModel *>(model()));
  qtcl_->setRow  (row());

  return qtcl_->evaluateExpression(expr, value, isDebug());
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
