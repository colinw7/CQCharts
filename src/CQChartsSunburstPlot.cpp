#include <CQChartsSunburstPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRotatedText.h>
#include <CQChartsTip.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QMenu>

//---

CQChartsSunburstPlotType::
CQChartsSunburstPlotType()
{
}

void
CQChartsSunburstPlotType::
addParameters()
{
  CQChartsHierPlotType::addParameters();
}

QString
CQChartsSunburstPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Sunburst Plot").
    h3("Summary").
     p("Draw hierarchical data as segments of concentric circles.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/sunburst.png"));
}

CQChartsPlot *
CQChartsSunburstPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsSunburstPlot(view, model);
}

//---

CQChartsSunburstPlot::
CQChartsSunburstPlot(CQChartsView *view, const ModelP &model) :
 CQChartsHierPlot(view, view->charts()->plotType("sunburst"), model),
 CQChartsObjShapeData<CQChartsSunburstPlot>(this),
 CQChartsObjTextData <CQChartsSunburstPlot>(this)
{
  NoUpdate noUpdate(this);

  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setFilled (true);
  setStroked(true);

  setTextFontSize(8.0);

  setTextColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 1));

  setOuterMargin(CQChartsLength("4px"), CQChartsLength("4px"),
                 CQChartsLength("4px"), CQChartsLength("4px"));

  // addKey() // TODO

  addTitle();
}

CQChartsSunburstPlot::
~CQChartsSunburstPlot()
{
  resetRoots();
}

void
CQChartsSunburstPlot::
resetRoots()
{
  for (auto &root : roots_)
    delete root;

  roots_.clear();
}

//----

void
CQChartsSunburstPlot::
setInnerRadius(double r)
{
  CQChartsUtil::testAndSet(innerRadius_, r, [&]() { resetRoots(); updateObjs(); } );
}

void
CQChartsSunburstPlot::
setOuterRadius(double r)
{
  CQChartsUtil::testAndSet(outerRadius_, r, [&]() { resetRoots(); updateObjs(); } );
}

void
CQChartsSunburstPlot::
setStartAngle(double a)
{
  CQChartsUtil::testAndSet(startAngle_, a, [&]() { resetRoots(); updateObjs(); } );
}

void
CQChartsSunburstPlot::
setMultiRoot(bool b)
{
  CQChartsUtil::testAndSet(multiRoot_, b, [&]() { resetRoots(); updateObjs(); } );
}

//----

void
CQChartsSunburstPlot::
setTextFontSize(double s)
{
  if (s != textData_.font().pointSizeF()) {
    CQChartsFont f = textData_.font(); f.setPointSizeF(s); textData_.setFont(f);

    drawObjs();
  }
}

//---

void
CQChartsSunburstPlot::
setColorById(bool b)
{
  CQChartsUtil::testAndSet(colorById_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsSunburstPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  CQChartsHierPlot::addProperties();

  // columns
  addProp("columns", "nameColumns", "names", "Name columns");
  addProp("columns", "valueColumn", "value", "Value columns");

  // options
  addProp("options", "separator"  , "", "Name separator");
  addProp("options", "innerRadius", "", "Inner radius");
  addProp("options", "outerRadius", "", "Outer radius");
  addProp("options", "startAngle" , "", "Angle for first segment");
  addProp("options", "multiRoot"  , "", "Support multiple roots");

  // coloring
  addProp("coloring", "colorById", "colorById", "Color by id");

  // fill
  addProp("fill", "filled", "visible", "Fill visible");

  addFillProperties("fill", "fill", "");

  // stroke
  addProp("stroke", "stroked", "visible", "Stroke visible");

  addLineProperties("stroke", "stroke", "");

  // text
  addTextProperties("text", "text", "");

  // color map
  addColorMapProperties();
}

//------

CQChartsSunburstHierNode *
CQChartsSunburstPlot::
currentRoot() const
{
  CQChartsSunburstHierNode *currentRoot = nullptr;

  QStringList names = currentRootName_.split('@', QString::SkipEmptyParts);

  if (names.empty())
    return currentRoot;

  for (int i = 0; i < names.size(); ++i) {
    CQChartsSunburstHierNode *hier = childHierNode(currentRoot, names[i]);

    if (! hier)
      return currentRoot;

    currentRoot = hier;
  }

  return currentRoot;
}

void
CQChartsSunburstPlot::
setCurrentRoot(CQChartsSunburstHierNode *hier, bool update)
{
  if (hier)
    currentRootName_ = hier->hierName('@');
  else
    currentRootName_ = "";

  if (update) {
    replaceRoots();

    updateObjs();
  }
}

CQChartsGeom::Range
CQChartsSunburstPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsSunburstPlot::calcRange");

  CQChartsGeom::Range dataRange;

  double r = 1.0;

  dataRange.updateRange(-r, -r);
  dataRange.updateRange( r,  r);

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
  }

  return dataRange;
}

