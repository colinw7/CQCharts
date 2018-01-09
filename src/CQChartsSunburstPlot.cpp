#include <CQChartsSunburstPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsFillObj.h>
#include <CQChartsRotatedText.h>

#include <QMenu>
#include <QPainter>

//---

CQChartsSunburstPlotType::
CQChartsSunburstPlotType()
{
  addParameters();
}

void
CQChartsSunburstPlotType::
addParameters()
{
  addColumnParameter ("name" , "Name" , "nameColumn" , "", 0);
  addColumnsParameter("names", "Names", "nameColumns", "optional");
  addColumnParameter ("value", "Value", "valueColumn", "optional");
  addColumnParameter ("color", "Color", "colorColumn", "optional");

  addStringParameter("separator", "Separator", "separator", "optional", "/");
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
  textBoxObj_ = new CQChartsTextBoxObj(this);

  textBoxObj_->setBackgroundColor(CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE));

  setBorder(true);
  setFilled(true);

  textBoxObj_->setTextFontSize(8.0);

  CQChartsPaletteColor textColor(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  textBoxObj_->setTextColor(textColor);

  setMargins(1, 1, 1, 1);

  // addKey() // TODO

  addTitle();
}

CQChartsSunburstPlot::
~CQChartsSunburstPlot()
{
  delete textBoxObj_;

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
updateHierColumns()
{
  resetRoots();

  updateRangeAndObjs();
}

//----

bool
CQChartsSunburstPlot::
isFilled() const
{
  return textBoxObj_->isBackground();
}

void
CQChartsSunburstPlot::
setFilled(bool b)
{
  textBoxObj_->setBackground(b);

  update();
}

QString
CQChartsSunburstPlot::
fillColorStr() const
{
  return textBoxObj_->backgroundColorStr();
}

void
CQChartsSunburstPlot::
setFillColorStr(const QString &s)
{
  textBoxObj_->setBackgroundColorStr(s);

  update();
}

QColor
CQChartsSunburstPlot::
interpFillColor(int i, int n) const
{
  return textBoxObj_->interpBackgroundColor(i, n);
}

double
CQChartsSunburstPlot::
fillAlpha() const
{
  return textBoxObj_->backgroundAlpha();
}

void
CQChartsSunburstPlot::
setFillAlpha(double a)
{
  textBoxObj_->setBackgroundAlpha(a);

  update();
}

CQChartsSunburstPlot::Pattern
CQChartsSunburstPlot::
fillPattern() const
{
  return (Pattern) textBoxObj_->backgroundPattern();
}

void
CQChartsSunburstPlot::
setFillPattern(Pattern pattern)
{
  textBoxObj_->setBackgroundPattern((CQChartsBoxObj::Pattern) pattern);

  update();
}

//---

bool
CQChartsSunburstPlot::
isBorder() const
{
  return textBoxObj_->isBorder();
}

void
CQChartsSunburstPlot::
setBorder(bool b)
{
  textBoxObj_->setBorder(b);

  update();
}

QString
CQChartsSunburstPlot::
borderColorStr() const
{
  return textBoxObj_->borderColorStr();
}

void
CQChartsSunburstPlot::
setBorderColorStr(const QString &str)
{
  textBoxObj_->setBorderColorStr(str);

  update();
}

QColor
CQChartsSunburstPlot::
interpBorderColor(int i, int n) const
{
  return textBoxObj_->interpBorderColor(i, n);
}

double
CQChartsSunburstPlot::
borderAlpha() const
{
  return textBoxObj_->borderAlpha();
}

void
CQChartsSunburstPlot::
setBorderAlpha(double a)
{
  textBoxObj_->setBorderAlpha(a);

  update();
}

double
CQChartsSunburstPlot::
borderWidth() const
{
  return textBoxObj_->borderWidth();
}

void
CQChartsSunburstPlot::
setBorderWidth(double r)
{
  textBoxObj_->setBorderWidth(r);

  update();
}

//---

const QFont &
CQChartsSunburstPlot::
textFont() const
{
  return textBoxObj_->textFont();
}

void
CQChartsSunburstPlot::
setTextFont(const QFont &f)
{
  textBoxObj_->setTextFont(f);

  update();
}

QString
CQChartsSunburstPlot::
textColorStr() const
{
  return textBoxObj_->textColorStr();
}

void
CQChartsSunburstPlot::
setTextColorStr(const QString &s)
{
  textBoxObj_->setTextColorStr(s);

  update();
}

QColor
CQChartsSunburstPlot::
interpTextColor(int i, int n) const
{
  return textBoxObj_->interpTextColor(i, n);
}

bool
CQChartsSunburstPlot::
isTextContrast() const
{
  return textBoxObj_->isTextContrast();
}

void
CQChartsSunburstPlot::
setTextContrast(bool b)
{
  textBoxObj_->setTextContrast(b);

  update();
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

  addProperty("border", this, "border"     , "visible");
  addProperty("border", this, "borderColor", "color"  );
  addProperty("border", this, "borderAlpha", "alpha"  );
  addProperty("border", this, "borderWidth", "width"  );

  addProperty("fill", this, "filled"     , "visible");
  addProperty("fill", this, "fillColor"  , "color"  );
  addProperty("fill", this, "fillAlpha"  , "alpha"  );
  addProperty("fill", this, "fillPattern", "pattern");

  addProperty("text", this, "textFont"    , "font"    );
  addProperty("text", this, "textColor"   , "color"   );
  addProperty("text", this, "textContrast", "contrast");

  addProperty("color", this, "colorMapEnabled", "mapEnabled");
  addProperty("color", this, "colorMapMin"    , "mapMin"    );
  addProperty("color", this, "colorMapMax"    , "mapMax"    );
}

void
CQChartsSunburstPlot::
setCurrentRoot(CQChartsSunburstHierNode *hier, bool update)
{
  currentRoot_ = hier;

  if (update) {
    replaceRoots();

    updateObjs();
  }
}

void
CQChartsSunburstPlot::
updateRange(bool apply)
{
  double radius = 1.0;

  double xr = radius;
  double yr = radius;

  if (isEqualScale()) {
    double aspect = this->aspect();

    if (aspect > 1.0)
      xr *= aspect;
    else
      yr *= 1.0/aspect;
  }

  dataRange_.reset();

  dataRange_.updateRange(-xr, -yr);
  dataRange_.updateRange( xr,  yr);

  //---

  if (apply)
    applyDataRange();
}

void
CQChartsSunburstPlot::
initColorSet()
{
  colorSet_.clear();

  if (colorColumn() < 0)
    return;

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int nr = model->rowCount(QModelIndex());

  for (int i = 0; i < nr; ++i) {
    bool ok;

    QVariant value = CQChartsUtil::modelValue(model, i, colorColumn(), ok);

    colorSet_.addValue(value); // always add some value
  }
}

bool
CQChartsSunburstPlot::
colorSetColor(int i, OptColor &color)
{
  return colorSet_.icolor(i,color);
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
  if (colorSet_.empty())
    initColorSet();

  //---

  if (roots_.empty())
    initRoots();

  //---

  initColorIds();

  if (currentRoot_) {
    colorNodes(currentRoot_);
  }
  else {
    for (auto &root : roots_)
      colorNodes(root);
  }

  //---

  bool isUnnamedRoot = (roots_.size() == 1 && roots_[0]->name() == "");

  if (currentRoot_) {
    if (! isUnnamedRoot || roots_[0] != currentRoot_)
      addPlotObj(currentRoot_);

    addPlotObjs(currentRoot_);
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
  currentRoot_ = nullptr;

  //---

  CQChartsSunburstRootNode *root = nullptr;

  if (! isMultiRoot())
    root = createRootNode();

  if (isHierarchical())
    loadChildren(root);
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

  if (currentRoot_) {
    double da = 360.0;

    currentRoot_->setPosition(0.0, a, ri, da);

    currentRoot_->packNodes(currentRoot_, ri, ro, 0.0, a, da,
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
loadChildren(CQChartsSunburstHierNode *hier, const QModelIndex &index, int depth)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  int nr = model->rowCount(index);

  for (int r = 0; r < nr; ++r) {
    QModelIndex nameInd = model->index(r, nameColumn(), index);

    QModelIndex nameInd1 = normalizeIndex(nameInd);

    //---

    bool ok;

    QString name = CQChartsUtil::modelString(model, nameInd, ok);

    //---

    if (model->rowCount(nameInd) > 0) {
      CQChartsSunburstHierNode *hier1 = new CQChartsSunburstHierNode(hier, name);

      loadChildren(hier1, nameInd, depth + 1);

      hier1->setInd(nameInd1);
    }
    else {
      QModelIndex valueInd = model->index(r, valueColumn(), index);

      double size = 1.0;

      if (valueInd.isValid()) {
        bool ok;

        size = CQChartsUtil::modelReal(model, valueInd, ok);

        if (ok && size <= 0.0)
          ok = false;

        if (! ok)
          continue;
      }

      //---

      CQChartsSunburstNode *node = new CQChartsSunburstNode(hier, name);

      node->setSize(size);

      if (valueInd.isValid()) {
        QModelIndex valueInd1 = normalizeIndex(valueInd);

        node->setInd(valueInd1);
      }
      else
        node->setInd(nameInd1);

      hier->addNode(node);
    }
  }
}

void
CQChartsSunburstPlot::
loadFlat(CQChartsSunburstHierNode *root)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  CQBaseModel::Type valueColumnType = columnValueType(model, valueColumn());

  //---

  int nr = model->rowCount();

  for (int r = 0; r < nr; ++r) {
    QStringList  nameStrs;
    ModelIndices nameInds;

    if (! getHierColumnNames(r, nameColumns(), separator(), nameStrs, nameInds))
      continue;

    QModelIndex nameInd1 = normalizeIndex(nameInds[0]);

    //---

    double size = 1.0;

    QModelIndex valueInd = model->index(r, valueColumn());

    if (valueInd.isValid()) {
      bool ok2 = true;

      if      (valueColumnType == CQBaseModel::Type::REAL)
        size = CQChartsUtil::modelReal(model, valueInd, ok2);
      else if (valueColumnType == CQBaseModel::Type::INTEGER)
        size = CQChartsUtil::modelInteger(model, valueInd, ok2);
      else
        ok2 = false;

      if (ok2 && size <= 0.0)
        ok2 = false;

      if (! ok2)
        continue;
    }

    //---

    CQChartsSunburstHierNode *parent = root;

    for (int j = 0; j < nameStrs.length() - 1; ++j) {
      CQChartsSunburstHierNode *child = nullptr;

      if (j == 0 && isMultiRoot()) {
        CQChartsSunburstRootNode *root = rootNode(nameStrs[j]);

        if (! root) {
          root = createRootNode(nameStrs[j]);

          root->setInd(nameInd1);
        }

        child = root;
      }
      else {
        child = childHierNode(parent, nameStrs[j]);

        if (! child) {
          // remove any existing leaf node (save size to use in new hier node)
          double size = 0.0;

          CQChartsSunburstNode *node = childNode(parent, nameStrs[j]);

          if (node) {
            size = node->size();

            parent->removeNode(node);

            delete node;
          }

          //---

          child = new CQChartsSunburstHierNode(parent, nameStrs[j]);

          child->setSize(size);

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

        continue;
      }

      //---

      node = new CQChartsSunburstNode(parent, name);

      node->setSize(size);

      OptColor color;

      if (colorSetColor(r, color))
        node->setColor(*color);

      if (valueInd.isValid()) {
        QModelIndex valueInd1 = normalizeIndex(valueInd);

        node->setInd(valueInd1);
      }
      else
        node->setInd(nameInd1);

      parent->addNode(node);
    }
  }

  //----

  for (auto &root : roots_)
    addExtraNodes(root);
}

void
CQChartsSunburstPlot::
addExtraNodes(CQChartsSunburstHierNode *hier)
{
  if (hier->size() > 0) {
    CQChartsSunburstNode *node = new CQChartsSunburstNode(hier, "");

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
  CQChartsSunburstRootNode *root = new CQChartsSunburstRootNode(name);

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

  menu->addSeparator();

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
  dataRange_.reset();

  CQChartsPlot::handleResize();
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
    brush.setStyle(CQChartsFillObj::patternToStyle((CQChartsFillObj::Pattern) fillPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (isBorder()) {
    QColor bc = interpBorderColor(0, 1);

    bc.setAlphaF(borderAlpha());

    pen.setColor (bc);
    pen.setWidthF(borderWidth());
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  QColor tc = interpTextColor(0, 1);

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

  QModelIndex nameInd  = plot_->selectIndex(ind.row(), plot_->nameColumn (), ind.parent());
  QModelIndex valueInd = plot_->selectIndex(ind.row(), plot_->valueColumn(), ind.parent());
  QModelIndex colorInd = plot_->selectIndex(ind.row(), plot_->colorColumn(), ind.parent());

  plot_->addSelectIndex(nameInd);

  if (valueInd.isValid())
    plot_->addSelectIndex(valueInd);

  if (colorInd.isValid())
    plot_->addSelectIndex(colorInd);
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
CQChartsSunburstHierNode(CQChartsSunburstHierNode *parent, const QString &name) :
 CQChartsSunburstNode(parent, name)
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
CQChartsSunburstNode(CQChartsSunburstHierNode *parent, const QString &name) :
 parent_(parent), id_(nextId()), name_(name)
{
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
