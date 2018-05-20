#ifndef CQChartsModelFn_H
#define CQChartsModelFn_H

#include <CQExprModelFn.h>
#include <CQChartsUtil.h>
#include <CQChartsColumnType.h>

class CQChartsModelRemapImpl {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQChartsModelRemapImpl(CQCharts *charts, QAbstractItemModel *model, CQExprModel *exprModel) :
   charts_(charts), model_(model), exprModel_(exprModel) {
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

  //---

  QVariant exec(const Vars &vars) {
    if (vars.size() == 0)
      return QVariant(0.0);

    bool ok;

    int col = vars[0].toInt(&ok);

    if (! ok)
      return QVariant(0.0);

    //---

    int row = exprModel_->currentRow();

    if (! exprModel_->checkIndex(row, col))
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

 private:
  CQCharts*           charts_    { nullptr };
  QAbstractItemModel* model_     { nullptr };
  CQExprModel*        exprModel_ { nullptr };
};

//---

#ifdef CQExprModel_USE_CEXPR
// remap(column), remap(column,max), remap(column,min,max) remap column value
class CQChartsModelRemapExprFn : public CQExprModelExprFn {
 public:
  CQChartsModelRemapExprFn(CQCharts *charts, QAbstractItemModel *model, CQExprModel *exprModel) :
   CQExprModelExprFn(exprModel, "remap") {
    impl_ = new CQChartsModelRemapImpl(charts, model, exprModel);
  }

 ~CQChartsModelRemapExprFn() {
    delete impl_;
  }

  QVariant exec(const Vars &vars) {
    return impl_->exec(vars);
  }

 private:
  CQChartsModelRemapImpl *impl_ { nullptr };
};
#endif

//---

#ifdef CQExprModel_USE_TCL
// remap(column), remap(column,max), remap(column,min,max) remap column value
class CQChartsModelRemapTclFn : public CQExprModelTclFn {
 public:
  CQChartsModelRemapTclFn(CQCharts *charts, QAbstractItemModel *model, CQExprModel *exprModel) :
   CQExprModelTclFn(exprModel, "remap") {
    impl_ = new CQChartsModelRemapImpl(charts, model, exprModel);
  }

 ~CQChartsModelRemapTclFn() {
    delete impl_;
  }

  QVariant exec(const Vars &vars) {
    return impl_->exec(vars);
  }

 private:
  CQChartsModelRemapImpl *impl_ { nullptr };
};
#endif

#endif
