#include <CQChartsAnnotationPlot.h>
#include <CQChartsAnnotation.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsArrow.h>
#include <CQChartsGrahamHull.h>
#include <CQChartsDensity.h>
#include <CQChartsSmooth.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QMenu>

CQChartsAnnotationPlotType::
CQChartsAnnotationPlotType()
{
}

void
CQChartsAnnotationPlotType::
addParameters()
{
  CQChartsPlotType::addParameters();
}

QString
CQChartsAnnotationPlotType::
description() const
{
  return CQChartsHtml().
    h2("Annotation Plot").
     h3("Summary").
      p("Annotation plot which can used to display custom data using annotations.").
     h3("Limitations").
      p("None.");
}

CQChartsPlot *
CQChartsAnnotationPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsAnnotationPlot(view, model);
}

//------

CQChartsAnnotationPlot::
CQChartsAnnotationPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("annotation"), model)
{
}

CQChartsAnnotationPlot::
~CQChartsAnnotationPlot()
{
  CQChartsAnnotationPlot::term();
}

//---

void
CQChartsAnnotationPlot::
init()
{
  CQChartsPlot::init();

  //---

  addAxes();

  addTitle();

  xAxis()->setVisible(false);
  yAxis()->setVisible(false);

  //---

  setOuterMargin(PlotMargin(Length::plot(0), Length::plot(0), Length::plot(0), Length::plot(0)));
}

void
CQChartsAnnotationPlot::
term()
{
}

//---

void
CQChartsAnnotationPlot::
addProperties()
{
  addBaseProperties();
}

CQChartsGeom::Range
CQChartsAnnotationPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsAnnotationPlot::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsAnnotationPlot *>(this);

  th->clearErrors();

  th->objDatas_.clear();

  //---

  // process model data
  class PlotVisitor : public ModelVisitor {
   public:
    PlotVisitor(const CQChartsAnnotationPlot *annotationPlot) :
     annotationPlot_(annotationPlot) {
      const_cast<CQChartsAnnotationPlot *>(annotationPlot_)->setNumColumns(numCols());
    }

    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      annotationPlot_->addRow(model, data, dataRange_);

      return State::OK;
    }

    const Range &dataRange() const { return dataRange_; }

   private:
    const CQChartsAnnotationPlot *annotationPlot_ { nullptr };
    Range                         dataRange_;
  };

  PlotVisitor plotVisitor(this);

  visitModel(plotVisitor);

  return plotVisitor.dataRange();
}

void
CQChartsAnnotationPlot::
addRow(const QAbstractItemModel *, const ModelVisitor::VisitData &data, Range &range) const
{
  auto *th = const_cast<CQChartsAnnotationPlot *>(this);

  auto getValue = [&](const Column &column) {
    auto ind = ModelIndex(th, data.row, column, data.parent);

    bool ok;
    auto var = modelValue(ind, ok);

    if (! ok) return QVariant();

    return var;
  };

#if 0
  auto printVar = [](const std::string &name, const QVariant &var) {
    std::cerr << name << ": " << var.toString().toStdString() << "\n";
  };
#endif

  ObjData objData;

  // Type, Id, Geometry, Data, Style
  auto typeStr     = getValue(Column(0)).toString();
  auto idStr       = getValue(Column(1)).toString();
  auto geometryStr = getValue(Column(2)).toString();
  objData.data     = getValue(Column(3)).toString();
  objData.style    = getValue(Column(4));

  //---

  auto spos = idStr.indexOf('/');

  while (spos >= 0) {
    objData.idList.push_back(idStr.mid(0, spos));

    idStr = idStr.mid(spos + 1);

    spos = idStr.indexOf('/');
  }

  objData.id = idStr;

  objData.idList.push_back(idStr);

  //---

  auto ind = modelIndex(ModelIndex(th, data.row, Column(0), data.parent));

  objData.ind  = normalizeIndex(ind);

#if 0
  printVar("type"    , typeStr);
  printVar("  id"      , objData.id);
  printVar("  geometry", geometryStr);
  printVar("  data"    , objData.data);
  printVar("  style"   , objData.style);
#endif

  objData.type = static_cast<CQChartsAnnotationType>(CQChartsAnnotation::stringToType(typeStr));

  if (objData.type == CQChartsAnnotationType::NONE)
    return;

  auto pointToRect = [](const CQChartsPosition &point) {
    auto p1 = point.p() - Point(0.1, 0.1);
    auto p2 = point.p() + Point(0.1, 0.1);

    return CQChartsRect::plot(BBox(p1, p2));
  };

  if      (objData.type == CQChartsAnnotationType::GROUP) {
    objData.rect = CQChartsRect::plot(BBox(Point(0, 0), Point(1, 1)));
  }
  else if (objData.type == CQChartsAnnotationType::RECT) {
    objData.types = uint(CQChartsAnnotationObjData::Types::RECT);
    objData.rect  = CQChartsRect::plot(geometryStr);
  }
  else if (objData.type == CQChartsAnnotationType::ELLIPSE) {
    objData.types = uint(CQChartsAnnotationObjData::Types::RECT);
    objData.rect  = CQChartsRect::plot(geometryStr);
  }
  else if (objData.type == CQChartsAnnotationType::POLYGON ||
           objData.type == CQChartsAnnotationType::POLYLINE) {
    objData.types   = uint(CQChartsAnnotationObjData::Types::POLYGON);
    objData.polygon = CQChartsPolygon::plot(geometryStr);

    if (objData.polygon.isValid())
      objData.rect = objData.polygon.boundingBox();
    else
      objData.rect = CQChartsRect::plot(BBox(Point(0, 0), Point(1, 1)));
  }
  else if (objData.type == CQChartsAnnotationType::TEXT) {
    objData.types = uint(CQChartsAnnotationObjData::Types::RECT);
    objData.rect  = CQChartsRect::plot(geometryStr);

    if (! objData.rect.isValid()) {
      objData.types = uint(CQChartsAnnotationObjData::Types::POINT);
      objData.point = CQChartsPosition::plot(geometryStr);
      objData.rect  = pointToRect(objData.point);
    }
  }
  else if (objData.type == CQChartsAnnotationType::IMAGE) {
    objData.types = uint(CQChartsAnnotationObjData::Types::RECT);
    objData.rect  = CQChartsRect::plot(geometryStr);

    if (! objData.rect.isValid()) {
      objData.types = uint(CQChartsAnnotationObjData::Types::POINT);
      objData.point = CQChartsPosition::plot(geometryStr);
      objData.rect  = pointToRect(objData.point);
    }

    objData.image = CQChartsImage(objData.data);
  }
  else if (objData.type == CQChartsAnnotationType::PATH) {
    objData.types = uint(CQChartsAnnotationObjData::Types::PATH);
    objData.path  = CQChartsPath(geometryStr);
    objData.rect  = CQChartsRect(objData.path.bbox());
  }
  else if (objData.type == CQChartsAnnotationType::ARROW) {
    objData.types = uint(CQChartsAnnotationObjData::Types::PATH);
    objData.path  = CQChartsPath(geometryStr);
    objData.rect  = CQChartsRect(objData.path.bbox());
  }
  else if (objData.type == CQChartsAnnotationType::ARC) {
    objData.types = uint(CQChartsAnnotationObjData::Types::RECT);
    objData.rect  = CQChartsRect::plot(geometryStr);
  }
  else if (objData.type == CQChartsAnnotationType::ARC_CONNECTOR) {
    objData.types = uint(CQChartsAnnotationObjData::Types::RECT);
    objData.rect  = CQChartsRect::plot(geometryStr);
  }
  else if (objData.type == CQChartsAnnotationType::PIE_SLICE) {
    objData.types = uint(CQChartsAnnotationObjData::Types::POINT);
    objData.point = CQChartsPosition::plot(geometryStr);
    objData.rect  = pointToRect(objData.point);
  }
  else if (objData.type == CQChartsAnnotationType::AXIS) {
    objData.types = uint(CQChartsAnnotationObjData::Types::RECT);
    objData.rect  = CQChartsRect::plot(geometryStr);
  }
  else if (objData.type == CQChartsAnnotationType::KEY) {
    objData.types = uint(CQChartsAnnotationObjData::Types::RECT);
    objData.rect  = CQChartsRect::plot(geometryStr);
  }
  else if (objData.type == CQChartsAnnotationType::POINT) {
    objData.types = uint(CQChartsAnnotationObjData::Types::POINT);
    objData.point = CQChartsPosition::plot(geometryStr);
    objData.rect  = pointToRect(objData.point);
  }
  else if (objData.type == CQChartsAnnotationType::POINT_SET) {
    objData.types   = uint(CQChartsAnnotationObjData::Types::POLYGON);
    objData.polygon = CQChartsPolygon::plot(geometryStr);
    objData.rect    = objData.polygon.boundingBox();
  }
  else if (objData.type == CQChartsAnnotationType::VALUE_SET) {
    objData.types = uint(CQChartsAnnotationObjData::Types::RECT);
    objData.rect  = CQChartsRect::plot(geometryStr);
  }
  else {
    std::cerr << "Unhandled type\n";
    objData.rect = CQChartsRect::plot(BBox(Point(0, 0), Point(1, 1)));
  }

  if (! objData.rect.isValid())
    objData.rect = BBox(0, 0, 1, 1);

  if (objData.idList.length() == 1)
    range += objData.rect.bbox();

  th->objDatas_.push_back(objData);
}

