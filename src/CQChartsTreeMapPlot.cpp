#include <CQChartsTreeMapPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsFillObj.h>
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

  addColumnParameter("id", "Id", "idColumn", "optional");
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
 CQChartsHierPlot(view, view->charts()->plotType("treemap"), model)
{
  headerTextBoxObj_ = new CQChartsTextBoxObj(this);
  textBoxObj_       = new CQChartsTextBoxObj(this);

  headerTextBoxObj_->setBackgroundColor(CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE));
  textBoxObj_      ->setBackgroundColor(CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE));

  setHeaderBorder(true);
  setHeaderFilled(true);

  setBorder(true);
  setFilled(true);

  setTextContrast(true);

  headerTextBoxObj_->setTextFontSize(12.0);
  textBoxObj_      ->setTextFontSize(14.0);

  CQChartsPaletteColor textColor(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  headerTextBoxObj_->setTextColor(textColor);
  textBoxObj_      ->setTextColor(textColor);

  CQChartsPaletteColor headerFillColor(CQChartsPaletteColor::Type::THEME_VALUE, 0.4);

  headerTextBoxObj_->setBackgroundColor(headerFillColor);

  setMargins(1, 1, 1, 1);

  addTitle();
}

CQChartsTreeMapPlot::
~CQChartsTreeMapPlot()
{
  delete headerTextBoxObj_;
  delete textBoxObj_;

  delete root_;
}

//----

bool
CQChartsTreeMapPlot::
isHeaderFilled() const
{
  return headerTextBoxObj_->isBackground();
}

void
CQChartsTreeMapPlot::
setHeaderFilled(bool b)
{
  headerTextBoxObj_->setBackground(b);

  update();
}

QString
CQChartsTreeMapPlot::
headerFillColorStr() const
{
  return headerTextBoxObj_->backgroundColorStr();
}

void
CQChartsTreeMapPlot::
setHeaderFillColorStr(const QString &s)
{
  headerTextBoxObj_->setBackgroundColorStr(s);

  update();
}

QColor
CQChartsTreeMapPlot::
interpHeaderFillColor(int i, int n) const
{
  return headerTextBoxObj_->interpBackgroundColor(i, n);
}

double
CQChartsTreeMapPlot::
headerFillAlpha() const
{
  return headerTextBoxObj_->backgroundAlpha();
}

void
CQChartsTreeMapPlot::
setHeaderFillAlpha(double a)
{
  headerTextBoxObj_->setBackgroundAlpha(a);

  update();
}

//---

bool
CQChartsTreeMapPlot::
isHeaderBorder() const
{
  return headerTextBoxObj_->isBorder();
}

void
CQChartsTreeMapPlot::
setHeaderBorder(bool b)
{
  headerTextBoxObj_->setBorder(b);

  update();
}

QString
CQChartsTreeMapPlot::
headerBorderColorStr() const
{
  return headerTextBoxObj_->borderColorStr();
}

void
CQChartsTreeMapPlot::
setHeaderBorderColorStr(const QString &str)
{
  headerTextBoxObj_->setBorderColorStr(str);

  update();
}

QColor
CQChartsTreeMapPlot::
interpHeaderBorderColor(int i, int n) const
{
  return headerTextBoxObj_->interpBorderColor(i, n);
}

double
CQChartsTreeMapPlot::
headerBorderAlpha() const
{
  return headerTextBoxObj_->borderAlpha();
}

void
CQChartsTreeMapPlot::
setHeaderBorderAlpha(double a)
{
  headerTextBoxObj_->setBorderAlpha(a);

  update();
}

double
CQChartsTreeMapPlot::
headerBorderWidth() const
{
  return headerTextBoxObj_->borderWidth();
}

void
CQChartsTreeMapPlot::
setHeaderBorderWidth(double r)
{
  headerTextBoxObj_->setBorderWidth(r);

  update();
}

//---

const QFont &
CQChartsTreeMapPlot::
headerTextFont() const
{
  return headerTextBoxObj_->textFont();
}

void
CQChartsTreeMapPlot::
setHeaderTextFont(const QFont &f)
{
  headerTextBoxObj_->setTextFont(f);

  update();
}

QString
CQChartsTreeMapPlot::
headerTextColorStr() const
{
  return headerTextBoxObj_->textColorStr();
}

