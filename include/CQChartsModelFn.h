#ifndef CQChartsModelFn_H
#define CQChartsModelFn_H

#include <CQExprModelFn.h>
#include <CQChartsUtil.h>
#include <CQChartsColumnType.h>

// remap(color), remap(column,max), remap(column,min,max) remap column value
class CQChartsModelRemapFn : public CQExprModelFn {
 public:
  CQChartsModelRemapFn(CQCharts *charts, QAbstractItemModel *model, CQExprModel *exprModel) :
   CQExprModelFn(exprModel), charts_(charts), model_(model), exprModel_(exprModel) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long row = exprModel_->currentRow(), col = exprModel_->currentCol();

    if (values.size() == 0)
      return mapValue(expr, 0.0);

    if (! values[0]->getIntegerValue(col))
      return mapValue(expr, 0.0);

    //---

    if (! checkIndex(row, col))
      return mapValue(expr, 0.0);

    QModelIndex ind = exprModel_->index(row, col, QModelIndex());

    //---

    double rmin = 0.0, rmax = 1.0;

    getColumnRange(ind, rmin, rmax);

    //---

    double r1 = 0.0, r2 = 1.0;

    if      (values.size() > 2) {
      double r;

      if (values[1]->getRealValue(r))
        r1 = r;

      if (values[2]->getRealValue(r))
        r2 = r;
    }
    else if (values.size() > 1) {
      double r;

      if (values[1]->getRealValue(r))
        r2 = r;
    }

    //---

    bool ok;

    double r = CQChartsUtil::modelReal(exprModel_, ind, ok);

    if (! ok)
      return mapValue(expr, 0.0);

    double rm = CQChartsUtil::map(r, rmin, rmax, r1, r2);

    return mapValue(expr, rm);
  }

  bool getColumnRange(const QModelIndex &ind, double &rmin, double &rmax) {
    CQChartsColumn column(ind.column());

    CQBaseModel::Type  type;
    CQChartsNameValues nameValues;

    if (! CQChartsUtil::columnValueType(charts_, exprModel_, column, type, nameValues))
      return false;

    CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

    CQChartsColumnType *typeData = columnTypeMgr->getType(type);

    CQChartsColumnRealType *rtypeData = dynamic_cast<CQChartsColumnRealType *>(typeData);

    if (! rtypeData)
      return false;

    CQChartsModelData *modelData = charts_->getModelData(model_);

    if (! modelData) {
      //int modelInd = charts_->addModel(model_);

      //modelData = charts_->getModelData(modelInd);
    }

    if (! rtypeData->rmin(nameValues, rmin)) {
      if (modelData)
        rmin = modelData->details().columnDetails(ind.column()).minValue().toReal();
    }

    if (! rtypeData->rmax(nameValues, rmax)) {
      if (modelData)
        rmax = modelData->details().columnDetails(ind.column()).maxValue().toReal();
    }

    return true;
  }

  CExprValuePtr mapValue(CExpr *expr, double r) {
    return expr->createRealValue(r);
  }

 private:
  CQCharts*           charts_    { nullptr };
  QAbstractItemModel* model_     { nullptr };
  CQExprModel*        exprModel_ { nullptr };
};

#endif
