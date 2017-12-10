#include <CQChartsRadarPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsBoxObj.h>
#include <CQChartsRenderer.h>
#include <CGradientPalette.h>
#include <CQStrParse.h>

CQChartsRadarPlotType::
CQChartsRadarPlotType()
{
  addParameters();
}

void
CQChartsRadarPlotType::
addParameters()
{
  addColumnParameter ("name" , "Name" , "nameColumn"  , "optional");
  addColumnsParameter("value", "Value", "valueColumns", "", "1");
}

CQChartsPlot *
CQChartsRadarPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsRadarPlot(view, model);
}

//------

CQChartsRadarPlot::
CQChartsRadarPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("radar"), model)
{
  gridColor_ = CQChartsPaletteColor(CQChartsPaletteColor::Type::THEME_VALUE, 0.5);

  boxObj_ = new CQChartsBoxObj(this);

  boxObj_->setBackgroundColor(CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE));
  boxObj_->setBackgroundAlpha(0.5);

  textColor_ = CQChartsPaletteColor(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  setBorder(true);

  addKey();

  addTitle();
}

CQChartsRadarPlot::
~CQChartsRadarPlot()
{
  delete boxObj_;
}

QString
CQChartsRadarPlot::
valueColumnsStr() const
{
  return CQChartsUtil::toString(valueColumns());
}

bool
CQChartsRadarPlot::
setValueColumnsStr(const QString &s)
{
  std::vector<int> valueColumns;

  if (! CQChartsUtil::fromString(s, valueColumns))
    return false;

  setValueColumns(valueColumns);

  return true;
}

QString
CQChartsRadarPlot::
fillColorStr() const
{
  return boxObj_->backgroundColorStr();
}

void
CQChartsRadarPlot::
setFillColorStr(const QString &s)
{
  boxObj_->setBackgroundColorStr(s);

  update();
}

QColor
CQChartsRadarPlot::
interpFillColor(int i, int n)
{
  return boxObj_->interpBackgroundColor(i, n);
}

double
CQChartsRadarPlot::
fillAlpha() const
{
  return boxObj_->backgroundAlpha();
}

void
CQChartsRadarPlot::
setFillAlpha(double r)
{
  boxObj_->setBackgroundAlpha(r);

  update();
}

bool
CQChartsRadarPlot::
isBorder() const
{
  return boxObj_->isBorder();
}

void
CQChartsRadarPlot::
setBorder(bool b)
{
  boxObj_->setBorder(b);

  update();
}

QString
CQChartsRadarPlot::
borderColorStr() const
{
  return boxObj_->borderColorStr();
}

void
CQChartsRadarPlot::
setBorderColorStr(const QString &str)
{
  boxObj_->setBorderColorStr(str);

  update();
}

QColor
CQChartsRadarPlot::
interpBorderColor(int i, int n) const
{
  return boxObj_->interpBorderColor(i, n);
}

double
CQChartsRadarPlot::
borderAlpha() const
{
  return boxObj_->borderAlpha();
}

void
CQChartsRadarPlot::
setBorderAlpha(double a)
{
  boxObj_->setBorderAlpha(a);

  update();
}

double
CQChartsRadarPlot::
borderWidth() const
{
  return boxObj_->borderWidth();
}

void
CQChartsRadarPlot::
setBorderWidth(double r)
{
  boxObj_->setBorderWidth(r);

  update();
}

void
CQChartsRadarPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumn"  , "name"     );
  addProperty("columns", this, "valueColumn" , "value"    );
  addProperty("columns", this, "valueColumns", "valuesSet");
  addProperty(""       , this, "angleStart"               );
  addProperty("grid"   , this, "gridColor"   , "color"    );
  addProperty("grid"   , this, "gridAlpha"   , "alpha"    );
  addProperty("border" , this, "borderColor" , "color"    );
  addProperty("border" , this, "borderAlpha" , "alpha"    );
  addProperty("border" , this, "borderWidth" , "width"    );
  addProperty("fill"   , this, "fillColor"   , "color"    );
  addProperty("fill"   , this, "fillAlpha"   , "alpha"    );
  addProperty("text"   , this, "textFont"    , "font"     );
  addProperty("text"   , this, "textColor"   , "color"    );
}