void
CQChartsTreeMapPlot::
setHeaderTextColorStr(const QString &s)
{
  headerTextBoxObj_->setTextColorStr(s);

  update();
}

QColor
CQChartsTreeMapPlot::
interpHeaderTextColor(int i, int n) const
{
  return headerTextBoxObj_->interpTextColor(i, n);
}

bool
CQChartsTreeMapPlot::
isHeaderTextContrast() const
{
  return headerTextBoxObj_->isTextContrast();
}

void
CQChartsTreeMapPlot::
setHeaderTextContrast(bool b)
{
  headerTextBoxObj_->setTextContrast(b);

  update();
}

//----

double
CQChartsTreeMapPlot::
calcHeaderHeight() const
{
  QFontMetricsF fm(headerTextFont());

  return std::max(headerHeight(), fm.height() + 4);
}

//----

bool
CQChartsTreeMapPlot::
isFilled() const
{
  return textBoxObj_->isBackground();
}

void
CQChartsTreeMapPlot::
setFilled(bool b)
{
  textBoxObj_->setBackground(b);

  update();
}

QString
CQChartsTreeMapPlot::
fillColorStr() const
{
  return textBoxObj_->backgroundColorStr();
}

void
CQChartsTreeMapPlot::
setFillColorStr(const QString &s)
{
  textBoxObj_->setBackgroundColorStr(s);

  update();
}

QColor
CQChartsTreeMapPlot::
interpFillColor(int i, int n) const
{
  return textBoxObj_->interpBackgroundColor(i, n);
}

double
CQChartsTreeMapPlot::
fillAlpha() const
{
  return textBoxObj_->backgroundAlpha();
}

void
CQChartsTreeMapPlot::
setFillAlpha(double a)
{
  textBoxObj_->setBackgroundAlpha(a);

  update();
}

CQChartsTreeMapPlot::Pattern
CQChartsTreeMapPlot::
fillPattern() const
{
  return (Pattern) textBoxObj_->backgroundPattern();
}

void
CQChartsTreeMapPlot::
setFillPattern(Pattern pattern)
{
  textBoxObj_->setBackgroundPattern((CQChartsBoxObj::Pattern) pattern);

  update();
}

//---

bool
CQChartsTreeMapPlot::
isBorder() const
{
  return textBoxObj_->isBorder();
}

void
CQChartsTreeMapPlot::
setBorder(bool b)
{
  textBoxObj_->setBorder(b);

  update();
}

QString
CQChartsTreeMapPlot::
borderColorStr() const
{
  return textBoxObj_->borderColorStr();
}

void
CQChartsTreeMapPlot::
setBorderColorStr(const QString &str)
{
  textBoxObj_->setBorderColorStr(str);

  update();
}

QColor
CQChartsTreeMapPlot::
interpBorderColor(int i, int n) const
{
  return textBoxObj_->interpBorderColor(i, n);
}

double
CQChartsTreeMapPlot::
borderAlpha() const
{
  return textBoxObj_->borderAlpha();
}

void
CQChartsTreeMapPlot::
setBorderAlpha(double a)
{
  textBoxObj_->setBorderAlpha(a);

  update();
}

double
CQChartsTreeMapPlot::
borderWidth() const
{
  return textBoxObj_->borderWidth();
}

void
CQChartsTreeMapPlot::
setBorderWidth(double r)
{
  textBoxObj_->setBorderWidth(r);

  update();
}

//---

const QFont &
CQChartsTreeMapPlot::
textFont() const
{
  return textBoxObj_->textFont();
}

void
CQChartsTreeMapPlot::
setTextFont(const QFont &f)
{
  textBoxObj_->setTextFont(f);

  update();
}

QString
CQChartsTreeMapPlot::
textColorStr() const
{
  return textBoxObj_->textColorStr();
}

void
CQChartsTreeMapPlot::
setTextColorStr(const QString &s)
{
  textBoxObj_->setTextColorStr(s);

  update();
}

QColor
CQChartsTreeMapPlot::
interpTextColor(int i, int n) const
{
  return textBoxObj_->interpTextColor(i, n);
}

bool
CQChartsTreeMapPlot::
isTextContrast() const
{
  return textBoxObj_->isTextContrast();
}

void
CQChartsTreeMapPlot::
setTextContrast(bool b)
{
  textBoxObj_->setTextContrast(b);

  update();
}

//---

