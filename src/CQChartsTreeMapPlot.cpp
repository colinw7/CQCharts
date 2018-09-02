#include <CQChartsTreeMapPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsTip.h>

#include <QMenu>
#include <QPainter>

CQChartsTreeMapPlotType::
CQChartsTreeMapPlotType()
{
}

void
CQChartsTreeMapPlotType::
addParameters()
{
  CQChartsHierPlotType::addParameters();
}

QString
CQChartsTreeMapPlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draw hierarchical data values using sized boxes.</p>\n";
}

CQChartsPlot *
CQChartsTreeMapPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsTreeMapPlot(view, model);
}

//------

CQChartsTreeMapPlot::
CQChartsTreeMapPlot(CQChartsView *view, const ModelP &model) :
 CQChartsHierPlot(view, view->charts()->plotType("treemap"), model),
 CQChartsPlotHeaderShapeData<CQChartsTreeMapPlot>(this),
 CQChartsPlotHeaderTextData <CQChartsTreeMapPlot>(this),
 CQChartsPlotShapeData      <CQChartsTreeMapPlot>(this),
 CQChartsPlotTextData       <CQChartsTreeMapPlot>(this)
{
  setHeaderFillColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.4));
  setHeaderTextColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 1));

  setHeaderTextFontSize(12.0);
  setHeaderTextAlign(Qt::AlignLeft | Qt::AlignVCenter);

  setHeaderBorder(true);
  setHeaderFilled(true);

  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));
  setTextColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 1));

  setBorder(true);
  setFilled(true);

  setTextFontSize(14.0);
  setTextAlign(Qt::AlignHCenter | Qt::AlignVCenter);

  setTextContrast(true);

  setMargins(1, 1, 1, 1);

  addTitle();
}

CQChartsTreeMapPlot::
~CQChartsTreeMapPlot()
{
  delete root_;
}

//----

void
CQChartsTreeMapPlot::
setTitles(bool b)
{
  CQChartsUtil::testAndSet(titles_, b, [&]() { updateCurrentRoot(); } );
}

void
CQChartsTreeMapPlot::
setTitleMaxExtent(double r)
{
  CQChartsUtil::testAndSet(titleMaxExtent_, r, [&]() { invalidateLayers(); } );
}

void
CQChartsTreeMapPlot::
setHeaderHeight(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(headerHeight_, l, [&]() { updateCurrentRoot(); } );
}

//----

double
CQChartsTreeMapPlot::
calcHeaderHeight() const
{
  QFont font = view()->plotFont(this, headerTextFont());

  QFontMetricsF fm(font);

  double hh = lengthPixelHeight(headerHeight());

  return std::max(hh, fm.height() + 4);
}

//----

void
CQChartsTreeMapPlot::
setMarginWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(marginWidth_, l, [&]() { updateCurrentRoot(); } );
}

//----

void
CQChartsTreeMapPlot::
setHeaderTextFontSize(double s)
{
  if (s != headerTextData_.font.pointSizeF()) {
    headerTextData_.font.setPointSizeF(s);

    invalidateLayers();
  }
}

//----

void
CQChartsTreeMapPlot::
setTextFontSize(double s)
{
  if (s != textData_.font.pointSizeF()) {
    textData_.font.setPointSizeF(s);

    invalidateLayers();
  }
}

//---

void
CQChartsTreeMapPlot::
addProperties()
{
  CQChartsHierPlot::addProperties();

  addProperty("options", this, "marginWidth");

  addProperty("header", this, "titles"        , "visible"  );
  addProperty("header", this, "titleMaxExtent", "maxExtent");
  addProperty("header", this, "headerHeight"  , "height"   );

  addProperty("header/stroke", this, "headerBorder", "visible");

  addLineProperties("header/stroke", "headerBorder");

  addProperty("header/fill", this, "headerFilled", "visible");

  addFillProperties("header/fill", "headerFill");

  addTextProperties("header/text", "headerText");

  addProperty("header/text", this, "headerTextAlign", "align");

  addProperty("stroke", this, "border", "visible");

  addLineProperties("stroke", "border");

  addProperty("fill", this, "filled", "visible");

  addFillProperties("fill", "fill");

  addTextProperties("text", "text");

  addProperty("text", this, "textAlign"    , "align"    );
  addProperty("text", this, "textFormatted", "formatted");
  addProperty("text", this, "textScaled"   , "scaled"   );
}

