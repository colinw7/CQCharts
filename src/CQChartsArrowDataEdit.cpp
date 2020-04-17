#include <CQChartsArrowDataEdit.h>
#include <CQChartsArrow.h>

#include <CQChartsLengthEdit.h>
#include <CQChartsAngleEdit.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsPaintDevice.h>

#include <CQWidgetMenu.h>
#include <CQCheckBox.h>
#include <CQUtil.h>

#include <QLabel>
#include <QVBoxLayout>

CQChartsArrowDataLineEdit::
CQChartsArrowDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("arrowDataLineEdit");

  setToolTip("Arrow Data");

  //---

  menuEdit_ = dataEdit_ = new CQChartsArrowDataEdit;

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);

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

  connectSlots(true);

  if (updateText)
    arrowDataToWidgets();

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
  connectBaseSlots(b);

  CQChartsWidgetUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(arrowDataChanged()), this, SLOT(menuEditChanged()));
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
  auto *edit = qobject_cast<CQChartsArrowDataLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(arrowDataChanged()), obj, method);
}

QVariant
CQChartsArrowDataPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsArrowDataLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->arrowData());
}

void
CQChartsArrowDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsArrowDataLineEdit *>(w);
  assert(edit);

  CQChartsArrowData data = var.value<CQChartsArrowData>();

  edit->setArrowData(data);
}

//------

CQChartsArrowDataEdit::
CQChartsArrowDataEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("lineDataEdit");

  //---

  int row = 0;

  auto *layout = CQUtil::makeLayout<QGridLayout>(this, 0, 2);

  //---

#if 0
  // relative
  relativeEdit_ = CQUtil::makeWidget<CQCheckBox>("relative");

  relativeEdit_->setToolTip("Is end point a delta from start point");

  CQChartsWidgetUtil::addGridLabelWidget(layout, "Relative", relativeEdit_, row);
#endif

  // length
  lengthEdit_ = CQUtil::makeWidget<CQChartsLengthEdit>("length");

  lengthEdit_->setToolTip("arrow head length");

  CQChartsWidgetUtil::addGridLabelWidget(layout, "Length", lengthEdit_, row);

  // angle
  angleEdit_ = CQUtil::makeWidget<CQChartsAngleEdit>("angle");

  angleEdit_->setToolTip("arrow head angle");

  CQChartsWidgetUtil::addGridLabelWidget(layout, "Angle", angleEdit_, row);

  // back angle
  backAngleEdit_ = CQUtil::makeWidget<CQChartsAngleEdit>("back_angle");

  backAngleEdit_->setToolTip("arrow head back angle");

  CQChartsWidgetUtil::addGridLabelWidget(layout, "Back Angle", backAngleEdit_, row);

  // fhead
  fheadEdit_ = CQUtil::makeWidget<CQCheckBox>("fhead");

  fheadEdit_->setToolTip("draw arrow head at start");

  CQChartsWidgetUtil::addGridLabelWidget(layout, "Front Head", fheadEdit_, row);

  // fhead
  theadEdit_ = CQUtil::makeWidget<CQCheckBox>("thead");

  theadEdit_->setToolTip("draw arrow head at end");

  CQChartsWidgetUtil::addGridLabelWidget(layout, "Tail Head", theadEdit_, row);

  // line ends
  lineEndsEdit_ = CQUtil::makeWidget<CQCheckBox>("line_ends");

  lineEndsEdit_->setToolTip("draw lines for arrow heads");

  CQChartsWidgetUtil::addGridLabelWidget(layout, "Line Ends", lineEndsEdit_, row);

  // line width
  lineWidthEdit_ = CQUtil::makeWidget<CQChartsLengthEdit>("line_width");

  lineWidthEdit_->setToolTip("connecting line width");

  CQChartsWidgetUtil::addGridLabelWidget(layout, "Line Width", lineWidthEdit_, row);

  //---

  preview_ = new CQChartsArrowDataEditPreview(this);

  layout->addWidget(preview_, 8, 1);

  //---

  layout->setRowStretch(9, 1);

  //---

  connectSlots(true);

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
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(connected_, w, from, this, to);
  };

//connectDisconnect(relativeEdit_, SIGNAL(stateChanged(int)), SLOT(widgetsToData()));
  connectDisconnect(lengthEdit_, SIGNAL(lengthChanged()), SLOT(widgetsToData()));
  connectDisconnect(angleEdit_, SIGNAL(angleChanged()), SLOT(widgetsToData()));
  connectDisconnect(backAngleEdit_, SIGNAL(angleChanged()), SLOT(widgetsToData()));
  connectDisconnect(fheadEdit_, SIGNAL(stateChanged(int)), SLOT(widgetsToData()));
  connectDisconnect(theadEdit_, SIGNAL(stateChanged(int)), SLOT(widgetsToData()));
  connectDisconnect(lineEndsEdit_, SIGNAL(stateChanged(int)), SLOT(widgetsToData()));
  connectDisconnect(lineWidthEdit_, SIGNAL(lengthChanged()), SLOT(widgetsToData()));
}

void
CQChartsArrowDataEdit::
dataToWidgets()
{
  connectSlots(false);

//relativeEdit_ ->setChecked(data_.isRelative());
  lengthEdit_   ->setLength (data_.length());
  angleEdit_    ->setAngle  (data_.angle());
  backAngleEdit_->setAngle  (data_.backAngle());
  fheadEdit_    ->setChecked(data_.isFHead());
  theadEdit_    ->setChecked(data_.isTHead());
  lineEndsEdit_ ->setChecked(data_.isLineEnds());
  lineWidthEdit_->setLength (data_.lineWidth());

  preview_->update();

  connectSlots(true);
}

void
CQChartsArrowDataEdit::
widgetsToData()
{
//data_.setRelative (relativeEdit_->isChecked());
  data_.setLength   (lengthEdit_->length());
  data_.setAngle    (angleEdit_->angle());
  data_.setBackAngle(backAngleEdit_->angle());
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
  setToolTip("Arrow Preview");
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
  CQChartsGeom::Point from(rect.left () + 2, rect.center().y());
  CQChartsGeom::Point to  (rect.right() - 2, rect.center().y());

  CQChartsArrow arrow((CQChartsView *) nullptr, from, to);

  arrow.setData(data);

  CQChartsPixelPainter device(painter);

  arrow.draw(&device);
}