bool
CQChartsAnnotationPlot::
createObjs(PlotObjs &objs) const
{
  auto *th = const_cast<CQChartsAnnotationPlot *>(this);

  th->objs_.clear();

  int iv = 0;
  int nv = int(objDatas_.size());

  for (const auto &objData : objDatas_) {
    CQChartsAnnotationBaseObj *obj;

    if      (objData.type == CQChartsAnnotationType::GROUP)
      obj = new CQChartsAnnotationGroupObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::RECT)
      obj = new CQChartsAnnotationRectObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::ELLIPSE)
      obj = new CQChartsAnnotationEllipseObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::POLYGON)
      obj = new CQChartsAnnotationPolygonObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::POLYLINE)
      obj = new CQChartsAnnotationPolylineObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::TEXT)
      obj = new CQChartsAnnotationTextObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::IMAGE)
      obj = new CQChartsAnnotationImageObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::PATH)
      obj = new CQChartsAnnotationPathObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::ARROW)
      obj = new CQChartsAnnotationArrowObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::ARC)
      obj = new CQChartsAnnotationArcObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::ARC_CONNECTOR)
      obj = new CQChartsAnnotationArcConnectorObj(this, objData.rect.bbox(), objData,
                                                  ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::POINT)
      obj = new CQChartsAnnotationPointObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::PIE_SLICE)
      obj = new CQChartsAnnotationPieSliceObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::AXIS)
      obj = new CQChartsAnnotationAxisObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::KEY)
      obj = new CQChartsAnnotationKeyObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::POINT_SET)
      obj = new CQChartsAnnotationPointSetObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else if (objData.type == CQChartsAnnotationType::VALUE_SET)
      obj = new CQChartsAnnotationValueSetObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));
    else
      obj = new CQChartsAnnotationPlotObj(this, objData.rect.bbox(), objData, ColorInd(iv, nv));

    obj->connectDataChanged(this, SLOT(updateSlot()));

    th->objs_.push_back(obj);

    objs.push_back(obj);

    ++iv;
  }

  return true;
}

void
CQChartsAnnotationPlot::
postCreateObjs()
{
}

CQChartsAnnotationBaseObj *
CQChartsAnnotationPlot::
findObject(const QString &id) const
{
  for (auto *obj : objs_) {
    if (obj->data().id == id)
      return obj;
  }

  return nullptr;
}

//------

bool
CQChartsAnnotationPlot::
addMenuItems(QMenu *, const Point &)
{
  return true;
}

//---

CQChartsPlotCustomControls *
CQChartsAnnotationPlot::
createCustomControls()
{
  auto *controls = new CQChartsAnnotationPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//---

CQChartsAnnotationGroupObj::
CQChartsAnnotationGroupObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                           const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
}

