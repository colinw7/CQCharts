#include <CQExprModel.h>
#include <CQExprModelExpr.h>
#include <CQBaseModel.h>

//------

CQExprModel::
CQExprModel(QAbstractItemModel *model) :
 model_(model)
{
  expr_ = new CQExprModelExpr(this);

  setSourceModel(model);
}

CQExprModel::
~CQExprModel()
{
  delete expr_;
}

bool
CQExprModel::
addExtraColumn(const QString &exprStr)
{
  QString header, expr;

  if (! decodeExpression(exprStr, header, expr))
    return false;

  return addExtraColumn(header, expr);
}

bool
CQExprModel::
addExtraColumn(const QString &header, const QString &expr)
{
  int nc = columnCount(QModelIndex());

  beginInsertColumns(QModelIndex(), nc, nc);

  //---

  // add new column
  extraColumns_.emplace_back(expr, header);

  int ecolumn = numExtraColumns() - 1;

  //---

  // init calculated values in separate array
  ExtraColumn &extraColumn = extraColumns_[ecolumn];

  extraColumn.values.resize(rowCount());

  for (int r = 0; r < rowCount(); ++r)
    extraColumn.values[r] = QVariant();

  // calculate new values
  extraColumn.function = Function::ADD;

  calcColumn(nc, ecolumn);

  extraColumn.function = Function::EVAL;

  // remove extra calculated values
  extraColumn.values.clear();

  //---

  endInsertColumns();

  return true;
}

bool
CQExprModel::
removeExtraColumn(int column)
{
  int numNonExtra = columnCount() - numExtraColumns();

  int ecolumn = column - numNonExtra;

  if (ecolumn < 0 || ecolumn >= numExtraColumns())
    return false;

  beginRemoveColumns(QModelIndex(), column, column);

  for (int i = ecolumn + 1; i < numExtraColumns(); ++i)
    extraColumns_[i - 1] = extraColumns_[i];

  extraColumns_.pop_back();

  endRemoveColumns();

  return true;
}

bool
CQExprModel::
assignExtraColumn(int column, const QString &exprStr)
{
  QString header, expr;

  if (! decodeExpression(exprStr, header, expr))
    return false;

  return assignExtraColumn(header, column, expr);
}

bool
CQExprModel::
assignExtraColumn(const QString &header, int column, const QString &expr)
{
  // set new expression and ensure all column values calculated
  int numNonExtra = columnCount() - numExtraColumns();

  int ecolumn = column - numNonExtra;

  if (ecolumn < 0 || ecolumn >= numExtraColumns())
    return false;

  calcColumn(column, ecolumn);

  // store calculated values in separate array
  ExtraColumn &extraColumn = extraColumns_[ecolumn];

  extraColumn.values.resize(rowCount());

  for (int r = 0; r < rowCount(); ++r)
    extraColumn.values[r] = extraColumn.variantMap[r];

  // set new expression and ensure all column values calculated
  extraColumn.header = header;
  extraColumn.expr   = expr;

  extraColumn.variantMap.clear();

  extraColumn.function = Function::ASSIGN;

  calcColumn(column, ecolumn);

  extraColumn.function = Function::EVAL;

  // remove extra calculated values
  extraColumn.values.clear();

  //---

  QModelIndex index1 = index(0             , column, QModelIndex());
  QModelIndex index2 = index(rowCount() - 1, column, QModelIndex());

  emit dataChanged(index1, index2);

  return true;
}

void
CQExprModel::
calcColumn(int column, int ecolumn)
{
  // ensure all values are evaluated
  for (int r = 0; r < rowCount(); ++r) {
    currentRow_ = r;
    currentCol_ = column;

    (void) getExtraColumnValue(r, column, ecolumn);
  }
}

bool
CQExprModel::
processExpr(const QString &expr)
{
  bool rc = true;

  QString expr1 = expr_->replaceNumericColumns(expr, -1, -1);

  for (int r = 0; r < rowCount(); ++r) {
    currentRow_ = r;

    CExprValuePtr value;

    if (! expr_->evaluateExpression(expr1.toStdString(), value)) {
      rc = false;
      continue;
    }
  }

  return rc;
}

int
CQExprModel::
columnStringBucket(int column, const QString &value) const
{
  ColumnData &columnData = columnDatas_[column];

  auto p = columnData.stringMap.find(value);

  if (p == columnData.stringMap.end())
    p = columnData.stringMap.insert(p, StringMap::value_type(value, columnData.stringMap.size()));

  return (*p).second;
}

bool
CQExprModel::
columnRange(int column, double &minVal, double &maxVal) const
{
  ColumnData &columnData = columnDatas_[column];

  if (columnData.rmin) {
    minVal = *columnData.rmin;
    maxVal = *columnData.rmax;

    return true;
  }

  for (int r = 0; r < rowCount(); ++r) {
    QModelIndex ind = index(r, column, QModelIndex());

    QVariant var = data(ind, Qt::DisplayRole);

    double value = var.toDouble();

    if (r == 0) {
      minVal = value;
      maxVal = value;
    }
    else {
      minVal = std::min(minVal, value);
      maxVal = std::max(maxVal, value);
    }
  }

  columnData.rmin = minVal;
  columnData.rmax = maxVal;

  return true;
}

