#include <CQChartsTextDataEdit.h>

#include <CQChartsFontEdit.h>
#include <CQChartsColorEdit.h>
#include <CQChartsAlphaEdit.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQUtil.h>

#include <CQAngleSpinBox.h>
#include <CQAlignEdit.h>
#include <CQGroupBox.h>
#include <CQCheckBox.h>

#include <QLabel>
#include <QVBoxLayout>

CQChartsTextDataLineEdit::
CQChartsTextDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("textDataLineEdit");

  //---

  menuEdit_ = dataEdit_ = new CQChartsTextDataEdit;

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);

  textDataToWidgets();
}

const CQChartsTextData &
CQChartsTextDataLineEdit::
textData() const
{
  return dataEdit_->data();
}

void
CQChartsTextDataLineEdit::
setTextData(const CQChartsTextData &textData)
{
  updateTextData(textData, /*updateText*/ true);
}

void
CQChartsTextDataLineEdit::
setNoFocus()
{
  dataEdit_->setNoFocus();
}

void
CQChartsTextDataLineEdit::
updateTextData(const CQChartsTextData &textData, bool updateText)
{
  connectSlots(false);

  dataEdit_->setData(textData);

  connectSlots(true);

  if (updateText)
    textDataToWidgets();

  emit textDataChanged();
}

void
CQChartsTextDataLineEdit::
textChanged()
{
  CQChartsTextData textData(edit_->text());

  if (! textData.isValid())
    return;

  updateTextData(textData, /*updateText*/ false);
}

void
CQChartsTextDataLineEdit::
textDataToWidgets()
{
  connectSlots(false);

  if (textData().isValid())
    edit_->setText(textData().toString());
  else
    edit_->setText("");

  setToolTip(textData().toString());

  connectSlots(true);
}

void
CQChartsTextDataLineEdit::
menuEditChanged()
{
  textDataToWidgets();

  emit textDataChanged();
}

void
CQChartsTextDataLineEdit::
connectSlots(bool b)
{
  CQChartsLineEditBase::connectSlots(b);

  if (b)
    connect(dataEdit_, SIGNAL(textDataChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(dataEdit_, SIGNAL(textDataChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsTextDataLineEdit::
drawPreview(QPainter *painter, const QRect &rect)
{
  CQChartsTextDataEditPreview::draw(painter, textData(), rect, plot(), view());
}

//------

CQPropertyViewEditorFactory *
CQChartsTextDataPropertyViewType::
getEditor() const
{
  return new CQChartsTextDataPropertyViewEditor;
}

void
CQChartsTextDataPropertyViewType::
drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
            CQChartsPlot *plot, CQChartsView *view)
{
  CQChartsTextData data = value.value<CQChartsTextData>();

  CQChartsTextDataEditPreview::draw(painter, data, rect, plot, view);
}

QString
CQChartsTextDataPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsTextData>().toString();

  return str;
}

//------

CQChartsLineEditBase *
CQChartsTextDataPropertyViewEditor::
createPropertyEdit(QWidget *parent)
{
  CQChartsTextDataLineEdit *edit = new CQChartsTextDataLineEdit(parent);

  edit->setNoFocus();

  return edit;
}

void
CQChartsTextDataPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsTextDataLineEdit *edit = qobject_cast<CQChartsTextDataLineEdit *>(w);
  assert(edit);

  edit->setEditable(false);

  QObject::connect(edit, SIGNAL(textDataChanged()), obj, method);
}

QVariant
CQChartsTextDataPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsTextDataLineEdit *edit = qobject_cast<CQChartsTextDataLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->textData());
}

void
CQChartsTextDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsTextDataLineEdit *edit = qobject_cast<CQChartsTextDataLineEdit *>(w);
  assert(edit);

  CQChartsTextData data = var.value<CQChartsTextData>();

  edit->setTextData(data);
}

//------

