#ifndef CQChartsExprModelFn_H
#define CQChartsExprModelFn_H

#include <CQChartsExprModel.h>
#include <QVariant>

#ifdef CQCharts_USE_TCL
#include <CQTclUtil.h>

class CQChartsExprModelTclFn {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQChartsExprModelTclFn(CQChartsExprModel *model, const QString &name) :
   model_(model), name_(name) {
    qtcl_ = model->qtcl();

    cmdId_ = qtcl()->createExprCommand(name_,
               (CQTcl::ObjCmdProc) &CQChartsExprModelTclFn::commandProc,
               (CQTcl::ObjCmdData) this);
  }

  virtual ~CQChartsExprModelTclFn() { }

  CQTcl *qtcl() const { return qtcl_; }

  static int commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv) {
    CQChartsExprModelTclFn *command = (CQChartsExprModelTclFn *) clientData;

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
  CQChartsExprModel* model_ { nullptr };
  QString            name_;
  CQTcl*             qtcl_  { nullptr };
  Tcl_Command        cmdId_ { nullptr };
};
#endif

#endif
