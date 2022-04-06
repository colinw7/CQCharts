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
  if (allowPrefix())
    setToolTip("+<expr> OR -<column> OR =<column>:<expr>\n"
      "Use: @<number> as shorthand for column(<number>)\n"
      "Functions: column, row, cell, setColumn, setRow, setCell\n"
      " header, setHeader, type, setType, map, bucket, norm, key, rand");
  else
    setToolTip("tcl expression\n"
      "Use: @<number> as shorthand for column(<number>)\n"
      "Functions: column, row, cell, setColumn, setRow, setCell\n"
      " header, setHeader, type, setType, map, bucket, norm, key, rand");
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

  emit exprChanged();
}
