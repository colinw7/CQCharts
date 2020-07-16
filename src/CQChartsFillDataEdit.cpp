#include <CQChartsFillDataEdit.h>
#include <CQChartsColorEdit.h>
#include <CQChartsAlphaEdit.h>
#include <CQChartsFillPatternEdit.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQGroupBox.h>
#include <CQDragLabel.h>
#include <CQUtil.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

CQChartsFillDataLineEdit::
CQChartsFillDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("fillDataLineEdit");

  setToolTip("Fill Data");

  //---

  menuEdit_ = dataEdit_ = new CQChartsFillDataEdit;

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);

  fillDataToWidgets();
}

const CQChartsFillData &
CQChartsFillDataLineEdit::
fillData() const
{
  return dataEdit_->data();
}

void
CQChartsFillDataLineEdit::
setFillData(const CQChartsFillData &fillData)
{
  updateFillData(fillData, /*updateText*/ true);
}

void
CQChartsFillDataLineEdit::
updateFillData(const CQChartsFillData &fillData, bool updateText)
{
  connectSlots(false);

  dataEdit_->setData(fillData);

  connectSlots(true);

  if (updateText)
    fillDataToWidgets();

  emit fillDataChanged();
}

void
CQChartsFillDataLineEdit::
textChanged()
{
  CQChartsFillData fillData(edit_->text());

  if (! fillData.isValid())
    return;

  updateFillData(fillData, /*updateText*/ false);
}

void
CQChartsFillDataLineEdit::
fillDataToWidgets()
{
  connectSlots(false);

  if (fillData().isValid())
    edit_->setText(fillData().toString());
  else
    edit_->setText("");

  setToolTip(fillData().toString());

  connectSlots(true);
}

void
CQChartsFillDataLineEdit::
menuEditChanged()
{
  fillDataToWidgets();

  emit fillDataChanged();
}

void
CQChartsFillDataLineEdit::
connectSlots(bool b)
{
  connectBaseSlots(b);

  CQChartsWidgetUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(fillDataChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsFillDataLineEdit::
drawPreview(QPainter *painter, const QRect &rect)
{
  CQChartsFillDataEditPreview::draw(painter, fillData(), rect, plot(), view());
}

//------

CQPropertyViewEditorFactory *
CQChartsFillDataPropertyViewType::
getEditor() const
{
  return new CQChartsFillDataPropertyViewEditor;
}

void
CQChartsFillDataPropertyViewType::
drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
            CQChartsPlot *plot, CQChartsView *view)
{
  CQChartsFillData data = value.value<CQChartsFillData>();

  CQChartsFillDataEditPreview::draw(painter, data, rect, plot, view);
}

QString
CQChartsFillDataPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsFillData>().toString();

  return str;
}

//------

CQChartsLineEditBase *
CQChartsFillDataPropertyViewEditor::
createPropertyEdit(QWidget *parent)
{
  return new CQChartsFillDataLineEdit(parent);
}

void
CQChartsFillDataPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsFillDataLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(fillDataChanged()), obj, method);
}

QVariant
CQChartsFillDataPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsFillDataLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->fillData());
}

void
CQChartsFillDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsFillDataLineEdit *>(w);
  assert(edit);

  CQChartsFillData data = var.value<CQChartsFillData>();

  edit->setFillData(data);
}

//------