//------

void
CQChartsSunburstPlot::
clearPlotObjects()
{
  resetRoots();

  CQChartsPlot::clearPlotObjects();
}

bool
CQChartsSunburstPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsSunburstPlot::createObjs");

  NoUpdate noUpdate(this);

  CQChartsSunburstPlot *th = const_cast<CQChartsSunburstPlot *>(this);

  //---

  // init value sets
  //initValueSets();

  //---

  if (roots_.empty())
    th->initRoots();

  //---

  th->initColorIds();

  if (currentRoot()) {
    colorNodes(currentRoot());
  }
  else {
    for (auto &root : roots_)
      colorNodes(root);
  }

  //---

  int nr = roots_.size();

  bool isUnnamedRoot = (nr == 1 && roots_[0]->name() == "");

  if (currentRoot()) {
    ColorInd ci(0, 1);

    if (! isUnnamedRoot || roots_[0] != currentRoot())
      addPlotObj(currentRoot(), objs, ci);

    addPlotObjs(currentRoot(), objs, ci);
  }
  else {
    int ir = 0;

    for (auto &root : roots_) {
      ColorInd ci(ir, nr);

      if (! isUnnamedRoot)
        addPlotObj(root, objs, ci);

      addPlotObjs(root, objs, ci);

      ++ir;
    }
  }

  //---

  int in = 0;

  for (auto &obj : objs) {
    CQChartsSunburstNodeObj *nodeObj = dynamic_cast<CQChartsSunburstNodeObj *>(obj);

    if (nodeObj) { nodeObj->setInd(in); ++in; }
  }

  for (auto &obj : objs) {
    CQChartsSunburstNodeObj *nodeObj = dynamic_cast<CQChartsSunburstNodeObj *>(obj);

    if (nodeObj)
      nodeObj->setIv(ColorInd(nodeObj->ind(), in));
  }

  return true;
}

void
CQChartsSunburstPlot::
initRoots()
{
  CQChartsSunburstRootNode *root = nullptr;

  if (! isMultiRoot())
    root = createRootNode();

  if (isHierarchical())
    loadHier(root);
  else
    loadFlat(root);

  //---

  replaceRoots();
}

void
CQChartsSunburstPlot::
replaceRoots() const
{
  double ri = std::max(innerRadius(), 0.0);
  double ro = CMathUtil::clamp(outerRadius(), ri, 1.0);

  double a = startAngle();

  if (currentRoot()) {
    double da = 360.0;

    currentRoot()->setPosition(0.0, a, ri, da);

    currentRoot()->packNodes(currentRoot(), ri, ro, 0.0, a, da,
                             CQChartsSunburstRootNode::Order::SIZE, true);
  }
  else {
    double da = (! roots_.empty() ? 360.0/roots_.size() : 0.0);

    for (auto &root : roots_) {
      root->setPosition(0.0, a, ri, da);

      root->packNodes(ri, ro, 0.0, a, da);

      a += da;
    }
  }
}

void
CQChartsSunburstPlot::
colorNodes(CQChartsSunburstHierNode *hier) const
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
CQChartsSunburstPlot::
colorNode(CQChartsSunburstNode *node) const
{
  if (! node->color().isValid()) {
    CQChartsSunburstPlot *th = const_cast<CQChartsSunburstPlot *>(this);

    node->setColorId(th->nextColorId());
  }
}

