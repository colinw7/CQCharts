#include <CQChartsTextDataEdit.h>

#include <CQChartsColorEdit.h>
#include <CQChartsAlphaEdit.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>

#include <CQFontEdit.h>
#include <CQAngleSpinBox.h>
#include <CQAlignEdit.h>
#include <CQGroupBox.h>
#include <CQCheckBox.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

CQChartsTextDataLineEdit::
CQChartsTextDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("textDataLineEdit");

  //---

  menuEdit_ = dataEdit_ = new CQChartsTextDataEdit;

  menu_->setWidget(dataEdit_);

  connect(dataEdit_, SIGNAL(textDataChanged()), this, SLOT(menuEditChanged()));

  //---

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

  if (updateText)
    textDataToWidgets();

  connectSlots(true);

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

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  //---

  if (optional) {
    groupBox_ = new CQGroupBox;

    groupBox_->setObjectName("groupBox");
    groupBox_->setCheckable(true);
    groupBox_->setChecked(false);
    groupBox_->setTitle("Visible");

    connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));

    layout->addWidget(groupBox_);
  }

  //---

  QGridLayout *groupLayout = new QGridLayout(groupBox_);

  if (! optional)
    layout->addLayout(groupLayout);

  // font
  QLabel *fontLabel = new QLabel("Font");
  fontLabel->setObjectName("fontLabel");

  fontEdit_ = new CQFontEdit;

  connect(fontEdit_, SIGNAL(fontChanged(const QFont &)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(fontLabel, 0, 0);
  groupLayout->addWidget(fontEdit_, 0, 1);

  // color
  QLabel *colorLabel = new QLabel("Color");
  colorLabel->setObjectName("colorLabel");

  colorEdit_ = new CQChartsColorLineEdit;

  connect(colorEdit_, SIGNAL(colorChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(colorLabel, 1, 0);
  groupLayout->addWidget(colorEdit_, 1, 1);

  // alpha
  QLabel *alphaLabel = new QLabel("Alpha");

  alphaEdit_ = new CQChartsAlphaEdit;

  connect(alphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(alphaLabel, 2, 0);
  groupLayout->addWidget(alphaEdit_, 2, 1);

  // angle
  QLabel *angleLabel = new QLabel("Angle");

  angleLabel->setObjectName("angleLabel");

  angleEdit_ = new CQAngleSpinBox;

  connect(angleEdit_, SIGNAL(angleChanged(const CAngle &)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(angleLabel, 3, 0);
  groupLayout->addWidget(angleEdit_, 3, 1);

  // contrast
  QLabel *contrastLabel = new QLabel("Contrast");

  contrastLabel->setObjectName("contrastLabel");

  contrastEdit_ = new CQCheckBox;

  contrastEdit_->setObjectName("contrastEdit");

  connect(contrastEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(contrastLabel, 4, 0);
  groupLayout->addWidget(contrastEdit_, 4, 1);

  // align
  QLabel *alignLabel = new QLabel("Align");

  alignLabel->setObjectName("alignLabel");

  alignEdit_ = new CQAlignEdit;

  alignEdit_->setObjectName("alignEdit");

  connect(alignEdit_, SIGNAL(valueChanged(Qt::Alignment)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(alignLabel, 5, 0);
  groupLayout->addWidget(alignEdit_, 5, 1);

  // formatted
  QLabel *formattedLabel = new QLabel("Formatted");

  formattedLabel->setObjectName("formattedLabel");

  formattedEdit_ = new CQCheckBox;

  formattedEdit_->setObjectName("formattedEdit");

  connect(formattedEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(formattedLabel, 6, 0);
  groupLayout->addWidget(formattedEdit_, 6, 1);

  // scaled
  QLabel *scaledLabel = new QLabel("Scaled");

  scaledLabel->setObjectName("scaledLabel");

  scaledEdit_ = new CQCheckBox;

  scaledEdit_->setObjectName("scaledEdit");

  connect(scaledEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(scaledLabel, 7, 0);
  groupLayout->addWidget(scaledEdit_, 7, 1);

  // html
  QLabel *htmlLabel = new QLabel("Html");

  htmlLabel->setObjectName("htmlLabel");

  htmlEdit_ = new CQCheckBox;

  htmlEdit_->setObjectName("htmlEdit");

  connect(htmlEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(htmlLabel, 8, 0);
  groupLayout->addWidget(htmlEdit_, 8, 1);

  //---

  preview_ = new CQChartsTextDataEditPreview(this);

  groupLayout->addWidget(preview_, 9, 1);

  //---

  groupLayout->setRowStretch(10, 1);

  //---

  layout->addStretch(1);

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
dataToWidgets()
{
  if (groupBox_)
    disconnect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));

  disconnect(fontEdit_, SIGNAL(fontChanged(const QFont &)), this, SLOT(widgetsToData()));
  disconnect(colorEdit_, SIGNAL(colorChanged()), this, SLOT(widgetsToData()));
  disconnect(alphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  disconnect(angleEdit_, SIGNAL(angleChanged(const CAngle &)), this, SLOT(widgetsToData()));
  disconnect(contrastEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));
  disconnect(alignEdit_, SIGNAL(valueChanged(Qt::Alignment)), this, SLOT(widgetsToData()));
  disconnect(formattedEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));
  disconnect(scaledEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));
  disconnect(htmlEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

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

  if (groupBox_)
    connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));

  connect(fontEdit_, SIGNAL(fontChanged(const QFont &)), this, SLOT(widgetsToData()));
  connect(colorEdit_, SIGNAL(colorChanged()), this, SLOT(widgetsToData()));
  connect(alphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  connect(angleEdit_, SIGNAL(angleChanged(const CAngle &)), this, SLOT(widgetsToData()));
  connect(contrastEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));
  connect(alignEdit_, SIGNAL(valueChanged(Qt::Alignment)), this, SLOT(widgetsToData()));
  connect(formattedEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));
  connect(scaledEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));
  connect(htmlEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));
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
  painter->setFont(data.font());

  //---

  // draw text
  // TODO: angle, align, formatted, scaled, html

  QFontMetrics fm(data.font());

  int tx = rect.left() + 2;
  int ty = rect.center().y() + (fm.ascent() - fm.descent())/2;

  QString text("ABC abc");

  if (data.isContrast())
    CQChartsDrawUtil::drawContrastText(painter, tx, ty, text);
  else
    CQChartsDrawUtil::drawSimpleText(painter, tx, ty, text);
}
