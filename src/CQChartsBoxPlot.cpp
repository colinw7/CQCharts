#include <CQChartsBoxPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsRoundedPolygon.h>
#include <QPainter>

CQChartsBoxPlotType::
CQChartsBoxPlotType()
{
}

void
CQChartsBoxPlotType::
addParameters()
{
  addColumnParameter("x"    , "X"    , "xColumn"    , "", 0);
  addColumnParameter("y"    , "Y"    , "yColumn"    , "", 1);
  addColumnParameter("group", "Group", "groupColumn", "optional");
}

CQChartsPlot *
CQChartsBoxPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsBoxPlot(view, model);
}

//---

CQChartsBoxPlot::
CQChartsBoxPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("box"), model)
{
  boxObj_  = new CQChartsBoxObj(this);
  textObj_ = new CQChartsTextBoxObj(this);

  CQChartsColor bg(CQChartsColor::Type::PALETTE);

  boxObj_->setBackground(true);
  boxObj_->setBackgroundColor(bg);

  boxObj_->setBorder(true);

  addAxes();

  addKey();

  addTitle();

  xAxis_->setIntegral(true);
}

CQChartsBoxPlot::
~CQChartsBoxPlot()
{
  delete boxObj_;
  delete textObj_;
}

//------

bool
CQChartsBoxPlot::
isBoxFilled() const
{
  return boxObj_->isBackground();
}

void
CQChartsBoxPlot::
setBoxFilled(bool b)
{
  boxObj_->setBackground(b);

  update();
}

const CQChartsColor &
CQChartsBoxPlot::
boxColor() const
{
  return boxObj_->backgroundColor();
}

void
CQChartsBoxPlot::
setBoxColor(const CQChartsColor &c)
{
  boxObj_->setBackgroundColor(c);

  update();
}

QColor
CQChartsBoxPlot::
interpBoxColor(int i, int n) const
{
  return boxObj_->interpBackgroundColor(i, n);
}

double
CQChartsBoxPlot::
boxAlpha() const
{
  return boxObj_->backgroundAlpha();
}

void
CQChartsBoxPlot::
setBoxAlpha(double a)
{
  boxObj_->setBackgroundAlpha(a);

  update();
}

CQChartsBoxPlot::Pattern
CQChartsBoxPlot::
boxPattern() const
{
  return (Pattern) boxObj_->backgroundPattern();
}

void
CQChartsBoxPlot::
setBoxPattern(Pattern pattern)
{
  boxObj_->setBackgroundPattern((CQChartsBoxObj::Pattern) pattern);

  update();
}

//------

bool
CQChartsBoxPlot::
isBorderStroked() const
{
  return boxObj_->isBorder();
}

void
CQChartsBoxPlot::
setBorderStroked(bool b)
{
  return boxObj_->setBorder(b);
}

const CQChartsColor &
CQChartsBoxPlot::
borderColor() const
{
  return boxObj_->borderColor();
}

void
CQChartsBoxPlot::
setBorderColor(const CQChartsColor &c)
{
  boxObj_->setBorderColor(c);

  update();
}

QColor
CQChartsBoxPlot::
interpBorderColor(int i, int n) const
{
  return boxObj_->interpBorderColor(i, n);
}

double
CQChartsBoxPlot::
borderAlpha() const
{
  return boxObj_->borderAlpha();
}

void
CQChartsBoxPlot::
setBorderAlpha(double r)
{
  boxObj_->setBorderAlpha(r);

  update();
}

const CQChartsLength &
CQChartsBoxPlot::
borderWidth() const
{
  return boxObj_->borderWidth();
}

void
CQChartsBoxPlot::
setBorderWidth(const CQChartsLength &l)
{
  boxObj_->setBorderWidth(l);

  update();
}

double
CQChartsBoxPlot::
cornerSize() const
{
  return boxObj_->cornerSize();
}

void
CQChartsBoxPlot::
setCornerSize(double r)
{
  boxObj_->setCornerSize(r);

  update();
}

//------

