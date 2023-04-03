#include <CQChartsLineDataEdit.h>
#include <CQChartsColorEdit.h>
#include <CQChartsAlphaEdit.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsLineDashEdit.h>
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

CQChartsLineDataLineEdit::
CQChartsLineDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("lineDataLineEdit");

  setToolTip("Line Data");

  //---

  menuEdit_ = dataEdit_ = new CQChartsLineDataEdit;

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);

  lineDataToWidgets();
}

const CQChartsLineData &
CQChartsLineDataLineEdit::
lineData() const
{
  return dataEdit_->data();
}

void
CQChartsLineDataLineEdit::
setLineData(const CQChartsLineData &lineData)
{
  updateLineData(lineData, /*updateText*/ true);
}

void
CQChartsLineDataLineEdit::
updateLineData(const CQChartsLineData &lineData, bool updateText)
{
  connectSlots(false);

  dataEdit_->setData(lineData);

  connectSlots(true);

  if (updateText)
    lineDataToWidgets();

  Q_EMIT lineDataChanged();
}

void
CQChartsLineDataLineEdit::
textChanged()
{
  CQChartsLineData lineData(edit_->text());

  if (! lineData.isValid())
    return;

  updateLineData(lineData, /*updateText*/ false);
}

void
CQChartsLineDataLineEdit::
lineDataToWidgets()
{
  connectSlots(false);

  if (lineData().isValid())
    edit_->setText(lineData().toString());
  else
    edit_->setText("");

  auto tip = QString("%1 (%2)").arg(lineData().toString());

  edit_->setToolTip(tip);

  connectSlots(true);
}

void
CQChartsLineDataLineEdit::
menuEditChanged()
{
  lineDataToWidgets();

  Q_EMIT lineDataChanged();
}

void
CQChartsLineDataLineEdit::
connectSlots(bool b)
{
  connectBaseSlots(b);

  CQUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(lineDataChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsLineDataLineEdit::
drawPreview(QPainter *painter, const QRect &rect)
{
  CQChartsLineDataEditPreview::draw(painter, lineData(), rect, plot(), view());
}

//------

CQPropertyViewEditorFactory *
CQChartsLineDataPropertyViewType::
getEditor() const
{
  return new CQChartsLineDataPropertyViewEditor;
}

void
CQChartsLineDataPropertyViewType::
drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
            CQChartsPlot *plot, CQChartsView *view)
{
  auto data = CQChartsLineData::fromVariant(value);

  CQChartsLineDataEditPreview::draw(painter, data, rect, plot, view);
}

QString
CQChartsLineDataPropertyViewType::
tip(const QVariant &value) const
{
  auto str = CQChartsLineData::fromVariant(value).toString();

  return str;
}

//------

CQChartsLineEditBase *
CQChartsLineDataPropertyViewEditor::
createPropertyEdit(QWidget *parent)
{
  return new CQChartsLineDataLineEdit(parent);
}

void
CQChartsLineDataPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsLineDataLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(lineDataChanged()), obj, method);
}

QVariant
CQChartsLineDataPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsLineDataLineEdit *>(w);
  assert(edit);

  return CQChartsLineData::toVariant(edit->lineData());
}

void
CQChartsLineDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsLineDataLineEdit *>(w);
  assert(edit);

  auto data = CQChartsLineData::fromVariant(var);

  edit->setLineData(data);
}

//------

