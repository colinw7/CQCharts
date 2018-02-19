#include <CQChartsModelExprMatch.h>
#include <COSNaN.h>
#include <CQStrParse.h>

#include <QAbstractItemModel>
#include <QVariant>

class CQChartsModelExprMatchFn : public CExprFunctionObj {
 public:
  CQChartsModelExprMatchFn(CQChartsModelExprMatch *expr) :
   expr_(expr) {
  }

  bool checkColumn(int col) const {
    if (col < 0 || col >= expr_->model()->columnCount()) return false;

    return true;
  }

  bool checkIndex(int row, int col) const {
    // TODO: access model index not row/col
    if (row < 0 || row >= expr_->model()->rowCount   ()) return false;
    if (col < 0 || col >= expr_->model()->columnCount()) return false;

    return true;
  }

 protected:
  CQChartsModelExprMatch *expr_ { nullptr };
};

//---

// column(), column(col) : get column value
class CQChartsModelExprMatchColumnFn : public CQChartsModelExprMatchFn {
 public:
  CQChartsModelExprMatchColumnFn(CQChartsModelExprMatch *expr) :
   CQChartsModelExprMatchFn(expr) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long row = expr_->currentRow();
    long col = expr_->currentCol();

    if (values.size() == 0) {
      return expr->createIntegerValue(col);
    }

    if (! values[0]->getIntegerValue(col))
      return CExprValuePtr();

    int extraInd = -1;

    if (values.size() > 1)
      extraInd = 1;

    //---

    if (! checkIndex(row, col)) {
      if (extraInd >= 0)
        return values[extraInd];

      return CExprValuePtr();
    }

    //---

    QModelIndex parent; // TODO

    QModelIndex ind = expr_->model()->index(row, col, parent);

    QVariant var = expr_->model()->data(ind, Qt::EditRole);

    return CQChartsModelExprMatch::variantToValue(expr, var);
  }
};

//---

// row(), row(row) : get row value
class CQChartsModelExprMatchRowFn : public CQChartsModelExprMatchFn {
 public:
  CQChartsModelExprMatchRowFn(CQChartsModelExprMatch *expr) :
   CQChartsModelExprMatchFn(expr) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long row = expr_->currentRow();
    long col = expr_->currentCol();

    if (values.size() == 0) {
      return expr->createIntegerValue(row);
    }

    if (! values[0]->getIntegerValue(row))
      return CExprValuePtr();

    int extraInd = -1;

    if (values.size() > 1)
      extraInd = 1;

    //---

    if (! checkIndex(row, col)) {
      if (extraInd >= 0)
        return values[extraInd];

      return CExprValuePtr();
    }

    //---

    QModelIndex parent; // TODO

    QModelIndex ind = expr_->model()->index(row, col, parent);

    QVariant var = expr_->model()->data(ind, Qt::EditRole);

    return CQChartsModelExprMatch::variantToValue(expr, var);
  }
};

//---

// cell(), cell(row,column) : get cell value
class CQChartsModelExprMatchCellFn : public CQChartsModelExprMatchFn {
 public:
  CQChartsModelExprMatchCellFn(CQChartsModelExprMatch *expr) :
   CQChartsModelExprMatchFn(expr) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long row = expr_->currentRow();
    long col = expr_->currentCol();

    if (values.size() < 2) {
      // TODO: row and column string value ?
      return CExprValuePtr();
    }

    if (! values[0]->getIntegerValue(row))
      return CExprValuePtr();

    if (! values[1]->getIntegerValue(col))
      return CExprValuePtr();

    int extraInd = -1;

    if (values.size() > 2)
      extraInd = 2;

    //---

    if (! checkIndex(row, col)) {
      if (extraInd >= 0)
        return values[extraInd];

      return CExprValuePtr();
    }

    //---

    QModelIndex parent; // TODO

    QModelIndex ind = expr_->model()->index(row, col, parent);

    QVariant var = expr_->model()->data(ind, Qt::EditRole);

    return CQChartsModelExprMatch::variantToValue(expr, var);
  }
};

//---

// header(), header(col)
class CQChartsModelExprMatchHeaderFn : public CQChartsModelExprMatchFn {
 public:
  CQChartsModelExprMatchHeaderFn(CQChartsModelExprMatch *expr) :
   CQChartsModelExprMatchFn(expr) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long col = expr_->currentCol();

    if      (values.size() == 0) {
    }
    else if (values.size() == 1) {
      if (! values[0]->getIntegerValue(col))
        return CExprValuePtr();
    }
    else {
      return CExprValuePtr();
    }

    //---

    if (! checkColumn(col))
      return CExprValuePtr();

    QVariant var = expr_->model()->headerData(col, Qt::Horizontal, Qt::DisplayRole);

    return CQChartsModelExprMatch::variantToValue(expr, var);
  }
};

//------

CQChartsModelExprMatch::
CQChartsModelExprMatch(QAbstractItemModel *model) :
 model_(model)
{
  createRealVariable("pi" , M_PI);
  createRealVariable("NaN", COSNaN::get_nan());

  addFunction("colVal"   , "...", new CQChartsModelExprMatchColumnFn(this));
  addFunction("rowVal"   , "...", new CQChartsModelExprMatchRowFn   (this));
  addFunction("cellVal"  , "...", new CQChartsModelExprMatchCellFn  (this));
  addFunction("headerVal", "...", new CQChartsModelExprMatchHeaderFn(this));
}

