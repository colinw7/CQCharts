#include <CQChartsPlotControlWidgets.h>
#include <CQChartsGroupPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelDetails.h>
#include <CQChartsExprTcl.h>

#include <CQWidgetUtil.h>
#include <CQDoubleRangeSlider.h>
#include <CQIntRangeSlider.h>
#include <CQTimeRangeSlider.h>

#include <QRadioButton>
#include <QComboBox>
#include <QCheckBox>
#include <QButtonGroup>
#include <QLabel>
#include <QHBoxLayout>
#include <QGridLayout>

CQChartsPlotControlFrame::
CQChartsPlotControlFrame(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("controlFrame");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  auto *optionsFrame  = CQUtil::makeWidget<QFrame>("optionsFrame");
  auto *optionsLayout = CQUtil::makeLayout<QHBoxLayout>(optionsFrame, 2, 2);

  equalCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Equal", "equalCheck");
  andCheck_   = CQUtil::makeLabelWidget<QCheckBox>("And"  , "andCheck");

  equalCheck_->setToolTip("Compare Equal or Non-Equal");
  andCheck_  ->setToolTip("Combine with And or Or");

  equalCheck_->setChecked(true);
  andCheck_  ->setChecked(true);

  connect(equalCheck_, SIGNAL(stateChanged(int)), this, SLOT(controlsChanged()));
  connect(andCheck_, SIGNAL(stateChanged(int)), this, SLOT(controlsChanged()));

  optionsLayout->addWidget (equalCheck_);
  optionsLayout->addWidget (andCheck_);
  optionsLayout->addStretch(1);

  layout->addWidget(optionsFrame);

  //---

  controlArea_ = CQUtil::makeWidget<QFrame>("controlArea");

  layout->addWidget(controlArea_);

  controlLayout_ = new QGridLayout(controlArea_);

  //---

  layout->addStretch(1);
}

bool
CQChartsPlotControlFrame::
isEqual() const
{
  return equalCheck_->isChecked();
}

void
CQChartsPlotControlFrame::
setEqual(bool b)
{
  equalCheck_->setChecked(b);
}

bool
CQChartsPlotControlFrame::
isAnd() const
{
  return andCheck_->isChecked();
}

void
CQChartsPlotControlFrame::
setAnd(bool b)
{
  andCheck_->setChecked(b);
}

void
CQChartsPlotControlFrame::
setPlotControls()
{
  clearControls();

  if (plot_) {
    addPlotControls(plot_);

    int n = plot_->numChildPlots();

    for (int i = 0; i < n; ++i)
      addPlotControls(plot_->childPlot(i));
  }

  addIFaceTerm();
}

void
CQChartsPlotControlFrame::
addPlotControls(CQChartsPlot *plot)
{
  auto columns = plot->controlColumns();

  for (int ic = 0; ic < columns.count(); ++ic) {
    const auto &column = columns.getColumn(ic);
    if (! column.isValid()) continue;

    auto *details = plot->columnDetails(column);
    if (! details) continue;

    //---

    // create widget
    CQChartsPlotControlIFace *iface = nullptr;

    if      (details->type() == CQBaseModelType::REAL)
      iface = new CQChartsPlotRealControl(plot, column);
    else if (details->type() == CQBaseModelType::INTEGER)
      iface = new CQChartsPlotIntControl(plot, column);
    else if (details->type() == CQBaseModelType::TIME)
      iface = new CQChartsPlotTimeControl(plot, column);
    else
      iface = new CQChartsPlotValueControl(plot, column);

    addIFace(iface);
  }
}

void
CQChartsPlotControlFrame::
clearControls()
{
  CQUtil::removeGridItems(controlLayout_, /*deleteWidgets*/true);

  ifaces_.clear();

  delete groupButtonGroup_;

  groupButtonGroup_ = new QButtonGroup(this);

  groupButtonGroup_->setExclusive(false);
}

void
CQChartsPlotControlFrame::
addIFace(CQChartsPlotControlIFace *iface)
{
  auto *radio = iface->radio();

  if (radio)
    groupButtonGroup_->addButton(radio);

  controlLayout_->addWidget(iface, numIFaces(), 0);

  ifaces_.push_back(iface);

  iface->connectValueChanged(this, SLOT(controlsChanged()));
}

void
CQChartsPlotControlFrame::
addIFaceTerm()
{
  controlLayout_->setRowStretch(ifaces_.size(), 1);
  controlLayout_->setColumnStretch(2, 1);
}

