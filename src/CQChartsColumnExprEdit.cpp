#include <CQChartsColumnExprEdit.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>

CQChartsColumnExprEdit::
CQChartsColumnExprEdit(QWidget *parent) :
 CQChartsLineEdit(parent)
{
  updateTip();

  connect(this, SIGNAL(editingFinished()), this, SLOT(updateSlot()));
}

void
CQChartsColumnExprEdit::
setModelData(CQChartsModelData *modelData)
{
  modelData_ = modelData;
}

void
CQChartsColumnExprEdit::
setAllowPrefix(bool b)
{
  allowPrefix_ = b;
}

void
CQChartsColumnExprEdit::
setExpr(const QString &expr)
{
  expr_ = expr;

  updateSlot();
}

void
CQChartsColumnExprEdit::
updateTip()
{
  auto tip = QString("tcl expression\n");

  if (allowPrefix())
    tip += "+<expr> OR -<column> OR =<column>:<expr>\n";

  tip += "Use: $header for column row value\n"
         "Use: <header> = <expression> to assign header with expression\n";

  tip += "Variables:\n";
  tip += "  @<N>      : Value of column N\n";
  tip += "  @#<N>     : Stringify Value of column N\n";
  tip += "  @c        : column number\n";
  tip += "  @r        : row number\n";
  tip += "  @nc       : column count\n";
  tip += "  @nr       : row count\n";
  tip += "  @v        : cell value\n";
  tip += "  @{<name>} : named column value\n";
  tip += "  #{<name>} : column number for name\n";

  tip += "Functions: ";

  QStringList fnNames;

  if (modelData_) {
  //auto *absModel  = CQChartsModelUtil::getBaseModel(modelData_->currentModel().data());
    auto *exprModel = CQChartsModelUtil::getExprModel(modelData_->currentModel().data());

    if (exprModel) {
      auto exprFnNames = exprModel->fnNames();

      for (const auto &fnName : exprFnNames)
        fnNames << fnName;
    }
  }

  if (! fnNames.length())
    fnNames = QStringList() <<
      "column" << "row" << "cell" << "setColumn" << "setRow" << "setCell" << "header" <<
      "setHeader" << "type" << "setType" << "map" << "bucket" << "norm" << "key" << "rand";

  int  len   = 11;
  bool first = true;

  for (const auto &fnName : fnNames) {
    if (! first) {
      tip += ", ";
      len += 2;
    }

    tip += fnName;
    len += fnName.size();

    if (len > 60) {
      tip += "\n";

      len   = 0;
      first = true;
    }
    else
      first = false;
  }

  setToolTip(tip);
}

void
CQChartsColumnExprEdit::
updateSlot()
{
  expr_ = text().trimmed();

  function_ = CQChartsExprModel::Function::EVAL;
  column_   = CQChartsColumn();
  subExpr_  = expr_;

  if (allowPrefix_) {
    if (modelData_ && expr_.length()) {
      auto model = modelData_->currentModel();

      (void) CQChartsModelUtil::decodeExpression(model.data(), expr_, function_, column_, expr_);
    }
  }

  Q_EMIT exprChanged();
}
