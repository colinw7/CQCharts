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

#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

CQChartsLineDataLineEdit::
CQChartsLineDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("lineDataLineEdit");

  //---

  menuEdit_ = new CQChartsLineDataEdit;

  menu_->setWidget(menuEdit_);

  connect(menuEdit_, SIGNAL(lineDataChanged()), this, SLOT(menuEditChanged()));

  //---

  lineDataToWidgets();
}

CQChartsPlot *
CQChartsLineDataLineEdit::
plot() const
{
  return menuEdit_->plot();
}

void
CQChartsLineDataLineEdit::
setPlot(CQChartsPlot *plot)
{
  menuEdit_->setPlot(plot);
}

CQChartsView *
CQChartsLineDataLineEdit::
view() const
{
  return menuEdit_->view();
}

void
CQChartsLineDataLineEdit::
setView(CQChartsView *view)
{
  menuEdit_->setView(view);
}

const CQChartsLineData &
CQChartsLineDataLineEdit::
lineData() const
{
  return menuEdit_->data();
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

  menuEdit_->setData(lineData);

  if (updateText)
    lineDataToWidgets();

  connectSlots(true);

  emit lineDataChanged();
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

  connectSlots(true);
}

void
CQChartsLineDataLineEdit::
menuEditChanged()
{
  lineDataToWidgets();

  emit lineDataChanged();
}

void
CQChartsLineDataLineEdit::
connectSlots(bool b)
{
  CQChartsLineEditBase::connectSlots(b);

  if (b)
    connect(menuEdit_, SIGNAL(lineDataChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(menuEdit_, SIGNAL(lineDataChanged()), this, SLOT(menuEditChanged()));
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsLineDataPropertyViewType::
CQChartsLineDataPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsLineDataPropertyViewType::
getEditor() const
{
  return new CQChartsLineDataPropertyViewEditor;
}

bool
CQChartsLineDataPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsLineDataPropertyViewType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  CQPropertyViewItem *item = CQPropertyViewMgrInst->drawItem();

  QObject *obj = (item ? item->object() : nullptr);

  CQChartsPlot *plot = qobject_cast<CQChartsPlot *>(obj);
  CQChartsView *view = qobject_cast<CQChartsView *>(obj);

  //---

  delegate->drawBackground(painter, option, ind, inside);

  CQChartsLineData data = value.value<CQChartsLineData>();

  QColor pc;

  if      (plot)
    pc = plot->charts()->interpColor(data.color(), 0, 1);
  else if (view)
    pc = view->charts()->interpColor(data.color(), 0, 1);
  else
    pc = data.color().color();

  double width = CQChartsUtil::limitLineWidth(data.width().value());

  QPen pen;

  CQChartsUtil::setPen(pen, data.isVisible(), pc, data.alpha(), width, data.dash());

  painter->setRenderHints(QPainter::Antialiasing);

  painter->setPen(pen);

  QPoint p1(option.rect.left (), option.rect.center().y());
  QPoint p2(option.rect.right(), option.rect.center().y());

  painter->drawLine(p1, p2);
}

QString
CQChartsLineDataPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsLineData>().toString();

  return str;
}

//------

CQChartsLineDataPropertyViewEditor::
CQChartsLineDataPropertyViewEditor()
{
}

QWidget *
CQChartsLineDataPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQPropertyViewItem *item = CQPropertyViewMgrInst->editItem();

  QObject *obj = (item ? item->object() : nullptr);

  CQChartsPlot *plot = qobject_cast<CQChartsPlot *>(obj);
  CQChartsView *view = qobject_cast<CQChartsView *>(obj);

  CQChartsLineDataLineEdit *edit = new CQChartsLineDataLineEdit(parent);

  if      (plot) edit->setPlot(plot);
  else if (view) edit->setView(view);

  return edit;
}

void
CQChartsLineDataPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsLineDataLineEdit *edit = qobject_cast<CQChartsLineDataLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(lineDataChanged()), obj, method);
}

QVariant
CQChartsLineDataPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsLineDataLineEdit *edit = qobject_cast<CQChartsLineDataLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->lineData());
}

