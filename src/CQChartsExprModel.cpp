#include <CQChartsExprModel.h>
#include <CQChartsExprModelFn.h>
#include <CQChartsExprCmdValues.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelFilter.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColumnType.h>
#include <CQChartsRand.h>
#include <CQCharts.h>
#include <CQDataModel.h>
#include <CQTclUtil.h>
#include <CQPerfMonitor.h>

#include <COSNaN.h>
#include <QColor>
#include <iostream>

//------

class CQChartsExprTcl : public CQTcl {
 public:
  CQChartsExprTcl(CQChartsExprModel *model) :
   model_(model) {
  }

  int row() const { return row_; }
  void setRow(int i) { row_ = i; }

  int column() const { return column_; }
  void setColumn(int i) { column_ = i; }

  void handleTrace(const char *name, int flags) override {
    if (flags & TCL_TRACE_READS) {
      model_->setVar(name, row(), column());
    }
  }

 private:
  CQChartsExprModel *model_  { nullptr };
  int                row_    { -1 };
  int                column_ { -1 };
};

//------

CQChartsExprModel::
CQChartsExprModel(CQCharts *charts, CQChartsModelFilter *filter, QAbstractItemModel *model) :
 charts_(charts), filter_(filter), model_(model)
{
  assert(model);

  setObjectName("exprModel");

  qtcl_ = new CQChartsExprTcl(this);

  addBuiltinFunctions();

  setSourceModel(model);

  connect(model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
          this, SLOT(dataChangedSlot(const QModelIndex &, const QModelIndex &)));
}

CQChartsExprModel::
~CQChartsExprModel()
{
  for (auto &tclCmd : tclCmds_)
    delete tclCmd;

  delete qtcl_;

  for (auto &extraColumn : extraColumns_)
    delete extraColumn;
}

void
CQChartsExprModel::
addBuiltinFunctions()
{
  // get/set model data (TODO: hierarchical)
  addFunction("column"   );
  addFunction("row"      );
  addFunction("cell"     );
  addFunction("setColumn");
  addFunction("setRow"   );
  addFunction("setCell"  );

  // get/set header data
  addFunction("header"   );
  addFunction("setHeader");

  // get/set column type
  addFunction("type"   );
  addFunction("setType");

  // map values
  addFunction("map"   );
  addFunction("remap" );
  addFunction("bucket");
  addFunction("norm"  );
  addFunction("scale" );

  // random
  addFunction("rand" );
  addFunction("rnorm");

  // string
  addFunction("match");

  // color
  addFunction("color");

  // time
  addFunction("timeval");

//addFunction("key"      );
//addFunction("concat"   );
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

  CQChartsExprModelFn *fn = new CQChartsExprModelFn(this, name);

  tclCmds_.push_back(fn);
}

bool
CQChartsExprModel::
isOrigColumn(int column) const
{
  int nc = columnCount();

  int numNonExtra = nc - numExtraColumns();

  return (column >= 0 && column < numNonExtra);
}

bool
CQChartsExprModel::
isExtraColumn(int column) const
{
  int nc = columnCount();

  int numNonExtra = nc - numExtraColumns();

  if (column < numNonExtra)
    return false;

  int ecolumn = column - numNonExtra;

  return (ecolumn >= numExtraColumns());
}

bool
CQChartsExprModel::
isReadOnly() const
{
  CQDataModel *dataModel = qobject_cast<CQDataModel *>(model_);

  return (dataModel ? dataModel->isReadOnly() : true);
}

void
CQChartsExprModel::
setReadOnly(bool b)
{
  CQDataModel *dataModel = qobject_cast<CQDataModel *>(model_);

  if (dataModel)
    dataModel->setReadOnly(b);
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
  CQPerfTrace trace("CQChartsExprModel::addExtraColumn");

  initCalc();

  //---

  nc_ = columnCount(QModelIndex());

  beginInsertColumns(QModelIndex(), nc_, nc_);

  //---

  // add new column
  extraColumns_.push_back(new ExtraColumn(expr, header));

  int ecolumn = numExtraColumns() - 1;

  //---

  // init calculated values in separate array
  ExtraColumn *extraColumn = extraColumns_[ecolumn];

  nr_ = rowCount();

  extraColumn->values.resize(nr_);

  for (int r = 0; r < nr_; ++r)
    extraColumn->values[r] = QVariant();

  // calculate new values
  extraColumn->function = Function::ADD;

  calcExtraColumn(nc_, ecolumn);

  extraColumn->function = Function::EVAL;

  // remove extra calculated values
  extraColumn->values.clear();

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

  delete extraColumns_[ecolumn];

  beginRemoveColumns(QModelIndex(), column, column);

  for (int i = ecolumn + 1; i < numExtraColumns(); ++i)
    extraColumns_[i - 1] = extraColumns_[i];

  extraColumns_.pop_back();

  endRemoveColumns();

  return true;
}

