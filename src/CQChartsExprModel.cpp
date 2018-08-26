#include <CQChartsExprModel.h>
#include <CQChartsExprModelFn.h>
#include <CQChartsExprCmdValues.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsColumnType.h>
#include <CQChartsUtil.h>
#include <CQChartsRand.h>
#include <CQCharts.h>
#ifdef CQCharts_USE_TCL
#include <CQTclUtil.h>
#endif

#include <CQStrParse.h>
#include <COSNaN.h>
#include <QColor>
#include <iostream>

//------

#ifdef CQCharts_USE_TCL
class CQChartsExprTcl : public CQTcl {
 public:
  CQChartsExprTcl(CQChartsExprModel *model) :
   model_(model), row_(-1) {
  }

  int row() const { return row_; }
  void setRow(int i) { row_ = i; }

  void handleTrace(const char *name, int flags) override {
    if (flags & TCL_TRACE_READS) {
      model_->setVar(name, row());
    }
  }

 private:
  CQChartsExprModel *model_ { nullptr };
  int                row_   { -1 };
};
#endif

//------

CQChartsExprModel::
CQChartsExprModel(CQCharts *charts, QAbstractItemModel *model) :
 charts_(charts), model_(model)
{
#ifdef CQCharts_USE_TCL
  qtcl_ = new CQChartsExprTcl(this);
#endif

  addBuiltinFunctions();

  setSourceModel(model);
}

CQChartsExprModel::
~CQChartsExprModel()
{
#ifdef CQCharts_USE_TCL
  for (auto &tclCmd : tclCmds_)
    delete tclCmd;

  delete qtcl_;
#endif
}

void
CQChartsExprModel::
addBuiltinFunctions()
{
#ifdef CQCharts_USE_TCL
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
  addFunction("rnorm"    );
  addFunction("concat"   );
  addFunction("color"    );

  addFunction("remap"    );
  addFunction("timeval"  );
}

CQTcl *
CQChartsExprModel::
qtcl() const
{
  return qtcl_;
}

void
CQChartsExprModel::
addFunction(const QString &name)
{
  assert(name.length());

#ifdef CQCharts_USE_TCL
  CQChartsExprModelFn *fn = new CQChartsExprModelFn(this, name);

  tclCmds_.push_back(fn);
#endif
}

bool
CQChartsExprModel::
addExtraColumn(const QString &exprStr, int &column)
{
  QString header, expr;

  if (! decodeExpression(exprStr, header, expr))
    return false;

  return addExtraColumn(header, expr, column);
}

