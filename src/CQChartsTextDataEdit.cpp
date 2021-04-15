#include <CQChartsTextDataEdit.h>
#include <CQChartsFontEdit.h>
#include <CQChartsColorEdit.h>
#include <CQChartsAlphaEdit.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsAngleEdit.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQUtil.h>

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

  setToolTip("Text Data");

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

  auto tip = QString("%1 (%2)").arg(textData().toString());

  edit_->setToolTip(tip);

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
  connectBaseSlots(b);

  CQChartsWidgetUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(textDataChanged()), this, SLOT(menuEditChanged()));
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
  auto data = value.value<CQChartsTextData>();

  CQChartsTextDataEditPreview::draw(painter, data, rect, plot, view);
}

QString
CQChartsTextDataPropertyViewType::
tip(const QVariant &value) const
{
  auto str = value.value<CQChartsTextData>().toString();

  return str;
}

//------

CQChartsLineEditBase *
CQChartsTextDataPropertyViewEditor::
createPropertyEdit(QWidget *parent)
{
  auto *edit = new CQChartsTextDataLineEdit(parent);

  edit->setNoFocus();

  return edit;
}

void
CQChartsTextDataPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsTextDataLineEdit *>(w);
  assert(edit);

  edit->setEditable(false);

  QObject::connect(edit, SIGNAL(textDataChanged()), obj, method);
}

QVariant
CQChartsTextDataPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsTextDataLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->textData());
}

void
CQChartsTextDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsTextDataLineEdit *>(w);
  assert(edit);

  auto data = var.value<CQChartsTextData>();

  edit->setTextData(data);
}

//------

