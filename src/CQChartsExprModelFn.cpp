#include <CQChartsExprModelFn.h>
#include <CQChartsExprModel.h>
#include <CQTclUtil.h>

CQChartsExprModelFn::
CQChartsExprModelFn(CQChartsExprModel *model, const QString &name) :
 model_(model), name_(name)
{
#ifdef CQCharts_USE_TCL
  qtcl_ = model->qtcl();

  cmdId_ = qtcl()->createExprCommand(name_,
             (CQTcl::ObjCmdProc) &CQChartsExprModelFn::commandProc,
             (CQTcl::ObjCmdData) this);
#endif
}

#ifdef CQCharts_USE_TCL
int
CQChartsExprModelFn::
commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  CQChartsExprModelFn *command = (CQChartsExprModelFn *) clientData;

  Values values;

  for (int i = 1; i < objc; ++i) {
    Tcl_Obj *obj = const_cast<Tcl_Obj *>(objv[i]);

    values.push_back(CQTclUtil::variantFromObj(command->qtcl()->interp(), obj));
  }

  QVariant var = command->exec(values);

  command->qtcl()->setResult(var);

  return TCL_OK;
}
#endif

QVariant
CQChartsExprModelFn::
exec(const Values &values)
{
#ifdef CQCharts_USE_TCL
  return model_->processCmd(name_, values);
#else
  return QVariant();
#endif
}

bool
CQChartsExprModelFn::
checkColumn(int col) const
{
  return model_->checkColumn(col);
}

bool
CQChartsExprModelFn::
checkIndex(int row, int col) const
{
  return model_->checkIndex(row, col);
}