int
CQChartsPlotControlFrame::
numIFaces() const
{
  return ifaces_.size();
}

CQChartsPlotControlIFace *
CQChartsPlotControlFrame::
iface(int i) const
{
  return ifaces_[i];
}

void
CQChartsPlotControlFrame::
controlsChanged()
{
  auto *obj = sender();
  if (! obj) return;

  auto id = obj->property("plot").toString();

  auto *view = plot_->view();
  if (! view) return;

  auto *plot = view->getPlotForId(id);
  if (! plot) return;

  auto *groupPlot = dynamic_cast<CQChartsGroupPlot *>(plot);

  QStringList filters;

  auto cmpStr = (this->isEqual() ? "==" : "!=");

  int n = numIFaces(); // all same size

  for (int i = 0; i < n; ++i) {
    auto *iface = this->iface(i);
    assert(iface);

    auto *radio = iface->radio();

    if (radio->isChecked()) {
      const auto &column = iface->column();

      if (groupPlot)
        groupPlot->setGroupColumn(CQChartsColumn(column));
    }
    else {
      auto filter = iface->filterStr(cmpStr);

      if (filter.length())
        filters.push_back(filter);
    }
  }

  auto combStr = (this->isAnd() ? "&&" : "||");

  auto filterStr = filters.join(QString(" %1 ").arg(combStr));

  //std::cerr << filterStr.toStdString() << "\n";

  plot->setVisibleFilterStr(filterStr);
}

//---

CQChartsPlotRealControl::
CQChartsPlotRealControl(QWidget *parent) :
 CQChartsPlotControlIFace(parent)
{
  init();
}

CQChartsPlotRealControl::
CQChartsPlotRealControl(Plot *plot, const Column &column) :
 CQChartsPlotControlIFace(plot, column)
{
  init();
}

void
CQChartsPlotRealControl::
init()
{
  setObjectName("realControl");

  //---

  if (! slider_) {
    auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

    //---

    label_ = CQUtil::makeLabelWidget<QLabel>("", "label");

    layout->addWidget(label_);

    //---

    slider_ = new CQDoubleRangeSlider;

    layout->addWidget(slider_);

    connect(slider_, SIGNAL(sliderRangeChanged(double, double)),
            this, SIGNAL(valueChanged()));

    //---

    radio_ = CQUtil::makeLabelWidget<QRadioButton>("Group", "radio");

    layout->addWidget(radio_);

    connect(radio_, SIGNAL(toggled(bool)), this, SIGNAL(valueChanged()));
  }

  //---

  if (plot_) {
    setProperty("plot", plot_->id());

    slider_->setProperty("plot", plot_->id());
    radio_ ->setProperty("plot", plot_->id());

    if (column_.isValid()) {
      radio_->setProperty("column", column_.column());

      bool ok;

      auto header = plot_->modelHHeaderString(column_, ok);

      if (ok)
        setProperty("header", header);

      label_->setText(header);

      //---

      auto *details = plot_->columnDetails(column_);
      assert(details);

      slider_->setRangeMin(CQChartsVariant::toReal(details->minValue(), ok));
      slider_->setRangeMax(CQChartsVariant::toReal(details->maxValue(), ok));

      slider_->setSliderMin(slider_->rangeMin());
      slider_->setSliderMax(slider_->rangeMax());
    }
  }
}

void
CQChartsPlotRealControl::
connectValueChanged(QObject *obj, const char *slotName)
{
  connect(this, SIGNAL(valueChanged()), obj, slotName);
}

QString
CQChartsPlotRealControl::
filterStr(const QString &) const
{
  auto header = property("header").toString();
  if (! header.length()) return "";

  auto header1 = CQChartsExprTcl::encodeColumnName(header);

  QString filter;

  if      (slider_->sliderMin() != slider_->rangeMin() &&
           slider_->sliderMax() != slider_->rangeMax()) {
    filter = QString("($%1 >= %2 && $%1 <= %3)").arg(header1).
               arg(slider_->sliderMin()).arg(slider_->sliderMax());
  }
  else if (slider_->sliderMin() != slider_->rangeMin()) {
    filter = QString("$%1 >= %2").arg(header1).arg(slider_->sliderMin());
  }
  else if (slider_->sliderMax() != slider_->rangeMax()) {
    filter = QString("$%1 <= %2").arg(header1).arg(slider_->sliderMax());
  }

  return filter;
}

