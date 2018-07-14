#ifndef CQExprModelFn_H
#define CQExprModelFn_H

#include <CQExprModel.h>
#include <QVariant>

#ifdef CQCharts_USE_TCL
#include <CQTclUtil.h>

class CQExprModelTclFn {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQExprModelTclFn(CQExprModel *model, const QString &name) :
   model_(model), name_(name) {
    qtcl_ = model->qtcl();

    cmdId_ = qtcl()->createExprCommand(name_,
               (CQTcl::ObjCmdProc) &CQExprModelTclFn::commandProc,
               (CQTcl::ObjCmdData) this);
  }

  virtual ~CQExprModelTclFn() { }

  CQTcl *qtcl() const { return qtcl_; }

  static int commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv) {
    CQExprModelTclFn *command = (CQExprModelTclFn *) clientData;

    Vars vars;

    for (int i = 1; i < objc; ++i) {
      Tcl_Obj *obj = const_cast<Tcl_Obj *>(objv[i]);

      vars.push_back(CQTclUtil::variantFromObj(command->qtcl()->interp(), obj));
    }

    QVariant var = command->exec(vars);

    command->qtcl()->setResult(var);

    return TCL_OK;
  }

  virtual QVariant exec(const Vars &vars) = 0;

  bool checkColumn(int col) const { return model_->checkColumn(col); }

  bool checkIndex(int row, int col) const { return model_->checkIndex(row, col); }

 protected:
  CQExprModel* model_ { nullptr };
  QString      name_;
  CQTcl*       qtcl_  { nullptr };
  Tcl_Command  cmdId_ { nullptr };
};
#endif

#endif