void
CQChartsModelExprMatch::
initMatch(const QString &expr)
{
  matchExpr_ = replaceNumericColumns(expr, QModelIndex());
}

void
CQChartsModelExprMatch::
initColumns()
{
  assert(model_);

  rowValue_ = createIntegerValue(-1);
  colValue_ = createIntegerValue(-1);

  rowVar_ = createVariable("row", rowValue_);
  colVar_ = createVariable("col", colValue_);

  columnVars_.clear();

  int nc = model_->columnCount();

  for (int column = 0; column < nc; ++column) {
    QString name = model_->headerData(column, Qt::Horizontal).toString();

    CExprVariablePtr var;

    if (isValidVariableName(name.toStdString()))
      var = createVariable(name.toStdString(), CExprValuePtr());

    columnVars_.push_back(var);
  }
}

bool
CQChartsModelExprMatch::
match(const QString &expr, const QModelIndex &ind, bool &ok)
{
  ok = true;

  CExprValuePtr value;

  if (! evaluateExpression(expr, ind, value, /*replace*/ true)) {
    ok = false;
    return true;
  }

  bool rc;

  if (! value->getBooleanValue(rc)) {
    ok = false;
    return true;
  }

  return rc;
}

bool
CQChartsModelExprMatch::
match(const QModelIndex &ind, bool &ok)
{
  ok = true;

  CExprValuePtr value;

  if (! evaluateExpression(matchExpr_, ind, value, /*replace*/ false)) {
    ok = false;
    return true;
  }

  bool rc;

  if (! value->getBooleanValue(rc)) {
    ok = false;
    return true;
  }

  return rc;
}

int
CQChartsModelExprMatch::
currentRow() const
{
  long row = -1;

  if (rowValue_.isValid())
    rowValue_->getIntegerValue(row);

  return row;
}

int
CQChartsModelExprMatch::
currentCol() const
{
  long column = -1;

  if (colValue_.isValid())
    colValue_->getIntegerValue(column);

  return column;
}

bool
CQChartsModelExprMatch::
evaluateExpression(const QString &expr, const QModelIndex &ind, CExprValuePtr &value, bool replace)
{
  if (rowValue_.isValid())
    rowValue_->setIntegerValue(ind.row());

  if (colValue_.isValid())
    colValue_->setIntegerValue(ind.column());

  //---

  QString expr1 = (replace ? replaceNumericColumns(expr, ind) : expr);

  // TODO: optimize, get header variable names once (checked for valid chars)
  // and only add column value for columns which are found (possibly) in expression.

  int nc = (model_ ? model_->columnCount() : 0);

  for (int column = 0; column < nc; ++column) {
    CExprVariablePtr columnVar = columnVars_[column];

    if (! columnVar.isValid())
      continue;

    QModelIndex ind1 = model_->index(ind.row(), column, ind.parent());

    QVariant var = model_->data(ind1, Qt::EditRole);

    columnVar->setValue(variantToValue(this, var));
  }

  if (! CExpr::evaluateExpression(expr1.toStdString(), value))
    return false;

  if (! value.isValid())
    return false;

  return true;
}

QString
CQChartsModelExprMatch::
replaceNumericColumns(const QString &expr, const QModelIndex &ind) const
{
  CQStrParse parse(expr);

  QString expr1;

  while (! parse.eof()) {
    // @<n> get column value (current row)
    if (parse.isChar('@')) {
      parse.skipChar();

      if (parse.isDigit()) {
        int pos = parse.getPos();

        while (parse.isDigit())
          parse.skipChar();

        QString str = parse.getBefore(pos);

        int column1 = str.toInt();

        expr1 += QString("column(%1)").arg(column1);
      }
      else if (parse.isChar('c') && ind.column() > 0) {
        parse.skipChar();

        expr1 += QString("%1").arg(ind.column());
      }
      else if (parse.isChar('r') && ind.row() > 0) {
        parse.skipChar();

        expr1 += QString("%1").arg(ind.row());
      }
      else
        expr1 += "@";
    }
    else
      expr1 += parse.getChar();
  }

  return expr1;
}

CExprValuePtr
CQChartsModelExprMatch::
variantToValue(CExpr *expr, const QVariant &var)
{
  if (! var.isValid())
    return CExprValuePtr();

  if      (var.type() == QVariant::Double)
    return expr->createRealValue(var.toDouble());
  else if (var.type() == QVariant::Int)
    return expr->createIntegerValue((long) var.toInt());
  else
    return expr->createStringValue(var.toString().toStdString());
}

QVariant
CQChartsModelExprMatch::
valueToVariant(CExpr *, const CExprValuePtr &value)
{
  if      (value->isRealValue()) {
    double r = 0.0;
    value->getRealValue(r);
    return QVariant(r);
  }
  else if (value->isIntegerValue()) {
    long i = 0;
    value->getIntegerValue(i);
    return QVariant((int) i);
  }
  else {
    std::string s;
    value->getStringValue(s);
    return QVariant(s.c_str());
  }

  return QVariant();
}