void
CQChartsAnnotationGroupObj::
setShapeType(const ShapeType &s)
{
  CQChartsUtil::testAndSet(shapeType_, s, [&]() { invalidate(); } );
}

void
CQChartsAnnotationGroupObj::
invalidate()
{
  if (needsLayout_) {
    if (layoutType() != LayoutType::NONE)
      doLayout();

    needsLayout_ = false;
  }

  CQChartsAnnotationBaseObj::invalidate();
}

void
CQChartsAnnotationGroupObj::
doLayout()
{
  assert(layoutType() != LayoutType::NONE);

  //---

  if      (layoutType() == LayoutType::HV)
    layoutHV();
  else if (layoutType() == LayoutType::CIRCLE)
    layoutCircle();
  else if (layoutType() == LayoutType::TEXT_OVERLAP)
    layoutTextOverlap();
  else if (layoutType() == LayoutType::TEXT_CLOUD)
    layoutTextCloud();
  else if (layoutType() == LayoutType::TREEMAP)
    layoutTreemap();
  else if (layoutType() == LayoutType::GRAPH)
    layoutGraph();

  if (plot())
    plot()->drawObjs();

  //initBBox_ = true;
}

void
CQChartsAnnotationGroupObj::
layoutHV()
{
#if 0
  // place child annotations in bbox
  double x    { 0.0 }, y    { 0.0 };
  double maxW { 0.0 }, maxH { 0.0 };
  double sumW { 0.0 }, sumH { 0.0 };

  int n = 0;

  for (auto *annotation : annotations_) {
    const auto &bbox1 = annotation->annotationBBox();
    if (! bbox1.isSet()) continue;

    if (n == 0) {
      x = bbox1.getXMin();
      y = bbox1.getYMax();
    }

    double w1 = bbox1.getWidth ();
    double h1 = bbox1.getHeight();

    maxW = std::max(maxW, w1);
    maxH = std::max(maxH, h1);

    sumW += w1;
    sumH += h1;

    ++n;
  }

  if (annotationBBox().isSet()) {
    x = annotationBBox().getXMin();
    y = annotationBBox().getYMax();
  }

  // left to right
  if (layoutOrient() == Qt::Horizontal) {
    double spacing = pixelToWindowWidth(layoutSpacing());
    double margin  = pixelToWindowWidth(layoutMargin());

    //---

    double aw = sumW + 2*margin + (n - 1)*spacing;
    double ah = maxH + 2*margin;

    y -= ah;

    setAnnotationBBox(BBox(x, y, x + aw, y + ah));

    x += margin;

    for (auto *annotation : annotations_) {
      const auto &bbox1 = annotation->annotationBBox();
      if (! bbox1.isSet()) continue;

      double w1 = bbox1.getWidth ();
      double h1 = bbox1.getHeight();

      double y1 = y;

      if      (layoutAlign() & Qt::AlignBottom)
        y1 = y;
      else if (layoutAlign() & Qt::AlignVCenter)
        y1 = y + (ah - h1)/2;
      else if (layoutAlign() & Qt::AlignTop)
        y1 = y + ah - h1;

      annotation->setDisableSignals(true);

      annotation->setEditBBox(BBox(x, y1, x + w1, y1 + h1), CQChartsResizeSide::NONE);

      annotation->setDisableSignals(false);

      x += w1 + spacing;
    }
  }
  // top to bottom
  else {
    double spacing = pixelToWindowHeight(layoutSpacing());
    double margin  = pixelToWindowHeight(layoutMargin());

    //---

    double aw = maxW + 2*margin;
    double ah = sumH + 2*margin + (n - 1)*spacing;

    setAnnotationBBox(BBox(x, y - ah, x + aw, y));

    y -= margin;

    for (auto *annotation : annotations_) {
      const auto &bbox1 = annotation->annotationBBox();
      if (! bbox1.isSet()) continue;

      double w1 = bbox1.getWidth ();
      double h1 = bbox1.getHeight();

      double x1 = x;

      if      (layoutAlign() & Qt::AlignLeft)
        x1 = x;
      else if (layoutAlign() & Qt::AlignHCenter)
        x1 = x + (aw - w1)/2;
      else if (layoutAlign() & Qt::AlignRight)
        x1 = x + aw - w1;

      annotation->setDisableSignals(true);

      annotation->setEditBBox(BBox(x1, y - h1, x1 + w1, y), CQChartsResizeSide::NONE);

      annotation->setDisableSignals(false);

      y -= h1 + spacing;
    }
  }
#endif
}

void
CQChartsAnnotationGroupObj::
layoutCircle()
{
  class CircleNode : public CQChartsCircleNode {
   public:
    CircleNode(CQChartsAnnotationBaseObj *obj) :
     obj_(obj) {
    }

    CQChartsAnnotationBaseObj *obj() const { return obj_; }

    double radius() const override {
      auto bbox = obj_->rect();

      return (bbox.isValid() ? bbox.getWidth()/2.0 : 1.0);
    }

   private:
    CQChartsAnnotationBaseObj *obj_ { nullptr };
  };

  //---

  // pack child annotations using circle pack
  using Pack = CQChartsCirclePack<CircleNode>;

  Pack pack;

  using Nodes = std::vector<CircleNode *>;

  Nodes nodes;

  for (auto *child : children_) {
    auto *group = dynamic_cast<CQChartsAnnotationGroupObj *>(child);

    if (group)
      group->layoutCircle();

    //---

    auto *node = new CircleNode(child);

    nodes.push_back(node);

    pack.addNode(node);
  }

  //---

  // get bounding circle
  double xc { 0.0 }, yc { 0.0 }, r { 1.0 };

  pack.boundingCircle(xc, yc, r);

  //---

  auto xs = rect().getWidth ()/(2.0*r);
  auto ys = rect().getHeight()/(2.0*r);

  auto ss = std::min(xs, ys);

  //---

  for (auto *node : nodes) {
    auto *obj = node->obj();

    auto r1 = node->radius()*ss;

    auto dx1 = (node->x() + r)/(2.0*r);
    auto dy1 = (node->y() + r)/(2.0*r);

    auto x1 = rect().getXMin() + dx1*rect().getWidth ();
    auto y1 = rect().getYMin() + dy1*rect().getHeight();

    obj->setRect(BBox(x1 - r1, y1 - r1, x1 + r1, y1 + r1));

    delete node;
  }
}