void
CQChartsSunburstPlot::
loadHier(CQChartsSunburstHierNode *root) const
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsSunburstPlot *plot, CQChartsSunburstHierNode *root) :
     plot_(plot) {
      hierStack_.push_back(root);
    }

    State hierVisit(const QAbstractItemModel *, const VisitData &data) override {
      QString     name;
      QModelIndex nameInd;

      (void) getName(data, name, nameInd);

      //---

      CQChartsSunburstHierNode *hier = plot_->addHierNode(parentHier(), name, nameInd);

      //---

      hierStack_.push_back(hier);

      return State::OK;
    }

    State hierPostVisit(const QAbstractItemModel *, const VisitData &) override {
      hierStack_.pop_back();

      assert(! hierStack_.empty());

      return State::OK;
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      QString     name;
      QModelIndex nameInd;

      (void) getName(data, name, nameInd);

      //---

      double      size = 1.0;
      QModelIndex valueInd;

      if (! getSize(data, size, valueInd))
        return State::SKIP;

      //---

      (void) plot_->addNode(parentHier(), name, size, nameInd, valueInd);

      return State::OK;
    }

   private:
    CQChartsSunburstHierNode *parentHier() const {
      assert(! hierStack_.empty());

      return hierStack_.back();
    }

    bool getName(const VisitData &data, QString &name, QModelIndex &nameInd) const {
      nameInd = plot_->modelIndex(data.row, plot_->nameColumns().column(), data.parent);

      bool ok;

      name = plot_->modelString(data.row, plot_->nameColumns().column(), data.parent, ok);

      return ok;
    }

    bool getSize(const VisitData &data, double size, QModelIndex &valueInd) const {
      valueInd = plot_->modelIndex(data.row, plot_->valueColumn(), data.parent);

      size = 1.0;

      if (! plot_->valueColumn().isValid())
        return true;

      bool ok = true;

      size = plot_->modelReal(data.row, plot_->valueColumn(), data.parent, ok);

      if (ok && size <= 0.0)
        ok = false;

      return ok;
    }

   private:
    using HierStack = std::vector<CQChartsSunburstHierNode *>;

    const CQChartsSunburstPlot* plot_ { nullptr };
    HierStack                   hierStack_;
  };

  RowVisitor visitor(this, root);

  visitModel(visitor);
}

CQChartsSunburstHierNode *
CQChartsSunburstPlot::
addHierNode(CQChartsSunburstHierNode *hier, const QString &name, const QModelIndex &nameInd) const
{
  QModelIndex nameInd1 = normalizeIndex(nameInd);

  CQChartsSunburstHierNode *hier1 = new CQChartsSunburstHierNode(this, hier, name);

  hier1->setInd(nameInd1);

  return hier1;
}

CQChartsSunburstNode *
CQChartsSunburstPlot::
addNode(CQChartsSunburstHierNode *hier, const QString &name, double size,
        const QModelIndex &nameInd, const QModelIndex &valueInd) const
{
  CQChartsSunburstNode *node = new CQChartsSunburstNode(this, hier, name);

  node->setSize(size);

  if (valueInd.isValid()) {
    QModelIndex valueInd1 = normalizeIndex(valueInd);

    node->setInd(valueInd1);
  }
  else {
    QModelIndex nameInd1 = normalizeIndex(nameInd);

    node->setInd(nameInd1);
  }

  hier->addNode(node);

  return node;
}

void
CQChartsSunburstPlot::
loadFlat(CQChartsSunburstHierNode *root) const
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsSunburstPlot *plot, CQChartsSunburstHierNode *root) :
     plot_(plot), root_(root) {
      valueColumnType_ = plot_->columnValueType(plot_->valueColumn());
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      QStringList  nameStrs;
      ModelIndices nameInds;

      if (! plot_->getHierColumnNames(data.parent, data.row, plot_->nameColumns(),
                                      plot_->separator(), nameStrs, nameInds))
        return State::SKIP;

      QModelIndex nameInd1 = plot_->normalizeIndex(nameInds[0]);

      //---

      double size = 1.0;

      if (plot_->valueColumn().isValid()) {
        bool ok2 = true;

        if      (valueColumnType_ == ColumnType::REAL)
          size = plot_->modelReal(data.row, plot_->valueColumn(), data.parent, ok2);
        else if (valueColumnType_ == ColumnType::INTEGER)
          size = plot_->modelInteger(data.row, plot_->valueColumn(), data.parent, ok2);
        else
          ok2 = false;

        if (ok2 && size <= 0.0)
          ok2 = false;

        if (! ok2)
          return State::SKIP;
      }

      //---

      QModelIndex valueInd = plot_->modelIndex(data.row, plot_->valueColumn(), data.parent);

      CQChartsSunburstNode *node = plot_->addNode(root_, nameStrs, size, nameInd1, valueInd);

      if (node) {
        CQChartsColor color;

        if (plot_->columnColor(data.row, data.parent, color))
          node->setColor(color);
      }

      return State::OK;
    }

   private:
    const CQChartsSunburstPlot *plot_            { nullptr };
    CQChartsSunburstHierNode   *root_            { nullptr };
    ColumnType                  valueColumnType_ { ColumnType::NONE };
  };

  RowVisitor visitor(this, root);

  visitModel(visitor);

  //---

  for (auto &root : roots_)
    addExtraNodes(root);
}