CQChartsFillDataEdit::
CQChartsFillDataEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("fillDataEdit");

  setToolTip("Fill Data");

  //---

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  groupBox_ = CQUtil::makeWidget<CQGroupBox>("groupBox");

  groupBox_->setCheckable(true);
  groupBox_->setChecked(false);
  groupBox_->setTitle("Visible");

  layout->addWidget(groupBox_);

  //---

  auto *groupLayout = CQUtil::makeLayout<QGridLayout>(groupBox_, 2, 2);

  int row = 0;

  // color
  auto *colorLabel = CQUtil::makeLabelWidget<QLabel>("Color", "colorLabel");

  colorEdit_ = new CQChartsColorLineEdit;

  groupLayout->addWidget(colorLabel, row, 0);
  groupLayout->addWidget(colorEdit_, row, 1); ++row;

  // alpha
  alphaLabel_ = CQUtil::makeLabelWidget<CQDragLabel>("Alpha", "alphaLabel");

  alphaEdit_ = new CQChartsAlphaEdit;

  groupLayout->addWidget(alphaLabel_, row, 0);
  groupLayout->addWidget(alphaEdit_ , row, 1); ++row;

  // pattern
  auto *patternLabel = CQUtil::makeLabelWidget<QLabel>("Pattern", "patternLabel");

  patternEdit_ = new CQChartsFillPatternLineEdit;

  groupLayout->addWidget(patternLabel, row, 0);
  groupLayout->addWidget(patternEdit_, row, 1); ++row;

  //---

  preview_ = new CQChartsFillDataEditPreview(this);

  groupLayout->addWidget(preview_, row, 1); ++row;

  //---

  groupLayout->setRowStretch(row, 1);

  //---

  layout->addStretch(1);

  //---

  connectSlots(true);

  dataToWidgets();
}

void
CQChartsFillDataEdit::
setData(const CQChartsFillData &d)
{
  data_ = d;

  dataToWidgets();
}

void
CQChartsFillDataEdit::
setTitle(const QString &title)
{
  groupBox_->setTitle(title);
}

void
CQChartsFillDataEdit::
setPreview(bool b)
{
  preview_->setVisible(b);
}

void
CQChartsFillDataEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(connected_, w, from, this, to);
  };

  connectDisconnect(groupBox_, SIGNAL(clicked(bool)), SLOT(widgetsToData()));
  connectDisconnect(colorEdit_, SIGNAL(colorChanged()), SLOT(widgetsToData()));
  connectDisconnect(alphaEdit_, SIGNAL(alphaChanged()), SLOT(widgetsToData()));
  connectDisconnect(alphaLabel_, SIGNAL(dragValueChanged(double)),
                    SLOT(alphaDragValueChanged(double)));
  connectDisconnect(patternEdit_, SIGNAL(fillPatternChanged()), SLOT(widgetsToData()));
}

void
CQChartsFillDataEdit::
dataToWidgets()
{
  connectSlots(false);

  groupBox_   ->setChecked    (data_.isVisible());
  colorEdit_  ->setColor      (data_.color());
  alphaEdit_  ->setAlpha      (data_.alpha());
  patternEdit_->setFillPattern(data_.pattern());

  preview_->update();

  connectSlots(true);
}

void
CQChartsFillDataEdit::
widgetsToData()
{
  data_.setVisible(groupBox_   ->isChecked());
  data_.setColor  (colorEdit_  ->color());
  data_.setAlpha  (alphaEdit_  ->alpha());
  data_.setPattern(patternEdit_->fillPattern());

  preview_->update();

  emit fillDataChanged();
}

void
CQChartsFillDataEdit::
alphaDragValueChanged(double da)
{
  double a = std::min(std::max(data_.alpha().value() + da, 0.0), 1.0);

  alphaEdit_->setAlpha(CQChartsAlpha(a));

  widgetsToData();
}

//------

CQChartsFillDataEditPreview::
CQChartsFillDataEditPreview(CQChartsFillDataEdit *edit) :
 CQChartsEditPreview(edit), edit_(edit)
{
  setToolTip("Fill Preview");
}

void
CQChartsFillDataEditPreview::
draw(QPainter *painter)
{
  const auto &data = edit_->data();

  draw(painter, data, rect(), edit_->plot(), edit_->view());
}

void
CQChartsFillDataEditPreview::
draw(QPainter *painter, const CQChartsFillData &data, const QRect &rect,
     CQChartsPlot *plot, CQChartsView *view)
{
  // set brush
  QColor fc = interpColor(plot, view, data.color());

  QBrush brush;

  CQChartsBrushData brushData;

  brushData.setVisible(data.isVisible());
  brushData.setColor  (fc);
  brushData.setAlpha  (data.alpha());
  brushData.setPattern(data.pattern());

  CQChartsDrawUtil::setBrush(brush, brushData);

  //---

  // draw fill
  painter->fillRect(rect, brush);
}