CQChartsLineDataEdit::
CQChartsLineDataEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("lineDataEdit");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  groupBox_ = CQUtil::makeWidget<CQGroupBox>("groupBox");

  groupBox_->setCheckable(true);
  groupBox_->setChecked(false);
  groupBox_->setTitle("Visible");

  layout->addWidget(groupBox_);

  //---

  auto *groupLayout = CQUtil::makeLayout<QGridLayout>(groupBox_, 2, 2);

  // color
  auto *colorLabel = CQUtil::makeLabelWidget<QLabel>("Color", "color");

  colorEdit_ = new CQChartsColorLineEdit;

  groupLayout->addWidget(colorLabel, 0, 0);
  groupLayout->addWidget(colorEdit_, 0, 1);

  // alpha
  auto *alphaLabel = CQUtil::makeLabelWidget<QLabel>("Alpha", "alpha");

  alphaEdit_ = new CQChartsAlphaEdit;

  groupLayout->addWidget(alphaLabel, 1, 0);
  groupLayout->addWidget(alphaEdit_, 1, 1);

  // width
  auto *widthLabel = CQUtil::makeLabelWidget<QLabel>("Width", "width");

  widthEdit_ = new CQChartsLengthEdit;

  groupLayout->addWidget(widthLabel, 2, 0);
  groupLayout->addWidget(widthEdit_, 2, 1);

  // dash
  auto *dashLabel = CQUtil::makeLabelWidget<QLabel>("Dash", "dash");

  dashEdit_ = new CQChartsLineDashEdit;

  groupLayout->addWidget(dashLabel, 3, 0);
  groupLayout->addWidget(dashEdit_, 3, 1);

  //---

  preview_ = new CQChartsLineDataEditPreview(this);

  groupLayout->addWidget(preview_, 4, 1);

  //---

  groupLayout->setRowStretch(5, 1);

  //---

  layout->addStretch(1);

  //---

  connectSlots(true);

  dataToWidgets();
}

void
CQChartsLineDataEdit::
setData(const CQChartsLineData &d)
{
  data_ = d;

  dataToWidgets();
}

void
CQChartsLineDataEdit::
setTitle(const QString &title)
{
  groupBox_->setTitle(title);
}

void
CQChartsLineDataEdit::
setPreview(bool b)
{
  preview_->setVisible(b);
}

void
CQChartsLineDataEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](QWidget *w, const char *from, const char *to) {
    CQUtil::connectDisconnect(connected_, w, from, this, to);
  };

  connectDisconnect(groupBox_, SIGNAL(clicked(bool)), SLOT(widgetsToData()));
  connectDisconnect(colorEdit_, SIGNAL(colorChanged()), SLOT(widgetsToData()));
  connectDisconnect(alphaEdit_, SIGNAL(alphaChanged()), SLOT(widgetsToData()));
  connectDisconnect(widthEdit_, SIGNAL(lengthChanged()), SLOT(widgetsToData()));
  connectDisconnect(dashEdit_, SIGNAL(valueChanged(const CQChartsLineDash &)),
                    SLOT(widgetsToData()));
}

void
CQChartsLineDataEdit::
dataToWidgets()
{
  connectSlots(false);

  groupBox_ ->setChecked (data_.isVisible());
  colorEdit_->setColor   (data_.color());
  alphaEdit_->setAlpha   (data_.alpha());
  widthEdit_->setLength  (data_.width());
  dashEdit_ ->setLineDash(data_.dash());

  preview_->update();

  connectSlots(true);
}

void
CQChartsLineDataEdit::
widgetsToData()
{
  data_.setVisible(groupBox_ ->isChecked());
  data_.setColor  (colorEdit_->color());
  data_.setAlpha  (alphaEdit_->alpha());
  data_.setWidth  (widthEdit_->length());
  data_.setDash   (dashEdit_ ->getLineDash());

  preview_->update();

  Q_EMIT lineDataChanged();
}

//------

CQChartsLineDataEditPreview::
CQChartsLineDataEditPreview(CQChartsLineDataEdit *edit) :
 CQChartsEditPreview(edit), edit_(edit)
{
  setToolTip("Line Preview");
}

void
CQChartsLineDataEditPreview::
draw(QPainter *painter)
{
  const auto &data = edit_->data();

  draw(painter, data, rect(), edit_->plot(), edit_->view());
}

void
CQChartsLineDataEditPreview::
draw(QPainter *painter, const CQChartsLineData &data, const QRect &rect,
     CQChartsPlot *plot, CQChartsView *view)
{
  // set pen
  auto pc = interpColor(plot, view, data.color());

  double width = data.width().value();

  width = (plot ? plot->limitLineWidth(width) : view->limitLineWidth(width));

  QPen pen;

  CQChartsUtil::setPen(pen, data.isVisible(), pc, data.alpha(), width, data.dash());

  painter->setPen(pen);

  //---

  // draw line
  QPoint p1(rect.left (), rect.center().y());
  QPoint p2(rect.right(), rect.center().y());

  painter->drawLine(p1, p2);
}