CQChartsSunburstNode *
CQChartsSunburstPlot::
addNode(CQChartsSunburstHierNode *root, const QStringList &nameStrs, double size,
        const QModelIndex &nameInd, const QModelIndex &valueInd) const
{
  CQChartsSunburstHierNode *parent = root;

  for (int i = 0; i < nameStrs.length() - 1; ++i) {
    CQChartsSunburstHierNode *child = nullptr;

    if (i == 0 && isMultiRoot()) {
      CQChartsSunburstRootNode *root = rootNode(nameStrs[i]);

      if (! root) {
        CQChartsSunburstPlot *th = const_cast<CQChartsSunburstPlot *>(this);

        root = th->createRootNode(nameStrs[i]);

        root->setInd(nameInd);
      }

      child = root;
    }
    else {
      child = childHierNode(parent, nameStrs[i]);

      if (! child) {
        // remove any existing leaf node (save size to use in new hier node)
        QModelIndex nameInd1;
        double      size1 = 0.0;

        CQChartsSunburstNode *node = childNode(parent, nameStrs[i]);

        if (node) {
          nameInd1 = node->ind();
          size1    = node->size();

          parent->removeNode(node);

          delete node;
        }

        //---

        child = new CQChartsSunburstHierNode(this, parent, nameStrs[i]);

        child->setSize(size1);

        child->setInd(nameInd1);
      }
    }

    parent = child;
  }

  //---

  QString name = nameStrs[nameStrs.length() - 1];

  CQChartsSunburstNode *node = childNode(parent, name);

  if (! node) {
    // use hier node if already created
    CQChartsSunburstHierNode *child = childHierNode(parent, name);

    if (child) {
      child->setSize(size);
      return nullptr;
    }

    //---

    node = new CQChartsSunburstNode(this, parent, name);

    node->setSize(size);

    if (valueInd.isValid()) {
      QModelIndex valueInd1 = normalizeIndex(valueInd);

      node->setInd(valueInd1);
    }
    else
      node->setInd(nameInd);

    parent->addNode(node);
  }

  return node;
}

void
CQChartsSunburstPlot::
addExtraNodes(CQChartsSunburstHierNode *hier) const
{
  if (hier->size() > 0) {
    CQChartsSunburstNode *node = new CQChartsSunburstNode(this, hier, "");

    QModelIndex ind1 = unnormalizeIndex(hier->ind());

    CQChartsColor color;

    if (columnColor(ind1.row(), ind1.parent(), color))
      node->setColor(color);

    node->setSize(hier->size());
    node->setInd (hier->ind());

    node->setFiller(true);

    hier->addNode(node);

    hier->setSize(0.0);
  }

  for (const auto &child : hier->getChildren())
    addExtraNodes(child);
}

CQChartsSunburstRootNode *
CQChartsSunburstPlot::
createRootNode(const QString &name)
{
  CQChartsSunburstRootNode *root = new CQChartsSunburstRootNode(this, name);

  roots_.push_back(root);

  return root;
}

CQChartsSunburstRootNode *
CQChartsSunburstPlot::
rootNode(const QString &name) const
{
  for (const auto &root : roots_)
    if (root->name() == name)
      return root;

  return nullptr;
}

CQChartsSunburstHierNode *
CQChartsSunburstPlot::
childHierNode(CQChartsSunburstHierNode *parent, const QString &name) const
{
  if (! parent) {
    for (const auto &root : roots_) {
      CQChartsSunburstHierNode *hier = childHierNode(root, name);
      if (hier) return hier;
    }

    return nullptr;
  }

  //--

  for (const auto &child : parent->getChildren())
    if (child->name() == name)
      return child;

  return nullptr;
}

CQChartsSunburstNode *
CQChartsSunburstPlot::
childNode(CQChartsSunburstHierNode *parent, const QString &name) const
{
  assert(parent);

  for (const auto &node : parent->getNodes())
    if (node->name() == name)
      return node;

  return nullptr;
}

//------

void
CQChartsSunburstPlot::
addPlotObjs(CQChartsSunburstHierNode *hier, PlotObjs &objs, const ColorInd &ir) const
{
  for (auto &node : hier->getNodes()) {
    addPlotObj(node, objs, ir);
  }

  for (auto &hierNode : hier->getChildren()) {
    addPlotObj(hierNode, objs, ir);

    addPlotObjs(hierNode, objs, ir);
  }
}