void
CQChartsRadarPlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  valueDatas_.clear();

  int nr = model->rowCount(QModelIndex());
  int nv = valueColumns().size();

  for (int r = 0; r < nr; ++r) {
    for (int iv = 0; iv < nv; ++iv) {
      QModelIndex valueInd = model->index(r, valueColumns()[iv]);

      bool ok;

      double value = CQChartsUtil::modelReal(model, valueInd, ok);

      valueDatas_[iv].add(value);
    }
  }

  //---

  valueRadius_ = 0.0;

  for (int iv = 0; iv < nv; ++iv)
    valueRadius_ = std::max(valueRadius_, valueDatas_[iv].max()/valueDatas_[iv].sum());

  //---

  double xr = valueRadius_;
  double yr = valueRadius_;

  if (isEqualScale()) {
    double aspect = this->aspect();

    if (aspect > 1.0)
      xr *= aspect;
    else
      yr *= 1.0/aspect;
  }

  dataRange_.updateRange(-xr, -yr);
  dataRange_.updateRange( xr,  yr);

  //---

  if (apply)
    applyDataRange();
}

bool
CQChartsRadarPlot::
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return false;
  }

  //---

  if (! plotObjs_.empty())
    return false;

  //---

  QAbstractItemModel *model = this->model();

  if (! model)
    return false;

  //---

  int nr = model->rowCount(QModelIndex());
  int nv = valueColumns().size();

  double da = (nv > 0 ? 360.0/nv : 0.0);

  for (int r = 0; r < nr; ++r) {
    bool hidden = isSetHidden(r);

    if (hidden)
      continue;

    //---

    QModelIndex nameInd = model->index(r, nameColumn());

    QModelIndex nameInd1 = normalizeIndex(nameInd);

    bool ok;

    QString name = CQChartsUtil::modelString(model, nameInd, ok);

    //---

    QPolygonF poly;

    double a = angleStart();

    for (int iv = 0; iv < nv; ++iv) {
      QModelIndex valueInd = model->index(r, valueColumns()[iv]);

      bool ok1;

      double value = CQChartsUtil::modelReal(model, valueInd, ok1);

      double scale = valueDatas_[iv].sum();

      double ra = CQChartsUtil::Deg2Rad(a);

      double x = value*cos(ra)/scale;
      double y = value*sin(ra)/scale;

      poly << QPointF(x, y);

      a -= da;
    }

    //---

    CQChartsGeom::BBox bbox(-1, -1, 1, 1);

    CQChartsRadarObj *radarObj =
      new CQChartsRadarObj(this, bbox, name, poly, nameInd1, r, nr);

    addPlotObject(radarObj);
  }

  //---

  resetKeyItems();

  //---

  return true;
}

