#include <CQChartsExprModelFn.h>
#include <CQChartsExprModel.h>
#include <CQTclUtil.h>

CQChartsExprModelFn::
CQChartsExprModelFn(CQChartsExprModel *model, const QString &name) :
 model_(model), name_(name)
{
  qtcl_ = model->qtcl();

  cmdId_ = qtcl()->createExprCommand(name_,
             (CQTcl::ObjCmdProc) &CQChartsExprModelFn::commandProc,
             (CQTcl::ObjCmdData) this);
}

int
CQChartsExprModelFn::
commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *command = (CQChartsExprModelFn *) clientData;

  Values values;

  for (int i = 1; i < objc; ++i) {
    const Tcl_Obj *obj = objv[i];

    values.push_back(CQTclUtil::variantFromObj(command->qtcl()->interp(), obj));
  }

  QVariant var = command->exec(values);

  command->qtcl()->setResult(var);

  return TCL_OK;
}

QVariant
CQChartsExprModelFn::
exec(const Values &values)
{
  return model_->processCmd(name_, values);
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
