#ifndef CQExprModelFn_H
#define CQExprModelFn_H

#include <CQExprModel.h>
#include <QVariant>

#ifdef CQExprModel_USE_CEXPR
#include <CExpr.h>

class CQExprModelFn : public CExprFunctionObj {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQExprModelFn(CQExprModel *model, const QString &name) :
   model_(model), name_(name) {
  }

  virtual ~CQExprModelFn() { }

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
#elif CQExprModel_USE_TCL
#include <tcl.h>

class CQExprModelFn {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQExprModelFn(CQExprModel *model, const QString &name) :
   model_(model), name_(name) {
    Tcl_CreateCommand(model->tclInterp(), (char *) name_.toLatin1().constData(),
                      (Tcl_CmdProc *) &CQExprModelFn::commandProc,
                      (ClientData) this, nullptr);
  }

  virtual ~CQExprModelFn() { }

  static int commandProc(ClientData clientData, Tcl_Interp *, int argc, const char **argv) {
    CQExprModelFn *command = (CQExprModelFn *) clientData;

    Vars vars;

    for (int i = 1; i < argc; ++i)
      vars.push_back(QVariant(QString(argv[i])));

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
};
#endif

#endif
