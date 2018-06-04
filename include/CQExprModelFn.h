#ifndef CQExprModelFn_H
#define CQExprModelFn_H

#include <CQExprModel.h>
#include <QVariant>

#ifdef CQExprModel_USE_CEXPR
#include <CExpr.h>

class CQExprModelExprFn : public CExprFunctionObj {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQExprModelExprFn(CQExprModel *model, const QString &name) :
   model_(model), name_(name) {
    CExpr *expr = model->expr();

    expr->addFunction(name_.toLatin1().constData(), "...", this);
  }

  virtual ~CQExprModelExprFn() { }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    expr_ = expr;

    Vars vars;

    for (auto &value : values)
      vars.push_back(valueToVariant(value));

    QVariant var = exec(vars);

    return variantToValue(var);
  }

  virtual QVariant exec(const Vars &vars) = 0;

  bool checkColumn(int col) const { return model_->checkColumn(col); }

  bool checkIndex(int row, int col) const { return model_->checkIndex(row, col); }

  CExprValuePtr variantToValue(const QVariant &var) {
    return variantToValue(expr_, var);
  }

  CExprValuePtr variantToValue(CExpr *expr, const QVariant &var) {
    CExprValuePtr value;

    (void) model_->variantToValue(expr, var, value);

    return value;
  }

  QVariant valueToVariant(const CExprValuePtr &value) const {
    return valueToVariant(expr_, value);
  }

  QVariant valueToVariant(CExpr *expr, const CExprValuePtr &value) const {
    return model_->valueToVariant(expr, value);
  }

 protected:
  CQExprModel*   model_ { nullptr };
  QString        name_;
  mutable CExpr* expr_  { nullptr };
};
#endif

//---

#ifdef CQExprModel_USE_TCL
#include <CQTclUtil.h>

class CQExprModelTclFn {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQExprModelTclFn(CQExprModel *model, const QString &name) :
   model_(model), name_(name) {
    QString mathName = "tcl::mathfunc::" + name_;

    cmdId_ = model->qtcl()->createObjCommand(mathName,
               (CQTcl::ObjCmdProc) &CQExprModelTclFn::commandProc,
               (CQTcl::ObjCmdData) this);
  }

  virtual ~CQExprModelTclFn() { }

  static int commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv) {
    CQExprModelTclFn *command = (CQExprModelTclFn *) clientData;

    Vars vars;

    for (int i = 1; i < objc; ++i) {
      Tcl_Obj *obj = const_cast<Tcl_Obj *>(objv[i]);

      vars.push_back(CQTclUtil::variantFromObj(command->model_->qtcl()->interp(), obj));
    }

    QVariant var = command->exec(vars);

    command->model_->setTclResult(var);

    return TCL_OK;
  }

  virtual QVariant exec(const Vars &vars) = 0;

  bool checkColumn(int col) const { return model_->checkColumn(col); }

  bool checkIndex(int row, int col) const { return model_->checkIndex(row, col); }

 protected:
  CQExprModel* model_ { nullptr };
  QString      name_;
  Tcl_Command  cmdId_ { nullptr };
};
#endif

#endif