void
CQChartsAnnotationGroupObj::
layoutTextOverlap()
{
#if 0
  if (! plot())
    return;

  const auto &rect = plot()->dataRect();
  if (! rect.isValid()) return;

  using TextPlacer     = CQChartsTextPlacer;
  using TextAnnotation = CQChartsTextAnnotation;

  TextPlacer placer;

  using AnnotationDrawTexts = std::map<TextAnnotation *, TextPlacer::DrawText *>;

  AnnotationDrawTexts annotationDrawTexts;

  for (auto *annotation : annotations_) {
    auto *textAnnotation = dynamic_cast<TextAnnotation *>(annotation);
    if (! textAnnotation) continue;

    CQChartsTextOptions textOptions;

    auto c = QColor(Qt::black);

    auto bbox = textAnnotation->annotationBBox();

    auto *drawText =
      new TextPlacer::DrawText(textAnnotation->textStr(), bbox.getCenter(), textOptions,
                               c, Alpha(), bbox.getCenter(), QFont());

    drawText->setBBox(bbox);

    placer.addDrawText(drawText);

    annotationDrawTexts[textAnnotation] = drawText;
  }

  placer.place(rect);

  for (const auto &p : annotationDrawTexts) {
    auto *textAnnotation = p.first;
    auto *drawText       = p.second;

    auto bbox = textAnnotation->calcBBox();

    auto c = (bbox.isValid() ? bbox.getCenter() : Point(0, 0));

    double dx = drawText->point.x - c.x;
    double dy = drawText->point.y - c.y;

    if (bbox.isValid())
      bbox.moveBy(Point(dx, dy));
    else
      bbox = BBox(drawText->point.x - 0.0, drawText->point.y - 0.5,
                  drawText->point.x + 0.5, drawText->point.y + 0.5);

    textAnnotation->setDisableSignals(true);

    textAnnotation->moveTo(bbox.getCenter());

    textAnnotation->setDisableSignals(false);
  }
#endif
}

void
CQChartsAnnotationGroupObj::
layoutTextCloud()
{
#if 0
  using WordCloud      = CQChartsWordCloud;
  using TextAnnotation = CQChartsTextAnnotation;

  WordCloud wordCloud;

  using WordTextAnnotation = std::map<int, TextAnnotation *>;

  WordTextAnnotation wordTextAnnotation;

  for (auto *annotation : annotations_) {
    auto *textAnnotation = dynamic_cast<TextAnnotation *>(annotation);
    if (! textAnnotation) continue;

    auto value = std::max(textAnnotation->value().realOr(1.0), 1.0);

    int ind = wordCloud.addWord(textAnnotation->textStr(), int(value));

    wordTextAnnotation[ind] = textAnnotation;
  }

  wordCloud.setMinFontSize(windowToPixelWidth(0.04));
  wordCloud.setMaxFontSize(windowToPixelWidth(0.30));

  wordCloud.place(plot());

  for (const auto &wordData : wordCloud.wordDatas()) {
    BBox bbox(wordData->wordRect.xmin(), wordData->wordRect.ymin(),
              wordData->wordRect.xmax(), wordData->wordRect.ymax());

    auto *textAnnotation = wordTextAnnotation[wordData->ind];

    textAnnotation->setDisableSignals(true);

    textAnnotation->setTextAlign (Qt::AlignHCenter | Qt::AlignVCenter);
    textAnnotation->setTextHtml  (true); // TODO: html should not be needed !!
    textAnnotation->setTextScaled(true);

    //auto f = textAnnotation->textFont();
    //f.setPointSizeF(wordData->fontSize);
    //textAnnotation->setTextFont(f);

    textAnnotation->setRectangle(CQChartsRect::plot(bbox));

    textAnnotation->setEditBBox(bbox, CQChartsResizeSide::NONE);

    textAnnotation->setDisableSignals(false);
  }
#endif
}

void
CQChartsAnnotationGroupObj::
layoutTreemap()
{
#if 0
  auto bbox = annotationBBox();
  if (! bbox.isValid()) return;

  CQChartsTreeMapPlace place(bbox);

  for (auto *annotation : annotations_) {
    auto value = std::max(annotation->value().realOr(1.0), 1.0);
    if (value <= 0.0) continue;

    place.addValue(value, annotation);
  }

  place.placeValues();

  place.processAreas([&](const BBox &bbox, const CQChartsTreeMapPlace::IArea &iarea) {
    auto *annotation = static_cast<CQChartsAnnotation *>(iarea.data);

    annotation->setDisableSignals(true);

    annotation->setEditBBox(bbox, CQChartsResizeSide::NONE);

    annotation->setDisableSignals(true);
  });
#endif
}

