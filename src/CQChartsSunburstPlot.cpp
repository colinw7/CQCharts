#include <CQChartsSunburstPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsBoxObj.h>
#include <CQChartsRenderer.h>
#include <CQRotatedText.h>
#include <CGradientPalette.h>

namespace {

int colorId   = -1;
int numColors = 0;

int nextColorId() {
  ++colorId;

  if (colorId >= numColors)
    numColors = colorId + 1;

  return colorId;
}

}

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
  addColumnParameter("name" , "Name" , "nameColumn" , "", 0);
  addColumnParameter("value", "Value", "valueColumn", "", 1);
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
 CQChartsPlot(view, view->charts()->plotType("sunburst"), model)
{
  boxObj_ = new CQChartsBoxObj(this);

  boxObj_->setBackgroundColor(CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE));

  setBorder(true);

  textFont_.setPointSizeF(8.0);

  textColor_ = CQChartsPaletteColor(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  setMargins(1, 1, 1, 1);

  // addKey() // TODO

  addTitle();
}

CQChartsSunburstPlot::
~CQChartsSunburstPlot()
{
  delete boxObj_;

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

QString
CQChartsSunburstPlot::
fillColorStr() const
{
  return boxObj_->backgroundColorStr();
}

void
CQChartsSunburstPlot::
setFillColorStr(const QString &s)
{
  boxObj_->setBackgroundColorStr(s);

  update();
}

QColor
CQChartsSunburstPlot::
interpFillColor(int i, int n) const
{
  return boxObj_->interpBackgroundColor(i, n);
}

double
CQChartsSunburstPlot::
fillAlpha() const
{
  return boxObj_->backgroundAlpha();
}

void
CQChartsSunburstPlot::
setFillAlpha(double a)
{
  boxObj_->setBackgroundAlpha(a);

  update();
}

bool
CQChartsSunburstPlot::
isBorder() const
{
  return boxObj_->isBorder();
}

void
CQChartsSunburstPlot::
setBorder(bool b)
{
  boxObj_->setBorder(b);

  update();
}

QString
CQChartsSunburstPlot::
borderColorStr() const
{
  return boxObj_->borderColorStr();
}

void
CQChartsSunburstPlot::
setBorderColorStr(const QString &str)
{
  boxObj_->setBorderColorStr(str);

  update();
}

QColor
CQChartsSunburstPlot::
interpBorderColor(int i, int n) const
{
  return boxObj_->interpBorderColor(i, n);
}

double
CQChartsSunburstPlot::
borderAlpha() const
{
  return boxObj_->borderAlpha();
}

void
CQChartsSunburstPlot::
setBorderAlpha(double a)
{
  boxObj_->setBorderAlpha(a);

  update();
}

double
CQChartsSunburstPlot::
borderWidth() const
{
  return boxObj_->borderWidth();
}

void
CQChartsSunburstPlot::
setBorderWidth(double r)
{
  boxObj_->setBorderWidth(r);

  update();
}

QColor
CQChartsSunburstPlot::
interpTextColor(int i, int n) const
{
  return textColor_.interpColor(this, i, n);
}

void
CQChartsSunburstPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty(""      , this, "innerRadius"             );
  addProperty(""      , this, "outerRadius"             );
  addProperty(""      , this, "startAngle"              );
  addProperty("fill"  , this, "fillColor"  , "color"    );
  addProperty("fill"  , this, "fillAlpha"  , "alpha"    );
  addProperty("border", this, "border"     , "displayed");
  addProperty("border", this, "borderColor", "color"    );
  addProperty("border", this, "borderAlpha", "alpha"    );
  addProperty("border", this, "borderWidth", "width"    );
  addProperty("text"  , this, "textFont"   , "font"     );
  addProperty("text"  , this, "textColor"  , "color"    );
}

void
CQChartsSunburstPlot::
updateRange(bool apply)
{
  double xr = 1.0;
  double yr = 1.0;

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

  if (roots_.empty()) {
    colorId  = -1;
    numColors = 0;

    CQChartsSunburstRootNode *root = new CQChartsSunburstRootNode;

    roots_.push_back(root);

    loadChildren(root);

    //---

    roots_[0]->packNodes(innerRadius(), outerRadius(), 0.0, startAngle(), 360);
  }

  //---

  addPlotObjs(roots_[0]);

  //---

  return true;
}

