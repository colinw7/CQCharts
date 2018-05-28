#include <CQExprModel.h>
#include <CQExprModelFn.h>
#include <CQTclUtil.h>

#include <CQStrParse.h>
#include <COSNaN.h>
#include <COSRand.h>
#include <iostream>

#ifdef CQExprModel_USE_CEXPR
class CQExprModelExprNameFn : public CQExprModelExprFn {
 public:
  CQExprModelExprNameFn(CQExprModel *model, const QString &name) :
   CQExprModelExprFn(model, name) {
  }

  QVariant exec(const Vars &vars) override { return model_->processCmd(name_, vars); }
};
#endif

#ifdef CQExprModel_USE_TCL
class CQExprModelTclNameFn : public CQExprModelTclFn {
 public:
  CQExprModelTclNameFn(CQExprModel *model, const QString &name) :
   CQExprModelTclFn(model, name) {
  }

  QVariant exec(const Vars &vars) override { return model_->processCmd(name_, vars); }
};
#endif

//------

CQExprModel::
CQExprModel(QAbstractItemModel *model) :
 model_(model)
{
#ifdef CQExprModel_USE_CEXPR
  expr_ = new CExpr;
#endif

#ifdef CQExprModel_USE_TCL
  qtcl_ = new CQTcl;
#endif

  addBuiltinFunctions();

  setSourceModel(model);
}

CQExprModel::
~CQExprModel()
{
#ifdef CQExprModel_USE_CEXPR
  //for (auto &exprCmd : exprCmds_)
  //  delete exprCmd;

  delete expr_;
#endif

#ifdef CQExprModel_USE_TCL
  for (auto &tclCmd : tclCmds_)
    delete tclCmd;

  delete qtcl_;
#endif
}

void
CQExprModel::
addBuiltinFunctions()
{
#ifdef CQExprModel_USE_CEXPR
  expr_->createRealVariable("pi" , M_PI);
  expr_->createRealVariable("NaN", COSNaN::get_nan());
#endif

#ifdef CQExprModel_USE_TCL
  qtcl_->createVar("pi" , QVariant(M_PI));
  qtcl_->createVar("NaN", QVariant(COSNaN::get_nan()));
#endif

  addFunction("column"   );
  addFunction("row"      );
  addFunction("cell"     );
  addFunction("setColumn");
  addFunction("setRow"   );
  addFunction("setCell"  );
  addFunction("header"   );
  addFunction("setHeader");
  addFunction("type"     );
  addFunction("setType"  );
  addFunction("map"      );
  addFunction("bucket"   );
  addFunction("norm"     );
  addFunction("key"      );
  addFunction("rand"     );
}

void
CQExprModel::
addFunction(const QString &name)
{
  addExprFunction(name, new CQExprModelExprNameFn(this, name));
  addTclFunction (name, new CQExprModelTclNameFn (this, name));
}

void
CQExprModel::
addExprFunction(const QString &name, CQExprModelExprFn *fn)
{
  assert(name.length());

#ifdef CQExprModel_USE_TCL
  exprCmds_.push_back(fn);
#endif
}

void
CQExprModel::
addTclFunction(const QString &name, CQExprModelTclFn *fn)
{
  assert(name.length());

#ifdef CQExprModel_USE_TCL
  tclCmds_.push_back(fn);
#endif
}

bool
CQExprModel::
addExtraColumn(const QString &exprStr, int &column)
{
  QString header, expr;

  if (! decodeExpression(exprStr, header, expr))
    return false;

  return addExtraColumn(header, expr, column);
}