void
CQChartsSunburstPlot::
addPlotObj(CQChartsSunburstNode *node, PlotObjs &objs, const ColorInd &ir) const
{
  double r1 = node->r();
  double r2 = r1 + node->dr();

  CQChartsGeom::BBox bbox(-r2, -r2, r2, r2);

  CQChartsSunburstNodeObj *obj = new CQChartsSunburstNodeObj(this, bbox, node);

  obj->setIs(ir);

  node->setObj(obj);

  objs.push_back(obj);
}

//------

bool
CQChartsSunburstPlot::
addMenuItems(QMenu *menu)
{
  auto addMenuAction = [&](QMenu *menu, const QString &name, const char *slot) -> QAction *{
    QAction *action = new QAction(name, menu);

    connect(action, SIGNAL(triggered()), this, slot);

    menu->addAction(action);

    return action;
  };

  //---

  PlotObjs objs;

  selectedPlotObjs(objs);

  menu->addSeparator();

  QAction *pushAction   = addMenuAction(menu, "Push"   , SLOT(pushSlot()));
  QAction *popAction    = addMenuAction(menu, "Pop"    , SLOT(popSlot()));
  QAction *popTopAction = addMenuAction(menu, "Pop Top", SLOT(popTopSlot()));

  pushAction  ->setEnabled(! objs.empty());
  popAction   ->setEnabled(currentRoot() != nullptr);
  popTopAction->setEnabled(currentRoot() != nullptr);

  return true;
}

void
CQChartsSunburstPlot::
pushSlot()
{
  PlotObjs objs;

  selectedPlotObjs(objs);

  if (objs.empty()) {
    QPointF gpos = view()->menuPos();

    QPointF pos = view()->mapFromGlobal(QPoint(gpos.x(), gpos.y()));

    CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(pos));

    plotObjsAtPoint(w, objs);
  }

  if (objs.empty())
    return;

  for (const auto &obj : objs) {
    CQChartsSunburstNodeObj *sobj = dynamic_cast<CQChartsSunburstNodeObj *>(obj);
    if (! sobj) continue;

    CQChartsSunburstNode *node = sobj->node();

    CQChartsSunburstHierNode *hnode = dynamic_cast<CQChartsSunburstHierNode *>(node);

    if (! hnode)
      hnode = node->parent();

    if (hnode) {
      setCurrentRoot(hnode, /*update*/true);

      break;
    }
  }
}

void
CQChartsSunburstPlot::
popSlot()
{
  CQChartsSunburstHierNode *root = currentRoot();

  if (root && root->parent()) {
    setCurrentRoot(root->parent(), /*update*/true);
  }
}

void
CQChartsSunburstPlot::
popTopSlot()
{
  CQChartsSunburstHierNode *root = currentRoot();

  if (root)
    setCurrentRoot(nullptr, /*update*/true);
}

//------

void
CQChartsSunburstPlot::
postResize()
{
  CQChartsPlot::postResize();

  resetDataRange(/*updateRange*/true, /*updateObjs*/false);
}

void
CQChartsSunburstPlot::
drawNodes(CQChartsPaintDevice *device, CQChartsSunburstHierNode *hier) const
{
  for (auto &node : hier->getNodes())
    drawNode(device, nullptr, node);

  //------

  for (auto &hierNode : hier->getChildren()) {
    drawNode(device, nullptr, hierNode);

    drawNodes(device, hierNode);
  }
}