void
CQChartsSunburstPlot::
loadChildren(CQChartsSunburstHierNode *hier, const QModelIndex &index, int depth, int colorId)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  if (depth == 3)
    colorId = nextColorId();

  int colorId1 = 0;

  uint nc = model->rowCount(index);

  for (uint i = 0; i < nc; ++i) {
    QModelIndex nameInd  = model->index(i, nameColumn (), index);
    QModelIndex valueInd = model->index(i, valueColumn(), index);

    bool ok;

    QString name = CQChartsUtil::modelString(model, nameInd, ok);

    //---

    if (model->rowCount(nameInd) > 0) {
      CQChartsSunburstHierNode *hier1 = new CQChartsSunburstHierNode(hier, name);

      loadChildren(hier1, nameInd, depth + 1, colorId);

      hier1->setInd(normalizeIndex(nameInd));

      colorId1 = hier1->colorId();
    }
    else {
      bool ok;

      int size = CQChartsUtil::modelInteger(model, valueInd, ok);

      if (! ok) size = 1;

      //---

      CQChartsSunburstNode *node = new CQChartsSunburstNode(hier, name);

      node->setSize(size);
      node->setColorId(colorId);

      node->setInd(normalizeIndex(valueInd));

      hier->addNode(node);

      colorId1 = node->colorId();
    }
  }

  hier->setColorId(colorId1);
}

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

void
CQChartsSunburstPlot::
handleResize()
{
  dataRange_.reset();

  CQChartsPlot::handleResize();
}

void
CQChartsSunburstPlot::
draw(CQChartsRenderer *renderer)
{
  initPlotObjs();

  //---

  drawParts(renderer);
}

void
CQChartsSunburstPlot::
drawNodes(CQChartsRenderer *renderer, CQChartsSunburstHierNode *hier)
{
  for (auto node : hier->getNodes())
    drawNode(renderer, nullptr, node);

  //------

  for (auto hierNode : hier->getChildren()) {
    drawNode(renderer, nullptr, hierNode);

    drawNodes(renderer, hierNode);
  }
}

void
CQChartsSunburstPlot::
drawNode(CQChartsRenderer *renderer, CQChartsSunburstNodeObj *nodeObj, CQChartsSunburstNode *node)
{
  if (! node->placed()) return;

  double xc = 0.0;
  double yc = 0.0;

  double r1 = node->r();
  double r2 = r1 + node->dr();

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

  //a1 = 90 - a1;
  //a2 = 90 - a2;

  //---

  // create arc path
  QPainterPath path;

  path.arcMoveTo(qr1, a1);

  path.arcTo(qr1, a1, da);
  path.arcTo(qr2, a2, -da);

  path.closeSubpath();

  //---

  // calc stroke and brush

  QColor fillColor = interpFillColor(node->colorId(), numColors);

  fillColor.setAlphaF(fillAlpha());

  QBrush brush(fillColor);

  QPen pen;

  if (isBorder()) {
    QColor bc = interpBorderColor(0, 1);

    bc.setAlphaF(borderAlpha());

    pen = QPen(bc);

    pen.setWidthF(borderWidth());
  }
  else
    pen = QPen(Qt::NoPen);

  if (nodeObj)
    updateObjPenBrushState(nodeObj, pen, brush);

  //---

  // draw path
  renderer->setPen  (pen);
  renderer->setBrush(brush);

  renderer->drawPath(path);

  //---

  // draw node label
  QColor tc = interpTextColor(0, 1);

  QPen tpen(tc);

  if (nodeObj)
    updateObjPenBrushState(nodeObj, tpen, brush);

  renderer->setPen(tpen);

  QFont font = textFont();

  renderer->setFont(font);

  double ta = a1 + da/2.0;
  double c  = cos(ta*M_PI/180.0);
  double s  = sin(ta*M_PI/180.0);

  double r3 = CQChartsUtil::avg(r1, r2);

  double tx = r3*c;
  double ty = r3*s;

  double px, py;

  windowToPixel(tx, ty, px, py);

  QString str = node->name();

  Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

  if (c >= 0)
    CQRotatedText::drawRotatedText(renderer, px, py, str, ta, align);
  else
    CQRotatedText::drawRotatedText(renderer, px, py, str, ta - 180, align);
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
  return QString("%1:%2").arg(node_->name()).arg(node_->size());
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
mousePress(const CQChartsGeom::Point &)
{
  plot_->beginSelect();

  plot_->addSelectIndex(node_->ind().row(), plot_->nameColumn (), node_->ind().parent());
  plot_->addSelectIndex(node_->ind().row(), plot_->valueColumn(), node_->ind().parent());

  plot_->endSelect();
}

bool
CQChartsSunburstNodeObj::
isIndex(const QModelIndex &ind) const
{
  return (ind == node_->ind());
}

void
CQChartsSunburstNodeObj::
draw(CQChartsRenderer *renderer, const CQChartsPlot::Layer &)
{
  plot_->drawNode(renderer, this, node_);
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
size() const
{
  double s = 0.0;

  for (const auto &child : children_)
    s += child->size();

  for (const auto &node : nodes_)
    s += node->size();

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

  double s = (order == Order::SIZE ? size() : numNodes());

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
    double s = (order == Order::SIZE ? node->size() : node->numNodes());

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