void
CQChartsAnnotationGroupObj::
layoutGraph()
{
#if 0
  auto rect = annotationBBox();

  if (! rect.isValid()) {
    setAnnotationBBox(BBox(0, 0, 100, 100));

    rect = annotationBBox();
  }

  double w = rect.getWidth ();
//double h = rect.getHeight();

  CQChartsGraphMgr mgr(plot());

  // use plot units to avoid conversion so we use annotation bbox
  mgr.setNodeWidth  (Length::plot(w/ 20.0));
  mgr.setNodeMargin (Length::plot(w/100.0));
  mgr.setNodeSpacing(Length::plot(0.2    ));

//mgr.setNodeScaled    (true);
  mgr.setNodePerpScaled(true);

  auto *graph = mgr.createGraph("graph");

  for (auto *annotation : annotations_) {
    if (annotation->subType() != SubType::CONNECTOR)
      continue;

    auto *connector = dynamic_cast<CQChartsConnectorAnnotationBase *>(annotation);
    assert(connector);

    BBox         bbox;
    CQChartsObj *startObj = nullptr, *endObj = nullptr;

    if (! intersectObjectRect(connector->startObjRef(), startObj, bbox) ||
        ! intersectObjectRect(connector->endObjRef  (), endObj  , bbox))
      continue;

    auto *startAnnotation = dynamic_cast<CQChartsAnnotation *>(startObj);
    auto *endAnnotation   = dynamic_cast<CQChartsAnnotation *>(endObj  );
    if (! startAnnotation || ! endAnnotation) continue;

    auto *startNode = mgr.findNode(startAnnotation->pathId());
    auto *endNode   = mgr.findNode(endAnnotation  ->pathId());

    if (! startNode) {
      startNode = mgr.addNode(startAnnotation->pathId());

      graph->addNode(startNode);
    }

    if (! endNode) {
      endNode = mgr.addNode(endAnnotation->pathId());

      graph->addNode(endNode);
    }

    CQChartsOptReal value(connector->value());

    auto *edge = mgr.createEdge(value, startNode, endNode);

    startNode->addDestEdge(edge);
    endNode  ->addSrcEdge (edge);
  }

  graph->placeGraph(rect);

  for (const auto *node : graph->nodes()) {
    auto *annotation = (plot() ? plot()->getAnnotationByPathId(node->name()) : nullptr);
    if (! annotation) continue;

    annotation->setDisableSignals(true);

    annotation->setEditBBox(node->rect(), CQChartsResizeSide::NONE);

    annotation->setDisableSignals(false);
  }

  delete graph;
#endif
}

//---

void
CQChartsAnnotationGroupObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->setObjectRoot(path, this);

  CQChartsAnnotationBaseObj::addProperties(model, path);

  model->addProperty(path, this, "shapeType" )->setDesc("Shape Type");
  model->addProperty(path, this, "layoutType")->setDesc("Layout Type");
}

void
CQChartsAnnotationGroupObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  CQChartsDrawUtil::drawRoundedRect(device, penBrush, rect());

  device->resetColorNames();
}

//---

CQChartsAnnotationRectObj::
CQChartsAnnotationRectObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                          const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
  rect_ = data.rect;
}

void
CQChartsAnnotationRectObj::
setRect(const Rect &rect)
{
  CQChartsUtil::testAndSet(rect_, rect, [&]() { invalidate(); } );
}

void
CQChartsAnnotationRectObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->setObjectRoot(path, this);

  CQChartsAnnotationBaseObj::addProperties(model, path);

  model->addProperty(path, this, "rect" )->setDesc("Rectangle");
}

void
CQChartsAnnotationRectObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  CQChartsDrawUtil::drawRoundedRect(device, penBrush, rect_.bbox());

  device->resetColorNames();
}

//---

CQChartsAnnotationEllipseObj::
CQChartsAnnotationEllipseObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                             const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
  rect_ = data.rect;
}

void
CQChartsAnnotationEllipseObj::
setRect(const Rect &rect)
{
  CQChartsUtil::testAndSet(rect_, rect, [&]() { invalidate(); } );
}

void
CQChartsAnnotationEllipseObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->setObjectRoot(path, this);

  CQChartsAnnotationBaseObj::addProperties(model, path);

  model->addProperty(path, this, "rect" )->setDesc("Rectangle");
}

void
CQChartsAnnotationEllipseObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  CQChartsDrawUtil::drawEllipse(device, rect_.bbox());

  device->resetColorNames();
}

//---

CQChartsAnnotationPolygonObj::
CQChartsAnnotationPolygonObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                             const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
  polygon_ = data_.polygon;
}

CQChartsAnnotationPolygonObj::
~CQChartsAnnotationPolygonObj()
{
}

void
CQChartsAnnotationPolygonObj::
setPolygon(const Polygon &polygon)
{
  CQChartsUtil::testAndSet(polygon_, polygon, [&]() { invalidate(); } );
}

void
CQChartsAnnotationPolygonObj::
setSmoothed(bool b)
{
  CQChartsUtil::testAndSet(smoothed_, b, [&]() { invalidate(); } );
}

void
CQChartsAnnotationPolygonObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->setObjectRoot(path, this);

  CQChartsAnnotationBaseObj::addProperties(model, path);

  model->addProperty(path, this, "polygon" )->setDesc("Polygon");
  model->addProperty(path, this, "smoothed")->setDesc("Smooth lines");
}

void
CQChartsAnnotationPolygonObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  // create path
  QPainterPath path;

  if (isSmoothed()) {
    initSmooth();

    // smooth path
    path = smooth_->createPath(/*closed*/true);
  }
  else {
    path = CQChartsDrawUtil::polygonToPath(polygon_.polygon(), /*closed*/true);
  }

  device->drawPath(path);

  device->resetColorNames();
}

void
CQChartsAnnotationPolygonObj::
initSmooth() const
{
  // init smooth if needed
  if (! smooth_) {
    auto *th = const_cast<CQChartsAnnotationPolygonObj *>(this);

    th->smooth_ = std::make_unique<Smooth>(polygon_.polygon(), /*sorted*/false);
  }
}

//---

CQChartsAnnotationPolylineObj::
CQChartsAnnotationPolylineObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                              const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
  polygon_ = data_.polygon;
}

CQChartsAnnotationPolylineObj::
~CQChartsAnnotationPolylineObj()
{
}

void
CQChartsAnnotationPolylineObj::
setPolygon(const Polygon &polygon)
{
  CQChartsUtil::testAndSet(polygon_, polygon, [&]() { invalidate(); } );
}

void
CQChartsAnnotationPolylineObj::
setSmoothed(bool b)
{
  CQChartsUtil::testAndSet(smoothed_, b, [&]() { invalidate(); } );
}

void
CQChartsAnnotationPolylineObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->setObjectRoot(path, this);

  CQChartsAnnotationBaseObj::addProperties(model, path);

  model->addProperty(path, this, "polygon" )->setDesc("Polygon");
  model->addProperty(path, this, "smoothed")->setDesc("Smooth lines");
}