bool
CQChartsExprModel::
addExtraColumn(const QString &header, const QString &expr, int &column)
{
  const_cast<CQChartsExprModel *>(this)->initCalc();

  //---

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
CQChartsExprModel::
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
CQChartsExprModel::
assignExtraColumn(int column, const QString &exprStr)
{
  QString header, expr;

  if (! decodeExpression(exprStr, header, expr))
    return false;

  return assignExtraColumn(header, column, expr);
}

bool
CQChartsExprModel::
assignExtraColumn(const QString &header, int column, const QString &expr)
{
  const_cast<CQChartsExprModel *>(this)->initCalc();

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
CQChartsExprModel::
calcColumn(int column, const QString &expr, Values &values) const
{
  for (int r = 0; r < nr_; ++r) {
    currentRow_ = r;
    currentCol_ = column;

    QVariant var;

    QString expr1 = replaceExprColumns(expr, currentRow_, currentCol_).simplified();

    (void) evaluateExpression(expr1, var);

    values.push_back(var);
  }
}

bool
CQChartsExprModel::
queryColumn(int column, const QString &expr, Rows &rows) const
{
  const_cast<CQChartsExprModel *>(this)->initCalc();

  bool rc = true;

  for (int r = 0; r < nr_; ++r) {
    currentRow_ = r;
    currentCol_ = column;

    QVariant var;

    QString expr1 = replaceExprColumns(expr, currentRow_, currentCol_).simplified();

    if (! evaluateExpression(expr1, var)) {
      rc = false;
      continue;
    }

    if (var.toBool())
      rows.push_back(r);
  }

  return rc;
}

void
CQChartsExprModel::
initCalc()
{
  nr_ = rowCount();
  nc_ = columnCount();

  columnNames_.clear();
  nameColumns_.clear();

  for (const auto &np : charts_->procs()) {
    const auto &proc = np.second;

    qtcl_->eval(QString("proc ::tcl::mathfunc::%1 {%2} {%3}").
                 arg(proc.name).arg(proc.args).arg(proc.body));
  }

  for (int column = 0; column < nc_; ++column) {
    QVariant var = this->headerData(column, Qt::Horizontal);

    bool ok;

    QString name = CQChartsVariant::toString(var, ok);

    columnNames_[column] = name;
    nameColumns_[name  ] = column;

    qtcl_->traceVar(name);
  }
}

void
CQChartsExprModel::
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
CQChartsExprModel::
processExpr(const QString &expr)
{
  initCalc();

  bool rc = true;

  QString expr1 = replaceExprColumns(expr, -1, -1);

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

bool
CQChartsExprModel::
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
CQChartsExprModel::
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
CQChartsExprModel::
index(int row, int column, const QModelIndex &parent) const
{
  int nc = sourceModel()->columnCount(sourceModel()->index(row, column, mapToSource(parent)));

  if (column < nc)
    return mapFromSource(sourceModel()->index(row, column, mapToSource(parent)));

  return createIndex(row, column, nullptr);
}

QModelIndex
CQChartsExprModel::
parent(const QModelIndex &child) const
{
  return sourceModel()->parent(mapToSource(child));
}

int
CQChartsExprModel::
rowCount(const QModelIndex &parent) const
{
  return sourceModel()->rowCount(mapToSource(parent));
}

int
CQChartsExprModel::
columnCount(const QModelIndex &parent) const
{
  int nc = sourceModel()->columnCount(mapToSource(parent));

  return nc + numExtraColumns();
}

bool
CQChartsExprModel::
hasChildren(const QModelIndex &parent) const
{
  return sourceModel()->hasChildren(mapToSource(parent));
}

QVariant
CQChartsExprModel::
data(const QModelIndex &index, int role) const
{
  int nc = sourceModel()->columnCount(mapToSource(index));

  if (! index.isValid() || index.column() < nc)
    return sourceModel()->data(mapToSource(index), role);

  //---

  int column = index.column() - nc;

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    currentRow_ = index.row();
    currentCol_ = index.column();

    return getExtraColumnValue(currentRow_, currentCol_, column);
  }

  if (role == Qt::TextAlignmentRole) {
    CQChartsExprModel *th = const_cast<CQChartsExprModel *>(this);

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
CQChartsExprModel::
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
CQChartsExprModel::
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
CQChartsExprModel::
getExtraColumnValue(int row, int column, int ecolumn) const
{
  CQChartsExprModel *th = const_cast<CQChartsExprModel *>(this);

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

    expr = replaceExprColumns(expr, row, column).simplified();

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
CQChartsExprModel::
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
CQChartsExprModel::
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

  if      (role == Qt::DisplayRole || role == Qt::EditRole) {
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
CQChartsExprModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  if (orientation != Qt::Horizontal)
    return QAbstractProxyModel::setHeaderData(section, orientation, value, role);

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
CQChartsExprModel::
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
CQChartsExprModel::
mapToSource(const QModelIndex &index) const
{
  if (! index.isValid())
    return QModelIndex();

  return model_->index(index.row(), index.column(), QModelIndex());
}

QModelIndex
CQChartsExprModel::
mapFromSource(const QModelIndex &index) const
{
  if (! index.isValid())
    return QModelIndex();

  return createIndex(index.row(), index.column(), index.internalPointer());
}

//------

QVariant
CQChartsExprModel::
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
  else if (name == "map"      ) return mapCmd      (values);
  else if (name == "bucket"   ) return bucketCmd   (values);
  else if (name == "norm"     ) return normCmd     (values);
  else if (name == "key"      ) return keyCmd      (values);
  else if (name == "rand"     ) return randCmd     (values);
  else if (name == "rnorm"    ) return rnormCmd    (values);
  else if (name == "concat"   ) return concatCmd   (values);
  else if (name == "color"    ) return colorCmd    (values);

  else if (name == "remap"    ) return remapCmd    (values);
  else if (name == "timeval"  ) return timevalCmd  (values);

  else                          return QVariant(false);
}

//------

// column(), column(col), column(col,defVal) : get column value
QVariant
CQChartsExprModel::
columnCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  int row = currentRow();
  int col = currentCol();

  if (! cmdValues.hasValues())
    return col;

  if (! cmdValues.getInt(col))
    return QVariant();

  //---

  if (! checkIndex(row, col)) {
    QString defStr;

    if (! cmdValues.getStr(defStr))
      return QVariant();

    return QVariant(defStr);
  }

  //---

  return getCmdData(row, col);
}

//---

// row(), row(row) : get row value
QVariant
CQChartsExprModel::
rowCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  int row = currentRow();
  int col = currentCol();

  if (! cmdValues.hasValues())
    return row;

  if (! cmdValues.getInt(row))
    return QVariant();

  //---

  if (! checkIndex(row, col)) {
    QString defStr;

    if (! cmdValues.getStr(defStr))
      return QVariant();

    return QVariant(defStr);
  }

  //---

  return getCmdData(row, col);
}

//---

// cell(), cell(row,column), cell(row,column,defVal) : get cell value
QVariant
CQChartsExprModel::
cellCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  int row = currentRow();
  int col = currentCol();

  if (cmdValues.numValues() < 2) {
    // TODO: row and column string value ?
    return QVariant();
  }

  if (! cmdValues.getInt(row))
    return QVariant();

  if (! cmdValues.getInt(col))
    return QVariant();

  //---

  if (! checkIndex(row, col)) {
    QString defStr;

    if (! cmdValues.getStr(defStr))
      return QVariant();

    return QVariant(defStr);
  }

  //---

  return getCmdData(row, col);
}

//---

// setColumn(value), setColumn(col,value) : set column value
QVariant
CQChartsExprModel::
setColumnCmd(const Values &values)
{
  CQChartsExprCmdValues cmdValues(values);

  if (cmdValues.numValues() < 1)
    return QVariant();

  QVariant var = cmdValues.popValue(); // last value

  //---

  int row = currentRow();
  int col = currentCol();

  (void) cmdValues.getInt(col);

  //---

  if (! checkIndex(row, col))
    return QVariant();

  //---

  bool b = setCmdData(row, col, var);

  return QVariant(b);
}

//---

// setRow(value), setRow(row,value), setRow(row,col,value) : set row value
QVariant
CQChartsExprModel::
setRowCmd(const Values &values)
{
  CQChartsExprCmdValues cmdValues(values);

  if (cmdValues.numValues() < 1)
    return QVariant();

  QVariant var = cmdValues.popValue(); // last value

  //---

  int row = currentRow();
  int col = currentCol();

  (void) cmdValues.getInt(row);

  //---

  if (! checkIndex(row, col))
    return QVariant();

  //---

  bool b = setCmdData(row, col, var);

  return QVariant(b);
}

//---

// setCell(value), setCell(row,col,value) : set row value
QVariant
CQChartsExprModel::
setCellCmd(const Values &values)
{
  CQChartsExprCmdValues cmdValues(values);

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

  bool b = setCmdData(row, col, var);

  return QVariant(b);
}

//---

// header(), header(col)
QVariant
CQChartsExprModel::
headerCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  int col = currentCol();

  (void) cmdValues.getInt(col);

  //---

  if (! checkColumn(col))
    return QVariant();

  //---

  return headerData(col, Qt::Horizontal, Qt::DisplayRole);
}

//---

// setHeader(s), setHeader(col,s)
QVariant
CQChartsExprModel::
setHeaderCmd(const Values &values)
{
  CQChartsExprCmdValues cmdValues(values);

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

//---

// type(), type(col)
QVariant
CQChartsExprModel::
typeCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

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

//---

// setType(s), setType(col,s)
QVariant
CQChartsExprModel::
setTypeCmd(const Values &values)
{
  CQChartsExprCmdValues cmdValues(values);

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

//---

// map(), map(max), map(min,max)
QVariant
CQChartsExprModel::
mapCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  int row = currentRow();

  double min = 0.0, max = 1.0;

  if (cmdValues.numValues() == 0)
    return QVariant(row);

  if (cmdValues.numValues() == 1)
    (void) cmdValues.getReal(max);
  else {
    (void) cmdValues.getReal(min);
    (void) cmdValues.getReal(max);
  }

  //---

  // scale row number to 0->1
  double x = 0.0;

  if (rowCount())
    x = (1.0*row)/rowCount();

  // map 0->1 -> min->max
  double x1 = x*(max - min) + min;

  return QVariant(x1);
}

//---

// bucket(col,delta), bucket(col,start,delta)
QVariant
CQChartsExprModel::
bucketCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

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

  QVariant var = getCmdData(row, col);

  //---

  int bucket = -1;

  CQBucketer &bucketer = columnDatas_[col].bucketer;

  if      (var.type() == QVariant::Double) {
    double value = var.toDouble();

    double start = 0;
    double delta = 1;

    if      (cmdValues.numValues() == 2) {
      double minVal, maxVal;

      columnRange(col, minVal, maxVal);

      start = minVal;

      (void) cmdValues.getReal(delta);
    }
    else if (cmdValues.numValues() == 3) {
      (void) cmdValues.getReal(start);
      (void) cmdValues.getReal(delta);
    }
    else {
      return QVariant(-1);
    }

    bucketer.setType(CQBucketer::Type::REAL_RANGE);

    bucketer.setRStart(start);
    bucketer.setRDelta(delta);

    bucket = bucketer.realBucket(value);
  }
  else if (var.type() == QVariant::Int) {
    return QVariant(var.toInt());
  }
  else {
    QString str = var.toString();

    bucket = bucketer.stringBucket(str);
  }

  return QVariant(bucket);
}

//---

// norm(col), norm(col,scale)
QVariant
CQChartsExprModel::
normCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

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

  QVariant var = getCmdData(row, col);

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

//---

// key(str1,str2,...)
QVariant
CQChartsExprModel::
keyCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

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

//---

// rand(min=0,max=1)
QVariant
CQChartsExprModel::
randCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  double min = 0.0;
  double max = 1.0;

  (void) cmdValues.getReal(min);
  (void) cmdValues.getReal(max);

  CQChartsRand::RealInRange rand(min, max);

  double r = rand.gen();

  return QVariant(r);
}

