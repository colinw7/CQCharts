#include <CQChartsTitle.h>
#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsEditHandles.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsVariant.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

CQChartsTitle::
CQChartsTitle(CQChartsPlot *plot) :
 CQChartsTextBoxObj(plot)
{
  init();
}

CQChartsTitle::
~CQChartsTitle()
{
  delete subTitle_;
}

void
CQChartsTitle::
init()
{
  setObjectName("title");

  setTextStr("Title");

  //---

  textData_.setFont(CQChartsFont().incFontSize(4));

  //---

  setFilled (false);
  setStroked(false);

  setTextAlign(Qt::AlignHCenter | Qt::AlignVCenter);
  setTextColor(Color::makeInterfaceValue(1.0));

  setTextFormatted(true);
  setTextScaled(false);

  //---

  subTitle_ = new TextBoxObj(plot_);

  subTitle_->setTextAlign(Qt::AlignHCenter | Qt::AlignVCenter);
  subTitle_->setTextColor(Color::makeInterfaceValue(1.0));

  subTitle_->setTextFormatted(true);
  subTitle_->setTextScaled(false);

  connect(subTitle_, SIGNAL(textBoxObjInvalidated()), this, SLOT(subTitleChanged()));
}

QString
CQChartsTitle::
calcId() const
{
  return plot_->id() + "/title";
}

void
CQChartsTitle::
setSelected(bool b)
{
  CQChartsUtil::testAndSet(selected_, b, [&]() { redraw(); } );
}

void
CQChartsTitle::
setLocation(const TitleLocation &l)
{
  CQChartsUtil::testAndSet(location_, l, [&]() {
    if      (location().type() == TitleLocation::Type::ABSOLUTE_POSITION) {
      if (! absolutePosition().isSet())
        absolutePosition_ = CQChartsPosition(bbox().getUL(), CQChartsPosition::Units::PLOT);
    }
    else if (location().type() == TitleLocation::Type::ABSOLUTE_RECTANGLE) {
      if (! absoluteRectangle().isSet())
        absoluteRectangle_ = CQChartsRect(bbox(), CQChartsRect::Units::PLOT);
    }

    updatePlotPosition();
  } );
}

void
CQChartsTitle::
setAbsolutePosition(const CQChartsPosition &p)
{
  CQChartsUtil::testAndSet(absolutePosition_, p, [&]() { updatePlotPosition(); } );
}

void
CQChartsTitle::
setAbsoluteRectangle(const CQChartsRect &r)
{
  CQChartsUtil::testAndSet(absoluteRectangle_, r, [&]() { updatePlotPosition(); } );
}

void
CQChartsTitle::
setInsidePlot(bool b)
{
  CQChartsUtil::testAndSet(insidePlot_, b, [&]() { updateLocation(); updatePlotPosition(); } );
}

void
CQChartsTitle::
setExpandWidth(bool b)
{
  CQChartsUtil::testAndSet(expandWidth_, b, [&]() { updateLocation(); updatePlotPosition(); } );
}

void
CQChartsTitle::
setPosition(const Point &p)
{
  position_ = p;
}

void
CQChartsTitle::
updatePlotPosition()
{
  if (plot_->isAutoFit())
    plot_->setNeedsAutoFit(true);

  redraw();
}

void
CQChartsTitle::
redraw(bool wait)
{
  plot_->emitTitleChanged();

  if (wait)
    plot_->drawForeground();
  else
    plot_->invalidateLayer(CQChartsBuffer::Type::FOREGROUND);
}

QString
CQChartsTitle::
locationStr() const
{
  return location().toString();
}

void
CQChartsTitle::
setLocationStr(const QString &str)
{
  setLocation(TitleLocation(str));
}

