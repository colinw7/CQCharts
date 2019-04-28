#include <CQChartsPlotObj.h>
#include <CQChartsPlot.h>
#include <CQPropertyViewModel.h>

CQChartsPlotObj::
CQChartsPlotObj(CQChartsPlot *plot, const CQChartsGeom::BBox &rect, const ColorInd &is,
                const ColorInd &ig, const ColorInd &iv) :
 CQChartsObj(nullptr, rect), plot_(plot), is_(is), ig_(ig), iv_(iv)
{
  assert(! CMathUtil::isNaN(rect.getXMin()) && ! CMathUtil::isNaN(rect.getYMin()) &&
         ! CMathUtil::isNaN(rect.getXMax()) && ! CMathUtil::isNaN(rect.getYMax()));

  assert(is_.i >= 0 && is_.i < is_.n);
  assert(ig_.i >= 0 && ig_.i < ig_.n);
  assert(iv_.i >= 0 && iv_.i < iv_.n);
}

//---

bool
CQChartsPlotObj::
calcColumnId(const QModelIndex &ind, QString &str) const
{
  bool ok;

  str = plot_->idColumnString(ind.row(), ind.parent(), ok);

  return ok;
}

//---

double
CQChartsPlotObj::
xColorValue() const
{
  const CQChartsGeom::Range &dataRange = plot_->dataRange();

  return CMathUtil::map(rect_.getXMid(), dataRange.xmin(), dataRange.xmax(), 0.0, 1.0);
}

double
CQChartsPlotObj::
yColorValue() const
{
  const CQChartsGeom::Range &dataRange = plot_->dataRange();

  return CMathUtil::map(rect_.getYMid(), dataRange.ymin(), dataRange.ymax(), 0.0, 1.0);
}

//---

CQChartsPlotObj::ColorInd
CQChartsPlotObj::
calcColorInd() const
{
  ColorInd colorInd;

  if      (plot_->colorType() == CQChartsPlot::ColorType::AUTO)
    colorInd = (is_.n <= 1 ? (ig_.n <= 1 ? iv_ : ig_) : is_);
  else if (plot_->colorType() == CQChartsPlot::ColorType::SET)
    colorInd = is_;
  else if (plot_->colorType() == CQChartsPlot::ColorType::GROUP)
    colorInd = ig_;
  else if (plot_->colorType() == CQChartsPlot::ColorType::INDEX)
    colorInd = iv_;
  else if (plot_->colorType() == CQChartsPlot::ColorType::X_VALUE)
    colorInd = ColorInd(xColorValue());
  else if (plot_->colorType() == CQChartsPlot::ColorType::Y_VALUE)
    colorInd = ColorInd(yColorValue());

  return colorInd;
}

//---

QString
CQChartsPlotObj::
propertyId() const
{
  return QString("object:%1").arg(id());
}

void
CQChartsPlotObj::
addProperties(CQPropertyViewModel *, const QString &)
{
}

//---

bool
CQChartsPlotObj::
isSelectIndex(const QModelIndex &ind) const
{
  Indices inds;

  getHierSelectIndices(inds);

  for (auto &ind1 : inds) {
    QModelIndex ind2 = plot_->normalizeIndex(ind1);

    if (ind == ind2)
      return true;
  }

  return false;
}

void
CQChartsPlotObj::
addSelectIndices()
{
  Indices inds;

  getHierSelectIndices(inds);

  for (const auto &ind : inds)
    plot_->addSelectIndex(ind);
}

void
CQChartsPlotObj::
getHierSelectIndices(Indices &inds) const
{
  getSelectIndices(inds);

  if (plot_->idColumn().isValid())
    addColumnSelectIndex(inds, plot_->idColumn());

  if (plot_->visibleColumn().isValid())
    addColumnSelectIndex(inds, plot_->visibleColumn());

  if (plot_->colorColumn().isValid())
    addColumnSelectIndex(inds, plot_->colorColumn());
}

void
CQChartsPlotObj::
addSelectIndex(Indices &inds, const CQChartsModelIndex &ind) const
{
  addSelectIndex(inds, ind.row, ind.column, ind.parent);
}

void
CQChartsPlotObj::
addSelectIndex(Indices &inds, int row, const CQChartsColumn &column,
               const QModelIndex &parent) const
{
  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX)
    return;

  QModelIndex ind = plot_->selectIndex(row, column.column(), parent);

  addSelectIndex(inds, ind);
}

void
CQChartsPlotObj::
addSelectIndex(Indices &inds, const QModelIndex &ind) const
{
  inds.insert(ind);
}

//------

CQChartsGroupObj::
CQChartsGroupObj(CQChartsPlot *plot, const CQChartsGeom::BBox &bbox) :
 CQChartsPlotObj(plot, bbox)
{
}