const CQChartsColor &
CQChartsBoxPlot::
whiskerColor() const
{
  return whiskerData_.color;
}

void
CQChartsBoxPlot::
setWhiskerColor(const CQChartsColor &c)
{
  whiskerData_.color = c;

  update();
}

QColor
CQChartsBoxPlot::
interpWhiskerColor(int i, int n) const
{
  return whiskerColor().interpColor(this, i, n);
}

//------

const CQChartsColor &
CQChartsBoxPlot::
textColor() const
{
  return textObj_->textColor();
}

void
CQChartsBoxPlot::
setTextColor(const CQChartsColor &c)
{
  textObj_->setTextColor(c);

  update();
}

QColor
CQChartsBoxPlot::
interpTextColor(int i, int n) const
{
  return textObj_->interpTextColor(i, n);
}

const QFont &
CQChartsBoxPlot::
font() const
{
  return textObj_->textFont();
}

void
CQChartsBoxPlot::
setFont(const QFont &f)
{
  textObj_->setTextFont(f);

  update();
}

//---

void
CQChartsBoxPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "xColumn"    , "x"    );
  addProperty("columns", this, "yColumn"    , "y"    );
  addProperty("columns", this, "groupColumn", "group");

  addProperty("", this, "skipOutliers", "skipOutliers");
  addProperty("", this, "connected"   , "connected"   );
  addProperty("", this, "whiskerRange", "whiskerRange");
  addProperty("", this, "boxWidth"    , "boxWidth"    );

  addProperty("stroke", this, "boxStroked" , "visible"   );
  addProperty("stroke", this, "borderColor", "color"     );
  addProperty("stroke", this, "borderAlpha", "alpha"     );
  addProperty("stroke", this, "borderWidth", "width"     );
  addProperty("stroke", this, "cornerSize" , "cornerSize");

  addProperty("fill", this, "boxFilled" , "visible");
  addProperty("fill", this, "boxColor"  , "color"  );
  addProperty("fill", this, "boxAlpha"  , "alpha"  );
  addProperty("fill", this, "boxPattern", "pattern");

  addProperty("whisker", this, "whiskerColor"    , "color" );
  addProperty("whisker", this, "whiskerLineWidth", "width" );
  addProperty("whisker", this, "whiskerExtent"   , "extent");

  addProperty("text", this, "textColor" , "color" );
  addProperty("text", this, "font"      , "font"  );
  addProperty("text", this, "textMargin", "margin");

  addProperty("outlier", this, "symbolSize", "size");
}

void
CQChartsBoxPlot::
updateRange(bool apply)
{
  CScopeTimer timer("updateRange");

  //---

  updateWhiskers();

  //---

  dataRange_.reset();

  xAxis_->clearTickLabels();

  int ig = 0;

  for (const auto &groupWhiskers : this->groupWhiskers()) {
    const SetWhiskerMap &setWhiskerMap = groupWhiskers.second;

    int is = 0;

    for (const auto &setWhiskers : setWhiskerMap) {
      bool hidden = (isGrouped() ? isSetHidden(ig) : isSetHidden(is));

      if (! hidden) {
        int setId = setWhiskers.first;

        QString setIdName = this->setIdName(setId);

        xAxis_->setTickLabel(setId, setIdName);

        //---

        const CQChartsBoxPlotWhisker &whisker = setWhiskers.second;

        double min, max;

        if (! isSkipOutliers()) {
          min = whisker.rvalue(0);
          max = whisker.rvalue(whisker.numValues() - 1);
        }
        else {
          min = whisker.min();
          max = whisker.max();
        }

        dataRange_.updateRange(setId - 0.5, min);
        dataRange_.updateRange(setId + 0.5, max);
      }

      ++is;
    }

    ++ig;
  }

  //---

  xAxis_->setColumn(xColumn());
  yAxis_->setColumn(yColumn());

  QAbstractItemModel *model = this->model();

  if (model) {
    bool ok;

    QString xname = CQChartsUtil::modelHeaderString(model, xColumn(), ok);
    QString yname = CQChartsUtil::modelHeaderString(model, yColumn(), ok);

    xAxis_->setLabel(xname);
    yAxis_->setLabel(yname);
  }

  //---

  if (apply)
    applyDataRange();
}

