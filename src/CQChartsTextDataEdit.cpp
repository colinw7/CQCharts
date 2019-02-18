#include <CQChartsTextDataEdit.h>

#include <CQChartsRotatedText.h>
#include <CQChartsColorEdit.h>
#include <CQChartsAlphaEdit.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>

#include <CQFontChooser.h>
#include <CQAngleSpinBox.h>
#include <CQAlignEdit.h>

#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
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
  QColor c = palette().color(QPalette::Window);

  painter->fillRect(rect, QBrush(c));

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
  return new CQChartsTextDataLineEdit(parent);
}

void
CQChartsTextDataPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsTextDataLineEdit *edit = qobject_cast<CQChartsTextDataLineEdit *>(w);
  assert(edit);

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
CQChartsTextDataEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  QVBoxLayout *layout = new QVBoxLayout(this);

  groupBox_ = new QGroupBox;

  groupBox_->setCheckable(true);
  groupBox_->setChecked(false);
  groupBox_->setTitle("Visible");

  connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));

  layout->addWidget(groupBox_);

  //---

  QGridLayout *groupLayout = new QGridLayout(groupBox_);

  // font
  QLabel *fontLabel = new QLabel("Font");

  fontEdit_ = new CQFontChooser;

  connect(fontEdit_, SIGNAL(fontChanged(const QFont &)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(fontLabel, 0, 0);
  groupLayout->addWidget(fontEdit_, 0, 1);

  // color
  QLabel *colorLabel = new QLabel("Color");

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

  angleEdit_ = new CQAngleSpinBox;

  connect(angleEdit_, SIGNAL(angleChanged(const CAngle &)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(angleLabel, 3, 0);
  groupLayout->addWidget(angleEdit_, 3, 1);

  // contrast
  QLabel *contrastLabel = new QLabel("Contrast");

  contrastEdit_ = new QCheckBox;

  connect(contrastEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(contrastLabel, 4, 0);
  groupLayout->addWidget(contrastEdit_, 4, 1);

  // align
  QLabel *alignLabel = new QLabel("Align");

  alignEdit_ = new CQAlignEdit;

  connect(alignEdit_, SIGNAL(valueChanged(Qt::Alignment)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(alignLabel, 5, 0);
  groupLayout->addWidget(alignEdit_, 5, 1);

  // formatted
  QLabel *formattedLabel = new QLabel("Formatted");

  formattedEdit_ = new QCheckBox;

  connect(formattedEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(formattedLabel, 6, 0);
  groupLayout->addWidget(formattedEdit_, 6, 1);

  // scaled
  QLabel *scaledLabel = new QLabel("Scaled");

  scaledEdit_ = new QCheckBox;

  connect(scaledEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(scaledLabel, 7, 0);
  groupLayout->addWidget(scaledEdit_, 7, 1);

  // html
  QLabel *htmlLabel = new QLabel("Html");

  htmlEdit_ = new QCheckBox;

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
dataToWidgets()
{
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

  groupBox_     ->setChecked(data_.isVisible());
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
  data_.setVisible  (groupBox_->isChecked());
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
  // TODO: contrast, formatted, html

  CQChartsRotatedText::drawRotatedText(painter, rect.center().x(), rect.center().y(),
                                       "ABC abc", data.angle(), data.align(),
                                       /*alignBox*/false);
}