CQChartsTextDataEdit::
CQChartsTextDataEdit(QWidget *parent, bool optional) :
 CQChartsEditBase(parent)
{
  setObjectName("textDataEdit");

  setToolTip("Text Data");

  //---

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  if (optional) {
    groupBox_ = CQUtil::makeLabelWidget<CQGroupBox>("Visible", "groupBox");

    groupBox_->setCheckable(true);
    groupBox_->setChecked(false);

    layout->addWidget(groupBox_);
  }

  //---

  auto *groupLayout = CQUtil::makeLayout<QGridLayout>(groupBox_, 2, 2);

  if (! optional)
    layout->addLayout(groupLayout);

  int row = 0;

  // color
  auto *colorLabel = CQUtil::makeLabelWidget<QLabel>("Color", "colorLabel");

  colorEdit_ = new CQChartsColorLineEdit;

  groupLayout->addWidget(colorLabel, row, 0);
  groupLayout->addWidget(colorEdit_, row, 1); ++row;

  // alpha
  auto *alphaLabel = CQUtil::makeLabelWidget<QLabel>("Alpha", "alphaLabel");

  alphaEdit_ = new CQChartsAlphaEdit;

  alphaEdit_->setToolTip("Alpha");

  groupLayout->addWidget(alphaLabel, row, 0);
  groupLayout->addWidget(alphaEdit_, row, 1); ++row;

  // font
  auto *fontLabel = CQUtil::makeLabelWidget<QLabel>("Font", "fontLabel");

  fontEdit_ = new CQChartsFontLineEdit;

  groupLayout->addWidget(fontLabel, row, 0);
  groupLayout->addWidget(fontEdit_, row, 1); ++row;

  // angle
  auto *angleLabel = CQUtil::makeLabelWidget<QLabel>("Angle", "angleLabel");

  angleEdit_ = CQUtil::makeWidget<CQChartsAngleEdit>("angleEdit");

  groupLayout->addWidget(angleLabel, row, 0);
  groupLayout->addWidget(angleEdit_, row, 1); ++row;

  // contrast
  auto *contrastLabel = CQUtil::makeLabelWidget<QLabel>("Contrast", "contrastLabel");

  contrastEdit_ = CQUtil::makeWidget<CQCheckBox>("contrastEdit");

  contrastEdit_->setToolTip("Draw contrast border around text");

  groupLayout->addWidget(contrastLabel, row, 0);
  groupLayout->addWidget(contrastEdit_, row, 1); ++row;

  // contrast alpha
  auto *contrastAlphaLabel =
    CQUtil::makeLabelWidget<QLabel>("Contrast Alpha", "contrastAlphaLabel");

  contrastAlphaEdit_ = new CQChartsAlphaEdit;

  contrastAlphaEdit_->setToolTip("Contrast Alpha");

  groupLayout->addWidget(contrastAlphaLabel, row, 0);
  groupLayout->addWidget(contrastAlphaEdit_, row, 1); ++row;

  // align
  auto *alignLabel = CQUtil::makeLabelWidget<QLabel>("Align", "alignLabel");

  alignEdit_ = CQUtil::makeWidget<CQAlignEdit>("alignEdit");

  alignEdit_->setToolTip("Text alignment");

  groupLayout->addWidget(alignLabel, row, 0);
  groupLayout->addWidget(alignEdit_, row, 1); ++row;

  // formatted
  auto *formattedLabel = CQUtil::makeLabelWidget<QLabel>("Formatted", "formattedLabel");

  formattedEdit_ = CQUtil::makeWidget<CQCheckBox>("formattedEdit");

  formattedEdit_->setToolTip("Is text reformatted to fit in rectangle");

  groupLayout->addWidget(formattedLabel, row, 0);
  groupLayout->addWidget(formattedEdit_, row, 1); ++row;

  // scaled
  auto *scaledLabel = CQUtil::makeLabelWidget<QLabel>("Scaled", "scaledLabel");

  scaledEdit_ = CQUtil::makeWidget<CQCheckBox>("scaledEdit");

  scaledEdit_->setToolTip("Is text scaled to fit in rectangle");

  groupLayout->addWidget(scaledLabel, row, 0);
  groupLayout->addWidget(scaledEdit_, row, 1); ++row;

  // html
  auto *htmlLabel = CQUtil::makeLabelWidget<QLabel>("Html", "htmlLabel");

  htmlEdit_ = CQUtil::makeWidget<CQCheckBox>("htmlEdit");

  htmlEdit_->setToolTip("Does text contain HTML control tags");

  groupLayout->addWidget(htmlLabel, row, 0);
  groupLayout->addWidget(htmlEdit_, row, 1); ++row;

  // clip length
  auto *clipLengthLabel = CQUtil::makeLabelWidget<QLabel>("Clip Length", "clipLength");

  clipLengthEdit_ = CQUtil::makeWidget<CQChartsLengthEdit>("clipLengthEdit");

  clipLengthEdit_->setToolTip("Text clip length");

  groupLayout->addWidget(clipLengthLabel, row, 0);
  groupLayout->addWidget(clipLengthEdit_, row, 1); ++row;

  // TODO : clip elide

  //---

  preview_ = new CQChartsTextDataEditPreview(this);

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
  if (groupBox_)
    groupBox_->setFocusPolicy(Qt::NoFocus);

  colorEdit_        ->setNoFocus();
  alphaEdit_        ->setFocusPolicy(Qt::NoFocus);
  fontEdit_         ->setNoFocus();
  angleEdit_        ->setFocusPolicy(Qt::NoFocus);
  contrastEdit_     ->setFocusPolicy(Qt::NoFocus);
  contrastAlphaEdit_->setFocusPolicy(Qt::NoFocus);
  alignEdit_        ->setFocusPolicy(Qt::NoFocus);
  formattedEdit_    ->setFocusPolicy(Qt::NoFocus);
  scaledEdit_       ->setFocusPolicy(Qt::NoFocus);
  htmlEdit_         ->setFocusPolicy(Qt::NoFocus);
  clipLengthEdit_   ->setFocusPolicy(Qt::NoFocus);
}

