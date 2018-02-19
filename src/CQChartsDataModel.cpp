#include <CQChartsDataModel.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <CQDataModel.h>
#include <cassert>

CQChartsDataModel::
CQChartsDataModel(CQCharts *charts, int nc, int nr, bool fill) :
 CQChartsModelFilter(charts)
{
  QModelIndex parent; // OK ?

  dataModel_ = new CQDataModel(nc, nr);

  if (fill) {
    for (int r = 0; r < nr; ++r) {
      for (int c = 0; c < nc; ++c) {
        QModelIndex ind = dataModel_->index(r, c, parent);

        dataModel_->setData(ind, QVariant(r*nc + c));
      }
    }
  }

  setSourceModel(dataModel_);
}

CQChartsDataModel::
~CQChartsDataModel()
{
  delete dataModel_;
}