//---

// rnorm(mean=0,stddev=1)
QVariant
CQChartsExprModel::
rnormCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  double mean   = 0.0;
  double stddev = 1.0;

  (void) cmdValues.getReal(mean);
  (void) cmdValues.getReal(stddev);

  CQChartsRand::NormalRealInRange rand(mean, stddev);

  double r = rand.gen();

  return QVariant(r);
}

//---

// concat(str1,str2,...)
QVariant
CQChartsExprModel::
concatCmd(const Values &values) const
{
  QString str;

  for (std::size_t i = 0; i < values.size(); ++i)
    str += values[i].toString();

  return QVariant(str);
}

//---

// color(name)
QVariant
CQChartsExprModel::
colorCmd(const Values &values) const
{
  if (values.size() == 1)
    return QVariant(QColor(values[0].toString()));

  return QVariant();
}

//---

// remap()            - current column, 0.0, 1.0
// remap(col)         - specified column, 0.0, 1.0
// remap(col,max)     - specified column, 0.0, max
// remap(col,min,max) - specified column, min, max
QVariant
CQChartsExprModel::
remapCmd(const Values &values)
{
  CQChartsExprCmdValues cmdValues(values);

  int row = currentRow();
  int col = currentCol();

  double r1 = 0.0, r2 = 1.0;

  if (cmdValues.numValues() >= 1) {
    (void) cmdValues.getInt(col);

    if      (cmdValues.numValues() >= 3) {
      (void) cmdValues.getReal(r1);
      (void) cmdValues.getReal(r2);
    }
    else if (cmdValues.numValues() >= 2) {
      (void) cmdValues.getReal(r2);
    }
  }

  //---

  if (! this->checkIndex(row, col))
    return QVariant(0.0);

  QModelIndex ind = this->index(row, col, QModelIndex());

  //---

  double rmin = 0.0, rmax = 1.0;

  getColumnRange(ind, rmin, rmax);

  //---

  bool ok;

  double r = CQChartsUtil::modelReal(this, ind, ok);

  if (! ok)
    return QVariant(0.0);

  double rm = CMathUtil::map(r, rmin, rmax, r1, r2);

  return QVariant(rm);
}