//------

CQChartsTreeMapHierNode *
CQChartsTreeMapPlot::
currentRoot() const
{
  QStringList names = currentRootName_.split(separator(), QString::SkipEmptyParts);

  if (names.empty())
    return firstHier();

  CQChartsTreeMapHierNode *currentRoot = root();

  if (! currentRoot)
    return nullptr;

  for (int i = 0; i < names.size(); ++i) {
    CQChartsTreeMapHierNode *hier = childHierNode(currentRoot, names[i]);

    if (! hier)
      return currentRoot;

    currentRoot = hier;
  }

  return currentRoot;
}

void
CQChartsTreeMapPlot::
setCurrentRoot(CQChartsTreeMapHierNode *hier, bool update)
{
  if (hier)
    currentRootName_ = hier->hierName();
  else
    currentRootName_ = "";

  if (update)
    updateCurrentRoot();
}

void
CQChartsTreeMapPlot::
updateCurrentRoot()
{
  replaceNodes();

  updateObjs();
}

void
CQChartsTreeMapPlot::
calcRange()
{
  double r = 1.0;

  dataRange_.reset();

  dataRange_.updateRange(-r, -r);
  dataRange_.updateRange( r,  r);

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange_.equalScale(aspect);
  }
}

//------

void
CQChartsTreeMapPlot::
updateObjs()
{
  clearValueSets();

  resetNodes();

  CQChartsPlot::updateObjs();
}

bool
CQChartsTreeMapPlot::
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

  // init value sets
  initValueSets();

  //---

  if (! root_)
    initNodes();

  //---

  initColorIds();

  colorNodes(firstHier());

  //---

  if (currentRoot())
    initNodeObjs(currentRoot(), nullptr, 0);

  //---

  return true;
}

void
CQChartsTreeMapPlot::
initNodeObjs(CQChartsTreeMapHierNode *hier, CQChartsTreeMapHierObj *parentObj, int depth)
{
  CQChartsTreeMapHierObj *hierObj = 0;

  if (hier != root_) {
    CQChartsGeom::BBox rect(hier->x(), hier->y(), hier->x() + hier->w(), hier->y() + hier->h());

    hierObj = new CQChartsTreeMapHierObj(this, hier, parentObj, rect, hier->depth(), maxDepth());

    addPlotObject(hierObj);
  }

  //---

  for (auto &hierNode : hier->getChildren()) {
    initNodeObjs(hierNode, hierObj, depth + 1);
  }

  //---

  for (auto &node : hier->getNodes()) {
    if (! node->placed()) continue;

    //---

    CQChartsGeom::BBox rect(node->x(), node->y(), node->x() + node->w(), node->y() + node->h());

    CQChartsTreeMapObj *obj =
      new CQChartsTreeMapObj(this, node, parentObj, rect, node->depth(), maxDepth());

    addPlotObject(obj);
  }
}

void
CQChartsTreeMapPlot::
resetNodes()
{
  delete root_;

  root_      = nullptr;
  firstHier_ = nullptr;
}

void
CQChartsTreeMapPlot::
initNodes()
{
  hierInd_ = 0;

  root_ = new CQChartsTreeMapHierNode(this, nullptr, "<root>");

  root_->setDepth(0);
  root_->setHierInd(hierInd_++);

  //---

  if (isHierarchical())
    loadHier();
  else
    loadFlat();

  //---

  firstHier_ = root_;

  while (firstHier_ && firstHier_->numChildren() == 1)
    firstHier_ = firstHier_->childAt(0);

  //---

  replaceNodes();
}

void
CQChartsTreeMapPlot::
replaceNodes()
{
  windowHeaderHeight_ = pixelToWindowHeight(calcHeaderHeight());
  windowMarginWidth_  = lengthPixelWidth   (marginWidth());

  CQChartsTreeMapHierNode *hier = currentRoot();

  if (hier)
    placeNodes(hier);
}

void
CQChartsTreeMapPlot::
placeNodes(CQChartsTreeMapHierNode *hier)
{
  hier->setPosition(-1, -1, 2, 2);
}