bool
CQExprModel::
addExtraColumn(const QString &header, const QString &expr, int &column)
{
  nc_ = columnCount(QModelIndex());

  beginInsertColumns(QModelIndex(), nc_, nc_);

  //---

  // add new column
  extraColumns_.emplace_back(expr, header);

  int ecolumn = numExtraColumns() - 1;

  //---

  // init calculated values in separate array
  ExtraColumn &extraColumn = extraColumns_[ecolumn];

  nr_ = rowCount();

  extraColumn.values.resize(nr_);

  for (int r = 0; r < nr_; ++r)
    extraColumn.values[r] = QVariant();

  // calculate new values
  extraColumn.function = Function::ADD;

  calcColumn(nc_, ecolumn);

  extraColumn.function = Function::EVAL;

  // remove extra calculated values
  extraColumn.values.clear();

  //---

  endInsertColumns();

  column = columnCount(QModelIndex()) - 1;

  return true;
}

bool
CQExprModel::
removeExtraColumn(int column)
{
  nc_ = columnCount();

  int numNonExtra = nc_ - numExtraColumns();

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
  nc_ = columnCount();

  // set new expression and ensure all column values calculated
  int numNonExtra = nc_ - numExtraColumns();

  int ecolumn = column - numNonExtra;

  if (ecolumn < 0 || ecolumn >= numExtraColumns())
    return false;

  calcColumn(column, ecolumn);

  // store calculated values in separate array
  ExtraColumn &extraColumn = extraColumns_[ecolumn];

  nr_ = rowCount();

  extraColumn.values.resize(nr_);

  for (int r = 0; r < nr_; ++r)
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

  QModelIndex index1 = index(0      , column, QModelIndex());
  QModelIndex index2 = index(nr_ - 1, column, QModelIndex());

  emit dataChanged(index1, index2);

  return true;
}

void
CQExprModel::
calcColumn(int column, int ecolumn)
{
  nr_ = rowCount();
  nc_ = columnCount();

  // ensure all values are evaluated
  for (int r = 0; r < nr_; ++r) {
    currentRow_ = r;
    currentCol_ = column;

    (void) getExtraColumnValue(r, column, ecolumn);
  }
}