void
CQChartsBoxPlot::
updateWhiskers()
{
  CScopeTimer timer("updateWhiskers");

  //---

  groupWhiskers_.clear();

  //---

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  // determine group data type
  groupType_ = ColumnType::NONE;

  if (groupColumn() >= 0)
    groupType_ = columnValueType(model, groupColumn());

  groupValueInd_.clear();

  // determine x data type
  xType_ = columnValueType(model, xColumn());

  xValueInd_.clear();

  //---

  // process model data
  class BoxPlotVisitor : public ModelVisitor {
   public:
    BoxPlotVisitor(CQChartsBoxPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *model, const QModelIndex &ind, int row) override {
      plot_->addWhiskerRow(model, ind, row);

      return State::OK;
    }

   private:
    CQChartsBoxPlot *plot_ { nullptr };
  };

  BoxPlotVisitor boxPlotVisitor(this);

  visitModel(boxPlotVisitor);

  //---

  grouped_ = (numGroups() > 1);

  //---

  for (auto &groupWhiskers : groupWhiskers_) {
    SetWhiskerMap &setWhiskerMap = groupWhiskers.second;

    for (auto &setWhiskers : setWhiskerMap)
      setWhiskers.second.init();
  }
}

void
CQChartsBoxPlot::
addWhiskerRow(QAbstractItemModel *model, const QModelIndex &parent, int r)
{
  // get group
  int groupId = -1;

  if (groupColumn() >= 0) {
    QModelIndex groupInd = model->index(r, groupColumn(), parent);

    // get value set id
    if      (groupType_ == ColumnType::INTEGER) {
      bool ok1;

      int i = CQChartsUtil::modelInteger(model, groupInd, ok1);

      groupId = groupValueInd_.calcId(i);
    }
    else if (groupType_ == ColumnType::REAL) {
      bool ok1;

      double r = CQChartsUtil::modelReal(model, groupInd, ok1);

      if (CQChartsUtil::isNaN(r))
        return;

      groupId = groupValueInd_.calcId(r);
    }
    else {
      bool ok1;

      QString s = CQChartsUtil::modelString(model, groupInd, ok1);

      groupId = groupValueInd_.calcId(s);
    }
  }

  //---

  QModelIndex xind = model->index(r, xColumn(), parent);

  QModelIndex xind1 = normalizeIndex(xind);

  //---

  // get value set id
  int setId = -1;

  if      (xType_ == ColumnType::INTEGER) {
    bool ok1;

    int i = CQChartsUtil::modelInteger(model, xind, ok1);

    setId = xValueInd_.calcId(i);
  }
  else if (xType_ == ColumnType::REAL) {
    bool ok1;

    double r = CQChartsUtil::modelReal(model, xind, ok1);

    if (CQChartsUtil::isNaN(r))
      return;

    setId = xValueInd_.calcId(r);
  }
  else {
    bool ok1;

    QString s = CQChartsUtil::modelString(model, xind, ok1);

    setId = xValueInd_.calcId(s);
  }

  //---

  // add value to set
  QModelIndex yind = model->index(r, yColumn(), parent);

  bool ok2;

  double value = CQChartsUtil::modelReal(model, yind, ok2);

  if (! ok2) value = r;

  if (CQChartsUtil::isNaN(value))
    return;

  CQChartsBoxPlotValue wv(value, xind1);

  auto p = groupWhiskers_.find(groupId);

  if (p == groupWhiskers_.end()) {
    SetWhiskerMap setWhiskerMap;

    p = groupWhiskers_.insert(p, GroupSetWhiskerMap::value_type(groupId, SetWhiskerMap()));
  }

  SetWhiskerMap &setWhiskerMap = (*p).second;

  auto p1 = setWhiskerMap.find(setId);

  if (p1 == setWhiskerMap.end()) {
    CQChartsBoxPlotWhisker whisker;

    whisker.setRange(whiskerRange());

    p1 = setWhiskerMap.insert(p1, SetWhiskerMap::value_type(setId, whisker));
  }

  (*p1).second.addValue(wv);
}