CQChartsTextDataEdit::
CQChartsTextDataEdit(QWidget *parent, bool optional) :
 CQChartsEditBase(parent)
{
  setObjectName("textDataEdit");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  if (optional) {
    groupBox_ = CQUtil::makeLabelWidget<CQGroupBox>("Visible", "groupBox");

    groupBox_->setCheckable(true);
    groupBox_->setChecked(false);

    layout->addWidget(groupBox_);
  }

  //---

  QGridLayout *groupLayout = CQUtil::makeLayout<QGridLayout>(groupBox_, 2, 2);

  if (! optional)
    layout->addLayout(groupLayout);

  // font
  QLabel *fontLabel = CQUtil::makeLabelWidget<QLabel>("Font", "fontLabel");

  fontEdit_ = new CQChartsFontLineEdit;

  groupLayout->addWidget(fontLabel, 0, 0);
  groupLayout->addWidget(fontEdit_, 0, 1);

  // color
  QLabel *colorLabel = CQUtil::makeLabelWidget<QLabel>("Color", "colorLabel");

  colorEdit_ = new CQChartsColorLineEdit;

  groupLayout->addWidget(colorLabel, 1, 0);
  groupLayout->addWidget(colorEdit_, 1, 1);

  // alpha
  QLabel *alphaLabel = CQUtil::makeLabelWidget<QLabel>("Alpha", "alphaLabel");

  alphaEdit_ = new CQChartsAlphaEdit;

  groupLayout->addWidget(alphaLabel, 2, 0);
  groupLayout->addWidget(alphaEdit_, 2, 1);

  // angle
  QLabel *angleLabel = CQUtil::makeLabelWidget<QLabel>("Angle", "angleLabel");

  angleEdit_ = CQUtil::makeWidget<CQAngleSpinBox>("angleEdit");

  groupLayout->addWidget(angleLabel, 3, 0);
  groupLayout->addWidget(angleEdit_, 3, 1);

  // contrast
  QLabel *contrastLabel = CQUtil::makeLabelWidget<QLabel>("Contrast", "contrastLabel");

  contrastEdit_ = CQUtil::makeWidget<CQCheckBox>("contrastEdit");

  groupLayout->addWidget(contrastLabel, 4, 0);
  groupLayout->addWidget(contrastEdit_, 4, 1);

  // align
  QLabel *alignLabel = CQUtil::makeLabelWidget<QLabel>("Align", "alignLabel");

  alignEdit_ = CQUtil::makeWidget<CQAlignEdit>("alignEdit");

  groupLayout->addWidget(alignLabel, 5, 0);
  groupLayout->addWidget(alignEdit_, 5, 1);

  // formatted
  QLabel *formattedLabel = CQUtil::makeLabelWidget<QLabel>("Formatted", "formattedLabel");

  formattedEdit_ = CQUtil::makeWidget<CQCheckBox>("formattedEdit");

  groupLayout->addWidget(formattedLabel, 6, 0);
  groupLayout->addWidget(formattedEdit_, 6, 1);

  // scaled
  QLabel *scaledLabel = CQUtil::makeLabelWidget<QLabel>("Scaled", "scaledLabel");

  scaledEdit_ = CQUtil::makeWidget<CQCheckBox>("scaledEdit");

  groupLayout->addWidget(scaledLabel, 7, 0);
  groupLayout->addWidget(scaledEdit_, 7, 1);

  // html
  QLabel *htmlLabel = CQUtil::makeLabelWidget<QLabel>("Html", "htmlLabel");

  htmlEdit_ = CQUtil::makeWidget<CQCheckBox>("htmlEdit");

  groupLayout->addWidget(htmlLabel, 8, 0);
  groupLayout->addWidget(htmlEdit_, 8, 1);

  //---

  preview_ = new CQChartsTextDataEditPreview(this);

  groupLayout->addWidget(preview_, 9, 1);

  //---

  groupLayout->setRowStretch(10, 1);

  //---

  layout->addStretch(1);

  //---

  connectSlots(true);

  dataToWidgets();
}

void
CQChartsTextDataEdit::
setData(const CQChartsTextData &d)
{
  data_ = d;

  dataToWidgets();
}

void
CQChartsTextDataEdit::
setTitle(const QString &title)
{
  if (groupBox_)
    groupBox_->setTitle(title);
}

void
CQChartsTextDataEdit::
setPreview(bool b)
{
  preview_->setVisible(b);
}

void
CQChartsTextDataEdit::
setNoFocus()
{
  fontEdit_ ->setNoFocus();
  colorEdit_->setNoFocus();

  if (groupBox_)
    groupBox_->setFocusPolicy(Qt::NoFocus);

  alphaEdit_    ->setFocusPolicy(Qt::NoFocus);
  angleEdit_    ->setFocusPolicy(Qt::NoFocus);
  contrastEdit_ ->setFocusPolicy(Qt::NoFocus);
  alignEdit_    ->setFocusPolicy(Qt::NoFocus);
  formattedEdit_->setFocusPolicy(Qt::NoFocus);
  scaledEdit_   ->setFocusPolicy(Qt::NoFocus);
  htmlEdit_     ->setFocusPolicy(Qt::NoFocus);
}