void
CQChartsTreeMapPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumn" , "name" );
  addProperty("columns", this, "nameColumns", "names");
  addProperty("columns", this, "valueColumn", "value");
  addProperty("columns", this, "colorColumn", "color");
  addProperty("columns", this, "idColumn"   , "id"   );

  addProperty("", this, "separator"  );
  addProperty("", this, "marginWidth");

  addProperty("header", this, "titles"        , "visible"  );
  addProperty("header", this, "titleMaxExtent", "maxExtent");
  addProperty("header", this, "headerHeight"  , "height"   );

  addProperty("header/stroke", this, "headerBorder"     , "visible");
  addProperty("header/stroke", this, "headerBorderColor", "color"  );
  addProperty("header/stroke", this, "headerBorderAlpha", "alpha"  );
  addProperty("header/stroke", this, "headerBorderWidth", "width"  );

  addProperty("header/fill", this, "headerFilled"   , "visible");
  addProperty("header/fill", this, "headerFillColor", "color"  );
  addProperty("header/fill", this, "headerFillAlpha", "alpha"  );

  addProperty("header/text", this, "headerTextFont"    , "font"    );
  addProperty("header/text", this, "headerTextColor"   , "color"   );
  addProperty("header/text", this, "headerTextContrast", "contrast");

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
  addProperty("text", this, "textContrast", "contrast");

  addProperty("color", this, "colorMapEnabled", "mapEnabled");
  addProperty("color", this, "colorMapMin"    , "mapMin"    );
  addProperty("color", this, "colorMapMax"    , "mapMax"    );
}

//------

CQChartsTreeMapHierNode *
CQChartsTreeMapPlot::
currentRoot() const
{
  CQChartsTreeMapHierNode *currentRoot = root_;

  QStringList names = currentRootName_.split(separator(), QString::SkipEmptyParts);

  if (names.empty())
    return currentRoot;

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
CQChartsTreeMapPlot::
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
CQChartsTreeMapPlot::
colorSetColor(int i, OptColor &color)
{
  return colorSet_.icolor(i, color);
}

//------

void
CQChartsTreeMapPlot::
updateObjs()
{
  colorSet_.clear();

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
  if (colorSet_.empty())
    initColorSet();

  //---

  if (! root_)
    initNodes();

  //---

  initColorIds();

  colorNodes(root_);

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

  for (auto hierNode : hier->getChildren()) {
    initNodeObjs(hierNode, hierObj, depth + 1);
  }

  //---

  for (auto node : hier->getNodes()) {
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

  root_ = nullptr;
}

void
CQChartsTreeMapPlot::
initNodes()
{
  hierInd_ = 0;

  root_ = new CQChartsTreeMapHierNode(this, 0, "<root>");

  root_->setDepth(0);
  root_->setHierInd(hierInd_++);

  //---

  if (isHierarchical())
    loadChildren(root_);
  else
    loadFlat();

  //---

  firstHier_ = root_;

  while (firstHier_ && firstHier_->getChildren().size() == 1)
    firstHier_ = firstHier_->getChildren()[0];

  //---

  replaceNodes();
}

void
CQChartsTreeMapPlot::
replaceNodes()
{
  windowHeaderHeight_ = pixelToWindowHeight(calcHeaderHeight());
  windowMarginWidth_  = pixelToWindowWidth (marginWidth());

  if (currentRoot())
    placeNodes(currentRoot());
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
loadChildren(CQChartsTreeMapHierNode *hier, const QModelIndex &index, int depth)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //ColumnType valueColumnType = columnValueType(model, valueColumn());

  //---

  maxDepth_ = std::max(maxDepth_, depth + 1);

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
      CQChartsTreeMapHierNode *hier1 =
        new CQChartsTreeMapHierNode(this, hier, name, nameInd1);

      hier1->setDepth(depth);
      hier1->setHierInd(hierInd_++);

      loadChildren(hier1, nameInd, depth + 1);
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

      CQChartsTreeMapNode *node =
        new CQChartsTreeMapNode(this, hier, name, size, nameInd1);

      node->setDepth(depth);

      hier->addNode(node);
    }
  }
}

