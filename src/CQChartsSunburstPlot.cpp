#include <CQChartsSunburstPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsRotatedText.h>
#include <CQChartsTip.h>

#include <QMenu>
#include <QPainter>

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

CQChartsPlot *
CQChartsSunburstPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsSunburstPlot(view, model);
}

//---

CQChartsSunburstPlot::
CQChartsSunburstPlot(CQChartsView *view, const ModelP &model) :
 CQChartsHierPlot(view, view->charts()->plotType("sunburst"), model)
{
  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setBorder(true);
  setFilled(true);

  setTextFontSize(8.0);

  setTextColor(CQChartsColor(CQChartsColor::Type::THEME_VALUE, 1));

  setMargins(1, 1, 1, 1);

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
setFilled(bool b)
{
  CQChartsUtil::testAndSet(shapeData_.background.visible, b, [&]() { update(); } );
}

void
CQChartsSunburstPlot::
setFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(shapeData_.background.color, c, [&]() { update(); } );
}

void
CQChartsSunburstPlot::
setFillAlpha(double a)
{
  CQChartsUtil::testAndSet(shapeData_.background.alpha, a, [&]() { update(); } );
}

void
CQChartsSunburstPlot::
setFillPattern(Pattern pattern)
{
  if (pattern != (Pattern) shapeData_.background.pattern) {
    shapeData_.background.pattern = (CQChartsFillData::Pattern) pattern;

    update();
  }
}

QColor
CQChartsSunburstPlot::
interpFillColor(int i, int n) const
{
  return fillColor().interpColor(this, i, n);
}

//---

void
CQChartsSunburstPlot::
setBorder(bool b)
{
  CQChartsUtil::testAndSet(shapeData_.border.visible, b, [&]() { update(); } );
}

void
CQChartsSunburstPlot::
setBorderColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(shapeData_.border.color, c, [&]() { update(); } );
}

void
CQChartsSunburstPlot::
setBorderAlpha(double a)
{
  CQChartsUtil::testAndSet(shapeData_.border.alpha, a, [&]() { update(); } );
}

void
CQChartsSunburstPlot::
setBorderWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(shapeData_.border.width, l, [&]() { update(); } );
}

QColor
CQChartsSunburstPlot::
interpBorderColor(int i, int n) const
{
  return borderColor().interpColor(this, i, n);
}

//---

void
CQChartsSunburstPlot::
setTextFont(const QFont &f)
{
  CQChartsUtil::testAndSet(textData_.font, f, [&]() { update(); } );
}

void
CQChartsSunburstPlot::
setTextColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(textData_.color, c, [&]() { update(); } );
}

void
CQChartsSunburstPlot::
setTextAlpha(double a)
{
  CQChartsUtil::testAndSet(textData_.alpha, a, [&]() { update(); } );
}

void
CQChartsSunburstPlot::
setTextContrast(bool b)
{
  CQChartsUtil::testAndSet(textData_.contrast, b, [&]() { update(); } );
}

QColor
CQChartsSunburstPlot::
interpTextColor(int i, int n) const
{
  return textColor().interpColor(this, i, n);
}

void
CQChartsSunburstPlot::
setTextFontSize(double s)
{
  if (s != textData_.font.pointSizeF()) {
    textData_.font.setPointSizeF(s);

    update();
  }
}

//---

void
CQChartsSunburstPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumn" , "name" );
  addProperty("columns", this, "nameColumns", "names");
  addProperty("columns", this, "valueColumn", "value");
  addProperty("columns", this, "colorColumn", "color");

  addProperty("", this, "separator");

  addProperty("", this, "innerRadius");
  addProperty("", this, "outerRadius");
  addProperty("", this, "startAngle" );
  addProperty("", this, "multiRoot"  );

  addProperty("stroke", this, "border"     , "visible");
  addProperty("stroke", this, "borderColor", "color"  );
  addProperty("stroke", this, "borderAlpha", "alpha"  );
  addProperty("stroke", this, "borderWidth", "width"  );

  addProperty("fill", this, "filled"     , "visible");
  addProperty("fill", this, "fillColor"  , "color"  );
  addProperty("fill", this, "fillAlpha"  , "alpha"  );
  addProperty("fill", this, "fillPattern", "pattern");

  addProperty("text", this, "textFont"    , "font"    );
  addProperty("text", this, "textColor"   , "color"   );
  addProperty("text", this, "textAlpha"   , "alpha"   );
  addProperty("text", this, "textContrast", "contrast");

  addProperty("color", this, "colorMapEnabled", "mapEnabled");
  addProperty("color", this, "colorMapMin"    , "mapMin"    );
  addProperty("color", this, "colorMapMax"    , "mapMax"    );
}