void
CQChartsTextDataEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(connected_, w, from, this, to);
  };

  if (groupBox_)
    connectDisconnect(groupBox_, SIGNAL(clicked(bool)), SLOT(widgetsToData()));

  connectDisconnect(colorEdit_, SIGNAL(colorChanged()), SLOT(widgetsToData()));
  connectDisconnect(alphaEdit_, SIGNAL(alphaChanged()), SLOT(widgetsToData()));
  connectDisconnect(fontEdit_, SIGNAL(fontChanged()), SLOT(widgetsToData()));
  connectDisconnect(angleEdit_, SIGNAL(angleChanged()), SLOT(widgetsToData()));
  connectDisconnect(contrastEdit_, SIGNAL(stateChanged(int)), SLOT(widgetsToData()));
  connectDisconnect(contrastAlphaEdit_, SIGNAL(alphaChanged()), SLOT(widgetsToData()));
  connectDisconnect(alignEdit_, SIGNAL(valueChanged(Qt::Alignment)), SLOT(widgetsToData()));
  connectDisconnect(formattedEdit_, SIGNAL(stateChanged(int)), SLOT(widgetsToData()));
  connectDisconnect(scaledEdit_, SIGNAL(stateChanged(int)), SLOT(widgetsToData()));
  connectDisconnect(htmlEdit_, SIGNAL(stateChanged(int)), SLOT(widgetsToData()));
  connectDisconnect(clipLengthEdit_, SIGNAL(lengthChanged()), SLOT(widgetsToData()));
}

void
CQChartsTextDataEdit::
dataToWidgets()
{
  connectSlots(false);

  if (groupBox_)
    groupBox_->setChecked(data_.isVisible());

  colorEdit_        ->setColor  (data_.color());
  alphaEdit_        ->setAlpha  (data_.alpha());
  fontEdit_         ->setFont   (data_.font());
  angleEdit_        ->setAngle  (data_.angle());
  contrastEdit_     ->setChecked(data_.isContrast());
  contrastAlphaEdit_->setAlpha  (data_.contrastAlpha());
  alignEdit_        ->setAlign  (data_.align());
  formattedEdit_    ->setChecked(data_.isFormatted());
  scaledEdit_       ->setChecked(data_.isScaled());
  htmlEdit_         ->setChecked(data_.isHtml());
  clipLengthEdit_   ->setLength (data_.clipLength());

  preview_->update();

  connectSlots(true);
}

void
CQChartsTextDataEdit::
widgetsToData()
{
  if (groupBox_)
    data_.setVisible(groupBox_->isChecked());

  data_.setColor        (colorEdit_->color());
  data_.setAlpha        (alphaEdit_->alpha());
  data_.setFont         (fontEdit_->font());
  data_.setAngle        (angleEdit_->angle());
  data_.setContrast     (contrastEdit_->isChecked());
  data_.setContrastAlpha(contrastAlphaEdit_->alpha());
  data_.setAlign        (alignEdit_->align());
  data_.setFormatted    (formattedEdit_->isChecked());
  data_.setScaled       (scaledEdit_->isChecked());
  data_.setHtml         (htmlEdit_->isChecked());
  data_.setClipLength   (clipLengthEdit_->length());

  preview_->update();

  emit textDataChanged();
}

//------

CQChartsTextDataEditPreview::
CQChartsTextDataEditPreview(CQChartsTextDataEdit *edit) :
 CQChartsEditPreview(edit), edit_(edit)
{
  setToolTip("Text Preview");
}

void
CQChartsTextDataEditPreview::
draw(QPainter *painter)
{
  const auto &data = edit_->data();

  draw(painter, data, rect(), edit_->plot(), edit_->view());
}

void
CQChartsTextDataEditPreview::
draw(QPainter *painter, const CQChartsTextData &data, const QRect &rect,
     CQChartsPlot *plot, CQChartsView *view)
{
  // set pen
  auto pc = interpColor(plot, view, data.color());

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

  CQChartsGeom::Point pt(tx, ty);

  QString text("ABC abc");

  CQChartsPixelPaintDevice device(painter);

  CQChartsTextOptions options;

  options.angle         = CQChartsAngle();
  options.align         = Qt::AlignLeft;
  options.contrast      = data.isContrast();
  options.contrastAlpha = data.contrastAlpha();
  options.clipLength    = plot->lengthPixelWidth(data.clipLength());

  CQChartsDrawUtil::drawTextAtPoint(&device, pt, text, options);
}