void
CQChartsTextDataEdit::
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

  if (groupBox_)
    connectDisconnect(b, groupBox_, SIGNAL(clicked(bool)), SLOT(widgetsToData()));

  connectDisconnect(b, fontEdit_, SIGNAL(fontChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, colorEdit_, SIGNAL(colorChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, alphaEdit_, SIGNAL(valueChanged(double)), SLOT(widgetsToData()));
  connectDisconnect(b, angleEdit_, SIGNAL(angleChanged(const CAngle &)), SLOT(widgetsToData()));
  connectDisconnect(b, contrastEdit_, SIGNAL(stateChanged(int)), SLOT(widgetsToData()));
  connectDisconnect(b, alignEdit_, SIGNAL(valueChanged(Qt::Alignment)), SLOT(widgetsToData()));
  connectDisconnect(b, formattedEdit_, SIGNAL(stateChanged(int)), SLOT(widgetsToData()));
  connectDisconnect(b, scaledEdit_, SIGNAL(stateChanged(int)), SLOT(widgetsToData()));
  connectDisconnect(b, htmlEdit_, SIGNAL(stateChanged(int)), SLOT(widgetsToData()));
}

void
CQChartsTextDataEdit::
dataToWidgets()
{
  connectSlots(false);

  if (groupBox_)
    groupBox_->setChecked(data_.isVisible());

  fontEdit_     ->setFont   (data_.font());
  colorEdit_    ->setColor  (data_.color());
  alphaEdit_    ->setValue  (data_.alpha());
  angleEdit_    ->setAngle  (CAngle(data_.angle()));
  contrastEdit_ ->setChecked(data_.isContrast());
  alignEdit_    ->setAlign  (data_.align());
  formattedEdit_->setChecked(data_.isFormatted());
  scaledEdit_   ->setChecked(data_.isScaled());
  htmlEdit_     ->setChecked(data_.isHtml());

  preview_->update();

  connectSlots(true);
}

void
CQChartsTextDataEdit::
widgetsToData()
{
  if (groupBox_)
    data_.setVisible(groupBox_->isChecked());

  data_.setFont     (fontEdit_->font());
  data_.setColor    (colorEdit_->color());
  data_.setAlpha    (alphaEdit_->value());
  data_.setAngle    (angleEdit_->getAngle().value());
  data_.setContrast (contrastEdit_->isChecked());
  data_.setAlign    (alignEdit_->align());
  data_.setFormatted(formattedEdit_->isChecked());
  data_.setScaled   (scaledEdit_->isChecked());
  data_.setHtml     (htmlEdit_->isChecked());

  preview_->update();

  emit textDataChanged();
}

//------

CQChartsTextDataEditPreview::
CQChartsTextDataEditPreview(CQChartsTextDataEdit *edit) :
 CQChartsEditPreview(edit), edit_(edit)
{
}

void
CQChartsTextDataEditPreview::
draw(QPainter *painter)
{
  const CQChartsTextData &data = edit_->data();

  draw(painter, data, rect(), edit_->plot(), edit_->view());
}

void
CQChartsTextDataEditPreview::
draw(QPainter *painter, const CQChartsTextData &data, const QRect &rect,
     CQChartsPlot *plot, CQChartsView *view)
{
  // set pen
  QColor pc = interpColor(plot, view, data.color());

  QPen pen;

  CQChartsUtil::setPen(pen, data.isVisible(), pc, data.alpha());

  painter->setPen(pen);

  //---

  // set font
  painter->setFont(data.font().calcFont());

  //---

  // draw text
  // TODO: angle, align, formatted, scaled, html

  QFontMetrics fm(painter->font());

  int tx = rect.left() + 2;
  int ty = rect.center().y() + (fm.ascent() - fm.descent())/2;

  QString text("ABC abc");

  CQChartsPixelPainter device(painter);

  if (data.isContrast())
    CQChartsDrawUtil::drawContrastText(&device, QPointF(tx, ty), text);
  else
    CQChartsDrawUtil::drawSimpleText(&device, QPointF(tx, ty), text);
}
