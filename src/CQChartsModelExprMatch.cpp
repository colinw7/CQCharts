#include <CQChartsModelExprMatch.h>
#include <CQChartsExprCmdValues.h>
#include <CQChartsExprTcl.h>
#include <CQChartsModelUtil.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsVariant.h>

#include <QSortFilterProxyModel>
#include <QAbstractItemModel>
#include <QVariant>

//------

class CQChartsModelExprMatchFn {
 public:
  using Values = std::vector<QVariant>;

 public:
  CQChartsModelExprMatchFn(CQChartsModelExprMatch *model, const QString &name) :
   model_(model), name_(name) {
    qtcl_ = model->qtcl();

    cmdId_ = qtcl()->createExprCommand(name_,
      reinterpret_cast<CQChartsExprTcl::ObjCmdProc>(&CQChartsModelExprMatchFn::commandProc),
      static_cast<CQChartsExprTcl::ObjCmdData>(this));
  }

  virtual ~CQChartsModelExprMatchFn() { }

  CQChartsExprTcl *qtcl() const { return qtcl_; }

  static int commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv) {
    auto *command = static_cast<CQChartsModelExprMatchFn *>(clientData);

    Values values;

    for (int i = 1; i < objc; ++i) {
      const auto *obj = objv[i];

      values.push_back(command->qtcl()->variantFromObj(obj));
    }

    auto var = command->exec(values);

    command->qtcl()->setResult(var);

    return TCL_OK;
  }

  QVariant exec(const Values &values) { return model_->processCmd(name_, values); }

  bool checkColumn(int col) const { return model_->checkColumn(col); }

  bool checkIndex(int row, int col) const { return model_->checkIndex(row, col); }

 protected:
  CQChartsModelExprMatch* model_ { nullptr };
  QString                 name_;
  CQChartsExprTcl*        qtcl_  { nullptr };
  Tcl_Command             cmdId_ { nullptr };
};

//------

CQChartsModelExprMatch::
CQChartsModelExprMatch(QAbstractItemModel *model) :
 model_(model)
{
  qtcl_ = new CQChartsExprTcl(model);

  addBuiltinFunctions();
}

CQChartsModelExprMatch::
~CQChartsModelExprMatch()
{
  for (auto &tclCmd : tclCmds_)
    delete tclCmd;

  delete qtcl_;
}

void
CQChartsModelExprMatch::
addBuiltinFunctions()
{
  // get model data
  addFunction("column");
  addFunction("row"   );
  addFunction("cell"  );

  // get header data
  addFunction("header");

  // string
  addFunction("match");

  // special values
  addFunction("defined");
  addFunction("isnan"  );
}

void
CQChartsModelExprMatch::
setModelData(CQChartsModelData *modelData)
{
  modelData_ = modelData;

  auto *proxyModel = qobject_cast<QSortFilterProxyModel *>(modelData_->model().data());

  if (proxyModel)
    model_ = proxyModel->sourceModel();
  else
    model_ = modelData_->model().data();

  qtcl_->setModel(model_);

  if (! detailsFns_) {
    addFunction("min"         );
    addFunction("max"         );
    addFunction("mean"        );
    addFunction("stddev"      );
    addFunction("median"      );
    addFunction("lower_median");
    addFunction("upper_median");

    detailsFns_ = true;
  }
}

void
CQChartsModelExprMatch::
setModel(QAbstractItemModel *model)
{
  model_ = model;

  qtcl_->setModel(model_);
}

void
CQChartsModelExprMatch::
addFunction(const QString &name)
{
  assert(name.length());

  auto *fn = new CQChartsModelExprMatchFn(this, name);

  tclCmds_.push_back(fn);
}

void
CQChartsModelExprMatch::
initMatch(const QString &expr)
{
  nr_ = (model_ ? model_->rowCount   () : 0);
  nc_ = (model_ ? model_->columnCount() : 0);

  qtcl_->resetLastValue();

  //---

  matchExpr_ = replaceExprColumns(expr, QModelIndex());
}