void
CQChartsTitle::
updateLocation()
{
  if (plot_->isComposite())
    return;

  setBBox(BBox());

  //---

  // place outside all overlay plots, annotation boxes and axes
  auto bbox = plot_->calcGroupedDataRange(
                CQChartsPlot::RangeTypes().setExtra().setAxes().setKey());

  //---

  // calc title text size
  auto ts = calcSize();

  auto location = this->location();

//auto marginSize = plot_->pixelToWindowSize(Size(8, 8));
  auto marginSize = plot_->pixelToWindowSize(Size(0, 0));

  double kx = bbox.getXMid() - ts.optWidth()/2.0;
  double ky = 0.0;

  if      (location.type() == TitleLocation::Type::TOP) {
    if (! isInsidePlot())
      ky = bbox.getYMax() + marginSize.height();
    else
      ky = bbox.getYMax() - ts.height() - marginSize.height();
  }
  else if (location.type() == TitleLocation::Type::CENTER) {
    ky = bbox.getYMid() - ts.height()/2.0;
  }
  else if (location.type() == TitleLocation::Type::BOTTOM) {
    if (! isInsidePlot())
      ky = bbox.getYMin() - ts.height() - marginSize.height();
    else
      ky = bbox.getYMin() + marginSize.height();
  }
  else {
    ky = bbox.getYMid() - ts.height()/2.0;
  }

  Point kp(kx, ky);

  if      (location.type() == TitleLocation::Type::ABSOLUTE_POSITION) {
    kp = absolutePlotPosition();
  }
  else if (location.type() == TitleLocation::Type::ABSOLUTE_RECTANGLE) {
    auto bbox = absolutePlotRectangle();

    if (bbox.isValid())
      kp = Point(bbox.getUL());
  }

  setPosition(kp);
}

void
CQChartsTitle::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &name, const QString &desc, bool hidden=false) {
    auto *item = model->addProperty(path, this, name);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  //---

  addProp("visible"          , "Title visible");
  addProp("editable"         , "Title editable");
  addProp("location"         , "Title location");
  addProp("absolutePosition" , "Title absolute position");
  addProp("absoluteRectangle", "Title absolute rectangle");
  addProp("insidePlot"       , "Title is inside plot");
  addProp("expandWidth"      , "Title is sized to plot width");

  auto fitPath = path + "/fit";

  model->addProperty(fitPath, this, "fitHorizontal", "horizontal")->
    setDesc("Fit title horizontally");
  model->addProperty(fitPath, this, "fitVertical"  , "vertical"  )->
    setDesc("Fit title vertically");

  addTypeProperties(model, path, "", PropertyType::ALL & ~PropertyType::ANGLE);

  //---

  // subtitle
  auto subTitlePath = path + "/subtitle";

  model->addProperty(subTitlePath, subTitle_, "textStr", "string")->
    setDesc("Subtitle text string");

  subTitle_->addTextDataProperties(model, subTitlePath, "Subtitle",
                                   PropertyType::ALL & ~PropertyType::ANGLE);
}

CQChartsGeom::Point
CQChartsTitle::
absolutePlotPosition() const
{
  return plot_->positionToPlot(absolutePosition());
}

void
CQChartsTitle::
setAbsolutePlotPosition(const Point &p)
{
  setAbsolutePosition(CQChartsPosition::view(plot_->windowToView(p)));
}

CQChartsGeom::BBox
CQChartsTitle::
absolutePlotRectangle() const
{
  return plot_->rectToPlot(absoluteRectangle());
}

void
CQChartsTitle::
setAbsolutePlotRectangle(const BBox &bbox)
{
  setAbsoluteRectangle(CQChartsRect::view(plot_->windowToView(bbox)));
}