void
CQChartsTreeMapPlot::
colorNodes(CQChartsTreeMapHierNode *hier)
{
  if (! hier->hasNodes() && ! hier->hasChildren()) {
    colorNode(hier);
  }
  else {
    for (const auto &node : hier->getNodes())
      colorNode(node);

    for (const auto &child : hier->getChildren())
      colorNodes(child);
  }
}

void
CQChartsTreeMapPlot::
colorNode(CQChartsTreeMapNode *node)
{
  if (! node->color().isValid())
    node->setColorId(nextColorId());
}

void
CQChartsTreeMapPlot::
loadHier()
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *root) :
     plot_(plot) {
      hierStack_.push_back(root);

      valueColumnType_ = plot_->columnValueType(plot_->valueColumn());
    }

    State hierVisit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      QString     name;
      QModelIndex nameInd;

      (void) getName(parent, row, name, nameInd);

      //---

      CQChartsTreeMapHierNode *hier = plot_->addHierNode(parentHier(), name, nameInd);

      //---

      hierStack_.push_back(hier);

      return State::OK;
    }

    State hierPostVisit(QAbstractItemModel *, const QModelIndex &, int) override {
      hierStack_.pop_back();

      assert(! hierStack_.empty());

      return State::OK;
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      QString     name;
      QModelIndex nameInd;

      (void) getName(parent, row, name, nameInd);

      //---

      double size = 1.0;

      if (! getSize(parent, row, size))
        return State::SKIP;

      //---

      CQChartsTreeMapNode *node = plot_->addNode(parentHier(), name, size, nameInd);

      if (node) {
        CQChartsColor color;

        if (plot_->colorSetColor("color", row, color))
          node->setColor(color);
      }

      return State::OK;
    }

   private:
    CQChartsTreeMapHierNode *parentHier() const {
      assert(! hierStack_.empty());

      return hierStack_.back();
    }

    bool getName(const QModelIndex &parent, int row, QString &name, QModelIndex &nameInd) const {
      nameInd = plot_->modelIndex(row, plot_->nameColumn(), parent);

      bool ok;

      name = plot_->modelString(row, plot_->nameColumn(), parent, ok);

      return ok;
    }

    bool getSize(const QModelIndex &parent, int row, double &size) const {
      size = 1.0;

      if (! plot_->valueColumn().isValid())
        return true;

      bool ok = true;

      if      (valueColumnType_ == ColumnType::REAL)
        size = plot_->modelReal(row, plot_->valueColumn(), parent, ok);
      else if (valueColumnType_ == ColumnType::INTEGER)
        size = plot_->modelInteger(row, plot_->valueColumn(), parent, ok);
      else
        ok = false;

      if (ok && size <= 0.0)
        ok = false;

      return ok;
    }

   private:
    using HierStack = std::vector<CQChartsTreeMapHierNode *>;

    CQChartsTreeMapPlot *plot_            { nullptr };
    ColumnType           valueColumnType_ { ColumnType::NONE };
    HierStack            hierStack_;
  };

  RowVisitor visitor(this, root_);

  visitModel(visitor);
}

CQChartsTreeMapHierNode *
CQChartsTreeMapPlot::
addHierNode(CQChartsTreeMapHierNode *hier, const QString &name, const QModelIndex &nameInd)
{
  int depth1 = hier->depth() + 1;

  QModelIndex nameInd1 = normalizeIndex(nameInd);

  CQChartsTreeMapHierNode *hier1 = new CQChartsTreeMapHierNode(this, hier, name, nameInd1);

  hier1->setDepth(depth1);

  hier1->setHierInd(hierInd_++);

  maxDepth_ = std::max(maxDepth_, depth1);

  return hier1;
}

CQChartsTreeMapNode *
CQChartsTreeMapPlot::
addNode(CQChartsTreeMapHierNode *hier, const QString &name, double size,
        const QModelIndex &nameInd)
{
  int depth1 = hier->depth() + 1;

  QModelIndex nameInd1 = normalizeIndex(nameInd);

  CQChartsTreeMapNode *node = new CQChartsTreeMapNode(this, hier, name, size, nameInd1);

  node->setDepth(depth1);

  hier->addNode(node);

  maxDepth_ = std::max(maxDepth_, depth1);

  return node;
}