void
CQChartsSunburstPlot::
drawNode(CQChartsPaintDevice *device, CQChartsSunburstNodeObj *nodeObj,
         CQChartsSunburstNode *node) const
{
  if (! node->placed())
    return;

  CQChartsSunburstRootNode *root = dynamic_cast<CQChartsSunburstRootNode *>(node);
//CQChartsSunburstHierNode *hier = dynamic_cast<CQChartsSunburstHierNode *>(node);

  //---

  double xc = 0.0;
  double yc = 0.0;

  double r1, r2;

  if (root) {
    r1 = 0.0;
    r2 = std::max(innerRadius(), 0.0);
  }
  else {
    r1 = node->r();
    r2 = r1 + node->dr();
  }

  CQChartsGeom::Point p11 = CQChartsGeom::Point(xc - r1, yc - r1);
  CQChartsGeom::Point p21 = CQChartsGeom::Point(xc + r1, yc + r1);
  CQChartsGeom::Point p12 = CQChartsGeom::Point(xc - r2, yc - r2);
  CQChartsGeom::Point p22 = CQChartsGeom::Point(xc + r2, yc + r2);

  QRectF qr1(p11.x, p21.y, p21.x - p11.x, p11.y - p21.y);
  QRectF qr2(p12.x, p22.y, p22.x - p12.x, p12.y - p22.y);

  double a1 = node->a();
  double da = node->da();
  double a2 = a1 + da;

  //---

  // create arc path
  bool isCircle = (std::abs(da) > 360.0 || CMathUtil::realEq(std::abs(da), 360.0));

  QPainterPath path;

  if (isCircle) {
    if (qr1.width()) {
      path.arcMoveTo(qr1, 0);
      path.arcTo    (qr1, 0, 360.0);

      path.closeSubpath();
    }

    if (qr2.width()) {
      path.arcMoveTo(qr2, 0);
      path.arcTo    (qr2, 0, 360.0);

      path.closeSubpath();
    }
  }
  else {
    if      (qr1.width())
      path.arcMoveTo(qr1, a1);
    else if (qr2.width())
      path.arcMoveTo(qr2, a2);

    if (qr1.width())
      path.arcTo(qr1, a1,  da);

    if (qr2.width())
      path.arcTo(qr2, a2, -da);

    path.closeSubpath();
  }

  //---

  // calc stroke and brush
  ColorInd colorInd = nodeObj->calcColorInd();

  QPen   pen;
  QBrush brush;

  QColor bc = interpStrokeColor(colorInd);
  QColor fc = node->interpColor(this, fillColor(), colorInd, numColorIds());

  setPenBrush(pen, brush,
    isStroked(), bc, strokeAlpha(), strokeWidth(), strokeDash(),
    isFilled(), fc, fillAlpha(), fillPattern());

  if (nodeObj)
    updateObjPenBrushState(nodeObj, pen, brush);

  //---

  // draw path
  device->setPen  (pen);
  device->setBrush(brush);

  device->drawPath(path);

  //---

  // calc text pen
  QPen tpen;

  QColor tc = interpTextColor(colorInd);

  setPen(tpen, true, tc, textAlpha());

  if (nodeObj)
    updateObjPenBrushState(nodeObj, tpen, brush);

  //---

  // set font
  view()->setPlotPainterFont(this, device, textFont());

  //---

  // check if text visible (see treemap)
  if (! isCircle) {
    double c1 = cos(CMathUtil::Deg2Rad(a1));
    double s1 = sin(CMathUtil::Deg2Rad(a1));
    double c2 = cos(CMathUtil::Deg2Rad(a2));
    double s2 = sin(CMathUtil::Deg2Rad(a2));

    QPointF pw1(r2*c1, r2*s1);
    QPointF pw2(r2*c2, r2*s2);

    QPointF pp1 = windowToPixel(pw1);
    QPointF pp2 = windowToPixel(pw2);

    double d = std::hypot(pp2.x() - pp1.x(), pp2.y() - pp1.y());

    if (d < 1.5)
      return;
  }

  //---

  if (! isCircle) {
    device->save();

    device->setClipPath(path);
  }

  //---

  // draw node label
  double ta, c, s;

  if (isCircle) {
    ta = 0.0;
    c  = 1.0;
    s  = 0.0;
  }
  else {
    ta = a1 + da/2.0;
    c  = cos(CMathUtil::Deg2Rad(ta));
    s  = sin(CMathUtil::Deg2Rad(ta));
  }

  double tx, ty;

  if (isCircle && CMathUtil::isZero(r1)) {
    tx = 0.0;
    ty = 0.0;
  }
  else {
    double r3 = CMathUtil::avg(r1, r2);

    tx = r3*c;
    ty = r3*s;
  }

  device->setPen(tpen);

  Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

  CQChartsGeom::Point pt(tx, ty);

  QString name = (! node->isFiller() ? node->name() : node->parent()->name());

  double ta1 = (c >= 0 ? ta : ta - 180);

  CQChartsRotatedText::draw(device, pt.qpoint(), name, ta1, align, /*contrast*/false);

  //---

  if (! isCircle)
    device->restore();
}

//------

CQChartsSunburstNodeObj::
CQChartsSunburstNodeObj(const CQChartsSunburstPlot *plot, const CQChartsGeom::BBox &rect,
                        CQChartsSunburstNode *node) :
 CQChartsPlotObj(const_cast<CQChartsSunburstPlot *>(plot), rect), plot_(plot), node_(node)
{
  setModelInd(node_->ind());
}