CQChartsGeom::Size
CQChartsTitle::
calcSize()
{
  textSize_         = Size();
  subTitleTextSize_ = Size();
  allTextSize_      = Size();
  size_             = Size();

  if (isTextVisible() && textStr().length()) {
    // get font
    auto font = view()->plotFont(plot(), textFont());

    // get pixel size
    CQChartsTextOptions textOptions;

    textOptions.html = isTextHtml();

    auto psize = CQChartsDrawUtil::calcTextSize(textStr(), font, textOptions);

    // convert to window size
    auto wsize = plot_->pixelToWindowSize(psize);

    textSize_ = wsize;
  }

  if (subTitle_->isTextVisible() && subTitle_->textStr().length()) {
    // get font
    auto font = view()->plotFont(plot(), subTitle_->textFont());

    // get pixel size
    CQChartsTextOptions textOptions;

    textOptions.html = subTitle_->isTextHtml();

    auto psize = CQChartsDrawUtil::calcTextSize(subTitle_->textStr(), font, textOptions);

    // convert to window size
    auto wsize = plot_->pixelToWindowSize(psize);

    subTitleTextSize_ = Size(wsize);
  }

  if (textSize_.isSet() || subTitleTextSize_.isSet()) {
    // add outer margin and inner padding
    auto mm = parentMargin(margin ());
    auto pm = parentMargin(padding());

    double gap = 0.0;

    if (textSize_.isSet() && subTitleTextSize_.isSet())
      gap = plot_->pixelToWindowHeight(subTitleGap());

    auto textWidth  = std::max(textSize_.optWidth(), subTitleTextSize_.optWidth());
    auto textHeight = textSize_.optHeight() + subTitleTextSize_.optHeight();

    allTextSize_ = Size(textWidth, textHeight);

    size_ = Size(allTextSize_.width () + pm.width () + mm.width(),
                 allTextSize_.height() + pm.height() + mm.height() + gap);
  }

  if (isExpandWidth() && size_.isSet()) {
    auto bbox = plot_->calcGroupedDataRange();

    size_.setWidth(bbox.getWidth());
  }

  return size_;
}

void
CQChartsTitle::
preAutoFit()
{
  if (! isDrawn())
    return;

  isTitleInsideX1_ = false;
  isTitleInsideY1_ = false;

  auto vbbox = plot_->calcPlotViewRect();
  //std::cerr << vbbox << "\n";

  auto tbbox = fitBBox();
  isTitleInsideX1_ = vbbox.insideX(tbbox);
  isTitleInsideY1_ = vbbox.insideY(tbbox);
  //std::cerr << isTitleInsideX1_ << " " << isTitleInsideY1_ << " " << tbbox << "\n";
  //std::cerr << windowToPixel(tbbox) << "\n";
}

void
CQChartsTitle::
postAutoFit()
{
  if (! isDrawn())
    return;

  updateBBox();

  auto vbbox = plot_->calcPlotViewRect();
  //std::cerr << vbbox << "\n";

  auto tbbox = fitBBox();
  auto isTitleInsideX2 = vbbox.insideX(tbbox);
  auto isTitleInsideY2 = vbbox.insideY(tbbox);
  //std::cerr << isTitleInsideX2 << " " << isTitleInsideY2 << " " << tbbox << "\n";
  //std::cerr << windowToPixel(tbbox) << "\n";

  if (! isTitleInsideX1_ && ! isTitleInsideX2)
    setFitHorizontal(false);
  if (! isTitleInsideY1_ && ! isTitleInsideY2)
    setFitVertical(false);
}

CQChartsGeom::BBox
CQChartsTitle::
fitBBox() const
{
  auto bbox = this->bbox();

  if (! bbox.isValid())
    return bbox;

  if (calcFitHorizontal() && calcFitVertical())
    return bbox;

  if (calcFitHorizontal())
    return BBox(bbox.getXMin(), bbox.getYMid(), bbox.getXMax(), bbox.getYMid());

  if (calcFitVertical())
    return BBox(bbox.getXMid(), bbox.getYMin(), bbox.getXMid(), bbox.getYMax());

  return BBox();
}

bool
CQChartsTitle::
calcFitHorizontal() const
{
  return isFitHorizontal();
}

bool
CQChartsTitle::
calcFitVertical() const
{
  return isFitVertical();
}

bool
CQChartsTitle::
contains(const Point &p) const
{
  if (! isVisible())
    return false;

  return bbox().inside(p);
}

//------