//---

// timeval(fmt)     - timeval fmt for current column
// timeval(col,fmt) - timeval fmt for specified column
QVariant
CQChartsExprModel::
timevalCmd(const Values &values)
{
  CQChartsExprCmdValues cmdValues(values);

  if (cmdValues.numValues() < 1)
    return QVariant();

  QString fmt = cmdValues.popValue().toString(); // last value

  //---

  int row = currentRow();
  int col = currentCol();

  (void) cmdValues.getInt(col);

  //---

  if (! checkIndex(row, col))
    return QVariant();

  //---

  QModelIndex ind = index(row, col, QModelIndex());

  bool ok;

  QVariant var = CQChartsUtil::modelValue(this, ind, Qt::EditRole, ok);
  if (! ok) return QVariant();

  CQChartsColumn column(col);

  bool converted;

  QVariant var1 = CQChartsUtil::columnUserData(charts_, this, column, var, converted);

  if (var1.isValid())
    var = var1;

  double t = CQChartsVariant::toReal(var1, ok);
  if (! ok) return QVariant();

  return CQChartsUtil::timeToString(fmt, t);
}

//---

QVariant
CQChartsExprModel::
getCmdData(int row, int col) const
{
  QModelIndex parent; // TODO

  QModelIndex ind = this->index(row, col, parent);

  QVariant var = this->data(ind, Qt::EditRole);

  if (! var.isValid())
    var = this->data(ind, Qt::DisplayRole);

  return var;
}