void
CQChartsAnnotationPolylineObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // create path
  QPainterPath path;

  if (isSmoothed()) {
    initSmooth();

    // smooth path
    path = smooth_->createPath(/*closed*/false);
  }
  else {
    path = CQChartsDrawUtil::polygonToPath(polygon_.polygon(), /*closed*/false);
  }

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->strokePath(path, penBrush.pen);

  device->resetColorNames();
}

void
CQChartsAnnotationPolylineObj::
initSmooth() const
{
  // init smooth if needed
  if (! smooth_) {
    auto *th = const_cast<CQChartsAnnotationPolylineObj *>(this);

    th->smooth_ = std::make_unique<Smooth>(polygon_.polygon(), /*sorted*/false);
  }
}

//---

CQChartsAnnotationTextObj::
CQChartsAnnotationTextObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                          const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
  rect_  = data_.rect;
  point_ = data_.point;
  text_  = data_.data;

  if (style_.textFont().length())
    textOptions_.font = CQChartsFont(style_.textFont());

  if (style_.textAngle().length())
    textOptions_.angle = CQChartsAngle(style_.textAngle());

  textOptions_.align = style_.textAlign();
}

void
CQChartsAnnotationTextObj::
setText(const QString &s)
{
  text_ = s;

  invalidate();
}

void
CQChartsAnnotationTextObj::
setRect(const Rect &rect)
{
  CQChartsUtil::testAndSet(rect_, rect, [&]() { invalidate(); } );
}

void
CQChartsAnnotationTextObj::
setPoint(const Point &p)
{
  CQChartsUtil::testAndSet(point_, p, [&]() { invalidate(); } );
}

void
CQChartsAnnotationTextObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->setObjectRoot(path, this);

  CQChartsAnnotationBaseObj::addProperties(model, path);

  model->addProperty(path, this, "text")->setDesc("Text");
}

void
CQChartsAnnotationTextObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  auto p = point_.p();

  if (parent_)
    p += parent_->displayRect().getCenter();

#if 0
  auto p1 = plot_->windowToPixel(p);

  QFontMetricsF fm(device->font());

  auto tw = fm.horizontalAdvance(text_);
  auto ta = fm.ascent();
  auto td = fm.descent();

  if      (textOptions_.align & Qt::AlignHCenter)
    p1.setX(p1.x - tw/2.0);
  else if (textOptions_.align & Qt::AlignRight)
    p1.setX(p1.x - tw);

  if      (textOptions_.align & Qt::AlignVCenter)
    p1.setY(p1.y + (ta - td)/2.0);
  else if (textOptions_.align & Qt::AlignTop)
    p1.setY(p1.y + ta);
  else if (textOptions_.align & Qt::AlignBottom)
    p1.setY(p1.y - td);

  p = plot_->pixelToWindow(p1);
#endif

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  plot_->setPainterFont(device, textOptions_.font);

  CQChartsDrawUtil::drawTextAtPoint(device, p, text_, textOptions_, /*centered*/true);

  device->resetColorNames();
}

//---

CQChartsAnnotationImageObj::
CQChartsAnnotationImageObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                           const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
  rect_  = data_.rect;
  point_ = data_.point;
  image_ = data_.image;
}

void
CQChartsAnnotationImageObj::
setRect(const Rect &rect)
{
  CQChartsUtil::testAndSet(rect_, rect, [&]() { invalidate(); } );
}

void
CQChartsAnnotationImageObj::
setPoint(const Point &p)
{
  CQChartsUtil::testAndSet(point_, p, [&]() { invalidate(); } );
}

void
CQChartsAnnotationImageObj::
setImage(const Image &image)
{
  CQChartsUtil::testAndSet(image_, image, [&]() { invalidate(); } );
}

void
CQChartsAnnotationImageObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->setObjectRoot(path, this);

  CQChartsAnnotationBaseObj::addProperties(model, path);

  model->addProperty(path, this, "rect" )->setDesc("Rectangle");
  model->addProperty(path, this, "point")->setDesc("Point");
  model->addProperty(path, this, "image")->setDesc("Image");
}

void
CQChartsAnnotationImageObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  if (data_.types == uint(CQChartsAnnotationObjData::Types::RECT))
    device->drawImageInRect(rect_.bbox(), image_);
  else
    device->drawImage(point_.p(), image_);
}

//---

CQChartsAnnotationPathObj::
CQChartsAnnotationPathObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                          const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
  path_ = data_.path;
}

void
CQChartsAnnotationPathObj::
setPath(const Path &path)
{
  CQChartsUtil::testAndSet(path_, path, [&]() { invalidate(); } );
}

void
CQChartsAnnotationPathObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->setObjectRoot(path, this);

  CQChartsAnnotationBaseObj::addProperties(model, path);

  model->addProperty(path, this, "path")->setDesc("Path");
}

void
CQChartsAnnotationPathObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawPath(path_.path());

  device->resetColorNames();
}

//---

CQChartsAnnotationArrowObj::
CQChartsAnnotationArrowObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                           const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
}

void
CQChartsAnnotationArrowObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  QPainterPath arrowPath;

  const auto &arrowData = style_.arrowData();

  auto xlw = plot_->lengthPlotWidth (arrowData.lineWidth());
  auto ylw = plot_->lengthPlotHeight(arrowData.lineWidth());

  CQChartsArrow::pathAddArrows(device, data_.path.path(), arrowData, xlw, ylw,
                               arrowData.frontLength(), arrowData.tailLength(),
                               arrowPath);

  device->drawPath(arrowPath);
}

//---

CQChartsAnnotationArcObj::
CQChartsAnnotationArcObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                         const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
}

void
CQChartsAnnotationArcObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  CQChartsDrawUtil::drawArc(device, rect(), style_.arcStart(), style_.arcDelta());
}

//---

CQChartsAnnotationArcConnectorObj::
CQChartsAnnotationArcConnectorObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                                  const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
}

void
CQChartsAnnotationArcConnectorObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  CQChartsDrawUtil::drawArcsConnector(device, rect(), style_.arcStart(), style_.arcDelta(),
                                      style_.arcEnd(), style_.arcEndDelta());
}

//---