bool
CQChartsExprModel::
assignColumn(int column, const QString &exprStr)
{
  QString header, expr;

  if (! decodeExpression(exprStr, header, expr))
    return false;

  return assignColumn(header, column, expr);
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
assignColumn(const QString &header, int column, const QString &expr)
{
  CQPerfTrace trace("CQChartsExprModel::assignColumn");

  initCalc();

  nc_ = columnCount();

  // set new expression and ensure all column values calculated
  int numNonExtra = nc_ - numExtraColumns();

  if (column < 0 || column >= numNonExtra)
    return false;

  int role = Qt::EditRole;

  nr_ = rowCount();

  for (int r = 0; r < nr_; ++r) {
    currentRow_ = r;
    currentCol_ = column;

    QString expr1 = replaceExprColumns(expr, currentRow_, currentCol_).simplified();

    QVariant var;

    if (evaluateExpression(expr1, var)) {
      QModelIndex ind = index(currentRow_, currentCol_, QModelIndex());

      setData(ind, var, role);
    }
  }

  if (header != "")
    setHeaderData(column, Qt::Horizontal, header);

  //---

  QModelIndex index1 = index(0      , column, QModelIndex());
  QModelIndex index2 = index(nr_ - 1, column, QModelIndex());

  emit dataChanged(index1, index2);

  return true;
}

bool
CQChartsExprModel::
assignExtraColumn(const QString &header, int column, const QString &expr)
{
  CQPerfTrace trace("CQChartsExprModel::assignExtraColumn");

  initCalc();

  nc_ = columnCount();

  // set new expression and ensure all column values calculated
  int numNonExtra = nc_ - numExtraColumns();

  int ecolumn = column - numNonExtra;

  if (ecolumn < 0 || ecolumn >= numExtraColumns())
    return false;

  calcExtraColumn(column, ecolumn);

  // store calculated values in separate array
  ExtraColumn *extraColumn = extraColumns_[ecolumn];

  nr_ = rowCount();

  extraColumn->values.resize(nr_);

  for (int r = 0; r < nr_; ++r)
    extraColumn->values[r] = extraColumn->variantMap[r];

  // set new expression and ensure all column values calculated
  if (header != "")
    extraColumn->header = header;

  extraColumn->expr = expr;

  extraColumn->variantMap.clear();

  extraColumn->function = Function::ASSIGN;

  calcExtraColumn(column, ecolumn);

  extraColumn->function = Function::EVAL;

  // remove extra calculated values
  extraColumn->values.clear();

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
  CQPerfTrace trace("CQChartsExprModel::calcColumn");

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
  CQPerfTrace trace("CQChartsExprModel::queryColumn");

  initCalc();

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

bool
CQChartsExprModel::
getExtraColumnDetails(int column, QString &header, QString &expr) const
{
  nc_ = columnCount();

  int numNonExtra = nc_ - numExtraColumns();

  int ecolumn = column - numNonExtra;

  if (ecolumn < 0 || ecolumn >= numExtraColumns())
    return false;

  const ExtraColumn *extraColumn = extraColumns_[ecolumn];

  header = extraColumn->header;
  expr   = extraColumn->expr;

  return true;
}

void
CQChartsExprModel::
initCalc() const
{
  CQChartsExprModel *th = const_cast<CQChartsExprModel *>(this);

  th->initCalc();
}

void
CQChartsExprModel::
initCalc()
{
  std::unique_lock<std::mutex> lock(mutex_);

  nr_ = rowCount();
  nc_ = columnCount();

  columnNames_.clear();
  nameColumns_.clear();

  //---

  // add user defined functions
  // TODO: only add if needed ? (traces)
  for (const auto &np : charts_->procs()) {
    const auto &proc = np.second;

    qtcl_->eval(QString("proc ::tcl::mathfunc::%1 {%2} {%3}").
                 arg(proc.name).arg(proc.args).arg(proc.body));
  }

  //---

  // add traces for column names
  for (int column = 0; column < nc_; ++column) {
    QVariant var = this->headerData(column, Qt::Horizontal);

    bool ok;

    QString name = CQChartsVariant::toString(var, ok);

    columnNames_[column] = name;
    nameColumns_[name  ] = column;

    qtcl_->traceVar(name);
  }

  qtcl_->traceVar("row"   );
  qtcl_->traceVar("column");
  qtcl_->traceVar("col"   );
  qtcl_->traceVar("pi"    );
  qtcl_->traceVar("NaN"   );
}

void
CQChartsExprModel::
calcExtraColumn(int column, int ecolumn)
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
  CQPerfTrace trace("CQChartsExprModel::processExpr");

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
  auto p = columnDatas_.find(column);

  if (p != columnDatas_.end()) {
    const ColumnData &columnData = (*p).second;

    if (columnData.rmin) {
      minVal = *columnData.rmin;
      maxVal = *columnData.rmax;

      return true;
    }
  }

  //---

  CQChartsExprModel *th = const_cast<CQChartsExprModel *>(this);

  return th->calcColumnRange(column, minVal, maxVal);
}

bool
CQChartsExprModel::
calcColumnRange(int column, double &minVal, double &maxVal)
{
  std::unique_lock<std::mutex> lock(mutex_);

  ColumnData &columnData = columnDatas_[column];

  nr_ = rowCount();

  for (int r = 0; r < nr_; ++r) {
    QModelIndex ind = index(r, column, QModelIndex());

    QVariant var = data(ind, Qt::DisplayRole);

    bool ok;

    double value = CQChartsVariant::toReal(var, ok);

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
  auto p = columnDatas_.find(column);

  if (p != columnDatas_.end()) {
    const ColumnData &columnData = (*p).second;

    if (columnData.imin) {
      minVal = *columnData.imin;
      maxVal = *columnData.imax;

      return true;
    }
  }

  //---

  CQChartsExprModel *th = const_cast<CQChartsExprModel *>(this);

  return th->calcColumnRange(column, minVal, maxVal);
}

bool
CQChartsExprModel::
calcColumnRange(int column, int &minVal, int &maxVal)
{
  std::unique_lock<std::mutex> lock(mutex_);

  ColumnData &columnData = columnDatas_[column];

  nr_ = rowCount();

  for (int r = 0; r < nr_; ++r) {
    QModelIndex ind = index(r, column, QModelIndex());

    QVariant var = data(ind, Qt::DisplayRole);

    bool ok;

    int value = CQChartsVariant::toInt(var, ok);
    if (! ok) continue;

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
  if (! parent.isValid())
    return true;

  return sourceModel()->hasChildren(mapToSource(parent));
}

QVariant
CQChartsExprModel::
data(const QModelIndex &index, int role) const
{
  int nc = sourceModel()->columnCount(mapToSource(index));

  if (! index.isValid())
    return QVariant();

  //---

  if (index.column() >= 0 && index.column() < nc) {
    QModelIndex sind = mapToSource(index);

    QVariant var = sourceModel()->data(sind, role);

    if (role == Qt::ToolTipRole && ! var.isValid())
      var = sourceModel()->data(sind, Qt::DisplayRole);

    return var;
  }

  //---

  int column = index.column() - nc;

  if      (role == Qt::DisplayRole || role == Qt::EditRole) {
    currentRow_ = index.row();
    currentCol_ = index.column();

    return getExtraColumnValue(currentRow_, currentCol_, column);
  }
  else if (role == Qt::TextAlignmentRole) {
    const ExtraColumn &extraColumn = this->extraColumn(column);

    if (extraColumn.type == CQBaseModelType::INTEGER ||
        extraColumn.type == CQBaseModelType::REAL)
      return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    else
      return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
  }
  else if (role == Qt::ToolTipRole) {
    currentRow_ = index.row();
    currentCol_ = index.column();

    return getExtraColumnValue(currentRow_, currentCol_, column);
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

    column = CQChartsUtil::toInt(columnStr, ok);

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

    column = CQChartsUtil::toInt(columnStr, ok);

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
  const ExtraColumn &extraColumn = this->extraColumn(ecolumn);

  //---

  // if evaluating the expression for this row use cached values for referenced values
  if (extraColumn.evaluating.load()) {
    if (! extraColumn.values.empty())
      return extraColumn.values[row];

    return QVariant();
  }

  //---

  auto p = extraColumn.variantMap.find(row);

  if (p != extraColumn.variantMap.end())
    return (*p).second;

  //---

  CQChartsExprModel *th = const_cast<CQChartsExprModel *>(this);

  return th->calcExtraColumnValue(row, column, ecolumn);
}

QVariant
CQChartsExprModel::
calcExtraColumnValue(int row, int column, int ecolumn)
{
  std::unique_lock<std::mutex> lock(mutex_);

  ExtraColumn &extraColumn = this->extraColumn(ecolumn);

  extraColumn.evaluating = true;

  QString expr = extraColumn.expr;

  expr = replaceExprColumns(expr, row, column).simplified();

  QVariant var;

  if (evaluateExpression(expr, var)) {
    std::string str;

    if      (var.type() == QVariant::Double) {
      double real = var.value<double>();

      bool isInt = CQBaseModel::isInteger(real);

      if      (extraColumn.type == CQBaseModelType::NONE) {
        if (CQBaseModel::isInteger(real))
          extraColumn.type = CQBaseModelType::INTEGER;
        else
          extraColumn.type = CQBaseModelType::REAL;
      }
      else if (extraColumn.type == CQBaseModelType::INTEGER) {
        if (! isInt)
          extraColumn.type = CQBaseModelType::REAL;
      }
      else if (extraColumn.type == CQBaseModelType::REAL) {
      }
    }
    else if (var.type() == QVariant::Int) {
      if (extraColumn.type == CQBaseModelType::NONE)
        extraColumn.type = CQBaseModelType::INTEGER;
    }
    else if (var.type() == QVariant::Bool) {
      if (extraColumn.type == CQBaseModelType::NONE)
        extraColumn.type = CQBaseModelType::INTEGER;
    }
    else {
      if (extraColumn.type == CQBaseModelType::NONE)
        extraColumn.type = CQBaseModelType::STRING;
    }
  }

  auto p = extraColumn.variantMap.insert(extraColumn.variantMap.end(),
                                         VariantMap::value_type(row, var));

  if (extraColumn.function == Function::ADD) {
    if (! extraColumn.values.empty())
      extraColumn.values[row] = var;
  }

  if (debug_)
    std::cerr << "Set Row " << row << " Column " << column << " = " <<
                 var.toString().toStdString() << std::endl;

  extraColumn.evaluating = false;

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

    CQBaseModelType type = extraColumn.type;

    str += ":" + CQBaseModel::typeName(type);

    if (extraColumn.typeValues.length())
      str += ":" + extraColumn.typeValues;

    str += QString("\n(%1)").arg(extraColumn.expr);

    return str;
  }
  else if (role == static_cast<int>(CQBaseModelRole::Type)) {
    return CQBaseModel::typeToVariant(extraColumn.type);
  }
  else if (role == static_cast<int>(CQBaseModelRole::BaseType)) {
    return CQBaseModel::typeToVariant(extraColumn.baseType);
  }
  else if (role == static_cast<int>(CQBaseModelRole::TypeValues)) {
    return QVariant(extraColumn.typeValues);
  }
  else if (role == static_cast<int>(CQBaseModelRole::Min)) {
    auto p = extraColumn.nameValues.find("min");
    if (p == extraColumn.nameValues.end()) return QVariant();

    return (*p).second;
  }
  else if (role == static_cast<int>(CQBaseModelRole::Max)) {
    auto p = extraColumn.nameValues.find("max");
    if (p == extraColumn.nameValues.end()) return QVariant();

    return (*p).second;
  }
  else if (role == static_cast<int>(CQBaseModelRole::Sorted)) {
    auto p = extraColumn.nameValues.find("sorted");
    if (p == extraColumn.nameValues.end()) return QVariant();

    return (*p).second;
  }
  else if (role == static_cast<int>(CQBaseModelRole::SortOrder)) {
    auto p = extraColumn.nameValues.find("sort_order");
    if (p == extraColumn.nameValues.end()) return QVariant();

    return (*p).second;
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
  else if (role == static_cast<int>(CQBaseModelRole::Type)) {
    extraColumn.type = CQBaseModel::variantToType(value);

    return true;
  }
  else if (role == static_cast<int>(CQBaseModelRole::BaseType)) {
    extraColumn.baseType = CQBaseModel::variantToType(value);

    return true;
  }
  else if (role == static_cast<int>(CQBaseModelRole::TypeValues)) {
    extraColumn.typeValues = value.toString();

    return true;
  }
  else if (role == static_cast<int>(CQBaseModelRole::Min)) {
    extraColumn.nameValues["min"] = value;

    return true;
  }
  else if (role == static_cast<int>(CQBaseModelRole::Max)) {
    extraColumn.nameValues["max"] = value;

    return true;
  }
  else if (role == static_cast<int>(CQBaseModelRole::Sorted)) {
    extraColumn.nameValues["sorted"] = value;

    return true;
  }
  else if (role == static_cast<int>(CQBaseModelRole::SortOrder)) {
    extraColumn.nameValues["sort_order"] = value;

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
  // get/set model data
  if      (name == "column"   ) return columnCmd   (values);
  else if (name == "row"      ) return rowCmd      (values);
  else if (name == "cell"     ) return cellCmd     (values);
  else if (name == "setColumn") return setColumnCmd(values);
  else if (name == "setRow"   ) return setRowCmd   (values);
  else if (name == "setCell"  ) return setCellCmd  (values);

  // get/set header data
  else if (name == "header"   ) return headerCmd   (values);
  else if (name == "setHeader") return setHeaderCmd(values);

  // get/set column type
  else if (name == "type"   ) return typeCmd   (values);
  else if (name == "setType") return setTypeCmd(values);

  // map values
  else if (name == "map"   ) return mapCmd   (values);
  else if (name == "remap" ) return remapCmd (values);
  else if (name == "bucket") return bucketCmd(values);
  else if (name == "norm"  ) return normCmd  (values);
  else if (name == "scale" ) return scaleCmd (values);

  // random
  else if (name == "rand" ) return randCmd (values);
  else if (name == "rnorm") return rnormCmd(values);

  // string
  else if (name == "match") return matchCmd(values);

  // color
  else if (name == "color") return colorCmd(values);

  // time
  else if (name == "timeval") return timevalCmd(values);

//else if (name == "key"   ) return keyCmd   (values);
//else if (name == "concat") return concatCmd(values);

  else return QVariant(false);
}

//------

// get column value for current row:
//   column(), column(col), column(col,defVal) : get column value
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

// get row value for current column:
//   row(), row(row) : get row value
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

// get cell (row/column) value:
//   cell(), cell(row,column), cell(row,column,defVal) : get cell value
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

// set column value (for current row):
//   setColumn(value), setColumn(col,value) : set column value
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

// set row value (for current column):
//   setRow(value), setRow(row,value), setRow(row,col,value) : set row value
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

// set cell value (for row/column):
//   setCell(value), setCell(row,col,value) : set row value
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

// get header value (for column):
//   header(), header(col)
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

// set header value (for column):
//   setHeader(s), setHeader(col,s)
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

// get type (for column):
//   type(), type(col)
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

  int role = static_cast<int>(CQBaseModelRole::Type);

  QVariant var = headerData(col, Qt::Horizontal, role);

  bool ok;

  CQBaseModelType type = (CQBaseModelType) CQChartsVariant::toInt(var, ok);
  if (! ok) return "";

  QString typeName = CQBaseModel::typeName(type);

  return QVariant(typeName);
}

//---

// set type (for column):
//   setType(s), setType(col,s)
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

  int role = static_cast<int>(CQBaseModelRole::Type);

  CQBaseModelType type = CQBaseModel::nameType(var.toString());

  QVariant typeVar(static_cast<int>(type));

  bool b = setHeaderData(col, Qt::Horizontal, typeVar, role);

  return QVariant(b);
}

//---

// map row value to min/max
//   map(), map(max), map(min,max)
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

// bucket value (for row/column):
//   bucket(col,delta), bucket(col,start,delta)
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
    return QVariant(0);

  //---

  if (! checkIndex(row, col))
    return QVariant();

  //---

  QVariant var = getCmdData(row, col);

  //---

  int bucket = -1;

  auto p = columnDatas_.find(col);

  if (p == columnDatas_.end())
    return QVariant(0);

  const ColumnData &columnData = (*p).second;

  const CQBucketer &bucketer = columnData.bucketer;

  if      (var.type() == QVariant::Double) {
    bool ok;

    double value = CQChartsVariant::toReal(var, ok);

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

    CQBucketer bucketer1 = bucketer;

    bucketer1.setType(CQBucketer::Type::REAL_RANGE);

    bucketer1.setRStart(start);
    bucketer1.setRDelta(delta);

    bucket = bucketer1.realBucket(value);
  }
  else if (var.type() == QVariant::Int) {
    return var;
  }
  else {
    QString str = var.toString();

    bucket = bucketer.stringBucket(str);
  }

  return QVariant(bucket);
}

//---

// normalized column value
//   norm(col), norm(col,scale)
QVariant
CQChartsExprModel::
normCmd(const Values &values) const
{
  // get column and optional scale
  CQChartsExprCmdValues cmdValues(values);

  if (cmdValues.numValues() == 0)
    return QVariant(0.0);

  int col = currentCol();

  (void) cmdValues.getInt(col);

  if (col < 0 || col >= columnCount())
    return QVariant(0.0);

  double scale = 1.0;

  (void) cmdValues.getReal(scale);

  //---

  // check index valid and get value
  int row = currentRow();

  if (! checkIndex(row, col))
    return QVariant();

  QVariant var = getCmdData(row, col);

  if (! var.isValid())
    return QVariant();

  //---

  if      (var.type() == QVariant::Double) {
    bool ok;

    double value = CQChartsVariant::toReal(var, ok);

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
    bool ok;

    int value = CQChartsVariant::toInt(var, ok);

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

// scaled column value
//   scale(col,center=true,scale=true)
QVariant
CQChartsExprModel::
scaleCmd(const Values &values) const
{
  // get column and optional center, scale
  CQChartsExprCmdValues cmdValues(values);

  if (cmdValues.numValues() == 0)
    return QVariant(0.0);

  int col = currentCol();

  (void) cmdValues.getInt(col);

  if (col < 0 || col >= columnCount())
    return QVariant(0.0);

  bool center = true;
  bool scale  = true;

  (void) cmdValues.getBool(center);
  (void) cmdValues.getBool(scale );

  //---

  // check index valid and get value
  int row = currentRow();

  if (! checkIndex(row, col))
    return QVariant(0.0);

  QVariant var = getCmdData(row, col);

  if (! var.isValid())
    return QVariant(0.0);

  bool ok;

  double value = CQChartsVariant::toReal(var, ok);

  //---

  QModelIndex ind = this->index(row, col, QModelIndex());

  CQChartsColumn column(ind.column());

  CQChartsModelData *modelData = getModelData();
  if (! modelData) return QVariant(0.0);

  CQChartsModelDetails *details = modelData->details();
  assert(details);

  CQChartsModelColumnDetails *columnDetails = details->columnDetails(column);
  assert(columnDetails);

  QVariant meanVar   = columnDetails->meanValue  (/*useNaN*/false);
  QVariant stddevVar = columnDetails->stdDevValue(/*useNaN*/false);

  if (! meanVar.isValid() || ! stddevVar.isValid())
    return QVariant(0.0);

  //---

  double mean   = CQChartsVariant::toReal(meanVar  , ok);
  double stddev = CQChartsVariant::toReal(stddevVar, ok);

  double value1 = (stddev > 0.0 ? (value - mean)/stddev : 0.0);

  return QVariant(value1);
}

//---

// random value:
//   rand(min=0,max=1)
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

// random normalized value:
//   rnorm(mean=0,stddev=1)
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

#if 0
// key from values:
//   key(str1,str2,...)
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
#endif

//---

#if 0
// concat values:
//   concat(str1,str2,...)
QVariant
CQChartsExprModel::
concatCmd(const Values &values) const
{
  QString str;

  for (std::size_t i = 0; i < values.size(); ++i)
    str += values[i].toString();

  return QVariant(str);
}
#endif

//---

// match string to regexp:
//   match(name, regexp)
QVariant
CQChartsExprModel::
matchCmd(const Values &values) const
{
  if (values.size() == 2) {
    QString str     = values[0].toString();
    QString pattern = values[1].toString();

    QRegExp regexp(pattern);

    return regexp.exactMatch(str);
  }

  return QVariant();
}

//---

// string to color:
//   color(name)
QVariant
CQChartsExprModel::
colorCmd(const Values &values) const
{
  if (values.size() == 1)
    return QVariant(QColor(values[0].toString()));

  return QVariant();
}

//---

// remap column value range to min/max:
//   remap()            - current column, 0.0, 1.0
//   remap(col)         - specified column, 0.0, 1.0
//   remap(col,max)     - specified column, 0.0, max
//   remap(col,min,max) - specified column, min, max
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

  double r = CQChartsModelUtil::modelReal(this, ind, ok);
  if (! ok) return QVariant(0.0);

  double rm = CMathUtil::map(r, rmin, rmax, r1, r2);

  return QVariant(rm);
}

//---

// time value from column value:
//   timeval(fmt)     - timeval fmt for current column
//   timeval(col,fmt) - timeval fmt for specified column
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

  QVariant var = CQChartsModelUtil::modelValue(this, ind, Qt::EditRole, ok);
  if (! ok) return QVariant();

  CQChartsColumn column(col);

  bool converted;

  QVariant var1 = CQChartsModelUtil::columnUserData(charts_, this, column, var, converted);

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

  qtcl_->setRow   (currentRow());
  qtcl_->setColumn(currentCol());

  int rc = qtcl_->evalExpr(expr);

  if (rc != TCL_OK) {
    if (qtcl_->isDomainError(rc)) {
      double x = CMathUtil::getNaN();

      var = QVariant(x);

      return true;
    }

    if (debug_)
      std::cerr << qtcl_->errorInfo(rc).toStdString() << std::endl;

    return false;
  }

  return getTclResult(var);
}

void
CQChartsExprModel::
setVar(const QString &name, int row, int column)
{
  auto p = nameColumns_.find(name);

  if (p != nameColumns_.end()) {
    int col = (*p).second;

    // get model value
    QVariant var = getCmdData(row, col);

    // store value in column variable
    qtcl_->createVar(name, var);
  }
  else if (name == "row") {
    qtcl_->createVar(name, row);
  }
  else if (name == "column" || name == "col") {
    qtcl_->createVar(name, column);
  }
  else if (name == "pi") {
    qtcl_->createVar(name, QVariant(M_PI));
  }
  else if (name == "NaN") {
    qtcl_->createVar(name, QVariant(CMathUtil::getNaN()));
  }
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
  qtcl_->setResult(rc);
  return true;
}

bool
CQChartsExprModel::
getTclResult(QVariant &var) const
{
  var = qtcl_->getResult();
  return true;
}

QString
CQChartsExprModel::
replaceExprColumns(const QString &expr, int row, int column) const
{
  QModelIndex ind = this->index(row, column, QModelIndex());

  return CQChartsModelUtil::replaceModelExprVars(expr, this, ind, nr_, nc_);
}

//---

bool
CQChartsExprModel::
getColumnRange(const QModelIndex &ind, double &rmin, double &rmax)
{
  CQChartsColumn column(ind.column());

  CQChartsModelData *modelData = getModelData();

  //---

  CQBaseModelType    type;
  CQBaseModelType    baseType;
  CQChartsNameValues nameValues;

  if (modelData) {
    CQChartsModelDetails *details = modelData->details();
    assert(details);

    CQChartsModelColumnDetails *columnDetails = details->columnDetails(column);
    assert(columnDetails);

    type       = columnDetails->type();
    baseType   = columnDetails->baseType();
    nameValues = columnDetails->nameValues();
  }
  else {
    if (! CQChartsModelUtil::columnValueType(charts_, this, column, type, baseType, nameValues))
      return false;
  }

  //---

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  const CQChartsColumnType *typeData = columnTypeMgr->getType(type);

  const CQChartsColumnRealType *rtypeData = dynamic_cast<const CQChartsColumnRealType *>(typeData);

  if (! rtypeData)
    return false;

  //---

  if (! rtypeData->rmin(nameValues, rmin)) {
    if (modelData) {
      bool ok;

      rmin = CQChartsVariant::toReal(
        modelData->details()->columnDetails(ind.column())->minValue(), ok);
    }
  }

  if (! rtypeData->rmax(nameValues, rmax)) {
    if (modelData) {
      bool ok;

      rmax = CQChartsVariant::toReal(
        modelData->details()->columnDetails(ind.column())->maxValue(), ok);
    }
  }

  return true;
}

//---

CQChartsModelData *
CQChartsExprModel::
getModelData() const
{
  return charts_->getModelData(this->filter_);
}

//---

void
CQChartsExprModel::
dataChangedSlot(const QModelIndex &from, const QModelIndex &to)
{
  emit dataChanged(mapFromSource(from), mapFromSource(to));
}
