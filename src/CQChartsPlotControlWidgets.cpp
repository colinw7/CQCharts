#include <CQChartsPlotControlWidgets.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelDetails.h>
#include <CQChartsExprTcl.h>

#include <CQWidgetUtil.h>
#include <CQDoubleRangeSlider.h>
#include <CQIntRangeSlider.h>
#include <CQTimeRangeSlider.h>

#include <QRadioButton>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>

CQChartsPlotRealControl::
CQChartsPlotRealControl(QWidget *parent) :
 CQChartsPlotControlIFace(parent)
{
  init();
}

CQChartsPlotRealControl::
CQChartsPlotRealControl(CQChartsPlot *plot, const CQChartsColumn &column) :
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

      slider_->setRangeMin(details->minValue().toDouble(&ok));
      slider_->setRangeMax(details->maxValue().toDouble(&ok));

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

  QString header1 = CQChartsExprTcl::encodeColumnName(header);

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
CQChartsPlotIntControl(CQChartsPlot *plot, const CQChartsColumn &column) :
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

      slider_->setRangeMin(details->minValue().toInt(&ok));
      slider_->setRangeMax(details->maxValue().toInt(&ok));

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

  QString header1 = CQChartsExprTcl::encodeColumnName(header);

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
CQChartsPlotTimeControl(CQChartsPlot *plot, const CQChartsColumn &column) :
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

      slider_->setRangeMin(details->minValue().toDouble(&ok));
      slider_->setRangeMax(details->maxValue().toDouble(&ok));

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

  QString header1 = CQChartsExprTcl::encodeColumnName(header);

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
CQChartsPlotValueControl(CQChartsPlot *plot, const CQChartsColumn &column) :
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

      QVariantList uniqueValues = details->uniqueValues();

      for (const auto &value : uniqueValues) {
        QString str = value.toString();
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

  QString header1 = CQChartsExprTcl::encodeColumnName(header);

  return QString("$%1 %2 {%3}").arg(header1).arg(cmpStr).arg(combo_->itemText(ind));
}

//------

CQChartsPlotControlIFace::
CQChartsPlotControlIFace(QWidget *parent) :
 QFrame(parent)
{
}

CQChartsPlotControlIFace::
CQChartsPlotControlIFace(CQChartsPlot *plot, const CQChartsColumn &column) :
 QFrame(plot->view()), plot_(plot), column_(column)
{
}