//------

CQChartsSunburstHierNode *
CQChartsSunburstPlot::
currentRoot() const
{
  CQChartsSunburstHierNode *currentRoot = nullptr;

  QStringList names = currentRootName_.split(separator(), QString::SkipEmptyParts);

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
    currentRootName_ = hier->hierName();
  else
    currentRootName_ = "";

  if (update) {
    replaceRoots();

    updateObjs();
  }
}

void
CQChartsSunburstPlot::
updateRange(bool apply)
{
  double r = 1.0;

  dataRange_.reset();

  dataRange_.updateRange(-r, -r);
  dataRange_.updateRange( r,  r);

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange_.equalScale(aspect);
  }

  //---

  if (apply)
    applyDataRange();
}

//------

void
CQChartsSunburstPlot::
updateObjs()
{
  clearValueSets();

  resetRoots();

  CQChartsPlot::updateObjs();
}

bool
CQChartsSunburstPlot::
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

  if (roots_.empty())
    initRoots();

  //---

  initColorIds();

  if (currentRoot()) {
    colorNodes(currentRoot());
  }
  else {
    for (auto &root : roots_)
      colorNodes(root);
  }

  //---

  bool isUnnamedRoot = (roots_.size() == 1 && roots_[0]->name() == "");

  if (currentRoot()) {
    if (! isUnnamedRoot || roots_[0] != currentRoot())
      addPlotObj(currentRoot());

    addPlotObjs(currentRoot());
  }
  else {
    for (auto &root : roots_) {
      if (! isUnnamedRoot)
        addPlotObj(root);

      addPlotObjs(root);
    }
  }

  //---

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
replaceRoots()
{
  double ri = std::max(innerRadius(), 0.0);
  double ro = std::min(std::max(outerRadius(), ri), 1.0);

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
colorNodes(CQChartsSunburstHierNode *hier)
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
colorNode(CQChartsSunburstNode *node)
{
  if (! node->color().isValid())
    node->setColorId(nextColorId());
}

void
CQChartsSunburstPlot::
loadHier(CQChartsSunburstHierNode *root)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsSunburstPlot *plot, CQChartsSunburstHierNode *root) :
     plot_(plot) {
      hierStack_.push_back(root);
    }

    State hierVisit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      QString     name;
      QModelIndex nameInd;

      (void) getName(model, parent, row, name, nameInd);

      //---

      CQChartsSunburstHierNode *hier = plot_->addHierNode(parentHier(), name, nameInd);

      //---

      hierStack_.push_back(hier);

      return State::OK;
    }

    State hierPostVisit(QAbstractItemModel *, const QModelIndex &, int) override {
      hierStack_.pop_back();

      assert(! hierStack_.empty());

      return State::OK;
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      QString     name;
      QModelIndex nameInd;

      (void) getName(model, parent, row, name, nameInd);

      //---

      double      size = 1.0;
      QModelIndex valueInd;

      if (! getSize(model, parent, row, size, valueInd))
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

    bool getName(QAbstractItemModel *model, const QModelIndex &parent, int row,
                 QString &name, QModelIndex &nameInd) const {
      nameInd = model->index(row, plot_->nameColumn().column(), parent);

      bool ok;

      name = CQChartsUtil::modelString(model, row, plot_->nameColumn(), parent, ok);

      return ok;
    }

    bool getSize(QAbstractItemModel *model, const QModelIndex &parent, int row,
                 double size, QModelIndex &valueInd) const {
      valueInd = model->index(row, plot_->valueColumn().column(), parent);

      size = 1.0;

      if (! plot_->valueColumn().isValid())
        return true;

      bool ok = true;

      size = CQChartsUtil::modelReal(model, row, plot_->valueColumn(), parent, ok);

      if (ok && size <= 0.0)
        ok = false;

      return ok;
    }

   private:
    using HierStack = std::vector<CQChartsSunburstHierNode *>;

    CQChartsSunburstPlot *plot_ { nullptr };
    HierStack             hierStack_;
  };

  RowVisitor visitor(this, root);

  visitModel(visitor);
}