void
CQChartsRadarPlot::
addKeyItems(CQChartsKey *key)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int nr = model->rowCount(QModelIndex());

  for (int r = 0; r < nr; ++r) {
    QModelIndex nameInd = model->index(r, nameColumn());

    bool ok;

    QString name = CQChartsUtil::modelString(model, nameInd, ok);

    //---

    CQChartsKeyColorBox *color = new CQChartsKeyColorBox(this, r, nr);
    CQChartsKeyText     *text  = new CQChartsKeyText(this, name);

    color->setClickHide(true);

    key->addItem(color, r, 0);
    key->addItem(text , r, 1);
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsRadarPlot::
drawBackground(CQChartsRenderer *renderer)
{
  CQChartsPlot::drawBackground(renderer);

  //---

  int nv = valueColumns().size();

  if (! nv)
    return;

  double da = 360.0/nv;

  //---

  // draw grid spokes
  QColor gridColor1 = interpGridColor(0, 1);

  gridColor1.setAlphaF(gridAlpha());

  QPen gpen1(gridColor1);

  renderer->setPen(gpen1);

  double px1, py1;

  windowToPixel(0.0, 0.0, px1, py1);

  double a = angleStart();

  for (int iv = 0; iv < nv; ++iv) {
    double ra = CQChartsUtil::Deg2Rad(a);

    double x = valueRadius_*cos(ra);
    double y = valueRadius_*sin(ra);

    double px2, py2;

    windowToPixel(x, y, px2, py2);

    renderer->drawLine(QPointF(px1, py1), QPointF(px2, py2));

    a -= da;
  }

  //---

  // draw grid polygons
  QColor gridColor2 = interpGridColor(0, 1);

  QPen gpen2(gridColor2);

  QColor textColor = interpTextColor(0, 1);

  renderer->setFont(textFont());

  QPen tpen(textColor);

  int    nl = 5;
  double dr = valueRadius_/nl;

  for (int i = 0; i <= nl; ++i) {
    double r = dr*i;

    double a = angleStart();

    QPolygonF poly;

    for (int iv = 0; iv < nv; ++iv) {
      double ra = CQChartsUtil::Deg2Rad(a);

      double x = r*cos(ra);
      double y = r*sin(ra);

      double px, py;

      windowToPixel(x, y, px, py);

      poly << QPointF(px, py);

      //---

      if (i == nl) {
        renderer->setPen(tpen);

        //---

        bool ok;

        QString name = CQChartsUtil::modelHeaderString(model(), valueColumns()[iv], ok);

        Qt::Alignment align = 0;

        if      (CQChartsUtil::isZero(x)) align |= Qt::AlignHCenter;
        else if (x > 0)                   align |= Qt::AlignLeft;
        else if (x < 0)                   align |= Qt::AlignRight;

        if      (CQChartsUtil::isZero(y)) align |= Qt::AlignVCenter;
        else if (y > 0)                   align |= Qt::AlignBottom;
        else if (y < 0)                   align |= Qt::AlignTop;

        CQChartsDrawUtil::drawAlignedText(renderer, px, py, name, align, 2, 2);
      }

      //---

      a -= da;
    }

    poly << poly[0];

    renderer->setPen(gpen2);

    renderer->drawPolygon(poly);
  }
}

void
CQChartsRadarPlot::
draw(CQChartsRenderer *renderer)
{
  initPlotObjs();

  //---

  drawParts(renderer);
}

//------

CQChartsRadarObj::
CQChartsRadarObj(CQChartsRadarPlot *plot, const CQChartsGeom::BBox &rect, const QString &name,
                 const QPolygonF &poly, const QModelIndex &ind, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), name_(name), poly_(poly), ind_(ind), i_(i), n_(n)
{
}

QString
CQChartsRadarObj::
calcId() const
{
  return QString("%1").arg(name_);
}

bool
CQChartsRadarObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  return poly_.containsPoint(CQChartsUtil::toQPoint(p), Qt::OddEvenFill);
}

void
CQChartsRadarObj::
addSelectIndex()
{
  plot_->addSelectIndex(ind_);
}

bool
CQChartsRadarObj::
isIndex(const QModelIndex &ind) const
{
  return (ind == ind_);
}

void
CQChartsRadarObj::
draw(CQChartsRenderer *renderer, const CQChartsPlot::Layer &)
{
  if (! poly_.size())
    return;

  QPolygonF ppoly;

  for (int i = 0; i < poly_.size(); ++i) {
    double x = poly_[i].x();
    double y = poly_[i].y();

    double px, py;

    plot_->windowToPixel(x, y, px, py);

    ppoly << QPointF(px, py);
  }

  ppoly << ppoly[0];

  QColor c = plot_->interpFillColor(i_, n_);

  c.setAlphaF(plot_->fillAlpha());

  QBrush brush(c);

  QPen pen;

  if (plot_->isBorder()) {
    QColor bc = plot_->interpBorderColor(0, 1);

    bc.setAlphaF(plot_->borderAlpha());

    pen = QPen(bc);

    pen.setWidthF(plot_->borderWidth());
  }
  else
    pen = QPen(Qt::NoPen);

  plot_->updateObjPenBrushState(this, pen, brush);

  renderer->setPen  (pen);
  renderer->setBrush(brush);

  renderer->drawPolygon(ppoly);
}