//---

CQChartsPlotIntControl::
CQChartsPlotIntControl(QWidget *parent) :
 CQChartsPlotControlIFace(parent)
{
  init();
}

CQChartsPlotIntControl::
CQChartsPlotIntControl(Plot *plot, const Column &column) :
 CQChartsPlotControlIFace(plot, column)
{
  init();
}

void
CQChartsPlotIntControl::
init()
{
  setObjectName("intControl");

  //---

  if (! slider_) {
    auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

    //---

    label_ = CQUtil::makeLabelWidget<QLabel>("", "label");

    layout->addWidget(label_);

    //---

    slider_ = new CQIntRangeSlider;

    layout->addWidget(slider_);

    connect(slider_, SIGNAL(sliderRangeChanged(int, int)),
            this, SIGNAL(valueChanged()));

    //---

    radio_ = CQUtil::makeLabelWidget<QRadioButton>("Group", "radio");

    layout->addWidget(radio_);

    connect(radio_, SIGNAL(toggled(bool)), this, SIGNAL(valueChanged()));
  }

  //---

  if (plot_) {
    setProperty("plot", plot_->id());

    slider_->setProperty("plot", plot_->id());
    radio_ ->setProperty("plot", plot_->id());

    if (column_.isValid()) {
      radio_->setProperty("column", column_.column());

      bool ok;

      auto header = plot_->modelHHeaderString(column_, ok);

      if (ok)
        setProperty("header", header);

      label_->setText(header);

      //---

      auto *details = plot_->columnDetails(column_);
      assert(details);

      slider_->setRangeMin(CQChartsVariant::toInt(details->minValue(), ok));
      slider_->setRangeMax(CQChartsVariant::toInt(details->maxValue(), ok));

      slider_->setSliderMin(slider_->rangeMin());
      slider_->setSliderMax(slider_->rangeMax());
    }
  }
}

void
CQChartsPlotIntControl::
connectValueChanged(QObject *obj, const char *slotName)
{
  connect(this, SIGNAL(valueChanged()), obj, slotName);
}

QString
CQChartsPlotIntControl::
filterStr(const QString &) const
{
  auto header = property("header").toString();
  if (! header.length()) return "";

  auto header1 = CQChartsExprTcl::encodeColumnName(header);

  QString filter;

  if      (slider_->sliderMin() != slider_->rangeMin() &&
           slider_->sliderMax() != slider_->rangeMax()) {
    filter = QString("($%1 >= %2 && $%1 <= %3)").arg(header1).
               arg(slider_->sliderMin()).arg(slider_->sliderMax());
  }
  else if (slider_->sliderMin() != slider_->rangeMin()) {
    filter = QString("$%1 >= %2").arg(header1).arg(slider_->sliderMin());
  }
  else if (slider_->sliderMax() != slider_->rangeMax()) {
    filter = QString("$%1 <= %2").arg(header1).arg(slider_->sliderMax());
  }

  return filter;
}

//---

CQChartsPlotTimeControl::
CQChartsPlotTimeControl(QWidget *parent) :
 CQChartsPlotControlIFace(parent)
{
  init();
}

CQChartsPlotTimeControl::
CQChartsPlotTimeControl(Plot *plot, const Column &column) :
 CQChartsPlotControlIFace(plot, column)
{
  init();
}

void
CQChartsPlotTimeControl::
init()
{
  setObjectName("timeControl");

  //---

  if (! slider_) {
    auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

    //---

    label_ = CQUtil::makeLabelWidget<QLabel>("", "label");

    layout->addWidget(label_);

    //---

    slider_ = new CQTimeRangeSlider;

    layout->addWidget(slider_);

    connect(slider_, SIGNAL(sliderRangeChanged(double, double)),
            this, SIGNAL(valueChanged()));

    //---

    radio_ = CQUtil::makeLabelWidget<QRadioButton>("Group", "radio");

    layout->addWidget(radio_);

    connect(radio_, SIGNAL(toggled(bool)), this, SIGNAL(valueChanged()));
  }

  //---

  if (plot_) {
    setProperty("plot", plot_->id());

    slider_->setProperty("plot", plot_->id());
    radio_ ->setProperty("plot", plot_->id());

    if (column_.isValid()) {
      radio_->setProperty("column", column_.column());

      bool ok;

      auto header = plot_->modelHHeaderString(column_, ok);

      if (ok)
        setProperty("header", header);

      label_->setText(header);

      //---

      auto *details = plot_->columnDetails(column_);
      assert(details);

      slider_->setRangeMin(CQChartsVariant::toReal(details->minValue(), ok));
      slider_->setRangeMax(CQChartsVariant::toReal(details->maxValue(), ok));

      slider_->setSliderMin(slider_->rangeMin());
      slider_->setSliderMax(slider_->rangeMax());
    }
  }
}