void
CQChartsTreeMapPlot::
loadFlat()
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  ColumnType valueColumnType = columnValueType(model, valueColumn());

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

      if      (valueColumnType == ColumnType::REAL)
        size = CQChartsUtil::modelReal(model, valueInd, ok2);
      else if (valueColumnType == ColumnType::INTEGER)
        size = CQChartsUtil::modelInteger(model, valueInd, ok2);
      else
        ok2 = false;

      if (ok2 && size <= 0.0)
        ok2 = false;

      if (! ok2)
        continue;
    }

    //---

    int depth = nameStrs.length();

    maxDepth_ = std::max(maxDepth_, depth + 1);

    //---

    CQChartsTreeMapHierNode *parent = root_;

    for (int j = 0; j < nameStrs.length() - 1; ++j) {
      CQChartsTreeMapHierNode *child = childHierNode(parent, nameStrs[j]);

      if (! child) {
        // remove any existing leaf node (save size to use in new hier node)
        double size = 0.0;

        CQChartsTreeMapNode *node = childNode(parent, nameStrs[j]);

        if (node) {
          nameInd1 = node->ind();
          size     = node->size();

          parent->removeNode(node);

          delete node;
        }

        //---

        child = new CQChartsTreeMapHierNode(this, parent, nameStrs[j], nameInd1);

        child->setSize(size);

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

        continue;
      }

      //---

      node = new CQChartsTreeMapNode(this, parent, name, size, nameInd1);

      node->setDepth(depth);

      OptColor color;

      if (colorSetColor(r, color))
        node->setColor(*color);

      parent->addNode(node);
    }
  }

  //----

  addExtraNodes(root_);
}

void
CQChartsTreeMapPlot::
addExtraNodes(CQChartsTreeMapHierNode *hier)
{
  if (hier->size() > 0) {
    CQChartsTreeMapNode *node =
      new CQChartsTreeMapNode(this, hier, "", hier->size(), hier->ind());

    int r = unnormalizeIndex(hier->ind()).row();

    OptColor color;

    if (colorSetColor(r, color))
      node->setColor(*color);

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

  selectedObjs(objs);

  QAction *pushAction   = new QAction("Push"   , menu);
  QAction *popAction    = new QAction("Pop"    , menu);
  QAction *popTopAction = new QAction("Pop Top", menu);

  connect(pushAction  , SIGNAL(triggered()), this, SLOT(pushSlot()));
  connect(popAction   , SIGNAL(triggered()), this, SLOT(popSlot()));
  connect(popTopAction, SIGNAL(triggered()), this, SLOT(popTopSlot()));

  pushAction  ->setEnabled(! objs.empty());
  popAction   ->setEnabled(currentRoot() != root_);
  popTopAction->setEnabled(currentRoot() != root_);

  menu->addSeparator();

  menu->addAction(pushAction  );
  menu->addAction(popAction   );
  menu->addAction(popTopAction);

  menu->addSeparator();

  return true;
}

void
CQChartsTreeMapPlot::
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

  if (root != root_) {
    setCurrentRoot(root_, /*update*/true);
  }
}

//------

void
CQChartsTreeMapPlot::
handleResize()
{
  if (isEqualScale())
    dataRange_.reset();

  replaceNodes();

  updateObjs();

  CQChartsPlot::handleResize();
}

void
CQChartsTreeMapPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
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

bool
CQChartsTreeMapHierObj::
isIndex(const QModelIndex &ind) const
{
  const QModelIndex &nind = hier_->ind();

  return (ind == nind);
}

