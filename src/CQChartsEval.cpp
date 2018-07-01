#include <CQChartsEval.h>

#include <CExpr.h>
#include <CQExprUtil.h>
#include <CQTclUtil.h>

#include <QColor>

CQChartsEval *
CQChartsEval::
instance()
{
  static CQChartsEval *inst;

  if (! inst)
    inst = new CQChartsEval;

  return inst;
}

CQChartsEval::
CQChartsEval()
{
#ifdef CQCharts_USE_CEXPR
  expr_ = new CExpr;
#endif
#ifdef CQCharts_USE_TCL
  qtcl_ = new CQTcl;

  addFunc("color", (CQTcl::ObjCmdProc) &CQChartsEval::colorCmd);
#endif
}

CQChartsEval::
~CQChartsEval()
{
#ifdef CQCharts_USE_CEXPR
  delete expr_;
#endif
#ifdef CQCharts_USE_TCL
  delete qtcl_;
#endif
}

bool
CQChartsEval::
evalExpr(int row, const QString &exprStr, QVariant &var)
{
  if (exprType_ == ExprType::CEXPR) {
#ifdef CQCharts_USE_CEXPR
    expr_->createVariable("x", expr_->createRealValue(row));

    CExprValuePtr value;

    if (! expr_->evaluateExpression(exprStr.toStdString(), value))
      return false;

    var = CQExprUtil::valueToVariant(expr_, value);

    if (! var.isValid())
      return false;

    return true;
#else
    return false;
#endif
  }
  else if (exprType_ == ExprType::TCL) {
#ifdef CQCharts_USE_TCL
    qtcl()->createVar("x", row);

    return qtcl()->evalExpr(exprStr, var);
#else
    return false;
#endif
  }
  else {
    return false;
  }
}

void
CQChartsEval::
addFunc(const QString &name, CQTcl::ObjCmdProc proc)
{
#ifdef CQCharts_USE_TCL
  QString mathName = "tcl::mathfunc::" + name;

  qtcl_->createObjCommand(mathName, proc, (CQTcl::ObjCmdData) this);
#endif
}

int
CQChartsEval::
colorCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto clamp = [](double x) { return std::min(std::max(x, 0.0), 255.0); };

  CQChartsEval *eval = static_cast<CQChartsEval *>(clientData);

  std::vector<QVariant> vars;

  for (int i = 1; i < objc; ++i) {
    Tcl_Obj *obj = const_cast<Tcl_Obj *>(objv[i]);

    vars.push_back(CQTclUtil::variantFromObj(eval->qtcl()->interp(), obj));
  }

  if      (vars.size() == 1) {
    QColor c(vars[0].toString());

    eval->qtcl()->setResult(c);
  }
  else if (vars.size() == 3) {
    bool ok;

    double r = vars[0].toDouble(&ok);
    double g = vars[1].toDouble(&ok);
    double b = vars[2].toDouble(&ok);

    QColor c;

    c.setRgb(clamp(r), clamp(g), clamp(b));

    eval->qtcl()->setResult(c);
  }

  return TCL_OK;
}
