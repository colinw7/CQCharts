#include <CQChartsObj.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>

CQChartsObj::
CQChartsObj(CQCharts *charts, const CQChartsGeom::BBox &rect) :
 QObject(nullptr), charts_(charts), rect_(rect)
{
  assert(charts_);
}

const QString &
CQChartsObj::
id() const
{
  if (! id_) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (! id_) {
      const_cast<CQChartsObj*>(this)->id_ = calcId();

      assert((*id_).length());
    }
  }

  return *id_;
}

void
CQChartsObj::
setId(const QString &s)
{
  id_ = s;

  emit idChanged();
}

const QString &
CQChartsObj::
tipId() const
{
  if (! tipId_) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (! tipId_) {
      const_cast<CQChartsObj*>(this)->tipId_ = calcTipId();

      assert((*tipId_).length());
    }
  }

  return *tipId_;
}