void
CQChartsTreeMapHierObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  double px1, py1, px2, py2;

  plot_->windowToPixel(hier_->x()             , hier_->y()             , px1, py1);
  plot_->windowToPixel(hier_->x() + hier_->w(), hier_->y() + hier_->h(), px2, py2);

  QRectF qrect = CQChartsUtil::toQRect(CQChartsGeom::BBox(px1, py2, px2, py1));

  //---

  // calc header stroke and brush
  QBrush brush;

  if (plot_->isHeaderFilled()) {
    QColor hierColor = hier_->interpColor(plot_, plot_->numColorIds());

    QColor c = plot_->interpHeaderFillColor(0, 1);

    QColor c1 = CQChartsUtil::blendColors(c, hierColor, 0.8);

    c1.setAlphaF(plot_->headerFillAlpha());

    brush.setColor(c1);
    brush.setStyle(Qt::SolidPattern);
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (plot_->isHeaderBorder()) {
    QColor bc = plot_->interpHeaderBorderColor(0, 1);

    bc.setAlphaF(plot_->headerBorderAlpha());

    pen.setColor (bc);
    pen.setWidthF(plot_->headerBorderWidth());
  }
  else {
    pen = QPen(Qt::NoPen);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  QColor tc = plot_->interpHeaderTextColor(0, 1);

  QPen tpen(tc);

  plot_->updateObjPenBrushState(this, tpen, brush);

  //---

  painter->save();

  //---

  // draw rectangle
  painter->setPen  (pen);
  painter->setBrush(brush);

  painter->drawRect(qrect);

  //---

  // set font size
  QFont font = plot_->headerTextFont();

  //---

  // calc text size and position
  painter->setFont(font);

  QString name = hier_->name();

  QFontMetricsF fm(painter->font());

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

  if (plot_->idColumn() >= 0) {
    QAbstractItemModel *model = plot_->model();

    int r = plot_->unnormalizeIndex(node_->ind()).row();

    bool ok;

    QString idStr = CQChartsUtil::modelString(model, r, plot_->idColumn(), ok);

    if (ok)
      return idStr;
  }

  return QString("%1:%2").arg(node_->name()).arg(node_->hierSize());
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

  if (plot_->colorColumn() >= 0) {
    QAbstractItemModel *model = plot_->model();

    int r = plot_->unnormalizeIndex(node_->ind()).row();

    bool ok;

    QString colorStr = CQChartsUtil::modelString(model, r, plot_->colorColumn(), ok);

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
CQChartsTreeMapObj::
isIndex(const QModelIndex &ind) const
{
  const QModelIndex &nind = node_->ind();

  return (ind == nind);
}

void
CQChartsTreeMapObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  //CQChartsTreeMapHierNode *root = node_->rootNode(plot_->firstHier());

//CQChartsTreeMapHierNode *root = node_->parent();

  //---

  double px1, py1, px2, py2;

  plot_->windowToPixel(node_->x()             , node_->y()             , px1, py1);
  plot_->windowToPixel(node_->x() + node_->w(), node_->y() + node_->h(), px2, py2);

  QRectF qrect = CQChartsUtil::toQRect(CQChartsGeom::BBox(px1 + 1, py2 + 1, px2 - 1, py1 - 1));

  //---

  // calc stroke and brush
  QBrush brush;

  if (plot_->isFilled()) {
    QColor c = node_->interpColor(plot_, plot_->numColorIds());

    c.setAlphaF(plot_->fillAlpha());

    brush.setColor(c);
    brush.setStyle(CQChartsFillObj::patternToStyle(
      (CQChartsFillObj::Pattern) plot_->fillPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (plot_->isBorder()) {
    QColor bc = plot_->interpBorderColor(0, 1);

    bc.setAlphaF(plot_->borderAlpha());

    pen.setColor (bc);
    pen.setWidthF(plot_->borderWidth());
  }
  else {
    pen = QPen(Qt::NoPen);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  QColor tc = plot_->interpTextColor(0, 1);

  QPen tpen(tc);

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
  QFont font = plot_->textFont();

  painter->setFont(font);

  QFontMetricsF fm(painter->font());

  //---

  // calc text size and position
  QString name = (! node_->isFiller() ? node_->name() : node_->parent()->name());

  plot_->windowToPixel(node_->x() + node_->w()/2, node_->y() + node_->h()/2, px1, py1);

  //---

  double tw = fm.width(name);

  double fdy = (fm.ascent() - fm.descent())/2;

  //---

  // draw label
  painter->setClipRect(qrect);

  if (plot_->isTextContrast())
    plot_->drawContrastText(painter, px1 - tw/2, py1 + fdy, name, tpen);
  else {
    painter->setPen(tpen);

    painter->drawText(px1 - tw/2, py1 + fdy, name);
  }

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

  double maxExtent = CQChartsUtil::clamp(plot()->titleMaxExtent(), 0.0, 1.0);

  bool showTitle = (plot()->isTitles() && h*maxExtent > whh);

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
  if (parent() && parent() != plot()->root())
    return parent()->hierName() + "/" + name();
  else
    return name();
}

void
CQChartsTreeMapNode::
setPosition(double x, double y, double w, double h)
{
  assert(! CQChartsUtil::isNaN(x) && ! CQChartsUtil::isNaN(y) &&
         ! CQChartsUtil::isNaN(w) && ! CQChartsUtil::isNaN(h));

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
