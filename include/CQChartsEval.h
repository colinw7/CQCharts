#ifndef CQChartsEval_H
#define CQChartsEval_H

#include <CQTclUtil.h>

class CExpr;

#define CQChartsEvalInst CQChartsEval::instance()

class CQChartsEval {
 public:
  enum class ExprType {
    NONE,
    CEXPR,
    TCL
  };

 public:
  static CQChartsEval *instance();

 ~CQChartsEval();

  const ExprType &exprType() const { return exprType_; }
  void setExprType(const ExprType &type) { exprType_ = type; }

  bool evalExpr(int row, const QString &exprStr, QVariant &var);

#ifdef CQCharts_USE_TCL
  CQTcl* qtcl() const { return qtcl_; }
#endif

 private:
  CQChartsEval();

  void addFunc(const QString &name, CQTcl::ObjCmdProc proc);

 private:
  static int colorCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);

 private:
#if defined(CQCharts_USE_TCL)
  ExprType   exprType_      { ExprType::TCL };
#elif defined(CQCharts_USE_CEXPR)
  ExprType   exprType_      { ExprType::CEXPR };
#else
  ExprType   exprType_      { ExprType::NONE };
#endif

#ifdef CQCharts_USE_CEXPR
  CExpr* expr_ { nullptr };
#endif
#ifdef CQCharts_USE_TCL
  CQTcl* qtcl_ { nullptr };
#endif
};

#endif
