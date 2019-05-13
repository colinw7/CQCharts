#include <CQChartsFillDataEdit.h>

#include <CQChartsColorEdit.h>
#include <CQChartsAlphaEdit.h>
#include <CQChartsFillPatternEdit.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQGroupBox.h>
#include <CQUtil.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

CQChartsFillDataLineEdit::
CQChartsFillDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("fillDataLineEdit");

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
  CQChartsLineEditBase::connectSlots(b);

  if (b)
    connect(dataEdit_, SIGNAL(fillDataChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(dataEdit_, SIGNAL(fillDataChanged()), this, SLOT(menuEditChanged()));
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
  CQChartsFillDataLineEdit *edit = qobject_cast<CQChartsFillDataLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(fillDataChanged()), obj, method);
}

QVariant
CQChartsFillDataPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsFillDataLineEdit *edit = qobject_cast<CQChartsFillDataLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->fillData());
}

void
CQChartsFillDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsFillDataLineEdit *edit = qobject_cast<CQChartsFillDataLineEdit *>(w);
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

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  groupBox_ = CQUtil::makeWidget<CQGroupBox>("groupBox");

  groupBox_->setCheckable(true);
  groupBox_->setChecked(false);
  groupBox_->setTitle("Visible");

  layout->addWidget(groupBox_);

  //---

  QGridLayout *groupLayout = CQUtil::makeLayout<QGridLayout>(groupBox_, 2, 2);

  // color
  QLabel *colorLabel = CQUtil::makeLabelWidget<QLabel>("Color", "color");

  colorEdit_ = new CQChartsColorLineEdit;

  groupLayout->addWidget(colorLabel, 0, 0);
  groupLayout->addWidget(colorEdit_, 0, 1);

  // alpha
  QLabel *alphaLabel = CQUtil::makeLabelWidget<QLabel>("Alpha", "alpha");

  alphaEdit_ = new CQChartsAlphaEdit;

  groupLayout->addWidget(alphaLabel, 1, 0);
  groupLayout->addWidget(alphaEdit_, 1, 1);

  // pattern
  QLabel *patternLabel = CQUtil::makeLabelWidget<QLabel>("Pattern", "pattern");

  patternEdit_ = new CQChartsFillPatternEdit;

  groupLayout->addWidget(patternLabel, 2, 0);
  groupLayout->addWidget(patternEdit_, 2, 1);

  //---

  preview_ = new CQChartsFillDataEditPreview(this);

  groupLayout->addWidget(preview_, 3, 1);

  //---

  groupLayout->setRowStretch(4, 1);

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

  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    if (b)
      connect(w, from, this, to);
    else
      disconnect(w, from, this, to);
  };

  connectDisconnect(b, groupBox_, SIGNAL(clicked(bool)), SLOT(widgetsToData()));
  connectDisconnect(b, colorEdit_, SIGNAL(colorChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, alphaEdit_, SIGNAL(valueChanged(double)), SLOT(widgetsToData()));
  connectDisconnect(b, patternEdit_, SIGNAL(fillPatternChanged()), SLOT(widgetsToData()));
}

void
CQChartsFillDataEdit::
dataToWidgets()
{
  connectSlots(false);

  groupBox_->setChecked(data_.isVisible());

  colorEdit_  ->setColor      (data_.color());
  alphaEdit_  ->setValue      (data_.alpha());
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
  data_.setAlpha  (alphaEdit_  ->value());
  data_.setPattern(patternEdit_->fillPattern());

  preview_->update();

  emit fillDataChanged();
}

//------

CQChartsFillDataEditPreview::
CQChartsFillDataEditPreview(CQChartsFillDataEdit *edit) :
 CQChartsEditPreview(edit), edit_(edit)
{
}

void
CQChartsFillDataEditPreview::
draw(QPainter *painter)
{
  const CQChartsFillData &data = edit_->data();

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

  CQChartsUtil::setBrush(brush, data.isVisible(), fc, data.alpha(), data.pattern());

  //---

  // draw fill
  painter->fillRect(rect, brush);
}