bool
CQExprModel::
processExpr(const QString &expr)
{
  nr_ = rowCount();
  nc_ = columnCount();

  bool rc = true;

  QString expr1 = replaceNumericColumns(expr, -1, -1);

  for (int r = 0; r < nr_; ++r) {
    currentRow_ = r;

    QVariant var;

    if (! evaluateExpression(expr1, var)) {
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

  nr_ = rowCount();

  for (int r = 0; r < nr_; ++r) {
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

  nr_ = rowCount();

  for (int r = 0; r < nr_; ++r) {
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

    expr = replaceNumericColumns(expr, row, column);

    QVariant var;

    if (evaluateExpression(expr, var)) {
      std::string str;

      if      (var.type() == QVariant::Double) {
        double real = var.value<double>();

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
      else if (var.type() == QVariant::Int) {
        if (extraColumn.type == CQBaseModel::Type::NONE)
          extraColumn.type = CQBaseModel::Type::INTEGER;
      }
      else if (var.type() == QVariant::Bool) {
        if (extraColumn.type == CQBaseModel::Type::NONE)
          extraColumn.type = CQBaseModel::Type::INTEGER;
      }
      else {
        if (extraColumn.type == CQBaseModel::Type::NONE)
          extraColumn.type = CQBaseModel::Type::STRING;
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

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
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

  if      (role == Qt::DisplayRole || role == Qt::EditRole) {
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

  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (editable_)
    flags |= Qt::ItemIsEditable;

  return flags;
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

//------

class CQExprModelCmdValues {
 public:
  using Values = std::vector<QVariant>;

 public:
  CQExprModelCmdValues(const Values &values) :
   values_(values) {
    eind_ = numValues() - 1;
  }

  int ind() const { return ind_; }

  int numValues() const { return values_.size(); }

  QVariant popValue() { QVariant value = values_.back(); values_.pop_back(); return value; }

  bool getInt(int &i) {
    if (ind_ > eind_) return false;

    bool ok;

    int i1 = values_[ind_].toInt(&ok);

    if (ok) {
      i = i1;

      ++ind_;
    }

    return ok;
  }

  bool getReal(double &r) {
    if (ind_ > eind_) return false;

    bool ok;

    double r1 = values_[ind_].toDouble(&ok);

    if (ok) {
      r = r1;

      ++ind_;
    }

    return ok;
  }

  bool getStr(QString &s) {
    if (ind_ > eind_) return false;

    s = values_[ind_++].toString();

    return true;
  }

  //---

 private:
  Values values_;
  int    ind_  { 0 };
  int    eind_ { 0 };
};

QVariant
CQExprModel::
processCmd(const QString &name, const Values &values)
{
  if      (name == "column"   ) return columnCmd   (values);
  else if (name == "row"      ) return rowCmd      (values);
  else if (name == "cell"     ) return cellCmd     (values);
  else if (name == "setColumn") return setColumnCmd(values);
  else if (name == "setRow"   ) return setRowCmd   (values);
  else if (name == "setCell"  ) return setCellCmd  (values);
  else if (name == "header"   ) return headerCmd   (values);
  else if (name == "setHeader") return setHeaderCmd(values);
  else if (name == "type"     ) return typeCmd     (values);
  else if (name == "setType"  ) return setTypeCmd  (values);
  else if (name == "rand"     ) return randCmd     (values);
  else if (name == "map"      ) return mapCmd      (values);
  else if (name == "bucket"   ) return bucketCmd   (values);
  else if (name == "norm"     ) return normCmd     (values);
  else if (name == "key"      ) return keyCmd      (values);
  else if (name == "rand"     ) return randCmd     (values);
  else                          return QVariant(false);
}

//------

// column(), column(col) : get column value
QVariant
CQExprModel::
columnCmd(const Values &values) const
{
  CQExprModelCmdValues cmdValues(values);

  int row = currentRow();
  int col = currentCol();

  (void) cmdValues.getInt(col);

  //---

  if (! checkIndex(row, col)) {
    QString defStr;

    if (! cmdValues.getStr(defStr))
      return QVariant();

    return QVariant(defStr);
  }

  //---

  QModelIndex ind = index(row, col, QModelIndex());

  return data(ind, Qt::DisplayRole);
}

// row(), row(row) : get row value
QVariant
CQExprModel::
rowCmd(const Values &values) const
{
  CQExprModelCmdValues cmdValues(values);

  int row = currentRow();
  int col = currentCol();

  (void) cmdValues.getInt(row);

  //---

  if (! checkIndex(row, col)) {
    QString defStr;

    if (! cmdValues.getStr(defStr))
      return QVariant();

    return QVariant(defStr);
  }

  //---

  QModelIndex ind = index(row, col, QModelIndex());

  return data(ind, Qt::DisplayRole);
}

// cell(), cell(row,column) : get cell value
QVariant
CQExprModel::
cellCmd(const Values &values) const
{
  CQExprModelCmdValues cmdValues(values);

  int row = currentRow();
  int col = currentCol();

  (void) cmdValues.getInt(row);
  (void) cmdValues.getInt(col);

  //---

  if (! checkIndex(row, col)) {
    QString defStr;

    if (! cmdValues.getStr(defStr))
      return QVariant();

    return QVariant(defStr);
  }

  //---

  QModelIndex ind = index(row, col, QModelIndex());

  return data(ind, Qt::DisplayRole);
}

// setColumn(value), setColumn(col,value) : set column value
QVariant
CQExprModel::
setColumnCmd(const Values &values)
{
  CQExprModelCmdValues cmdValues(values);

  if (cmdValues.numValues() < 1)
    return QVariant();

  QVariant var = cmdValues.popValue();

  //---

  int row = currentRow();
  int col = currentCol();

  (void) cmdValues.getInt(col);

  //---

  if (! checkIndex(row, col))
    return QVariant();

  //---

  QModelIndex ind = index(row, col, QModelIndex());

  bool b = setData(ind, var, Qt::DisplayRole);

  return QVariant(b);
}

// setRow(value), setRow(row,value), setRow(row,col,value) : set row value
QVariant
CQExprModel::
setRowCmd(const Values &values)
{
  CQExprModelCmdValues cmdValues(values);

  if (cmdValues.numValues() < 1)
    return QVariant();

  QVariant var = cmdValues.popValue();

  //---

  int row = currentRow();
  int col = currentCol();

  (void) cmdValues.getInt(row);

  //---

  if (! checkIndex(row, col))
    return QVariant();

  //---

  QModelIndex ind = index(row, col, QModelIndex());

  bool b = setData(ind, var, Qt::DisplayRole);

  return QVariant(b);
}

// setCell(value), setCell(row,col,value) : set row value
QVariant
CQExprModel::
setCellCmd(const Values &values)
{
  CQExprModelCmdValues cmdValues(values);

  if (cmdValues.numValues() < 1)
    return QVariant();

  QVariant var = cmdValues.popValue();

  //---

  int row = currentRow();
  int col = currentCol();

  (void) cmdValues.getInt(row);
  (void) cmdValues.getInt(col);

  //---

  if (! checkIndex(row, col))
    return QVariant();

  //---

  QModelIndex ind = index(row, col, QModelIndex());

  bool b = setData(ind, var, Qt::DisplayRole);

  return QVariant(b);
}

// header(), header(col)
QVariant
CQExprModel::
headerCmd(const Values &values) const
{
  CQExprModelCmdValues cmdValues(values);

  int col = currentCol();

  (void) cmdValues.getInt(col);

  //---

  if (! checkColumn(col))
    return QVariant();

  //---

  return headerData(col, Qt::Horizontal, Qt::DisplayRole);
}

// setHeader(s), setHeader(col,s)
QVariant
CQExprModel::
setHeaderCmd(const Values &values)
{
  CQExprModelCmdValues cmdValues(values);

  if (cmdValues.numValues() < 1)
    return QVariant();

  QVariant var = cmdValues.popValue();

  //---

  int col = currentCol();

  (void) cmdValues.getInt(col);

  //---

  if (! checkColumn(col))
    return QVariant();

  //---

  return setHeaderData(col, Qt::Horizontal, var, Qt::DisplayRole);
}

// type(), type(col)
QVariant
CQExprModel::
typeCmd(const Values &values) const
{
  CQExprModelCmdValues cmdValues(values);

  int col = currentCol();

  (void) cmdValues.getInt(col);

  //---

  if (! checkColumn(col))
    return QVariant();

  //---

  int role = static_cast<int>(CQBaseModel::Role::Type);

  QVariant var = headerData(col, Qt::Horizontal, role);

  QString typeName = CQBaseModel::typeName((CQBaseModel::Type) var.toInt());

  return QVariant(typeName);
}

// setType(s), setType(col,s)
QVariant
CQExprModel::
setTypeCmd(const Values &values)
{
  CQExprModelCmdValues cmdValues(values);

  if (cmdValues.numValues() < 1)
    return QVariant();

  QVariant var = cmdValues.popValue();

  //---

  int col = currentCol();

  (void) cmdValues.getInt(col);

  //---

  if (! checkColumn(col))
    return QVariant();

  //---

  int role = static_cast<int>(CQBaseModel::Role::Type);

  CQBaseModel::Type type = CQBaseModel::nameType(var.toString());

  QVariant typeVar(static_cast<int>(type));

  bool b = setHeaderData(col, Qt::Horizontal, typeVar, role);

  return QVariant(b);
}

// map(), map(max), map(min,max)
QVariant
CQExprModel::
mapCmd(const Values &values) const
{
  CQExprModelCmdValues cmdValues(values);

  int row = currentRow();

  double min = 0.0, max = 1.0;

  if (cmdValues.numValues() == 0)
    return QVariant(row);

  if (cmdValues.numValues() == 1)
    (void) cmdValues.getReal(max);

  (void) cmdValues.getReal(min);
  (void) cmdValues.getReal(max);

  //---

  // scale row number to 0->1
  double x = 0.0;

  if (rowCount())
    x = (1.0*row)/rowCount();

  // map 0->1 -> min->max
  double x1 = x*(max - min) + min;

  return QVariant(x1);
}

// bucket(col), bucket(col,min,max)
QVariant
CQExprModel::
bucketCmd(const Values &values) const
{
  CQExprModelCmdValues cmdValues(values);

  if (cmdValues.numValues() == 0)
    return QVariant(0);

  int row = currentRow();
  int col = currentCol();

  (void) cmdValues.getInt(col);

  if (col < 0 || col >= columnCount())
    return QVariant(0.0);

  //---

  if (! checkIndex(row, col))
    return QVariant();

  //---

  QModelIndex ind = index(row, col, QModelIndex());

  QVariant var = data(ind, Qt::DisplayRole);

  //---

  if      (var.type() == QVariant::Double) {
    double value = var.toDouble();

    double minVal = 0;
    double maxVal = 1;
    int    scale  = 1;

    if      (cmdValues.numValues() == 2) {
      columnRange(col, minVal, maxVal);

      (void) cmdValues.getInt(scale);
    }
    else if (cmdValues.numValues() == 3) {
      (void) cmdValues.getReal(minVal);
      (void) cmdValues.getReal(maxVal);
    }
    else {
      return QVariant(0.0);
    }

    double d = maxVal - minVal;

    int ind = 0;

    if (d) {
      double s = (value - minVal)/d;

      ind = int(s*scale);
    }

    return QVariant(ind);
  }
  else if (var.type() == QVariant::Int) {
    return QVariant(var.toInt());
  }
  else {
    QString str = var.toString();

    int ind = columnStringBucket(col, str);

    return QVariant(ind);
  }
}

// norm(col), norm(col,scale)
QVariant
CQExprModel::
normCmd(const Values &values) const
{
  CQExprModelCmdValues cmdValues(values);

  if (cmdValues.numValues() == 0)
    return QVariant(0.0);

  int row = currentRow();
  int col = currentCol();

  (void) cmdValues.getInt(col);

  if (col < 0 || col >= columnCount())
    return QVariant(0.0);

  double scale = 1.0;

  (void) cmdValues.getReal(scale);

  //---

  if (! checkIndex(row, col))
    return QVariant();

  //---

  QModelIndex ind = index(row, col, QModelIndex());

  QVariant var = data(ind, Qt::DisplayRole);

  //---

  if      (var.type() == QVariant::Double) {
    double value = var.toDouble();

    double minVal = 0;
    double maxVal = 1;

    columnRange(col, minVal, maxVal);

    if (cmdValues.numValues() <= 3)
      (void) cmdValues.getReal(maxVal);
    else {
      (void) cmdValues.getReal(minVal);
      (void) cmdValues.getReal(maxVal);
    }

    double d = maxVal - minVal;
    double s = 0.0;

    if (d)
      s = scale*(value - minVal)/d;

    return QVariant(s);
  }
  else if (var.type() == QVariant::Int) {
    int value = var.toInt();

    int minVal = 0;
    int maxVal = 1;

    columnRange(col, minVal, maxVal);

    if (cmdValues.numValues() <= 3)
      (void) cmdValues.getInt(maxVal);
    else {
      (void) cmdValues.getInt(minVal);
      (void) cmdValues.getInt(maxVal);
    }

    int    d = maxVal - minVal;
    double s = 0.0;

    if (d)
      s = scale*double(value - minVal)/d;

    return QVariant(s);
  }
  else {
    return QVariant(0.0);
  }
}

QVariant
CQExprModel::
keyCmd(const Values &values) const
{
  CQExprModelCmdValues cmdValues(values);

  QString s;

  QString key;

  while (cmdValues.getStr(s)) {
    if (s.size())
      key += ":" + s;
    else
      key = s;
  }

  return QVariant(key);
}

QVariant
CQExprModel::
randCmd(const Values &values) const
{
  CQExprModelCmdValues cmdValues(values);

  double min = 0.0;
  double max = 1.0;

  (void) cmdValues.getReal(min);
  (void) cmdValues.getReal(max);

  double r = COSRand::randIn(min, max);

  return QVariant(r);
}

//---

bool
CQExprModel::
evaluateExpression(const QString &expr, QVariant &var) const
{
  if      (exprType_ == ExprType::EXPR) {
#ifdef CQExprModel_USE_CEXPR
    CExprValuePtr value;

    if (! expr_->evaluateExpression(expr.toStdString(), value))
      return false;

    if (! value.isValid())
      return false;

    std::string str;

    double real    = 0.0;
    long   integer = 0;

    if      (value->getRealValue(real))
      var = QVariant(real);
    else if (value->getIntegerValue(integer))
      var = QVariant((int) integer);
    else
      var = QVariant(QString(str.c_str()));

    return true;
#else
    return false;
#endif
  }
  else if (exprType_ == ExprType::TCL) {
#ifdef CQExprModel_USE_TCL
    int rc = qtcl_->evalExpr(expr);

    if (rc != TCL_OK) {
      std::cerr << qtcl_->errorInfo(rc).toStdString() << std::endl;
      return false;
    }

    return getTclResult(var);
#else
    return false;
#endif
  }
  else {
    return false;
  }
}

bool
CQExprModel::
checkColumn(int col) const
{
  if (col < 0 || col >= columnCount()) return false;

  return true;
}

bool
CQExprModel::
checkIndex(int row, int col) const
{
  if (row < 0 || row >= rowCount   ()) return false;
  if (col < 0 || col >= columnCount()) return false;

  return true;
}

bool
CQExprModel::
variantToValue(CExpr *expr, const QVariant &var, CExprValuePtr &value) const
{
#ifdef CQExprModel_USE_CEXPR
  if (! var.isValid()) {
    value = CExprValuePtr();
    return false;
  }

  if      (var.type() == QVariant::Double)
    value = expr->createRealValue(var.toDouble());
  else if (var.type() == QVariant::Int)
    value = expr->createIntegerValue((long) var.toInt());
  else if (var.type() == QVariant::Bool)
    value = expr->createBooleanValue(var.toBool());
  else
    value = expr->createStringValue(var.toString().toStdString());

  return true;
#else
  assert(false && expr && &var && &value);
  return false;
#endif
}

QVariant
CQExprModel::
valueToVariant(CExpr *, const CExprValuePtr &value) const
{
#ifdef CQExprModel_USE_CEXPR
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
#else
  assert(false && &value);
  return QVariant();
#endif
}

bool
CQExprModel::
setTclResult(const QVariant &rc)
{
#ifdef CQExprModel_USE_TCL
  qtcl_->setResult(rc);
  return true;
#endif
}

bool
CQExprModel::
getTclResult(QVariant &var) const
{
#ifdef CQExprModel_USE_TCL
  var = qtcl_->getResult();
  return true;
#endif
}

QString
CQExprModel::
replaceNumericColumns(const QString &expr, int row, int column) const
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
      else if (parse.isChar('c')) {
        parse.skipChar();

        expr1 += QString("%1").arg(column);
      }
      else if (parse.isChar('r')) {
        parse.skipChar();

        expr1 += QString("%1").arg(row);
      }
      else if (parse.isChar('n')) {
        parse.skipChar();

        if (parse.isChar('c')) {
          parse.skipChar();

          expr1 += QString("%1").arg(nc_);
        }
        else if (parse.isChar('r')) {
          parse.skipChar();

          expr1 += QString("%1").arg(nr_);
        }
        else {
          expr1 += "@nc";
        }
      }
      else
        expr1 += "@";
    }
    else
      expr1 += parse.getChar();
  }

  return expr1;
}