void
CQChartsPlotTimeControl::
connectValueChanged(QObject *obj, const char *slotName)
{
  connect(this, SIGNAL(valueChanged()), obj, slotName);
}

QString
CQChartsPlotTimeControl::
filterStr(const QString &) const
{
  auto header = property("header").toString();
  if (! header.length()) return "";

  auto header1 = CQChartsExprTcl::encodeColumnName(header);

  QString filter;

  if      (slider_->sliderMin() != slider_->rangeMin() &&
           slider_->sliderMax() != slider_->rangeMax()) {
    filter = QString("($%1 >= %2 && $%1 <= %3)").arg(header1).
               arg(slider_->sliderMin()).arg(slider_->sliderMax());
  }
  else if (slider_->sliderMin() != slider_->rangeMin()) {
    filter = QString("$%1 >= %2").arg(header1).arg(slider_->sliderMin());
  }
  else if (slider_->sliderMax() != slider_->rangeMax()) {
    filter = QString("$%1 <= %2").arg(header1).arg(slider_->sliderMax());
  }

  return filter;
}

//---

CQChartsPlotValueControl::
CQChartsPlotValueControl(QWidget *parent) :
 CQChartsPlotControlIFace(parent)
{
  init();
}

CQChartsPlotValueControl::
CQChartsPlotValueControl(Plot *plot, const Column &column) :
 CQChartsPlotControlIFace(plot, column)
{
  init();
}

void
CQChartsPlotValueControl::
init()
{
  setObjectName("valueControl");

  //---

  if (! combo_) {
    auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

    //---

    label_ = CQUtil::makeLabelWidget<QLabel>("", "label");

    layout->addWidget(label_);

    //---

    combo_ = new QComboBox;

    combo_->setFixedWidth(QFontMetrics(font()).width("XXXXXXXX"));
  //combo_->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

    layout->addWidget(combo_);

    connect(combo_, SIGNAL(currentIndexChanged(int)),
            this, SIGNAL(valueChanged()));

    //---

    radio_ = CQUtil::makeLabelWidget<QRadioButton>("Group", "radio");

    layout->addWidget(radio_);

    connect(radio_, SIGNAL(toggled(bool)), this, SIGNAL(valueChanged()));
  }

  //---

  if (plot_) {
    setProperty("plot", plot_->id());

    combo_->setProperty("plot", plot_->id());
    radio_->setProperty("plot", plot_->id());

    if (column_.isValid()) {
      radio_->setProperty("column", column_.column());

      bool ok;

      auto header = plot_->modelHHeaderString(column_, ok);

      if (ok)
        setProperty("header", header);

      label_->setText(header);

      //---

      auto *details = plot_->columnDetails(column_);
      assert(details);

      combo_->clear();

      combo_->addItem("<any>");

      auto uniqueValues = details->uniqueValues();

      for (const auto &value : uniqueValues) {
        auto str = value.toString();
        if (! str.length()) continue;

        combo_->addItem(str);
      }
    }
  }
}

void
CQChartsPlotValueControl::
connectValueChanged(QObject *obj, const char *slotName)
{
  connect(this, SIGNAL(valueChanged()), obj, slotName);
}

QString
CQChartsPlotValueControl::
filterStr(const QString &cmpStr) const
{
  int ind = combo_->currentIndex();
  if (ind == 0) return "";

  auto header = property("header").toString();
  if (! header.length()) return "";

  auto header1 = CQChartsExprTcl::encodeColumnName(header);

  return QString("$%1 %2 {%3}").arg(header1).arg(cmpStr).arg(combo_->itemText(ind));
}

//------

CQChartsPlotControlIFace::
CQChartsPlotControlIFace(QWidget *parent) :
 QFrame(parent)
{
}

CQChartsPlotControlIFace::
CQChartsPlotControlIFace(Plot *plot, const Column &column) :
 QFrame(plot->view()), plot_(plot), column_(column)
{
}