void
CQChartsModelExprMatch::
initColumns()
{
  assert(model_);

  columnNames_.clear();
  nameColumns_.clear();

  qtcl_->resetColumns();

  nr_ = (model_ ? model_->rowCount   () : 0);
  nc_ = (model_ ? model_->columnCount() : 0);

  for (int ic = 0; ic < nc_; ++ic) {
    CQChartsColumn c(ic);

    bool ok;

    auto name = CQChartsModelUtil::modelHHeaderString(model_, c, ok);

    if (! name.length())
      name = QString::number(ic + 1);

    columnNames_[ic  ] = name;
    nameColumns_[name] = ic;

    qtcl_->setNameColumn(qtcl_->encodeColumnName(name), ic);
  }

  qtcl_->initVars();
}

bool
CQChartsModelExprMatch::
match(const QString &expr, const QModelIndex &ind, bool &ok)
{
  ok = true;

  QVariant value;

  if (! evaluateExpression(expr, ind, value, /*replace*/ true)) {
    ok = false;
    return true;
  }

  bool rc = CQChartsVariant::toBool(value, ok);

  return rc;
}

bool
CQChartsModelExprMatch::
match(const QModelIndex &ind, bool &ok)
{
  ok = true;

  QVariant value;

  if (! evaluateExpression(matchExpr_, ind, value, /*replace*/ false)) {
    ok = false;
    return true;
  }

  bool rc = CQChartsVariant::toBool(value, ok);

  return rc;
}

//------

QVariant
CQChartsModelExprMatch::
processCmd(const QString &name, const Values &values)
{
  // get model data
  if      (name == "column") return columnCmd(values);
  else if (name == "row"   ) return rowCmd   (values);
  else if (name == "cell"  ) return cellCmd  (values);

  // get header data
  else if (name == "header") return headerCmd(values);

  // string
  else if (name == "match") return matchCmd(values);

  // math
  else if (name == "defined") return definedCmd(values);
  else if (name == "isnan"  ) return isnanCmd  (values);

  // details
  else if (name == "min"         ) return detailsCmd(name, values);
  else if (name == "max"         ) return detailsCmd(name, values);
  else if (name == "mean"        ) return detailsCmd(name, values);
  else if (name == "stddev"      ) return detailsCmd(name, values);
  else if (name == "median"      ) return detailsCmd(name, values);
  else if (name == "lower_median") return detailsCmd(name, values);
  else if (name == "upper_median") return detailsCmd(name, values);

  else return CQChartsVariant::fromBool(false);
}

//------

// column(), column(col), column(col, defVal) : get column value
QVariant
CQChartsModelExprMatch::
columnCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  int row = currentRow();
  int col = currentCol();

  if (! cmdValues.hasValues())
    return col;

  if (! getColumn(cmdValues, col))
    return QVariant();

  //---

  if (! checkIndex(row, col)) {
    QString defStr;

    if (! cmdValues.getStr(defStr))
      return QVariant();

    return CQChartsVariant::fromString(defStr);
  }

  //---

  return getCmdData(row, col);
}

//---

// row(), row(row) : get row value
QVariant
CQChartsModelExprMatch::
rowCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  int row = currentRow();
  int col = currentCol();

  if (! cmdValues.hasValues())
    return row;

  long row1;

  if (! cmdValues.getInt(row1))
    return QVariant();

  row = int(row1);

  //---

  if (! checkIndex(row, col)) {
    QString defStr;

    if (! cmdValues.getStr(defStr))
      return QVariant();

    return CQChartsVariant::fromString(defStr);
  }

  //---

  return getCmdData(row, col);
}

//---

// cell(), cell(row, column), cell(row, column, defVal) : get cell value
QVariant
CQChartsModelExprMatch::
cellCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  int row = currentRow();
  int col = currentCol();

  if (cmdValues.hasValues()) {
    if (cmdValues.numValues() < 2) {
      // TODO: row and column string value ?
      return QVariant();
    }

    long row1;

    if (! cmdValues.getInt(row1))
      return QVariant();

    row = int(row1);

    if (! getColumn(cmdValues, col))
      return QVariant();
  }

  //---

  if (! checkIndex(row, col)) {
    QString defStr;

    if (! cmdValues.getStr(defStr))
      return QVariant();

    return CQChartsVariant::fromString(defStr);
  }

  //---

  return getCmdData(row, col);
}

