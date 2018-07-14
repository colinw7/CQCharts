#ifndef CQChartsEval_H
#define CQChartsEval_H

#ifdef CQCharts_USE_TCL
#include <CQTclUtil.h>
#endif

#define CQChartsEvalInst CQChartsEval::instance()

class CQChartsEval {
 public:
  static CQChartsEval *instance();

 ~CQChartsEval();

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
#ifdef CQCharts_USE_TCL
  CQTcl* qtcl_ { nullptr };
#endif
};

#endif