bool
CQChartsTitle::
editPress(const Point &p)
{
  editHandles()->setDragPos(p);

  if (location().type() != TitleLocation::Type::ABSOLUTE_POSITION &&
      location().type() != TitleLocation::Type::ABSOLUTE_RECTANGLE) {
    setLocation(TitleLocation(TitleLocation::Type::ABSOLUTE_POSITION));

    setAbsolutePlotPosition(position_);
  }

  return true;
}

bool
CQChartsTitle::
editMove(const Point &p)
{
  const auto &dragPos  = editHandles()->dragPos();
  const auto &dragSide = editHandles()->dragSide();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  if      (location().type() == TitleLocation::Type::ABSOLUTE_POSITION) {
    if (dragSide == CQChartsResizeSide::MOVE) {
      setLocation(TitleLocation(TitleLocation::Type::ABSOLUTE_POSITION));

      setAbsolutePlotPosition(absolutePlotPosition() + Point(dx, dy));
    }
  }
  else if (location().type() == TitleLocation::Type::ABSOLUTE_RECTANGLE) {
    if (dragSide != CQChartsResizeSide::NONE)
      editHandles()->updateBBox(dx, dy);

    setAbsolutePlotRectangle(editHandles()->bbox());
  }

  editHandles()->setDragPos(p);

  redraw(/*wait*/false);

  return true;
}

bool
CQChartsTitle::
editMotion(const Point &p)
{
  return editHandles()->selectInside(p);
}

bool
CQChartsTitle::
editMoveBy(const Point &d)
{
  setLocation(TitleLocation(TitleLocation::Type::ABSOLUTE_POSITION));

  setAbsolutePlotPosition(position_ + d);

  redraw(/*wait*/false);

  return true;
}

//------

bool
CQChartsTitle::
isDrawn() const
{
  if (! isVisible())
    return false;

  if (! textStr().length())
    return false;

  return true;
}

void
CQChartsTitle::
draw(PaintDevice *device)
{
  if (! isDrawn())
    return;

  //---

  device->save();

  //---

  // clip to plot
  auto clipRect = plot_->calcPlotRect();

  device->setClipRect(clipRect);

  //---

  updateBBox();

  //---

  auto ibbox = BBox(tx_, ty_, tx_ + tw_, ty_ + th_); // text bbox

  //---

  // add outer margin and inner padding
  auto mm = parentMargin(margin ());
  auto pm = parentMargin(padding());

  auto obbox = CQChartsGeom::Margin::outsetBBox(ibbox, pm);
  auto bbox  = CQChartsGeom::Margin::outsetBBox(obbox, mm);

  //---

  // draw outer box (if stroked/filled) - sets bbox :(
  CQChartsBoxObj::draw(device, obbox);

  setBBox(bbox);

  //---

  double subTitleTextHeight = 0;
  double subTitleTextGap    = 0.0;

  if (subTitle_->isTextVisible() && subTitle_->textStr().length()) {
    assert(subTitleTextSize_.isSet());

    subTitleTextHeight = subTitleTextSize_.height();
    subTitleTextGap    = plot_->pixelToWindowHeight(subTitleGap());
  }

  //---

  device->setClipRect(ibbox);

  if (isTextVisible() && textStr().length()) {
    // set text pen
    CQChartsPenBrush penBrush;

    auto tc = interpTextColor(ColorInd());

    plot()->setPen(penBrush, PenData(true, tc, textAlpha()));

    device->setPen(penBrush.pen);

    //---

    // set text options
    BBox tbbox1(ibbox.getXMin(), ibbox.getYMax(),
                ibbox.getXMax(), ibbox.getYMin() + subTitleTextHeight + subTitleTextGap);

    plot_->setRefLength(Plot::OptReal(tbbox1.getWidth()));

    auto textOptions = this->textOptions();

    textOptions.angle = CQChartsAngle();

    textOptions = plot_->adjustTextOptions(textOptions);

    //---

    // set font
    plot()->setPainterFont(device, textFont());

    //---

    // draw text
    device->setRenderHints(QPainter::Antialiasing);

    CQChartsDrawUtil::drawTextInBox(device, tbbox1, textStr(), textOptions);

    plot_->resetRefLength();
  }

  //---

  if (subTitle_->isTextVisible() && subTitle_->textStr().length()) {
    // set text pen
    CQChartsPenBrush penBrush;

    auto tc = subTitle_->interpTextColor(ColorInd());

    plot()->setPen(penBrush, PenData(true, tc, subTitle_->textAlpha()));

    device->setPen(penBrush.pen);

    //---

    // set text options
    BBox tbbox1(ibbox.getXMin(), ibbox.getYMin(),
                ibbox.getXMax(), ibbox.getYMin() + subTitleTextHeight);

    plot_->setRefLength(Plot::OptReal(tbbox1.getWidth()));

    auto textOptions = subTitle_->textOptions();

    textOptions.angle = CQChartsAngle();

    textOptions = plot_->adjustTextOptions(textOptions);

    //---

    // set font
    plot()->setPainterFont(device, subTitle_->textFont());

    //---

    // draw text
    device->setRenderHints(QPainter::Antialiasing);

    CQChartsDrawUtil::drawTextInBox(device, tbbox1, subTitle_->textStr(), textOptions);

    plot_->resetRefLength();
  }

  //---

  if (plot_->isShowBoxes()) {
    device->setClipRect(clipRect);

    plot_->drawWindowColorBox(device, this->bbox(), Qt::red);
  }

  //---

  device->restore();
}