void
CQChartsTreeMapPlot::
loadFlat()
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsTreeMapPlot *plot) :
     plot_(plot) {
      valueColumnType_ = plot_->columnValueType(plot_->valueColumn());
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      QStringList  nameStrs;
      ModelIndices nameInds;

      if (! plot_->getHierColumnNames(parent, row, plot_->nameColumns(), plot_->separator(),
                                      nameStrs, nameInds))
        return State::SKIP;

      QModelIndex nameInd1 = plot_->normalizeIndex(nameInds[0]);

      //---

      double size = 1.0;

      if (plot_->valueColumn().isValid()) {
        bool ok2 = true;

        if      (valueColumnType_ == ColumnType::REAL)
          size = plot_->modelReal(row, plot_->valueColumn(), parent, ok2);
        else if (valueColumnType_ == ColumnType::INTEGER)
          size = plot_->modelInteger(row, plot_->valueColumn(), parent, ok2);
        else
          ok2 = false;

        if (ok2 && size <= 0.0)
          ok2 = false;

        if (! ok2)
          return State::SKIP;
      }

      //---

      CQChartsTreeMapNode *node = plot_->addNode(nameStrs, size, nameInd1);

      if (node) {
        CQChartsColor color;

        if (plot_->colorSetColor("color", row, color))
          node->setColor(color);
      }

      return State::OK;
    }

   private:
    CQChartsTreeMapPlot *plot_            { nullptr };
    ColumnType           valueColumnType_ { ColumnType::NONE };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  addExtraNodes(root());
}

CQChartsTreeMapNode *
CQChartsTreeMapPlot::
addNode(const QStringList &nameStrs, double size, const QModelIndex &nameInd)
{
  int depth = nameStrs.length();

  maxDepth_ = std::max(maxDepth_, depth + 1);

  //---

  CQChartsTreeMapHierNode *parent = root();

  for (int j = 0; j < nameStrs.length() - 1; ++j) {
    CQChartsTreeMapHierNode *child = childHierNode(parent, nameStrs[j]);

    if (! child) {
      // remove any existing leaf node (save size to use in new hier node)
      QModelIndex nameInd1;
      double      size1 = 0.0;

      CQChartsTreeMapNode *node = childNode(parent, nameStrs[j]);

      if (node) {
        nameInd1 = node->ind();
        size1    = node->size();

        parent->removeNode(node);

        delete node;
      }

      //---

      child = new CQChartsTreeMapHierNode(this, parent, nameStrs[j], nameInd1);

      child->setSize(size1);

      child->setDepth(depth);
      child->setHierInd(hierInd_++);
    }

    parent = child;
  }

  //---

  QString name = nameStrs[nameStrs.length() - 1];

  CQChartsTreeMapNode *node = childNode(parent, name);

  if (! node) {
    // use hier node if already created
    CQChartsTreeMapHierNode *child = childHierNode(parent, name);

    if (child) {
      child->setSize(size);
      return nullptr;
    }

    //---

    node = new CQChartsTreeMapNode(this, parent, name, size, nameInd);

    node->setDepth(depth);

    parent->addNode(node);
  }

  return node;
}

void
CQChartsTreeMapPlot::
addExtraNodes(CQChartsTreeMapHierNode *hier)
{
  if (hier->size() > 0) {
    CQChartsTreeMapNode *node =
      new CQChartsTreeMapNode(this, hier, "", hier->size(), hier->ind());

    QModelIndex ind1 = unnormalizeIndex(hier->ind());

    CQChartsColor color;

    if (colorSetColor("color", ind1.row(), color))
      node->setColor(color);

    node->setDepth (hier->depth() + 1);
    node->setFiller(true);

    hier->addNode(node);

    hier->setSize(0.0);
  }

  for (const auto &child : hier->getChildren())
    addExtraNodes(child);
}

CQChartsTreeMapHierNode *
CQChartsTreeMapPlot::
childHierNode(CQChartsTreeMapHierNode *parent, const QString &name) const
{
  for (const auto &child : parent->getChildren())
    if (child->name() == name)
      return child;

  return nullptr;
}

CQChartsTreeMapNode *
CQChartsTreeMapPlot::
childNode(CQChartsTreeMapHierNode *parent, const QString &name) const
{
  for (const auto &node : parent->getNodes())
    if (node->name() == name)
      return node;

  return nullptr;
}

//------

