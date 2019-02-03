#include <CQChartsTextDataEdit.h>

#include <CQChartsRotatedText.h>
#include <CQChartsColorEdit.h>
#include <CQChartsAlphaEdit.h>
#include <CQFontChooser.h>
#include <CQAngleSpinBox.h>
#include <CQAlignEdit.h>

#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

CQChartsTextDataEdit::
CQChartsTextDataEdit(QWidget *parent) :
 QFrame(parent)
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

  groupBox_     ->setChecked(data_.visible);
  fontEdit_     ->setFont   (data_.font);
  colorEdit_    ->setColor  (data_.color);
  alphaEdit_    ->setValue  (data_.alpha);
  angleEdit_    ->setAngle  (CAngle(data_.angle));
  contrastEdit_ ->setChecked(data_.contrast);
  alignEdit_    ->setAlign  (data_.align);
  formattedEdit_->setChecked(data_.formatted);
  scaledEdit_   ->setChecked(data_.scaled);
  htmlEdit_     ->setChecked(data_.html);

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
  data_.visible   = groupBox_->isChecked();
  data_.font      = fontEdit_->font();
  data_.color     = colorEdit_->color();
  data_.alpha     = alphaEdit_->value();
  data_.angle     = angleEdit_->getAngle().value();
  data_.contrast  = contrastEdit_->isChecked();
  data_.align     = alignEdit_->align();
  data_.formatted = formattedEdit_->isChecked();
  data_.scaled    = scaledEdit_->isChecked();
  data_.html      = htmlEdit_->isChecked();

  preview_->update();

  emit textDataChanged();
}

//------

CQChartsTextDataEditPreview::
CQChartsTextDataEditPreview(CQChartsTextDataEdit *edit) :
 edit_(edit)
{
}

void
CQChartsTextDataEditPreview::
paintEvent(QPaintEvent *)
{
  const CQChartsTextData &data = edit_->data();

  QPen pen;

  CQChartsUtil::setPen(pen, data.visible, data.color.color(), data.alpha,
                       0.0, CQChartsLineDash());

  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing);

  painter.setPen(pen);

  painter.setFont(data.font);

  // TODO: contrast, fomatted, html

  CQChartsRotatedText::drawRotatedText(&painter, rect().center().x(), rect().center().y(),
                                       "ABC abc", data.angle, data.align,
                                       /*alignBox*/false);
}

QSize
CQChartsTextDataEditPreview::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("XXXX"), fm.height() + 4);
}
