#include <CQChartsArrowDataEdit.h>
#include <CQChartsArrow.h>

#include <CQChartsLengthEdit.h>
#include <CQAngleSpinBox.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>

#include <CQWidgetMenu.h>

#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

CQChartsArrowDataLineEdit::
CQChartsArrowDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("arrowDataLineEdit");

  //---

  menuEdit_ = dataEdit_ = new CQChartsArrowDataEdit;

  menu_->setWidget(dataEdit_);

  connect(dataEdit_, SIGNAL(arrowDataChanged()), this, SLOT(menuEditChanged()));

  //---

  arrowDataToWidgets();
}

const CQChartsArrowData &
CQChartsArrowDataLineEdit::
arrowData() const
{
  return dataEdit_->data();
}

void
CQChartsArrowDataLineEdit::
setArrowData(const CQChartsArrowData &arrowData)
{
  updateArrowData(arrowData, /*updateText*/ true);
}

void
CQChartsArrowDataLineEdit::
updateArrowData(const CQChartsArrowData &arrowData, bool updateText)
{
  connectSlots(false);

  dataEdit_->setData(arrowData);

  if (updateText)
    arrowDataToWidgets();

  connectSlots(true);

  emit arrowDataChanged();
}

void
CQChartsArrowDataLineEdit::
textChanged()
{
  CQChartsArrowData arrowData(edit_->text());

  if (! arrowData.isValid())
    return;

  updateArrowData(arrowData, /*updateText*/ false);
}

void
CQChartsArrowDataLineEdit::
arrowDataToWidgets()
{
  connectSlots(false);

  if (arrowData().isValid())
    edit_->setText(arrowData().toString());
  else
    edit_->setText("");

  setToolTip(arrowData().toString());

  connectSlots(true);
}

void
CQChartsArrowDataLineEdit::
menuEditChanged()
{
  arrowDataToWidgets();

  emit arrowDataChanged();
}

void
CQChartsArrowDataLineEdit::
connectSlots(bool b)
{
  CQChartsLineEditBase::connectSlots(b);

  if (b)
    connect(dataEdit_, SIGNAL(arrowDataChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(dataEdit_, SIGNAL(arrowDataChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsArrowDataLineEdit::
drawPreview(QPainter *painter, const QRect &rect)
{
  CQChartsArrowData data = this->arrowData();

  data.setLength   (CQChartsLength(16, CQChartsUnits::PIXEL));
  data.setLineWidth(CQChartsLength( 6, CQChartsUnits::PIXEL));

  CQChartsArrowDataEditPreview::draw(painter, data, rect, plot(), view());
}

//------

CQPropertyViewEditorFactory *
CQChartsArrowDataPropertyViewType::
getEditor() const
{
  return new CQChartsArrowDataPropertyViewEditor;
}

void
CQChartsArrowDataPropertyViewType::
drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
            CQChartsPlot *plot, CQChartsView *view)
{
  CQChartsArrowData data = value.value<CQChartsArrowData>();

  CQChartsArrowDataEditPreview::draw(painter, data, rect, plot, view);
}

QString
CQChartsArrowDataPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsArrowData>().toString();

  return str;
}

//------

CQChartsLineEditBase *
CQChartsArrowDataPropertyViewEditor::
createPropertyEdit(QWidget *parent)
{
  return new CQChartsArrowDataLineEdit(parent);
}

void
CQChartsArrowDataPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsArrowDataLineEdit *edit = qobject_cast<CQChartsArrowDataLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(arrowDataChanged()), obj, method);
}

QVariant
CQChartsArrowDataPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsArrowDataLineEdit *edit = qobject_cast<CQChartsArrowDataLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->arrowData());
}

void
CQChartsArrowDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsArrowDataLineEdit *edit = qobject_cast<CQChartsArrowDataLineEdit *>(w);
  assert(edit);

  CQChartsArrowData data = var.value<CQChartsArrowData>();

  edit->setArrowData(data);
}

//------