bool
CQChartsTreeMapPlot::
addMenuItems(QMenu *menu)
{
  PlotObjs objs;

  selectedPlotObjs(objs);

  QAction *pushAction   = new QAction("Push"   , menu);
  QAction *popAction    = new QAction("Pop"    , menu);
  QAction *popTopAction = new QAction("Pop Top", menu);

  connect(pushAction  , SIGNAL(triggered()), this, SLOT(pushSlot()));
  connect(popAction   , SIGNAL(triggered()), this, SLOT(popSlot()));
  connect(popTopAction, SIGNAL(triggered()), this, SLOT(popTopSlot()));

  pushAction  ->setEnabled(! objs.empty());
  popAction   ->setEnabled(currentRoot() != firstHier());
  popTopAction->setEnabled(currentRoot() != firstHier());

  menu->addSeparator();

  menu->addAction(pushAction  );
  menu->addAction(popAction   );
  menu->addAction(popTopAction);

  return true;
}

void
CQChartsTreeMapPlot::
pushSlot()
{
  PlotObjs objs;

  selectedPlotObjs(objs);

  if (objs.empty()) {
    QPointF gpos = view()->menuPos();

    QPointF pos = view()->mapFromGlobal(QPoint(gpos.x(), gpos.y()));

    CQChartsGeom::Point w;

    pixelToWindow(CQChartsUtil::fromQPoint(pos), w);

    plotObjsAtPoint(w, objs);
  }

  if (objs.empty())
    return;

  for (const auto &obj : objs) {
    CQChartsTreeMapHierObj *hierObj = dynamic_cast<CQChartsTreeMapHierObj *>(obj);

    if (hierObj) {
      CQChartsTreeMapHierNode *hnode = hierObj->hierNode();

      setCurrentRoot(hnode, /*update*/true);

      break;
    }

    CQChartsTreeMapObj *nodeObj = dynamic_cast<CQChartsTreeMapObj *>(obj);

    if (nodeObj) {
      CQChartsTreeMapNode *node = nodeObj->node();

      CQChartsTreeMapHierNode *hnode = node->parent();

      if (hnode) {
        setCurrentRoot(hnode, /*update*/true);

        break;
      }
    }
  }
}

void
CQChartsTreeMapPlot::
popSlot()
{
  CQChartsTreeMapHierNode *root = currentRoot();

  if (root && root->parent()) {
    setCurrentRoot(root->parent(), /*update*/true);
  }
}

void
CQChartsTreeMapPlot::
popTopSlot()
{
  CQChartsTreeMapHierNode *root = currentRoot();

  if (root != firstHier()) {
    setCurrentRoot(firstHier(), /*update*/true);
  }
}

//------

void
CQChartsTreeMapPlot::
handleResize()
{
  CQChartsPlot::handleResize();

  replaceNodes();

  updateObjs();
}

//------

CQChartsTreeMapHierObj::
CQChartsTreeMapHierObj(CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *hier,
                       CQChartsTreeMapHierObj *hierObj, const CQChartsGeom::BBox &rect,
                       int i, int n) :
 CQChartsTreeMapObj(plot, hier, hierObj, rect, i, n), hier_(hier)
{
}

QString
CQChartsTreeMapHierObj::
calcId() const
{
  //return QString("%1:%2").arg(hier_->name()).arg(hier_->hierSize());
  return CQChartsTreeMapObj::calcId();
}

QString
CQChartsTreeMapHierObj::
calcTipId() const
{
  //return QString("%1:%2").arg(hier_->hierName()).arg(hier_->hierSize());
  return CQChartsTreeMapObj::calcTipId();
}

bool
CQChartsTreeMapHierObj::
inside(const CQChartsGeom::Point &p) const
{
  CQChartsGeom::BBox bbox(hier_->x(), hier_->y(), hier_->x() + hier_->w(), hier_->y() + hier_->h());

  if (bbox.inside(p))
    return true;

  return false;
}

void
CQChartsTreeMapHierObj::
getSelectIndices(Indices &inds) const
{
  return addColumnSelectIndex(inds, plot_->valueColumn());
}

void
CQChartsTreeMapHierObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    const QModelIndex &ind = hier_->ind();

    addSelectIndex(inds, ind.row(), column, ind.parent());
  }
}

