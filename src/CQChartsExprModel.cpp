#include <CQChartsExprModel.h>
#include <CQExprModelCmdValues.h>
#include <CQChartsColumnType.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>

CQChartsExprModel::
CQChartsExprModel(CQCharts *charts, QAbstractItemModel *model) :
 CQExprModel(model), charts_(charts)
{
  addFunction("remap");
  addFunction("timeval");
}

QVariant
CQChartsExprModel::
processCmd(const QString &name, const Values &values)
{
  if      (name == "remap") {
    return remapCmd(values);
  }
  else if (name == "timeval") {
    return timevalCmd(values);
  }

  return CQExprModel::processCmd(name, values);
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
  CQExprModelCmdValues cmdValues(values);

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

  double rm = CQChartsUtil::map(r, rmin, rmax, r1, r2);

  return QVariant(rm);
}

//---

// timeval(fmt)     - timeval fmt for current column
// timeval(col,fmt) - timeval fmt for specified column
QVariant
CQChartsExprModel::
timevalCmd(const Values &values)
{
  CQExprModelCmdValues cmdValues(values);

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

  double t = CQChartsUtil::toReal(var1, ok);
  if (! ok) return QVariant();

  return CQChartsUtil::timeToString(fmt, t);
}

//---

bool
CQChartsExprModel::
getColumnRange(const QModelIndex &ind, double &rmin, double &rmax)
{
  CQChartsColumn column(ind.column());

  CQBaseModel::Type  type;
  CQChartsNameValues nameValues;

  if (! CQChartsUtil::columnValueType(charts_, this, column, type, nameValues))
    return false;

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  CQChartsColumnType *typeData = columnTypeMgr->getType(type);

  CQChartsColumnRealType *rtypeData = dynamic_cast<CQChartsColumnRealType *>(typeData);

  if (! rtypeData)
    return false;

  CQChartsModelData *modelData = charts_->getModelData(this->model_);

  if (! modelData) {
    //int modelInd = charts_->addModel(this->model_);

    //modelData = charts_->getModelData(modelInd);
  }

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