QString
CQChartsSunburstNodeObj::
calcId() const
{
  QString name = (! node_->isFiller() ? node_->name() : node_->parent()->name());

  return QString("%1:%2:%3").arg(typeName()).arg(name).arg(node_->hierSize());
}

QString
CQChartsSunburstNodeObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  QString name = (! node_->isFiller() ? node_->hierName() : node_->parent()->hierName());

  //return QString("%1:%2").arg(name).arg(node_->hierSize());

  tableTip.addTableRow("Name", name);
  tableTip.addTableRow("Size", node_->hierSize());

  if (plot_->colorColumn().isValid()) {
    QModelIndex ind1 = plot_->unnormalizeIndex(node_->ind());

    bool ok;

    QString colorStr = plot_->modelString(ind1.row(), plot_->colorColumn(), ind1.parent(), ok);

    tableTip.addTableRow("Color", colorStr);
  }

  //---

  plot()->addTipColumns(tableTip, node_->ind());

  //---

  return tableTip.str();
}

bool
CQChartsSunburstNodeObj::
inside(const CQChartsGeom::Point &p) const
{
  double r1 = node_->r();
  double r2 = r1 + node_->dr();

  CQChartsGeom::Point c(0, 0);

  double r = p.distanceTo(c);

  if (r < r1 || r > r2)
    return false;

  //---

  // check angle
  double a = CMathUtil::Rad2Deg(atan2(p.y - c.y, p.x - c.x)); while (a < 0) a += 360.0;

  double a1 = node_->a();
  double a2 = a1 + node_->da();

  while (a1 < 0) a1 += 360.0;
  while (a2 < 0) a2 += 360.0;

  if (a1 > a2) {
    // crosses zero
    if (a >= 0 && a <= a2)
      return true;

    if (a <= 360 && a >= a1)
      return true;
  }
  else {
    if (a >= a1 && a <= a2)
      return true;
  }

  return false;
}

void
CQChartsSunburstNodeObj::
getSelectIndices(Indices &inds) const
{
  for (const auto &c : plot_->nameColumns())
    addColumnSelectIndex(inds, c);

  addColumnSelectIndex(inds, plot_->valueColumn());
}

void
CQChartsSunburstNodeObj::
draw(CQChartsPaintDevice *device)
{
  plot_->drawNode(device, this, node_);
}

//------

CQChartsSunburstHierNode::
CQChartsSunburstHierNode(const CQChartsSunburstPlot *plot, CQChartsSunburstHierNode *parent,
                         const QString &name) :
 CQChartsSunburstNode(plot, parent, name)
{
  if (parent_)
    parent_->children_.push_back(this);
}

CQChartsSunburstHierNode::
~CQChartsSunburstHierNode()
{
  for (auto &child : children_)
    delete child;

  for (auto &node : nodes_)
    delete node;
}

double
CQChartsSunburstHierNode::
hierSize() const
{
  double s = size();

  for (const auto &child : children_)
    s += child->hierSize();

  for (const auto &node : nodes_)
    s += node->hierSize();

  return s;
}

int
CQChartsSunburstHierNode::
depth() const
{
  int depth = 1;

  for (const auto &child : children_)
    depth = std::max(depth, child->depth() + 1);

  return depth;
}

int
CQChartsSunburstHierNode::
numNodes() const
{
  int num = nodes_.size();

  for (const auto &child : children_)
    num += child->numNodes();

  return std::max(num, 1);
}

void
CQChartsSunburstHierNode::
unplace()
{
  unplaceNodes();
}

void
CQChartsSunburstHierNode::
unplaceNodes()
{
  CQChartsSunburstNode::unplace();

  for (auto &child : children_)
    child->unplaceNodes();

  for (auto &node : nodes_)
    node->unplace();
}

void
CQChartsSunburstHierNode::
packNodes(CQChartsSunburstHierNode *root, double ri, double ro,
          double dr, double a, double da, const Order &order, bool sort)
{
  int d = depth();

  if (dr <= 0.0)
    dr = (ro - ri)/d;

  double s = (order == Order::SIZE ? hierSize() : numNodes());

  double da1 = da/s;

  packSubNodes(root, ri, dr, a, da1, order, sort);
}