void
CQChartsLineDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsLineDataLineEdit *edit = qobject_cast<CQChartsLineDataLineEdit *>(w);
  assert(edit);

  CQChartsLineData data = var.value<CQChartsLineData>();

  edit->setLineData(data);
}

//------

CQChartsLineDataEdit::
CQChartsLineDataEdit(QWidget *parent) :
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

  // color
  QLabel *colorLabel = new QLabel("Color");

  colorEdit_ = new CQChartsColorLineEdit;

  connect(colorEdit_, SIGNAL(colorChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(colorLabel, 0, 0);
  groupLayout->addWidget(colorEdit_, 0, 1);

  // alpha
  QLabel *alphaLabel = new QLabel("Alpha");

  alphaEdit_ = new CQChartsAlphaEdit;

  connect(alphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(alphaLabel, 1, 0);
  groupLayout->addWidget(alphaEdit_, 1, 1);

  // width
  QLabel *widthLabel = new QLabel("Width");

  widthEdit_ = new CQChartsLengthEdit;

  connect(widthEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(widthLabel, 2, 0);
  groupLayout->addWidget(widthEdit_, 2, 1);

  // dash
  QLabel *dashLabel = new QLabel("Dash");

  dashEdit_ = new CQChartsLineDashEdit;

  connect(dashEdit_, SIGNAL(valueChanged(const CQChartsLineDash &)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(dashLabel, 3, 0);
  groupLayout->addWidget(dashEdit_, 3, 1);

  //---

  preview_ = new CQChartsLineDataEditPreview(this);

  groupLayout->addWidget(preview_, 4, 1);

  //---

  groupLayout->setRowStretch(5, 1);

  //---

  layout->addStretch(1);

  dataToWidgets();
}

void
CQChartsLineDataEdit::
dataToWidgets()
{
  disconnect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  disconnect(colorEdit_, SIGNAL(colorChanged()), this, SLOT(widgetsToData()));
  disconnect(alphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  disconnect(widthEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));
  disconnect(dashEdit_, SIGNAL(valueChanged(const CQChartsLineDash &)),
             this, SLOT(widgetsToData()));

  groupBox_ ->setChecked (data_.isVisible());
  colorEdit_->setColor   (data_.color());
  alphaEdit_->setValue   (data_.alpha());
  widthEdit_->setLength  (data_.width());
  dashEdit_ ->setLineDash(data_.dash());

  preview_->update();

  connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  connect(colorEdit_, SIGNAL(colorChanged()), this, SLOT(widgetsToData()));
  connect(alphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  connect(widthEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));
  connect(dashEdit_, SIGNAL(valueChanged(const CQChartsLineDash &)),
          this, SLOT(widgetsToData()));
}

void
CQChartsLineDataEdit::
widgetsToData()
{
  data_.setVisible(groupBox_ ->isChecked());
  data_.setColor  (colorEdit_->color());
  data_.setAlpha  (alphaEdit_->value());
  data_.setWidth  (widthEdit_->length());
  data_.setDash   (dashEdit_ ->getLineDash());

  preview_->update();

  emit lineDataChanged();
}

//------

CQChartsLineDataEditPreview::
CQChartsLineDataEditPreview(CQChartsLineDataEdit *edit) :
 edit_(edit)
{
}

void
CQChartsLineDataEditPreview::
paintEvent(QPaintEvent *)
{
  const CQChartsLineData &data = edit_->data();

  QColor pc;

  if      (edit_->plot())
    pc = edit_->plot()->charts()->interpColor(data.color(), 0, 1);
  else if (edit_->view())
    pc = edit_->view()->charts()->interpColor(data.color(), 0, 1);
  else
    pc = data.color().color();

  double width = CQChartsUtil::limitLineWidth(data.width().value());

  QPen pen;

  CQChartsUtil::setPen(pen, data.isVisible(), pc, data.alpha(), width, data.dash());

  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing);

  painter.setPen(pen);

  QPoint p1(rect().left (), rect().center().y());
  QPoint p2(rect().right(), rect().center().y());

  painter.drawLine(p1, p2);
}

QSize
CQChartsLineDataEditPreview::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("XXXX"), fm.height() + 4);
}