CQChartsAnnotationAxisObj::
CQChartsAnnotationAxisObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                          const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
  bool isVertical = (style_.direction() == "vertical");

  CQChartsTclNameValues nameValues(data_.data);

  double from = 0.0, to = 1.0;
  bool ok;
  (void) nameValues.nameValueReal("from", from, ok);
  (void) nameValues.nameValueReal("to"  , to  , ok);

  CQChartsGeom::RMinMax range;

  range.add(from);
  range.add(to);

  axis_ = std::make_unique<CQChartsAxis>(plot());

  direction_ = (isVertical ? Qt::Vertical : Qt::Horizontal);
  axis_->setDirection(direction_);

  if (direction_ == Qt::Vertical) {
    start_ = rect.getYMin();
    end_   = rect.getYMax();
  }
  else {
    start_ = rect.getXMin();
    end_   = rect.getXMax();
  }

  axis_->setStart(start_);
  axis_->setEnd  (end_);

  axis_->setValueRange(range.min(), range.max());

  if (direction_ == Qt::Vertical)
    position_ = rect.getXMin();
  else
    position_ = rect.getYMin();

  axis_->setPosition(position_);
}

CQChartsAnnotationAxisObj::
~CQChartsAnnotationAxisObj()
{
}

void
CQChartsAnnotationAxisObj::
setDirection(Qt::Orientation &d)
{
  direction_ = d;

  axis_->setDirection(direction_);
}

void
CQChartsAnnotationAxisObj::
setPosition(double r)
{
  if (r != position_) {
    position_ = r;

    axis_->setPosition(CQChartsOptReal(position_));
  }
}

void
CQChartsAnnotationAxisObj::
setStart(double r)
{
  start_ = r;

  axis_->setStart(start_);
}

void
CQChartsAnnotationAxisObj::
setEnd(double r)
{
  end_ = r;

  axis_->setEnd(end_);
}

void
CQChartsAnnotationAxisObj::
setValueType(const ValueType &v)
{
  valueType_ = v;

  axis_->setValueType(v);
}

void
CQChartsAnnotationAxisObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->setObjectRoot(path, this);

  CQChartsAnnotationBaseObj::addProperties(model, path);

  model->addProperty(path, this, "direction")->setDesc("Axis direction");
  model->addProperty(path, this, "start"    )->setDesc("Axis start");
  model->addProperty(path, this, "end"      )->setDesc("Axis end");
  model->addProperty(path, this, "position" )->setDesc("Axis position");
  model->addProperty(path, this, "valueType")->setDesc("Axis value type");
}

void
CQChartsAnnotationAxisObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  axis_->draw(plot(), device);

  device->resetColorNames();
}

//---

CQChartsAnnotationPointObj::
CQChartsAnnotationPointObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                           const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
  point_ = data_.point;

  if (style_.symbolType() != "")
    symbol_ = Symbol(style_.symbolType());
  else
    symbol_ = Symbol::circle();

  size_ = Length::pixel(style_.symbolSize());
}

void
CQChartsAnnotationPointObj::
setPoint(const Point &p)
{
  CQChartsUtil::testAndSet(point_, p, [&]() { invalidate(); } );
}

void
CQChartsAnnotationPointObj::
setSymbol(const Symbol &s)
{
  CQChartsUtil::testAndSet(symbol_, s, [&]() { invalidate(); } );
}

void
CQChartsAnnotationPointObj::
setSize(const Length &l)
{
  CQChartsUtil::testAndSet(size_, l, [&]() { invalidate(); } );
}

void
CQChartsAnnotationPointObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->setObjectRoot(path, this);

  CQChartsAnnotationBaseObj::addProperties(model, path);

  model->addProperty(path, this, "point" )->setDesc("Point");
  model->addProperty(path, this, "symbol")->setDesc("Symbol");
  model->addProperty(path, this, "size"  )->setDesc("Size");
}

void
CQChartsAnnotationPointObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  auto p = point_.p();

  CQChartsDrawUtil::drawSymbol(device, penBrush, symbol_, p, size_);
}

//---

CQChartsAnnotationPieSliceObj::
CQChartsAnnotationPieSliceObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                              const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
  innerRadius_ = Length::pixel(style_.innerRadius());
  outerRadius_ = Length::pixel(style_.outerRadius());
  startAngle_  = Angle(style_.arcStart());
  spanAngle_   = Angle(style_.arcDelta());
}

void
CQChartsAnnotationPieSliceObj::
setInnerRadius(const Length &r)
{
  CQChartsUtil::testAndSet(innerRadius_, r, [&]() { invalidate(); } );
}

void
CQChartsAnnotationPieSliceObj::
setOuterRadius(const Length &r)
{
  CQChartsUtil::testAndSet(outerRadius_, r, [&]() { invalidate(); } );
}

void
CQChartsAnnotationPieSliceObj::
setStartAngle(const Angle &a)
{
  CQChartsUtil::testAndSet(startAngle_, a, [&]() { invalidate(); } );
}

void
CQChartsAnnotationPieSliceObj::
setSpanAngle(const Angle &a)
{
  CQChartsUtil::testAndSet(spanAngle_, a, [&]() { invalidate(); } );
}

void
CQChartsAnnotationPieSliceObj::
setArcType(const ArcType &t)
{
  CQChartsUtil::testAndSet(arcType_, t, [&]() { invalidate(); } );
}

void
CQChartsAnnotationPieSliceObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->setObjectRoot(path, this);

  CQChartsAnnotationBaseObj::addProperties(model, path);

  model->addProperty(path, this, "innerRadius")->setDesc("Pie slice inner radius");
  model->addProperty(path, this, "outerRadius")->setDesc("Pie slice outer radius");
  model->addProperty(path, this, "startAngle" )->setDesc("Pie slice start angle");
  model->addProperty(path, this, "spanAngle"  )->setDesc("Pie slice span angle");
  model->addProperty(path, this, "arcType"    )->setDesc("Pie slice arc type");
}

