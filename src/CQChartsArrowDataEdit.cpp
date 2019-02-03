#include <CQChartsArrowDataEdit.h>

#include <CQChartsLengthEdit.h>
#include <CQAngleSpinBox.h>

#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

CQChartsArrowDataEdit::
CQChartsArrowDataEdit(QWidget *parent) :
 QFrame(parent)
{
  QGridLayout *layout = new QGridLayout(this);

  // relative
  QLabel *relativeLabel = new QLabel("Relative");

  relativeEdit_ = new QCheckBox;

  connect(relativeEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

  layout->addWidget(relativeLabel, 0, 0);
  layout->addWidget(relativeEdit_, 0, 1);

  // length
  QLabel *lengthLabel = new QLabel("Length");

  lengthEdit_ = new CQChartsLengthEdit;

  connect(lengthEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));

  layout->addWidget(lengthLabel, 1, 0);
  layout->addWidget(lengthEdit_, 1, 1);

  // angle
  QLabel *angleLabel = new QLabel("Angle");

  angleEdit_ = new CQAngleSpinBox;

  connect(angleEdit_, SIGNAL(angleChanged(const CAngle &)), this, SLOT(widgetsToData()));

  layout->addWidget(angleLabel, 2, 0);
  layout->addWidget(angleEdit_, 2, 1);

  // back angle
  QLabel *backAngleLabel = new QLabel("Back Angle");

  backAngleEdit_ = new CQAngleSpinBox;

  connect(backAngleEdit_, SIGNAL(angleChanged(const CAngle &)), this, SLOT(widgetsToData()));

  layout->addWidget(backAngleLabel, 3, 0);
  layout->addWidget(backAngleEdit_, 3, 1);

  // fhead
  QLabel *fheadLabel = new QLabel("Front Head");

  fheadEdit_ = new QCheckBox;

  connect(fheadEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

  layout->addWidget(fheadLabel, 4, 0);
  layout->addWidget(fheadEdit_, 4, 1);

  // fhead
  QLabel *theadLabel = new QLabel("Tail Head");

  theadEdit_ = new QCheckBox;

  connect(theadEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

  layout->addWidget(theadLabel, 5, 0);
  layout->addWidget(theadEdit_, 5, 1);

  // line ends
  QLabel *lineEndsLabel = new QLabel("Line Ends");

  lineEndsEdit_ = new QCheckBox;

  connect(lineEndsEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

  layout->addWidget(lineEndsLabel, 6, 0);
  layout->addWidget(lineEndsEdit_, 6, 1);

  // line width
  QLabel *lineWidthLabel = new QLabel("Line Width");

  lineWidthEdit_ = new CQChartsLengthEdit;

  connect(lineWidthEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));

  layout->addWidget(lineWidthLabel, 7, 0);
  layout->addWidget(lineWidthEdit_, 7, 1);

  //---

  preview_ = new CQChartsArrowDataEditPreview(this);

  layout->addWidget(preview_, 8, 1);

  //---

  layout->setRowStretch(9, 1);

  //---

  dataToWidgets();
}

void
CQChartsArrowDataEdit::
dataToWidgets()
{
  disconnect(relativeEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));
  disconnect(lengthEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));
  disconnect(angleEdit_, SIGNAL(angleChanged(const CAngle &)), this, SLOT(widgetsToData()));
  disconnect(backAngleEdit_, SIGNAL(angleChanged(const CAngle &)), this, SLOT(widgetsToData()));
  disconnect(fheadEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));
  disconnect(theadEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));
  disconnect(lineEndsEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));
  disconnect(lineWidthEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));

  relativeEdit_ ->setChecked(data_.relative);
  lengthEdit_   ->setLength (data_.length);
  angleEdit_    ->setAngle  (CAngle(data_.angle));
  backAngleEdit_->setAngle  (CAngle(data_.backAngle));
  fheadEdit_    ->setChecked(data_.fhead);
  theadEdit_    ->setChecked(data_.thead);
  lineEndsEdit_ ->setChecked(data_.lineEnds);
  lineWidthEdit_->setLength (data_.lineWidth);

  preview_->update();

  connect(relativeEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));
  connect(lengthEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));
  connect(angleEdit_, SIGNAL(angleChanged(const CAngle &)), this, SLOT(widgetsToData()));
  connect(backAngleEdit_, SIGNAL(angleChanged(const CAngle &)), this, SLOT(widgetsToData()));
  connect(fheadEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));
  connect(theadEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));
  connect(lineEndsEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));
  connect(lineWidthEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));
}

void
CQChartsArrowDataEdit::
widgetsToData()
{
  data_.relative  = relativeEdit_->isChecked();
  data_.length    = lengthEdit_->length();
  data_.angle     = angleEdit_->getAngle().value();
  data_.backAngle = backAngleEdit_->getAngle().value();
  data_.fhead     = fheadEdit_->isChecked();
  data_.thead     = theadEdit_->isChecked();
  data_.lineEnds  = lineEndsEdit_->isChecked();
  data_.lineWidth = lineWidthEdit_->length();

  preview_->update();

  emit arrowDataChanged();
}

//------

CQChartsArrowDataEditPreview::
CQChartsArrowDataEditPreview(CQChartsArrowDataEdit *edit) :
 edit_(edit)
{
}

void
CQChartsArrowDataEditPreview::
paintEvent(QPaintEvent *)
{
  //QPainter painter(this);

  //painter.setRenderHints(QPainter::Antialiasing);

  //const CQChartsArrowData &data = edit_->data();
}

QSize
CQChartsArrowDataEditPreview::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("XXXX"), fm.height() + 4);
}