bool
CQChartsExprModel::
setCmdData(int row, int col, const QVariant &var)
{
  QModelIndex parent; // TODO

  QModelIndex ind = this->index(row, col, parent);

  bool b = this->setData(ind, var, Qt::EditRole);

  if (! b)
    b = this->setData(ind, var, Qt::DisplayRole);

  return b;
}

//---

bool
CQChartsExprModel::
evaluateExpression(const QString &expr, QVariant &var) const
{
  if (expr.length() == 0)
    return false;

#ifdef CQCharts_USE_TCL
  qtcl_->setRow(currentRow_);

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

void
CQChartsExprModel::
setVar(const QString &name, int row)
{
#ifdef CQCharts_USE_TCL
  auto p = nameColumns_.find(name);

  if (p == nameColumns_.end())
    return;

  int col = (*p).second;

  // get model value
  QVariant var = getCmdData(row, col);

  // store value in column variable
  qtcl_->createVar(name, var);
#endif
}

bool
CQChartsExprModel::
checkColumn(int col) const
{
  if (col < 0 || col >= columnCount()) return false;

  return true;
}

bool
CQChartsExprModel::
checkIndex(int row, int col) const
{
  if (row < 0 || row >= rowCount   ()) return false;
  if (col < 0 || col >= columnCount()) return false;

  return true;
}

bool
CQChartsExprModel::
setTclResult(const QVariant &rc)
{
#ifdef CQCharts_USE_TCL
  qtcl_->setResult(rc);
  return true;
#endif
}

bool
CQChartsExprModel::
getTclResult(QVariant &var) const
{
#ifdef CQCharts_USE_TCL
  var = qtcl_->getResult();
  return true;
#endif
}

QString
CQChartsExprModel::
replaceExprColumns(const QString &expr, int row, int column) const
{
  QModelIndex ind = this->index(row, column, QModelIndex());

  CQChartsExprModel *th = const_cast<CQChartsExprModel *>(this);

  return CQChartsUtil::replaceModelExprVars(expr, th, ind, nr_, nc_);
}

//---

bool
CQChartsExprModel::
getColumnRange(const QModelIndex &ind, double &rmin, double &rmax)
{
  CQChartsColumn column(ind.column());

  CQChartsModelData *modelData = charts_->getModelData(this->model_);

  if (! modelData) {
    //int modelInd = charts_->addModel(this->model_);

    //modelData = charts_->getModelData(modelInd);
  }

  //---

  CQBaseModel::Type  type;
  CQChartsNameValues nameValues;

  if (modelData) {
    CQChartsModelDetails *details = modelData->details();
    assert(details);

    CQChartsModelColumnDetails *columnDetails = details->columnDetails(column);
    assert(columnDetails);

    type       = columnDetails->type();
    nameValues = columnDetails->nameValues();
  }
  else {
    if (! CQChartsUtil::columnValueType(charts_, this, column, type, nameValues))
      return false;
  }

  //---

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  CQChartsColumnType *typeData = columnTypeMgr->getType(type);

  CQChartsColumnRealType *rtypeData = dynamic_cast<CQChartsColumnRealType *>(typeData);

  if (! rtypeData)
    return false;

  //---

  if (! rtypeData->rmin(nameValues, rmin)) {
    if (modelData)
      rmin = modelData->details()->columnDetails(ind.column())->minValue().toReal();
  }

  if (! rtypeData->rmax(nameValues, rmax)) {
    if (modelData)
      rmax = modelData->details()->columnDetails(ind.column())->maxValue().toReal();
  }

  return true;
}