void
CQChartsSunburstHierNode::
packSubNodes(CQChartsSunburstHierNode *root, double ri,
             double dr, double a, double da, const Order &order, bool sort)
{
  // make single list of nodes to pack
  Nodes nodes;

  for (auto &child : children_)
    nodes.push_back(child);

  for (auto &node : nodes_)
    nodes.push_back(node);

  if (sort) {
#if 0
    if (root->order() == Order::SIZE)
      std::sort(nodes.begin(), nodes.end(), CQChartsSunburstNodeSizeCmp());
    else
      std::sort(nodes.begin(), nodes.end(), CQChartsSunburstNodeCountCmp());
#else
    std::sort(nodes.begin(), nodes.end(), CQChartsSunburstNodeNameCmp());
  }
#endif

  //---

  placed_ = true;

  // place each node
  double a1 = a;

  for (auto &node : nodes) {
    double s = (order == Order::SIZE ? node->hierSize() : node->numNodes());

    node->setPosition(ri, a1, dr, s*da);

    CQChartsSunburstHierNode *hierNode = dynamic_cast<CQChartsSunburstHierNode *>(node);

    if (hierNode)
      hierNode->packSubNodes(root, ri + dr, dr, a1, da, order, sort);

    a1 += s*da;
  }
}

void
CQChartsSunburstHierNode::
addNode(CQChartsSunburstNode *node)
{
  nodes_.push_back(node);
}

void
CQChartsSunburstHierNode::
removeNode(CQChartsSunburstNode *node)
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
CQChartsSunburstHierNode::
interpColor(const CQChartsSunburstPlot *plot, const CQChartsColor &c,
            const ColorInd &colorInd, int n) const
{
  using Colors = std::vector<QColor>;

  Colors colors;

  for (auto &child : children_)
    colors.push_back(child->interpColor(plot, c, colorInd, n));

  for (auto &node : nodes_)
    colors.push_back(node->interpColor(plot, c, colorInd, n));

  if (colors.empty())
    return plot->interpColor(c, colorInd);

  return CQChartsUtil::blendColors(colors);
}

//------

CQChartsSunburstNode::
CQChartsSunburstNode(const CQChartsSunburstPlot *plot, CQChartsSunburstHierNode *parent,
                     const QString &name) :
 plot_(plot), parent_(parent), id_(nextId()), name_(name)
{
}

QString
CQChartsSunburstNode::
hierName(const QChar &separator) const
{
  if (parent() && (plot()->isMultiRoot() || parent() != plot()->roots()[0]))
    return parent()->hierName(separator) + separator + name();
  else
    return name();
}

void
CQChartsSunburstNode::
setPosition(double r, double a, double dr, double da)
{
  r_  = r ; a_  = a ;
  dr_ = dr; da_ = da;

  placed_ = true;
}

bool
CQChartsSunburstNode::
pointInside(double x, double y)
{
  if (! placed_) return false;

  double r = sqrt(x*x + y*y);

  if (r < r_ || r > r_ + dr_) return false;

  double a = normalizeAngle(CMathUtil::Rad2Deg(atan2(y, x)));

  double a1 = normalizeAngle(a_);
  double a2 = a1 + da_;

  if (a2 > a1) {
    if (a2 >= 360.0) {
      double da = a2 - 360.0; a -= da; a1 -= da; a2 = 360.0;
      a = normalizeAngle(a);
    }

    if (a < a1 || a > a2)
      return false;
  }
  else {
    if (a2 < 0.0) {
      double da = -a2; a += da; a1 += da; a2 = 0.0;

      a = normalizeAngle(a);
    }

    if (a < a2 || a > a1)
      return false;
  }

  return true;
}

QColor
CQChartsSunburstNode::
interpColor(const CQChartsSunburstPlot *plot, const CQChartsColor &c,
            const ColorInd &colorInd, int n) const
{
  if      (color().isValid())
    return plot->interpColor(color(), ColorInd());
  else if (colorId() >= 0 && plot_->isColorById())
    return plot->interpFillColor(ColorInd(colorId(), n));
  else
    return plot->interpColor(c, colorInd);
}

//------

// sort reverse alphabetic no case
bool
CQChartsSunburstNodeNameCmp::
operator()(const CQChartsSunburstNode *n1, const CQChartsSunburstNode *n2)
{
  const QString &name1 = n1->name();
  const QString &name2 = n2->name();

  int l1 = name1.size();
  int l2 = name2.size();

  for (int i = 0; i < std::max(l1, l2); ++i) {
    char c1 = (i < l1 ? tolower(name1[i].toLatin1()) : '\0');
    char c2 = (i < l2 ? tolower(name2[i].toLatin1()) : '\0');

    if (c1 > c2) return true;
    if (c1 < c2) return false;
  }

  return false;
}