CQChartsGeom::BBox
CQChartsBoxPlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    CQChartsBoxPlotObj *boxObj = dynamic_cast<CQChartsBoxPlotObj *>(plotObj);

    if (boxObj)
      bbox += boxObj->annotationBBox();
  }

  return bbox;
}

//------

bool
CQChartsBoxPlot::
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

  int ig = 0;
  int ng = numGroups();

  for (const auto &groupWhiskers : this->groupWhiskers()) {
    int groupId = groupWhiskers.first;

    const SetWhiskerMap &setWhiskerMap = groupWhiskers.second;

    if (! isConnected()) {
      int is = 0;
      int ns = setWhiskerMap.size();

      for (const auto &setWhiskers : setWhiskerMap) {
        bool hidden = (isGrouped() ? isSetHidden(ig) : isSetHidden(is));

        if (! hidden) {
          int setId = setWhiskers.first;

          const CQChartsBoxPlotWhisker &whisker = setWhiskers.second;

          //----

          CQChartsGeom::BBox rect(setId - 0.10, whisker.lower(), setId + 0.10, whisker.upper());

          CQChartsBoxPlotObj *boxObj =
            new CQChartsBoxPlotObj(this, rect, setId, whisker, ig, ng, is, ns);

          addPlotObject(boxObj);
        }

        ++is;
      }
    }
    else {
      bool hidden = (isGrouped() ? isSetHidden(ig) : false);

      if (! hidden) {
        CQChartsGeom::BBox rect(dataRange_.xmin(), dataRange_.ymin(),
                                dataRange_.xmax(), dataRange_.ymax());

        CQChartsBoxPlotConnectedObj *connectedObj =
          new CQChartsBoxPlotConnectedObj(this, rect, groupId, ig, ng);

        addPlotObject(connectedObj);
      }
    }

    ++ig;
  }

  //---

  resetKeyItems();

  //---

  return true;
}