bool
CQExprModel::
columnRange(int column, int &minVal, int &maxVal) const
{
  ColumnData &columnData = columnDatas_[column];

  if (columnData.imin) {
    minVal = *columnData.imin;
    maxVal = *columnData.imax;

    return true;
  }

  for (int r = 0; r < rowCount(); ++r) {
    QModelIndex ind = index(r, column, QModelIndex());

    QVariant var = data(ind, Qt::DisplayRole);

    int value = var.toInt();

    if (r == 0) {
      minVal = value;
      maxVal = value;
    }
    else {
      minVal = std::min(minVal, value);
      maxVal = std::max(maxVal, value);
    }
  }

  columnData.imin = minVal;
  columnData.imax = maxVal;

  return true;
}

QModelIndex
CQExprModel::
index(int row, int column, const QModelIndex &parent) const
{
  int nc = sourceModel()->columnCount(sourceModel()->index(row, column, mapToSource(parent)));

  if (column < nc)
    return mapFromSource(sourceModel()->index(row, column, mapToSource(parent)));

  return createIndex(row, column, nullptr);
}

QModelIndex
CQExprModel::
parent(const QModelIndex &child) const
{
  return sourceModel()->parent(mapToSource(child));
}

int
CQExprModel::
rowCount(const QModelIndex &parent) const
{
  return sourceModel()->rowCount(mapToSource(parent));
}

int
CQExprModel::
columnCount(const QModelIndex &parent) const
{
  int nc = sourceModel()->columnCount(mapToSource(parent));

  return nc + numExtraColumns();
}

bool
CQExprModel::
hasChildren(const QModelIndex &parent) const
{
  return sourceModel()->hasChildren(mapToSource(parent));
}

QVariant
CQExprModel::
data(const QModelIndex &index, int role) const
{
  int nc = sourceModel()->columnCount(mapToSource(index));

  if (! index.isValid() || index.column() < nc)
    return sourceModel()->data(mapToSource(index), role);

  //---

  int column = index.column() - nc;

  if (role == Qt::DisplayRole) {
    currentRow_ = index.row();
    currentCol_ = index.column();

    return getExtraColumnValue(currentRow_, currentCol_, column);
  }

  if (role == Qt::TextAlignmentRole) {
    CQExprModel *th = const_cast<CQExprModel *>(this);

    ExtraColumn &extraColumn = th->extraColumn(column);

    if (extraColumn.type == CQBaseModel::Type::INTEGER ||
        extraColumn.type == CQBaseModel::Type::REAL)
      return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    else
      return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
  }

  return QVariant();
}

bool
CQExprModel::
decodeExpressionFn(const QString &exprStr, Function &function, int &column, QString &expr) const
{
  function = Function::EVAL;
  column   = -1;
  expr     = "";

  if (! exprStr.length())
    return true;

  if      (exprStr[0] == '+') {
    function = Function::ADD;

    expr = exprStr.mid(1).simplified();
  }
  // delete column <n>
  else if (expr[0] == '-') {
    function = Function::DELETE;

    QString columnStr = exprStr.mid(1).simplified();

    bool ok;

    column = columnStr.toInt(&ok);

    if (! ok)
      return false;
  }
  else if (expr[0] == '=') {
    function = Function::ASSIGN;

    QString columnExprStr = exprStr.mid(1).simplified();

    int pos = columnExprStr.indexOf(':');

    if (pos < 0)
      return false;

    QString columnStr = columnExprStr.mid(0, pos).simplified();
    QString exprStr   = columnExprStr.mid(pos + 1).simplified();

    bool ok;

    column = columnStr.toInt(&ok);

    if (! ok)
      return false;

    expr = expr.mid(1).simplified();
  }
  else {
    function = Function::EVAL;
    expr     = exprStr;
  }

  return true;
}

bool
CQExprModel::
decodeExpression(const QString &exprStr, QString &header, QString &expr) const
{
  expr = exprStr;

  int pos = expr.indexOf('=');

  if (pos >= 0) {
    header = expr.mid(0, pos).simplified();

    expr = expr.mid(pos + 1).simplified();
  }

  return true;
}

