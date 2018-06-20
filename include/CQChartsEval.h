#ifndef CQChartsEval_H
#define CQChartsEval_H

#include <CExpr.h>
#include <CQExprUtil.h>
#include <CQTclUtil.h>

#define CQChartsEvalInst CQChartsEval::instance()

class CQChartsEval {
 public:
  enum class ExprType {
    NONE,
    CEXPR,
    TCL
  };

 public:
  static CQChartsEval *instance() {
    static CQChartsEval *inst;

    if (! inst)
      inst = new CQChartsEval;

    return inst;
  }

 ~CQChartsEval() {
#ifdef CQCharts_USE_CEXPR
    delete expr_;
#endif
#ifdef CQCharts_USE_TCL
    delete qtcl_;
#endif
  }

  const ExprType &exprType() const { return exprType_; }
  void setExprType(const ExprType &type) { exprType_ = type; }

  bool evalExpr(int row, const QString &exprStr, QVariant &var) {
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
      qtcl_->createVar("x", row);

      return qtcl_->evalExpr(exprStr, var);
#else
      return false;
#endif
    }
    else {
      return false;
    }
  }

 private:
  CQChartsEval() {
#ifdef CQCharts_USE_CEXPR
    expr_ = new CExpr;
#endif
#ifdef CQCharts_USE_TCL
    qtcl_ = new CQTcl;
#endif
  }

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