void
CQChartsBoxPlot::
addKeyItems(CQChartsPlotKey *key)
{
  int ig = 0;
  int ng = numGroups();

  for (const auto &groupWhiskers : this->groupWhiskers()) {
    int groupId = groupWhiskers.first;

    QString groupName = this->groupIdName(groupId);

    const SetWhiskerMap &setWhiskerMap = groupWhiskers.second;

    int is = 0;
    int ns = setWhiskerMap.size();

    for (const auto &setWhiskers : setWhiskerMap) {
      int setId = setWhiskers.first;

      QString setName = this->setIdName(setId);

      QString name = (isGrouped() ? groupName : setName);

      int i = (isGrouped() ? ig : is);
      int n = (isGrouped() ? ng : ns);

      CQChartsBoxKeyColor *color = new CQChartsBoxKeyColor(this, i, n);
      CQChartsBoxKeyText  *text  = new CQChartsBoxKeyText (this, i, name);

      key->addItem(color, i, 0);
      key->addItem(text , i, 1);

      if (isGrouped())
        break;

      ++is;
    }

    ++ig;
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

bool
CQChartsBoxPlot::
probe(ProbeData &probeData) const
{
  if (probeData.x < dataRange_.xmin() + 0.5)
    probeData.x = dataRange_.xmin() + 0.5;

  if (probeData.x > dataRange_.xmax() - 0.5)
    probeData.x = dataRange_.xmax() - 0.5;

  probeData.x = std::round(probeData.x);

  return true;
}

void
CQChartsBoxPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

//------

CQChartsBoxPlotObj::
CQChartsBoxPlotObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect, int setId,
                   const CQChartsBoxPlotWhisker &whisker, int ig, int ng, int is, int ns) :
 CQChartsPlotObj(plot, rect), plot_(plot), setId_(setId), whisker_(whisker),
 ig_(ig), ng_(ng), is_(is), ns_(ns)
{
}

QString
CQChartsBoxPlotObj::
calcId() const
{
  QString setName = plot_->setIdName(setId_);

  return QString("%1:%2:%3").arg(setName).arg(whisker_.lower()).arg(whisker_.upper());
}

void
CQChartsBoxPlotObj::
addSelectIndex()
{
  for (auto value : whisker_.values()) {
    plot_->addSelectIndex(value.ind.row(), plot_->xColumn(), value.ind.parent());
    plot_->addSelectIndex(value.ind.row(), plot_->yColumn(), value.ind.parent());
  }
}

bool
CQChartsBoxPlotObj::
isIndex(const QModelIndex &ind) const
{
  for (auto value : whisker_.values()) {
    if (ind == value.ind)
      return true;
  }

  return false;
}

void
CQChartsBoxPlotObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  QFontMetricsF fm(plot_->font());

  double yf = (fm.ascent() - fm.descent())/2.0;

  //---

  double wd1 = plot_->whiskerExtent()/2.0;
  double wd2 = plot_->lengthPlotWidth(plot_->boxWidth())/2;

  double px1, py1, px2, py2, px3, py3, px4, py4, px5, py5;

  plot_->windowToPixel(setId_ - wd1, whisker_.min   (), px1, py1);
  plot_->windowToPixel(setId_ - wd2, whisker_.lower (), px2, py2);
  plot_->windowToPixel(setId_      , whisker_.median(), px3, py3);
  plot_->windowToPixel(setId_ + wd2, whisker_.upper (), px4, py4);
  plot_->windowToPixel(setId_ + wd1, whisker_.max   (), px5, py5);

  //---

  QColor whiskerColor     = plot_->interpWhiskerColor(0, 1);
  double whiskerLineWidth = plot_->lengthPixelWidth(plot_->whiskerLineWidth());

  //---

  // draw extent line
  painter->setPen(QPen(whiskerColor, whiskerLineWidth, Qt::SolidLine));

  painter->drawLine(QPointF(px3, py1), QPointF(px3, py5));

  //---

  // draw lower/upper horizontal lines
  painter->setPen(QPen(whiskerColor, whiskerLineWidth, Qt::SolidLine));

  painter->drawLine(QPointF(px1, py1), QPointF(px5, py1));
  painter->drawLine(QPointF(px1, py5), QPointF(px5, py5));

  //---

  // draw box
  QRectF rect(px2, py2, px4 - px2, py4 - py2);

  // set fill and stroke
  QBrush brush;

  if (plot_->isBoxFilled()) {
    QColor boxColor = (plot_->isGrouped() ? plot_->interpBoxColor(ig_, ng_) :
                                            plot_->interpBoxColor(is_, ns_));

    boxColor.setAlphaF(plot_->boxAlpha());

    brush.setColor(boxColor);
    brush.setStyle(CQChartsFillPattern::toStyle(
     (CQChartsFillPattern::Type) plot_->boxPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (plot_->isBorderStroked()) {
    QColor borderColor = (plot_->isGrouped() ? plot_->interpBorderColor(ig_, ng_) :
                                               plot_->interpBorderColor(is_, ns_));

    borderColor.setAlphaF(plot_->borderAlpha());

    double bw = plot_->lengthPixelWidth(plot_->borderWidth());

    pen.setColor (borderColor);
    pen.setWidthF(bw);
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setBrush(brush);
  painter->setPen  (pen);

  CQChartsRoundedPolygon::draw(painter, rect, plot_->cornerSize());

  //---

  // draw median line
  painter->setPen(QPen(whiskerColor, whiskerLineWidth, Qt::SolidLine));

  painter->drawLine(QPointF(px2, py3), QPointF(px4, py3));

  //---

  // draw labels
  double margin = plot_->textMargin();

  painter->setFont(plot_->font());

  QColor textColor = plot_->interpTextColor(0, 1);

  painter->setPen(textColor);

  QString strl = QString("%1").arg(whisker_.min   ());
  QString lstr = QString("%1").arg(whisker_.lower ());
  QString mstr = QString("%1").arg(whisker_.median());
  QString ustr = QString("%1").arg(whisker_.upper ());
  QString strh = QString("%1").arg(whisker_.max   ());

  painter->drawText(QPointF(px5 + margin                 , py1 + yf), strl);
  painter->drawText(QPointF(px2 - margin - fm.width(lstr), py2 + yf), lstr);
  painter->drawText(QPointF(px4 + margin                 , py3 + yf), mstr);
  painter->drawText(QPointF(px2 - margin - fm.width(ustr), py4 + yf), ustr);
  painter->drawText(QPointF(px5 + margin                 , py5 + yf), strh);

  //---

  // draw outlier symbols
  if (! plot_->isSkipOutliers()) {
    painter->setPen(QPen(whiskerColor, whiskerLineWidth, Qt::SolidLine));

    painter->setBrush(brush);
    painter->setPen  (pen);

    double symbolSize = plot_->symbolSize();

    for (auto o : whisker_.outliers()) {
      double px1, py1;

      plot_->windowToPixel(setId_, whisker_.rvalue(o), px1, py1);

      QRectF rect(px1 - symbolSize, py1 - symbolSize, 2*symbolSize, 2*symbolSize);

      painter->drawEllipse(rect);
    }
  }
}

CQChartsGeom::BBox
CQChartsBoxPlotObj::
annotationBBox() const
{
  double margin = plot_->textMargin();

  QFontMetricsF fm(plot_->font());

  double fa = fm.ascent ();
  double fd = fm.descent();
  double yf = (fa - fd)/2.0;

  //---

  double wd1 = plot_->whiskerExtent()/2.0;
  double wd2 = plot_->lengthPlotWidth(plot_->boxWidth())/2;

  double px1, py1, px2, py2, px3, py3, px4, py4, px5, py5;

  plot_->windowToPixel(setId_ - wd1, whisker_.min   (), px1, py1);
  plot_->windowToPixel(setId_ - wd2, whisker_.lower (), px2, py2);
  plot_->windowToPixel(setId_      , whisker_.median(), px3, py3);
  plot_->windowToPixel(setId_ + wd2, whisker_.upper (), px4, py4);
  plot_->windowToPixel(setId_ + wd1, whisker_.max   (), px5, py5);

  //---

  QString strl = QString("%1").arg(whisker_.min   ());
  QString lstr = QString("%1").arg(whisker_.lower ());
  QString mstr = QString("%1").arg(whisker_.median());
  QString ustr = QString("%1").arg(whisker_.upper ());
  QString strh = QString("%1").arg(whisker_.max   ());

  CQChartsGeom::BBox pbbox;

  pbbox += CQChartsGeom::Point(px5 + margin + fm.width(strl), py1 + yf + fd);
  pbbox += CQChartsGeom::Point(px2 - margin - fm.width(lstr), py2 + yf + fd);
  pbbox += CQChartsGeom::Point(px4 + margin + fm.width(mstr), py3 + yf);
  pbbox += CQChartsGeom::Point(px2 - margin - fm.width(ustr), py4 + yf - fa);
  pbbox += CQChartsGeom::Point(px5 + margin + fm.width(strh), py5 + yf - fa);

  //---

  CQChartsGeom::BBox bbox;

  plot_->pixelToWindow(pbbox, bbox);

  return bbox;
}

//------

CQChartsBoxPlotConnectedObj::
CQChartsBoxPlotConnectedObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                            int groupId, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupId_(groupId), i_(i), n_(n)
{
  initPolygon();
}

QString
CQChartsBoxPlotConnectedObj::
calcId() const
{
  QString groupName = plot_->groupIdName(groupId_);

  return QString("%1").arg(groupName);
}

void
CQChartsBoxPlotConnectedObj::
initPolygon()
{
  QPolygonF maxPoly, minPoly;

  int i = 0;

  for (const auto &groupWhiskers : plot_->groupWhiskers()) {
    const CQChartsBoxPlot::SetWhiskerMap &setWhiskerMap = groupWhiskers.second;

    if (i == i_) {
      for (const auto &setWhiskers : setWhiskerMap) {
        double                        pos     = setWhiskers.first;
        const CQChartsBoxPlotWhisker &whisker = setWhiskers.second;

        double min    = whisker.min   ();
        double max    = whisker.max   ();
        double median = whisker.median();

        line_ << CQChartsUtil::toQPoint(CQChartsGeom::Point(pos, median));

        maxPoly << CQChartsUtil::toQPoint(CQChartsGeom::Point(pos, max));
        minPoly << CQChartsUtil::toQPoint(CQChartsGeom::Point(pos, min));
      }

      break;
    }

    ++i;
  }

  int np = maxPoly.count();

  for (int i = 0; i < np; ++i)
    poly_ << maxPoly.at(i);

  for (int i = 0; i < np; ++i)
    poly_ << minPoly.at(np - 1 - i);
}

bool
CQChartsBoxPlotConnectedObj::
inside(const CQChartsGeom::Point &p) const
{
  return poly_.containsPoint(CQChartsUtil::toQPoint(p), Qt::OddEvenFill);
}

void
CQChartsBoxPlotConnectedObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  // draw range polygon
  int np = poly_.count();

  if (np) {
    QPainterPath path;

    path.moveTo(plot_->windowToPixel(poly_.at(0)));

    for (int i = 1; i < np; ++i)
      path.lineTo(plot_->windowToPixel(poly_.at(i)));

    path.closeSubpath();

    QColor fillColor = plot_->interpBoxColor(i_, n_);

    QBrush pbrush;

    if (plot_->isBoxFilled()) {
      fillColor.setAlphaF(plot_->boxAlpha());

      pbrush.setColor(fillColor);
      pbrush.setStyle(CQChartsFillPattern::toStyle(
       (CQChartsFillPattern::Type) plot_->boxPattern()));
    }

    QPen ppen;

    if (plot_->isBorderStroked()) {
      QColor borderColor = plot_->interpBorderColor(i_, n_);

      borderColor.setAlphaF(plot_->borderAlpha());

      double bw = plot_->lengthPixelWidth(plot_->borderWidth());

      ppen.setColor (borderColor);
      ppen.setWidthF(bw);
    }
    else {
      ppen.setStyle(Qt::NoPen);
    }

    plot_->updateObjPenBrushState(this, ppen, pbrush);

    painter->setPen  (ppen);
    painter->setBrush(pbrush);

    painter->drawPath(path);
  }

  //---

  // draw median line
  QPolygonF line;

  for (int i = 0; i < line_.count(); ++i)
    line << plot_->windowToPixel(line_.at(i));

  QPen lpen;

  QColor lineColor = plot_->interpBorderColor(i_, n_);

  lineColor.setAlphaF(plot_->borderAlpha());

  double bw = plot_->lengthPixelWidth(plot_->borderWidth());

  lpen.setColor (lineColor);
  lpen.setWidthF(bw);

  QBrush lbrush;

  plot_->updateObjPenBrushState(this, lpen, lbrush);

  painter->setPen(lpen);

  painter->drawPolyline(line);
}

//------

CQChartsBoxKeyColor::
CQChartsBoxKeyColor(CQChartsBoxPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

bool
CQChartsBoxKeyColor::
selectPress(const CQChartsGeom::Point &)
{
  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->updateObjs();

  return true;
}

QBrush
CQChartsBoxKeyColor::
fillBrush() const
{
  QColor c = CQChartsKeyColorBox::fillBrush().color();

  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}

//------

CQChartsBoxKeyText::
CQChartsBoxKeyText(CQChartsBoxPlot *plot, int i, const QString &text) :
 CQChartsKeyText(plot, text), i_(i)
{
}

QColor
CQChartsBoxKeyText::
interpTextColor(int i, int n) const
{
  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  QColor c = CQChartsKeyText::interpTextColor(i, n);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}