CQChartsArrowDataEdit::
CQChartsArrowDataEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  QGridLayout *layout = new QGridLayout(this);

  // relative
  QLabel *relativeLabel = new QLabel("Relative");

  relativeEdit_ = new QCheckBox;

  relativeEdit_->setObjectName("relative");
  relativeEdit_->setToolTip("Is end point a delta from start point");

  connect(relativeEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

  layout->addWidget(relativeLabel, 0, 0);
  layout->addWidget(relativeEdit_, 0, 1);

  // length
  QLabel *lengthLabel = new QLabel("Length");

  lengthEdit_ = new CQChartsLengthEdit;

  lengthEdit_->setObjectName("relative");
  lengthEdit_->setToolTip("arrow head length");

  connect(lengthEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));

  layout->addWidget(lengthLabel, 1, 0);
  layout->addWidget(lengthEdit_, 1, 1);

  // angle
  QLabel *angleLabel = new QLabel("Angle");

  angleEdit_ = new CQAngleSpinBox;

  angleEdit_->setObjectName("angle");
  angleEdit_->setToolTip("arrow head angle");

  connect(angleEdit_, SIGNAL(angleChanged(const CAngle &)), this, SLOT(widgetsToData()));

  layout->addWidget(angleLabel, 2, 0);
  layout->addWidget(angleEdit_, 2, 1);

  // back angle
  QLabel *backAngleLabel = new QLabel("Back Angle");

  backAngleEdit_ = new CQAngleSpinBox;

  backAngleEdit_->setObjectName("back_angle");
  backAngleEdit_->setToolTip("arrow head back angle");

  connect(backAngleEdit_, SIGNAL(angleChanged(const CAngle &)), this, SLOT(widgetsToData()));

  layout->addWidget(backAngleLabel, 3, 0);
  layout->addWidget(backAngleEdit_, 3, 1);

  // fhead
  QLabel *fheadLabel = new QLabel("Front Head");

  fheadEdit_ = new QCheckBox;

  fheadEdit_->setObjectName("fhead");
  fheadEdit_->setToolTip("draw arrow head at start");

  connect(fheadEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

  layout->addWidget(fheadLabel, 4, 0);
  layout->addWidget(fheadEdit_, 4, 1);

  // fhead
  QLabel *theadLabel = new QLabel("Tail Head");

  theadEdit_ = new QCheckBox;

  theadEdit_->setObjectName("thead");
  theadEdit_->setToolTip("draw arrow head at end");

  connect(theadEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

  layout->addWidget(theadLabel, 5, 0);
  layout->addWidget(theadEdit_, 5, 1);

  // line ends
  QLabel *lineEndsLabel = new QLabel("Line Ends");

  lineEndsEdit_ = new QCheckBox;

  lineEndsEdit_->setObjectName("line_ends");
  lineEndsEdit_->setToolTip("draw lines for arrow heads");

  connect(lineEndsEdit_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToData()));

  layout->addWidget(lineEndsLabel, 6, 0);
  layout->addWidget(lineEndsEdit_, 6, 1);

  // line width
  QLabel *lineWidthLabel = new QLabel("Line Width");

  lineWidthEdit_ = new CQChartsLengthEdit;

  lineWidthEdit_->setObjectName("line_width");
  lineWidthEdit_->setToolTip("connecting line width");

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
setData(const CQChartsArrowData &d)
{
  data_ = d;

  dataToWidgets();
}

void
CQChartsArrowDataEdit::
setTitle(const QString &)
{
}

void
CQChartsArrowDataEdit::
setPreview(bool b)
{
  preview_->setVisible(b);
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

  relativeEdit_ ->setChecked(data_.isRelative());
  lengthEdit_   ->setLength (data_.length());
  angleEdit_    ->setAngle  (CAngle(data_.angle()));
  backAngleEdit_->setAngle  (CAngle(data_.backAngle()));
  fheadEdit_    ->setChecked(data_.isFHead());
  theadEdit_    ->setChecked(data_.isTHead());
  lineEndsEdit_ ->setChecked(data_.isLineEnds());
  lineWidthEdit_->setLength (data_.lineWidth());

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
  data_.setRelative (relativeEdit_->isChecked());
  data_.setLength   (lengthEdit_->length());
  data_.setAngle    (angleEdit_->getAngle().value());
  data_.setBackAngle(backAngleEdit_->getAngle().value());
  data_.setFHead    (fheadEdit_->isChecked());
  data_.setTHead    (theadEdit_->isChecked());
  data_.setLineEnds (lineEndsEdit_->isChecked());
  data_.setLineWidth(lineWidthEdit_->length());

  preview_->update();

  emit arrowDataChanged();
}

//------

CQChartsArrowDataEditPreview::
CQChartsArrowDataEditPreview(CQChartsArrowDataEdit *edit) :
 CQChartsEditPreview(edit), edit_(edit)
{
}

void
CQChartsArrowDataEditPreview::
draw(QPainter *painter)
{
  const CQChartsArrowData &data = edit_->data();

  draw(painter, data, rect(), edit_->plot(), edit_->view());
}

void
CQChartsArrowDataEditPreview::
draw(QPainter *painter, const CQChartsArrowData &data, const QRect &rect,
     CQChartsPlot * /*plot*/, CQChartsView * /*view*/)
{
  QPointF from(rect.left () + 2, rect.center().y());
  QPointF to  (rect.right() - 2, rect.center().y());

  CQChartsArrow arrow((CQChartsView *) nullptr, from, to);

  arrow.setData(data);

  arrow.draw(painter);
}
