#ifndef CQChartsModelFn_H
#define CQChartsModelFn_H

#include <CQExprModelFn.h>
#include <CQChartsUtil.h>
#include <CQChartsColumnType.h>

// remap(column), remap(column,max), remap(column,min,max) remap column value
class CQChartsModelRemapFn : public CQExprModelFn {
 public:
  CQChartsModelRemapFn(CQCharts *charts, QAbstractItemModel *model, CQExprModel *exprModel) :
   CQExprModelFn(exprModel, "remap"), charts_(charts), model_(model), exprModel_(exprModel) {
  }

  QVariant exec(const Vars &vars) {
    if (vars.size() == 0)
      return QVariant(0.0);

    bool ok;

    int col = vars[0].toInt(&ok);

    if (! ok)
      return QVariant(0.0);

    //---

    int row = exprModel_->currentRow();

    if (! checkIndex(row, col))
      return QVariant(0.0);

    QModelIndex ind = exprModel_->index(row, col, QModelIndex());

    //---

    double rmin = 0.0, rmax = 1.0;

    getColumnRange(ind, rmin, rmax);

    //---

    double r1 = 0.0, r2 = 1.0;

    if      (vars.size() > 2) {
      bool ok1, ok2;

      double r1t = vars[1].toDouble(&ok1);
      double r2t = vars[2].toDouble(&ok2);

      if (ok1) r1 = r1t;
      if (ok2) r2 = r2t;
    }
    else if (vars.size() > 1) {
      bool ok2;

      double r2t = vars[2].toDouble(&ok2);

      if (ok2) r2 = r2t;
    }

    //---

    bool ok3;

    double r = CQChartsUtil::modelReal(exprModel_, ind, ok3);

    if (! ok3)
      return QVariant(0.0);

    double rm = CQChartsUtil::map(r, rmin, rmax, r1, r2);

    return QVariant(rm);
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

 private:
  CQCharts*           charts_    { nullptr };
  QAbstractItemModel* model_     { nullptr };
  CQExprModel*        exprModel_ { nullptr };
};

#endif