void
CQChartsTreeMapHierObj::
draw(QPainter *painter)
{
  double px1, py1, px2, py2;

  plot_->windowToPixel(hier_->x()             , hier_->y()             , px1, py1);
  plot_->windowToPixel(hier_->x() + hier_->w(), hier_->y() + hier_->h(), px2, py2);

  QRectF qrect = CQChartsUtil::toQRect(CQChartsGeom::BBox(px1, py2, px2, py1));

  //---

  // calc header stroke and brush
  QPen   pen;
  QBrush brush;

  QColor bc = plot_->interpHeaderBorderColor(0, 1);

  QColor hierColor = hier_->interpColor(plot_, plot_->numColorIds());

  QColor c = plot_->interpHeaderFillColor(0, 1);

  QColor fc = CQChartsUtil::blendColors(c, hierColor, 0.8);

  plot_->setPenBrush(pen, brush,
                     plot_->isHeaderBorder(), bc, plot_->headerBorderAlpha(),
                     plot_->headerBorderWidth(), plot_->headerBorderDash(),
                     plot_->isHeaderFilled(), fc, plot_->headerFillAlpha(),
                     plot_->headerFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  QPen tpen;

  QColor tc = plot_->interpHeaderTextColor(0, 1);

  plot_->setPen(tpen, true, tc, plot_->headerTextAlpha(),
                CQChartsLength("0px"), CQChartsLineDash());

  plot_->updateObjPenBrushState(this, tpen, brush);

  //---

  painter->save();

  //---

  // draw rectangle
  painter->setPen  (pen);
  painter->setBrush(brush);

  painter->drawRect(qrect);

  //---

  // set font
  plot_->view()->setPlotPainterFont(plot_, painter, plot_->headerTextFont());

  //---

  // calc text size and position
  QFontMetricsF fm(painter->font());

  QString name = hier_->name();

  plot_->windowToPixel(hier_->x(), hier_->y() + hier_->h(), px1, py1);

  //---

  double hh = plot_->calcHeaderHeight();

  //---

  // draw label
  double tx = px1 + 4;
  double ty = py1 + hh/2 + (fm.ascent() - fm.descent())/2;

  painter->setClipRect(qrect);

  if (plot_->isHeaderTextContrast())
    plot_->drawContrastText(painter, tx, ty, name, tpen);
  else {
    painter->setPen(tpen);

    painter->drawText(tx, ty, name);
  }

  //---

  painter->restore();
}

//------

CQChartsTreeMapObj::
CQChartsTreeMapObj(CQChartsTreeMapPlot *plot, CQChartsTreeMapNode *node,
                   CQChartsTreeMapHierObj *hierObj, const CQChartsGeom::BBox &rect, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), node_(node), hierObj_(hierObj), i_(i), n_(n)
{
}

QString
CQChartsTreeMapObj::
calcId() const
{
  if (node_->isFiller())
    return hierObj_->calcId();

  QModelIndex ind1 = plot_->unnormalizeIndex(node_->ind());

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  return QString("cell:%1:%2").arg(node_->name()).arg(node_->hierSize());
}

QString
CQChartsTreeMapObj::
calcTipId() const
{
  if (node_->isFiller())
    return hierObj_->calcTipId();

  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", node_->hierName());
  tableTip.addTableRow("Size", node_->hierSize());

  if (plot_->colorColumn().isValid()) {
    QModelIndex ind1 = plot_->unnormalizeIndex(node_->ind());

    bool ok;

    QString colorStr = plot_->modelString(ind1.row(), plot_->colorColumn(), ind1.parent(), ok);

    tableTip.addTableRow("Color", colorStr);
  }

  return tableTip.str();
}

bool
CQChartsTreeMapObj::
inside(const CQChartsGeom::Point &p) const
{
  CQChartsGeom::BBox bbox(node_->x(), node_->y(), node_->x() + node_->w(), node_->y() + node_->h());

  if (bbox.inside(p))
    return true;

  return false;
}

void
CQChartsTreeMapObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->nameColumn ());
  addColumnSelectIndex(inds, plot_->valueColumn());
  addColumnSelectIndex(inds, plot_->colorColumn());
}

void
CQChartsTreeMapObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    const QModelIndex &ind = node_->ind();

    addSelectIndex(inds, ind.row(), column, ind.parent());
  }
}