QVariant
CQExprModel::
getExtraColumnValue(int row, int column, int ecolumn) const
{
  CQExprModel *th = const_cast<CQExprModel *>(this);

  ExtraColumn &extraColumn = th->extraColumn(ecolumn);

  //---

  // if evaluating the expression for this row use cached values for referenced values
  if (extraColumn.evaluating) {
    if (! extraColumn.values.empty())
      return extraColumn.values[row];

    return QVariant();
  }

  //---

  auto p = extraColumn.variantMap.find(row);

  if (p == extraColumn.variantMap.end()) {
    extraColumn.evaluating = true;

    QString expr = extraColumn.expr;

    expr = expr_->replaceNumericColumns(expr, row, column);

    QVariant var;

    CExprValuePtr value;

    if (expr_->evaluateExpression(expr.toStdString(), value)) {
      if (value.isValid()) {
        std::string str;

        double real    = 0.0;
        long   integer = 0;

        if      (value->getRealValue(real)) {
          var = QVariant(real);

          bool isInt = CQBaseModel::isInteger(real);

          if      (extraColumn.type == CQBaseModel::Type::NONE) {
            if (CQBaseModel::isInteger(real))
              extraColumn.type = CQBaseModel::Type::INTEGER;
            else
              extraColumn.type = CQBaseModel::Type::REAL;
          }
          else if (extraColumn.type == CQBaseModel::Type::INTEGER) {
            if (! isInt)
              extraColumn.type = CQBaseModel::Type::REAL;
          }
          else if (extraColumn.type == CQBaseModel::Type::REAL) {
          }
        }
        else if (value->getIntegerValue(integer)) {
          var = QVariant((int) integer);

          if (extraColumn.type == CQBaseModel::Type::NONE)
            extraColumn.type = CQBaseModel::Type::INTEGER;
        }
        else if (value->getStringValue(str)) {
          var = QVariant(QString(str.c_str()));

          if (extraColumn.type == CQBaseModel::Type::NONE)
            extraColumn.type = CQBaseModel::Type::STRING;
        }
      }
    }

    p = extraColumn.variantMap.insert(p, VariantMap::value_type(row, var));

    if (extraColumn.function == Function::ADD) {
      if (! extraColumn.values.empty())
        extraColumn.values[row] = var;
    }

    if (debug_)
      std::cerr << "Set Row " << row << " Column " << column << " = " <<
                   var.toString().toStdString() << std::endl;

    extraColumn.evaluating = false;
  }

  return (*p).second;
}

bool
CQExprModel::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  int nc = sourceModel()->columnCount(index);

  if (! index.isValid() || index.column() < nc)
    return sourceModel()->setData(mapToSource(index), value, role);

  int column = index.column() - nc;

  if (column >= numExtraColumns())
    return false;

  ExtraColumn &extraColumn = this->extraColumn(column);

  if (role == Qt::DisplayRole) {
    extraColumn.variantMap[index.row()] = value;

    emit dataChanged(index, index);

    return true;
  }

  return false;
}

QVariant
CQExprModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal)
    return QAbstractProxyModel::headerData(section, orientation, role);

  //--

  int nc = sourceModel()->columnCount(sourceModel()->index(0, section, QModelIndex()));

  if (section < nc)
    return sourceModel()->headerData(section, orientation, role);

  //---

  if (orientation != Qt::Horizontal)
    return QAbstractProxyModel::headerData(section, orientation, role);

  //---

  int column = section - nc;

  if (column >= numExtraColumns())
    return QVariant();

  const ExtraColumn &extraColumn = this->extraColumn(column);

  if      (role == Qt::DisplayRole) {
    if (extraColumn.header.length())
      return extraColumn.header;

    QString header = QString("%1").arg(section + 1);

    return header;
  }
  else if (role == Qt::ToolTipRole) {
    QString str = extraColumn.header;

    CQBaseModel::Type type = extraColumn.type;

    str += ":" + CQBaseModel::typeName(type);

    if (extraColumn.typeValues.length())
      str += ":" + extraColumn.typeValues;

    str += QString("\n(%1)").arg(extraColumn.expr);

    return str;
  }
  else if (role == static_cast<int>(CQBaseModel::Role::Type)) {
    return CQBaseModel::typeToVariant(extraColumn.type);
  }
  else if (role == static_cast<int>(CQBaseModel::Role::TypeValues)) {
    return QVariant(extraColumn.typeValues);
  }

  return QVariant();
}

bool
CQExprModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  if (orientation != Qt::Horizontal)
    return false;

  //--

  int nc = sourceModel()->columnCount(sourceModel()->index(0, section, QModelIndex()));

  if (section < nc)
    return sourceModel()->setHeaderData(section, orientation, value, role);

  //---

  int column = section - nc;

  if (column >= numExtraColumns())
    return false;

  ExtraColumn &extraColumn = this->extraColumn(column);

  if      (role == Qt::DisplayRole) {
    extraColumn.header = value.toString();

    return true;
  }
  else if (role == static_cast<int>(CQBaseModel::Role::Type)) {
    extraColumn.type = CQBaseModel::variantToType(value);

    return true;
  }
  else if (role == static_cast<int>(CQBaseModel::Role::TypeValues)) {
    extraColumn.typeValues = value.toString();

    return true;
  }

  return false;
}

Qt::ItemFlags
CQExprModel::
flags(const QModelIndex &index) const
{
  int nc = sourceModel()->columnCount(index);

  if (! index.isValid() || index.column() < nc)
    return sourceModel()->flags(mapToSource(index));

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex
CQExprModel::
mapToSource(const QModelIndex &index) const
{
  return model_->index(index.row(), index.column(), QModelIndex());
}

QModelIndex
CQExprModel::
mapFromSource(const QModelIndex &index) const
{
  return createIndex(index.row(), index.column(), index.internalPointer());
}