void
CQChartsAnnotationPieSliceObj::
draw(PaintDevice *device) const
{
  auto ri = plot_->lengthPlotWidth(innerRadius());
  auto ro = plot_->lengthPlotWidth(outerRadius());

  Point c(data_.point.p());

  //---

  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  path_ = QPainterPath();

  if (arcType() == ArcType::SLICE) { // works for zero innter radius ?
    auto a2 = startAngle() + spanAngle();

    CQChartsDrawUtil::pieSlicePath(path_, c, ri, ro, startAngle(), a2, false, false);

    if (CQChartsAngle::isCircle(startAngle(), a2))
      CQChartsDrawUtil::drawPieSlice(device, c, ri, ro, startAngle(), a2, false, false);
    else
      device->drawPath(path_);
  }
  else if (arcType() == ArcType::SEGMENT) {
    BBox ibbox(c.x - ri, c.y - ri, c.x + ri, c.x + ri);
    BBox obbox(c.x - ro, c.y - ro, c.x + ro, c.x + ro);

    CQChartsDrawUtil::arcSegmentPath(path_, ibbox, obbox, startAngle(), spanAngle());

    device->drawPath(path_);
  }
  else if (arcType() == ArcType::ARC) {
    BBox bbox(c.x - ro, c.y - ro, c.x + ro, c.x + ro);

    CQChartsDrawUtil::arcPath(path_, bbox, startAngle(), spanAngle());

    device->drawPath(path_);
  }

  //---

  device->resetColorNames();
}

CQChartsGeom::BBox
CQChartsAnnotationPieSliceObj::
displayRect() const
{
  return BBox(path_.boundingRect());
}

//---

CQChartsAnnotationKeyObj::
CQChartsAnnotationKeyObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                         const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
  key_ = new CQChartsPlotKey(plot());
}

void
CQChartsAnnotationKeyObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  key_->draw(device);
}

//---

CQChartsAnnotationPointSetObj::
CQChartsAnnotationPointSetObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                              const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
  hull_ = new Hull;

  hull_->clear();

  const auto &poly = data_.polygon.polygon();

  int np = poly.size();

  for (int i = 0; i < np; ++i)
    hull_->addPoint(poly.point(i));
}

void
CQChartsAnnotationPointSetObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  hull_->draw(device);
}

//---

CQChartsAnnotationValueSetObj::
CQChartsAnnotationValueSetObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                              const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
  valueList_.fromString(data.data);
}

void
CQChartsAnnotationValueSetObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  auto *th = const_cast<CQChartsAnnotationValueSetObj *>(this);

  bool isDensity  = (style_.drawType() == "whisker-bar");
  bool isVertical = (style_.direction() == "vertical");

  if (isDensity && ! density_)
    th->density_ = new Density;

  std::vector<double> values;

  auto n = valueList_.numValues();

  values.resize(n);

  bool ok;

  for (uint i = 0; i < n; ++i)
    values[i] = valueList_.valueOr(i).toReal(&ok);

  if (isDensity)
    density_->setXVals(values);

  if (style_.drawType() == "whisker-bar") {
    CQChartsDensity::DrawData drawData;

    if (isVertical)
      density_->setOrientation(Qt::Vertical);

    drawData.scaled = true;

    density_->draw(plot(), device, rect(), drawData);
  }
  else {
    CQChartsOptReal maxValue;

    QString paletteName("moreland");

    CQChartsDrawUtil::drawBarChart(device, rect(), values, maxValue,
                                   paletteName, penBrush);
  }

  //---

  device->resetColorNames();
}

//---

CQChartsAnnotationPlotObj::
CQChartsAnnotationPlotObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                          const ObjData &data, const ColorInd &iv) :
 CQChartsAnnotationBaseObj(annotationPlot, rect, data, iv)
{
}

void
CQChartsAnnotationPlotObj::
draw(PaintDevice *) const
{
}

//---

CQChartsAnnotationBaseObj::
CQChartsAnnotationBaseObj(const AnnotationPlot *annotationPlot, const BBox &rect,
                          const ObjData &data, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<AnnotationPlot *>(annotationPlot), rect, ColorInd(), ColorInd(), iv),
 annotationPlot_(annotationPlot), data_(data)
{
  setDetailHint(DetailHint::MAJOR);

  setModelInd(data.ind);

  style_.setPlot(const_cast<AnnotationPlot *>(annotationPlot));
  style_.setValue(data.style.toString());

  //---

  if (data.idList.length() > 1) {
    auto parentId = data.idList[data.idList.length() - 2];

    parent_ = annotationPlot->findObject(parentId);

    if (parent_)
      parent_->addChild(this);
  }

  //---

  if (style_.tip() != "")
    setTipId(style_.tip());
}

QString
CQChartsAnnotationBaseObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(iv().i);
}

QString
CQChartsAnnotationBaseObj::
calcTipId() const
{
  return calcId();
}

void
CQChartsAnnotationBaseObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  CQChartsPlotObj::addProperties(model, path);
}

void
CQChartsAnnotationBaseObj::
addChild(CQChartsAnnotationBaseObj *child)
{
  children_.push_back(child);
}

void
CQChartsAnnotationBaseObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  penBrush.pen   = style_.pen();
  penBrush.brush = style_.brush();

  if (updateState)
    annotationPlot_->updateObjPenBrushState(this, penBrush);
}

CQChartsGeom::BBox
CQChartsAnnotationBaseObj::
displayRect() const
{
  return rect();
}

void
CQChartsAnnotationBaseObj::
invalidate()
{
  plot_->drawObjs();
}

//---

CQChartsAnnotationPlotCustomControls::
CQChartsAnnotationPlotCustomControls(CQCharts *charts) :
 CQChartsPlotCustomControls(charts, "annotation")
{
}

void
CQChartsAnnotationPlotCustomControls::
init()
{
  connectSlots(true);
}

void
CQChartsAnnotationPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_ && annotationPlot_)
    disconnect(annotationPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  annotationPlot_ = dynamic_cast<CQChartsAnnotationPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);

#ifdef CQCHARTS_MODULE_SHLIB
  addModuleWidgets();
#endif

  if (annotationPlot_)
    connect(annotationPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  addLayoutStretch();
}