void
CQChartsTitle::
updateBBox()
{
  if (location().type() != TitleLocation::Type::ABSOLUTE_RECTANGLE)
    updateLocation();

  //---

  // add outer margin and inner padding
  auto mm = parentMargin(margin ());
  auto pm = parentMargin(padding());

  //---

  if (location().type() != TitleLocation::Type::ABSOLUTE_RECTANGLE) {
    tx_ = position_.x + mm.left() + pm.left(); // text left
    ty_ = position_.y + mm.top () + pm.top (); // text top

    if (! size_.isSet())
      return;

    tw_ = size_.width () - mm.width () - pm.width ();
    th_ = size_.height() - mm.height() - pm.height();

    auto ibbox = BBox(tx_, ty_, tx_ + tw_, ty_ + th_); // text bbox (inner bbox)

    auto obbox = CQChartsGeom::Margin::outsetBBox(ibbox, pm);
    auto bbox  = CQChartsGeom::Margin::outsetBBox(obbox, mm);

    setBBox(bbox);
  }
  else {
    setBBox(absolutePlotRectangle());

    if (bbox().isValid()) {
      auto obbox = CQChartsGeom::Margin::insetBBox(bbox(), mm);
      auto ibbox = CQChartsGeom::Margin::insetBBox(obbox , pm);

      tx_ = ibbox.getXMin  ();
      ty_ = ibbox.getYMin  ();
      tw_ = ibbox.getWidth ();
      th_ = ibbox.getHeight();
    }
  }
}

void
CQChartsTitle::
setEditHandlesBBox() const
{
  if (location().type() == TitleLocation::Type::ABSOLUTE_RECTANGLE)
    editHandles()->setMode(EditHandles::Mode::RESIZE);
  else
    editHandles()->setMode(EditHandles::Mode::MOVE);

  editHandles()->setBBox(this->bbox());
}

void
CQChartsTitle::
textBoxObjInvalidate()
{
  if (! isDrawn())
    setBBox(BBox());

  plot_->drawObjs();
}

void
CQChartsTitle::
subTitleChanged()
{
  updateLocation();
}

//---

void
CQChartsTitle::
write(const CQPropertyViewModel *propertyModel, const QString &plotName, std::ostream &os)
{
  CQPropertyViewModel::NameValues nameValues;

  propertyModel->getChangedNameValues(this, nameValues, /*tcl*/true);

  for (const auto &nv : nameValues) {
    QString str;

    if (! CQChartsVariant::toString(nv.second, str))
      str.clear();

    os << "set_charts_property -plot $" << plotName.toStdString();

    os << " -name " << nv.first.toStdString() << " -value {" << str.toStdString() << "}\n";
  }
}