CQChartsSunburstHierNode *
CQChartsSunburstPlot::
addHierNode(CQChartsSunburstHierNode *hier, const QString &name, const QModelIndex &nameInd)
{
  QModelIndex nameInd1 = normalizeIndex(nameInd);

  CQChartsSunburstHierNode *hier1 = new CQChartsSunburstHierNode(this, hier, name);

  hier1->setInd(nameInd1);

  return hier1;
}

CQChartsSunburstNode *
CQChartsSunburstPlot::
addNode(CQChartsSunburstHierNode *hier, const QString &name, double size,
        const QModelIndex &nameInd, const QModelIndex &valueInd)
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
loadFlat(CQChartsSunburstHierNode *root)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsSunburstPlot *plot, CQChartsSunburstHierNode *root) :
     plot_(plot), root_(root) {
      valueColumnType_ = plot_->columnValueType(plot_->valueColumn());
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
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
          size = CQChartsUtil::modelReal(model, row, plot_->valueColumn(), parent, ok2);
        else if (valueColumnType_ == ColumnType::INTEGER)
          size = CQChartsUtil::modelInteger(model, row, plot_->valueColumn(), parent, ok2);
        else
          ok2 = false;

        if (ok2 && size <= 0.0)
          ok2 = false;

        if (! ok2)
          return State::SKIP;
      }

      //---

      QModelIndex valueInd = model->index(row, plot_->valueColumn().column(), parent);

      CQChartsSunburstNode *node = plot_->addNode(root_, nameStrs, size, nameInd1, valueInd);

      if (node) {
        OptColor color;

        if (plot_->colorSetColor("color", row, color))
          node->setColor(*color);
      }

      return State::OK;
    }

   private:
    CQChartsSunburstPlot     *plot_            { nullptr };
    CQChartsSunburstHierNode *root_            { nullptr };
    ColumnType                valueColumnType_ { ColumnType::NONE };
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
        const QModelIndex &nameInd, const QModelIndex &valueInd)
{
  CQChartsSunburstHierNode *parent = root;

  for (int i = 0; i < nameStrs.length() - 1; ++i) {
    CQChartsSunburstHierNode *child = nullptr;

    if (i == 0 && isMultiRoot()) {
      CQChartsSunburstRootNode *root = rootNode(nameStrs[i]);

      if (! root) {
        root = createRootNode(nameStrs[i]);

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
addExtraNodes(CQChartsSunburstHierNode *hier)
{
  if (hier->size() > 0) {
    CQChartsSunburstNode *node = new CQChartsSunburstNode(this, hier, "");

    QModelIndex ind1 = unnormalizeIndex(hier->ind());

    OptColor color;

    if (colorSetColor("color", ind1.row(), color))
      node->setColor(*color);

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
  for (const auto &child : parent->getChildren())
    if (child->name() == name)
      return child;

  return nullptr;
}

CQChartsSunburstNode *
CQChartsSunburstPlot::
childNode(CQChartsSunburstHierNode *parent, const QString &name) const
{
  for (const auto &node : parent->getNodes())
    if (node->name() == name)
      return node;

  return nullptr;
}

//------

void
CQChartsSunburstPlot::
addPlotObjs(CQChartsSunburstHierNode *hier)
{
  for (auto node : hier->getNodes()) {
    addPlotObj(node);
  }

  for (auto hierNode : hier->getChildren()) {
    addPlotObj(hierNode);

    addPlotObjs(hierNode);
  }
}

void
CQChartsSunburstPlot::
addPlotObj(CQChartsSunburstNode *node)
{
  double r1 = node->r();
  double r2 = r1 + node->dr();

  CQChartsGeom::BBox bbox(-r2, -r2, r2, r2);

  CQChartsSunburstNodeObj *obj = new CQChartsSunburstNodeObj(this, bbox, node);

  node->setObj(obj);

  addPlotObject(obj);
}

//------

bool
CQChartsSunburstPlot::
addMenuItems(QMenu *menu)
{
  PlotObjs objs;

  selectedObjs(objs);

  QAction *pushAction   = new QAction("Push"   , menu);
  QAction *popAction    = new QAction("Pop"    , menu);
  QAction *popTopAction = new QAction("Pop Top", menu);

  connect(pushAction  , SIGNAL(triggered()), this, SLOT(pushSlot()));
  connect(popAction   , SIGNAL(triggered()), this, SLOT(popSlot()));
  connect(popTopAction, SIGNAL(triggered()), this, SLOT(popTopSlot()));

  pushAction  ->setEnabled(! objs.empty());
  popAction   ->setEnabled(currentRoot() != nullptr);
  popTopAction->setEnabled(currentRoot() != nullptr);

  menu->addSeparator();

  menu->addAction(pushAction  );
  menu->addAction(popAction   );
  menu->addAction(popTopAction);

  return true;
}

void
CQChartsSunburstPlot::
pushSlot()
{
  PlotObjs objs;

  selectedObjs(objs);

  if (objs.empty()) {
    QPointF gpos = view()->menuPos();

    QPointF pos = view()->mapFromGlobal(QPoint(gpos.x(), gpos.y()));

    CQChartsGeom::Point w;

    pixelToWindow(CQChartsUtil::fromQPoint(pos), w);

    objsAtPoint(w, objs);
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
handleResize()
{
  CQChartsPlot::handleResize();

  dataRange_.reset();
}

void
CQChartsSunburstPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

void
CQChartsSunburstPlot::
drawNodes(QPainter *painter, CQChartsSunburstHierNode *hier)
{
  for (auto node : hier->getNodes())
    drawNode(painter, nullptr, node);

  //------

  for (auto hierNode : hier->getChildren()) {
    drawNode(painter, nullptr, hierNode);

    drawNodes(painter, hierNode);
  }
}

void
CQChartsSunburstPlot::
drawNode(QPainter *painter, CQChartsSunburstNodeObj *nodeObj, CQChartsSunburstNode *node)
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

  double px11, py11, px21, py21;
  double px12, py12, px22, py22;

  windowToPixel(xc - r1, yc - r1, px11, py11);
  windowToPixel(xc + r1, yc + r1, px21, py21);
  windowToPixel(xc - r2, yc - r2, px12, py12);
  windowToPixel(xc + r2, yc + r2, px22, py22);

  QRectF qr1(px11, py21, px21 - px11, py11 - py21);
  QRectF qr2(px12, py22, px22 - px12, py12 - py22);

  double a1 = node->a();
  double da = node->da();
  double a2 = a1 + da;

  //---

  // create arc path
  bool isCircle = (std::abs(da) > 360.0 || CQChartsUtil::realEq(std::abs(da), 360.0));

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
  QBrush brush;

  if (isFilled()) {
    QColor fillColor = node->interpColor(this, numColorIds());

    fillColor.setAlphaF(fillAlpha());

    brush.setColor(fillColor);
    brush.setStyle(CQChartsFillPattern::toStyle(
     (CQChartsFillPattern::Type) fillPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (isBorder()) {
    QColor bc = interpBorderColor(0, 1);

    bc.setAlphaF(borderAlpha());

    double bw = lengthPixelWidth(borderWidth());

    pen.setColor (bc);
    pen.setWidthF(bw);
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  QColor tc = interpTextColor(0, 1);

  tc.setAlphaF(textAlpha());

  QPen tpen(tc);

  if (nodeObj) {
    updateObjPenBrushState(nodeObj, pen , brush);
    updateObjPenBrushState(nodeObj, tpen, brush);
  }

  //---

  // draw path
  painter->setPen  (pen);
  painter->setBrush(brush);

  painter->drawPath(path);

  //---

  // draw node label
  if (nodeObj)
    updateObjPenBrushState(nodeObj, tpen, brush);

  painter->setPen(tpen);

  QFont font = textFont();

  painter->setFont(font);

  double ta, c, s;

  if (isCircle) {
    ta = 0.0;
    c  = 1.0;
    s  = 0.0;
  }
  else {
    ta = a1 + da/2.0;
    c  = cos(ta*M_PI/180.0);
    s  = sin(ta*M_PI/180.0);
  }

  double tx, ty;

  if (isCircle && CQChartsUtil::isZero(r1)) {
    tx = 0.0;
    ty = 0.0;
  }
  else {
    double r3 = CQChartsUtil::avg(r1, r2);

    tx = r3*c;
    ty = r3*s;
  }

  Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

  double px, py;

  windowToPixel(tx, ty, px, py);

  QString name = (! node->isFiller() ? node->name() : node->parent()->name());

  if (c >= 0)
    CQChartsRotatedText::drawRotatedText(painter, px, py, name, ta, align);
  else
    CQChartsRotatedText::drawRotatedText(painter, px, py, name, ta - 180, align);
}

//------

CQChartsSunburstNodeObj::
CQChartsSunburstNodeObj(CQChartsSunburstPlot *plot, const CQChartsGeom::BBox &rect,
                        CQChartsSunburstNode *node) :
 CQChartsPlotObj(plot, rect), plot_(plot), node_(node)
{
}

QString
CQChartsSunburstNodeObj::
calcId() const
{
  QString name = (! node_->isFiller() ? node_->name() : node_->parent()->name());

  return QString("%1:%2").arg(name).arg(node_->hierSize());
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
    QAbstractItemModel *model = plot_->model();

    QModelIndex ind1 = plot_->unnormalizeIndex(node_->ind());

    bool ok;

    QString colorStr =
      CQChartsUtil::modelString(model, ind1.row(), plot_->colorColumn(), ind1.parent(), ok);

    tableTip.addTableRow("Color", colorStr);
  }

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
  double a = CQChartsUtil::Rad2Deg(atan2(p.y - c.y, p.x - c.x)); while (a < 0) a += 360.0;

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
addSelectIndex()
{
  const QModelIndex &ind = node_->ind();

  plot_->addSelectIndex(ind.row(), plot_->nameColumn(), ind.parent());

  if (plot_->valueColumn().isValid())
    plot_->addSelectIndex(ind.row(), plot_->valueColumn(), ind.parent());

  if (plot_->colorColumn().isValid())
    plot_->addSelectIndex(ind.row(), plot_->colorColumn(), ind.parent());
}

bool
CQChartsSunburstNodeObj::
isIndex(const QModelIndex &ind) const
{
  return (ind == node_->ind());
}

void
CQChartsSunburstNodeObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  plot_->drawNode(painter, this, node_);
}

//------

CQChartsSunburstHierNode::
CQChartsSunburstHierNode(CQChartsSunburstPlot *plot, CQChartsSunburstHierNode *parent,
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
interpColor(CQChartsSunburstPlot *plot, int n) const
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

CQChartsSunburstNode::
CQChartsSunburstNode(CQChartsSunburstPlot *plot, CQChartsSunburstHierNode *parent,
                     const QString &name) :
 plot_(plot), parent_(parent), id_(nextId()), name_(name)
{
}

QString
CQChartsSunburstNode::
hierName() const
{
  if (parent() && (plot()->isMultiRoot() || parent() != plot()->roots()[0]))
    return parent()->hierName() + "/" + name();
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

  double a = normalizeAngle(180.0*atan2(y, x)/M_PI);

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
interpColor(CQChartsSunburstPlot *plot, int n) const
{
  if      (colorId() >= 0)
    return plot->interpFillColor(colorId(), n);
  else if (color().isValid())
    return color().interpColor(plot, 0, 1);
  else
    return plot->interpPaletteColor(0, 1);
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