void
CQChartsTreeMapObj::
draw(QPainter *painter)
{
  double px1, py1, px2, py2;

  plot_->windowToPixel(node_->x()             , node_->y()             , px1, py1);
  plot_->windowToPixel(node_->x() + node_->w(), node_->y() + node_->h(), px2, py2);

  QRectF qrect = CQChartsUtil::toQRect(CQChartsGeom::BBox(px1 + 1, py2 + 1, px2 - 1, py1 - 1));

  //---

  // calc stroke and brush
  QPen   pen;
  QBrush brush;

  plot_->setPenBrush(pen, brush,
                     plot_->isBorder(), plot_->interpBorderColor(0, 1), plot_->borderAlpha(),
                     plot_->borderWidth(), plot_->borderDash(),
                     plot_->isFilled(), node_->interpColor(plot_, plot_->numColorIds()),
                     plot_->fillAlpha(), plot_->fillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  QPen tpen;

  QColor tc = plot_->interpTextColor(0, 1);

  plot_->setPen(tpen, true, tc, plot_->textAlpha(),
                CQChartsLength("0px"), CQChartsLineDash());

  plot_->updateObjPenBrushState(this, tpen, brush);

  //---

  painter->save();

  //---

  // draw rectangle
  painter->setPen  (pen);
  painter->setBrush(brush);

  painter->drawRect(qrect);

  //---

  // set font
  plot_->view()->setPlotPainterFont(plot_, painter, plot_->textFont());

  //---

  // calc text size and position
  QFontMetricsF fm(painter->font());

  QString name = (! node_->isFiller() ? node_->name() : node_->parent()->name());

  //---

  // draw label
  CQChartsTextOptions textOptions;

  textOptions.contrast  = plot_->isTextContrast();
  textOptions.formatted = plot_->isTextFormatted();
  textOptions.scaled    = plot_->isTextScaled();
  textOptions.align     = plot_->textAlign();

  plot_->drawTextInBox(painter, qrect, name, tpen, textOptions);

  //---

  painter->restore();
}

//------

CQChartsTreeMapHierNode::
CQChartsTreeMapHierNode(CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *parent,
                        const QString &name, const QModelIndex &ind) :
 CQChartsTreeMapNode(plot, parent, name, 0.0, ind)
{
  if (parent_)
    parent_->children_.push_back(this);
}

CQChartsTreeMapHierNode::
~CQChartsTreeMapHierNode()
{
  for (auto &child : children_)
    delete child;

  for (auto &node : nodes_)
    delete node;
}

double
CQChartsTreeMapHierNode::
hierSize() const
{
  double s = size();

  for (auto &child : children_)
    s += child->hierSize();

  for (auto &node : nodes_)
    s += node->hierSize();

  return s;
}

void
CQChartsTreeMapHierNode::
packNodes(double x, double y, double w, double h)
{
  double whh = plot()->windowHeaderHeight();
  double wmw = plot()->windowMarginWidth();

  double maxExtent = CMathUtil::clamp(plot()->titleMaxExtent(), 0.0, 1.0);

  bool showTitle = (plot()->isTitles() && h*maxExtent > whh);

  if (! parent())
    showTitle = false;

  double dh = (showTitle ? whh : 0.0);
  double m  = (w > wmw ? wmw : 0.0);

  // make single list of nodes to pack
  Nodes nodes;

  for (const auto &child : children_)
    nodes.push_back(child);

  for (const auto &node : nodes_)
    nodes.push_back(node);

  // sort nodes by size
  std::sort(nodes.begin(), nodes.end(), CQChartsTreeMapNodeCmp());

  //std::cerr << name() << "\n";
  //for (uint i = 0; i < nodes.size(); ++i)
  //  std::cerr << " " << nodes[i]->name() << ":" << nodes[i]->hierSize() << "\n";

  packSubNodes(x + m/2, y + m/2, w - m, h - dh - m, nodes);
}

void
CQChartsTreeMapHierNode::
packSubNodes(double x, double y, double w, double h, const Nodes &nodes)
{
  // place nodes
  int n = nodes.size();
  if (n == 0) return;

  if (n >= 2) {
    int n1 = n/2;

    Nodes  nodes1, nodes2;
    double size1 = 0.0, size2 = 0.0;

    for (int i = 0; i < n1; ++i) {
      size1 += nodes[i]->hierSize();

      nodes1.push_back(nodes[i]);
    }

    for (int i = n1; i <  n; ++i) {
      size2 += nodes[i]->hierSize();

      nodes2.push_back(nodes[i]);
    }

    // split area = (w*h) if largest direction
    // e.g. split at w1. area1 = w1*h; area2 = (w - w1)*h;
    // area1/area2 = w1/(w - w1) = size1/size2;
    // w1*size2 = w*size1 - w1*size1;
    // w1 = (w*size1)/(size1 + size2);

    double size12 = size1 + size2;

    if (size12 == 0.0)
      return;

    double f = size1/size12;

    if (w >= h) {
      double w1 = f*w;

      packSubNodes(x     , y,     w1, h, nodes1);
      packSubNodes(x + w1, y, w - w1, h, nodes2);
    }
    else {
      double h1 = f*h;

      packSubNodes(x, y     , w, h1    , nodes1);
      packSubNodes(x, y + h1, w, h - h1, nodes2);
    }
  }
  else {
    CQChartsTreeMapNode *node = nodes[0];

    node->setPosition(x, y, w, h);
  }
}

void
CQChartsTreeMapHierNode::
setPosition(double x, double y, double w, double h)
{
  CQChartsTreeMapNode::setPosition(x, y, w, h);

  packNodes(x, y, w, h);
}

void
CQChartsTreeMapHierNode::
addNode(CQChartsTreeMapNode *node)
{
  nodes_.push_back(node);
}

void
CQChartsTreeMapHierNode::
removeNode(CQChartsTreeMapNode *node)
{
  int n = nodes_.size();

  int i = 0;

  for ( ; i < n; ++i) {
    if (nodes_[i] == node)
      break;
  }

  assert(i < n);

  ++i;

  for ( ; i < n; ++i)
    nodes_[i - 1] = nodes_[i];

  nodes_.pop_back();
}

QColor
CQChartsTreeMapHierNode::
interpColor(CQChartsTreeMapPlot *plot, int n) const
{
  using Colors = std::vector<QColor>;

  Colors colors;

  for (auto &child : children_)
    colors.push_back(child->interpColor(plot, n));

  for (auto &node : nodes_)
    colors.push_back(node->interpColor(plot, n));

  if (colors.empty())
    return plot->interpPaletteColor(0, 1);

  return CQChartsUtil::blendColors(colors);
}

//------

CQChartsTreeMapNode::
CQChartsTreeMapNode(CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *parent,
                    const QString &name, double size, const QModelIndex &ind) :
 plot_(plot), parent_(parent), id_(nextId()), name_(name), size_(size), ind_(ind)
{
}

QString
CQChartsTreeMapNode::
hierName() const
{
  if (parent() && parent() != plot_->root())
    return parent()->hierName() + "/" + name();
  else
    return name();
}

void
CQChartsTreeMapNode::
setPosition(double x, double y, double w, double h)
{
  assert(! CMathUtil::isNaN(x) && ! CMathUtil::isNaN(y) &&
         ! CMathUtil::isNaN(w) && ! CMathUtil::isNaN(h));

  x_ = x; y_ = y;
  w_ = w; h_ = h;

  //std::cerr << "Node: " << name() << " @ ( " << x_ << "," << y_ << ")" <<
  //             " [" << w_ << "," << h_ << "]" << "\n";

  placed_ = true;
}

bool
CQChartsTreeMapNode::
contains(double x, double y) const
{
  return (x >= x_ && x <= (x_ + w_) && y >= y_ && y <= (y_ + h_));
}

CQChartsTreeMapHierNode *
CQChartsTreeMapNode::
rootNode(CQChartsTreeMapHierNode *root) const
{
  CQChartsTreeMapHierNode *parent = this->parent();

  while (parent && parent->parent() && parent->parent() != root)
    parent = parent->parent();

  return parent;
}

QColor
CQChartsTreeMapNode::
interpColor(CQChartsTreeMapPlot *plot, int n) const
{
  if      (colorId() >= 0)
    return plot->interpPaletteColor(colorId(), n);
  else if (color().isValid())
    return color().interpColor(plot, 0, 1);
  else
    return plot->interpPaletteColor(0, 1);
}