//---

// header(), header(col)
QVariant
CQChartsModelExprMatch::
headerCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  int col = currentCol();

  (void) getColumn(cmdValues, col);

  //---

  if (! checkColumn(col))
    return QVariant();

  //---

  bool ok;

  return CQChartsModelUtil::modelHHeaderString(model(), CQChartsColumn(col), ok);
}

//---

// match string to regexp:
//   match(name, regexp)
QVariant
CQChartsModelExprMatch::
matchCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  QString str;

  if (! cmdValues.getStr(str))
    return QVariant();

  QString pattern;

  if (! cmdValues.getStr(pattern))
    return QVariant();

  QRegExp regexp(pattern);

  return regexp.exactMatch(str);
}

//---

// column value is valid
//  defined(column)
QVariant
CQChartsModelExprMatch::
definedCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  int row = currentRow();
  int col = currentCol();

  if (! getColumn(cmdValues, col))
    return QVariant();

  //---

  if (! checkIndex(row, col))
    return CQChartsVariant::fromBool(false);

  //---

  auto var = getCmdData(row, col);

  return CQChartsVariant::fromBool(var.isValid());
}

//---

// real is nan
//  isnan(<value>)
QVariant
CQChartsModelExprMatch::
isnanCmd(const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  double r { 0.0 };

  (void) cmdValues.getReal(r);

  //---

  return CMathUtil::isNaN(r);
}

//---

QVariant
CQChartsModelExprMatch::
detailsCmd(const QString &name, const Values &values) const
{
  CQChartsExprCmdValues cmdValues(values);

  int col = currentCol();

  if (! getColumn(cmdValues, col))
    return QVariant();

  if (! modelData_)
    return QVariant();

  auto *details = modelData_->details();

  auto *columnDetails = details->columnDetails(CQChartsColumn(col));

  return columnDetails->getNamedValue(name);
}

//---

bool
CQChartsModelExprMatch::
getColumn(CQChartsExprCmdValues &cmdValues, int &col) const
{
  long col1;

  if (cmdValues.getInt(col1)) {
    col = int(col1);
    return true;
  }

  QString name;

  if (! cmdValues.getStr(name))
    return false;

  if (name == "")
    return false;

  auto p = nameColumns_.find(name);

  if (p == nameColumns_.end())
    return false;

  col = (*p).second;

  return true;
}

//------

bool
CQChartsModelExprMatch::
evaluateExpression(const QString &expr, const QModelIndex &ind, QVariant &value, bool replace)
{
  if (expr.length() == 0)
    return false;

  currentRow_ = ind.row   ();
  currentCol_ = ind.column();

  qtcl_->setRow   (currentRow_);
  qtcl_->setColumn(currentCol_);

  //---

  auto expr1 = (replace ? replaceExprColumns(expr, ind) : expr);

  return qtcl_->evaluateExpression(expr1, value, isDebug());
}

QVariant
CQChartsModelExprMatch::
getCmdData(int row, int col) const
{
  QModelIndex parent; // TODO

  auto ind = model()->index(row, col, parent);

  return getCmdData(ind);
}

QVariant
CQChartsModelExprMatch::
getCmdData(const QModelIndex &ind) const
{
  auto var = model()->data(ind, Qt::EditRole);

  if (! var.isValid())
    var = model()->data(ind, Qt::DisplayRole);

  return var;
}

QString
CQChartsModelExprMatch::
replaceExprColumns(const QString &expr, const QModelIndex &ind) const
{
  return CQChartsModelUtil::replaceModelExprVars(expr, model_, ind, nr_, nc_);
}

bool
CQChartsModelExprMatch::
checkColumn(int col) const
{
  if (col < 0 || col >= nc_) return false;

  return true;
}

bool
CQChartsModelExprMatch::
checkIndex(int row, int col) const
{
  if (row < 0 || row >= nr_) return false;
  if (col < 0 || col >= nc_) return false;

  return true;
}
