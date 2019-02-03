#include <CQChartsStrokeDataEdit.h>

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

CQChartsStrokeDataLineEdit::
CQChartsStrokeDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("strokeDataLineEdit");

  //---

  menuEdit_ = new CQChartsStrokeDataEdit;

  menu_->setWidget(menuEdit_);

  connect(menuEdit_, SIGNAL(strokeDataChanged()), this, SLOT(menuEditChanged()));
}

CQChartsPlot *
CQChartsStrokeDataLineEdit::
plot() const
{
  return menuEdit_->plot();
}

void
CQChartsStrokeDataLineEdit::
setPlot(CQChartsPlot *plot)
{
  menuEdit_->setPlot(plot);
}

CQChartsView *
CQChartsStrokeDataLineEdit::
view() const
{
  return menuEdit_->view();
}

void
CQChartsStrokeDataLineEdit::
setView(CQChartsView *view)
{
  menuEdit_->setView(view);
}

const CQChartsStrokeData &
CQChartsStrokeDataLineEdit::
strokeData() const
{
  return menuEdit_->data();
}

void
CQChartsStrokeDataLineEdit::
setStrokeData(const CQChartsStrokeData &strokeData)
{
  updateStrokeData(strokeData, /*updateText*/ true);
}

void
CQChartsStrokeDataLineEdit::
updateStrokeData(const CQChartsStrokeData &strokeData, bool updateText)
{
  connectSlots(false);

  menuEdit_->setData(strokeData);

  if (updateText)
    strokeDataToWidgets();

  connectSlots(true);

  emit strokeDataChanged();
}

void
CQChartsStrokeDataLineEdit::
textChanged()
{
  CQChartsStrokeData strokeData(edit_->text());

  if (! strokeData.isValid())
    return;

  updateStrokeData(strokeData, /*updateText*/ false);
}

void
CQChartsStrokeDataLineEdit::
strokeDataToWidgets()
{
  connectSlots(false);

  if (strokeData().isValid())
    edit_->setText(strokeData().toString());
  else
    edit_->setText("");

  connectSlots(true);
}

void
CQChartsStrokeDataLineEdit::
menuEditChanged()
{
  strokeDataToWidgets();

  emit strokeDataChanged();
}

void
CQChartsStrokeDataLineEdit::
connectSlots(bool b)
{
  CQChartsLineEditBase::connectSlots(b);

  if (b)
    connect(menuEdit_, SIGNAL(strokeDataChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(menuEdit_, SIGNAL(strokeDataChanged()), this, SLOT(menuEditChanged()));
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsStrokeDataPropertyViewType::
CQChartsStrokeDataPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsStrokeDataPropertyViewType::
getEditor() const
{
  return new CQChartsStrokeDataPropertyViewEditor;
}

bool
CQChartsStrokeDataPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsStrokeDataPropertyViewType::
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

  CQChartsStrokeData data = value.value<CQChartsStrokeData>();

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
CQChartsStrokeDataPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsStrokeData>().toString();

  return str;
}

//------

CQChartsStrokeDataPropertyViewEditor::
CQChartsStrokeDataPropertyViewEditor()
{
}

QWidget *
CQChartsStrokeDataPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQPropertyViewItem *item = CQPropertyViewMgrInst->editItem();

  QObject *obj = (item ? item->object() : nullptr);

  CQChartsPlot *plot = qobject_cast<CQChartsPlot *>(obj);
  CQChartsView *view = qobject_cast<CQChartsView *>(obj);

  CQChartsStrokeDataLineEdit *edit = new CQChartsStrokeDataLineEdit(parent);

  if      (plot) edit->setPlot(plot);
  else if (view) edit->setView(view);

  return edit;
}

void
CQChartsStrokeDataPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsStrokeDataLineEdit *edit = qobject_cast<CQChartsStrokeDataLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(strokeDataChanged()), obj, method);
}

QVariant
CQChartsStrokeDataPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsStrokeDataLineEdit *edit = qobject_cast<CQChartsStrokeDataLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->strokeData());
}

void
CQChartsStrokeDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsStrokeDataLineEdit *edit = qobject_cast<CQChartsStrokeDataLineEdit *>(w);
  assert(edit);

  CQChartsStrokeData data = var.value<CQChartsStrokeData>();

  edit->setStrokeData(data);
}

//------

CQChartsStrokeDataEdit::
CQChartsStrokeDataEdit(QWidget *parent) :
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

  // corner size
  QLabel *cornerLabel = new QLabel("Corner");

  cornerEdit_ = new CQChartsLengthEdit;

  connect(cornerEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(cornerLabel, 4, 0);
  groupLayout->addWidget(cornerEdit_, 4, 1);

  //---

  preview_ = new CQChartsStrokeDataEditPreview(this);

  groupLayout->addWidget(preview_, 5, 1);

  //---

  groupLayout->setRowStretch(6, 1);

  //---

  layout->addStretch(1);

  dataToWidgets();
}

void
CQChartsStrokeDataEdit::
setTitle(const QString &title)
{
  groupBox_->setTitle(title);
}

void
CQChartsStrokeDataEdit::
setPreview(bool b)
{
  preview_->setVisible(b);
}

void
CQChartsStrokeDataEdit::
dataToWidgets()
{
  disconnect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  disconnect(colorEdit_, SIGNAL(colorChanged()), this, SLOT(widgetsToData()));
  disconnect(alphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  disconnect(widthEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));
  disconnect(dashEdit_, SIGNAL(valueChanged(const CQChartsLineDash &)),
             this, SLOT(widgetsToData()));
  disconnect(cornerEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));

  groupBox_  ->setChecked (data_.isVisible());
  colorEdit_ ->setColor   (data_.color());
  alphaEdit_ ->setValue   (data_.alpha());
  widthEdit_ ->setLength  (data_.width());
  dashEdit_  ->setLineDash(data_.dash());
  cornerEdit_->setLength  (data_.cornerSize());

  preview_->update();

  connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  connect(colorEdit_, SIGNAL(colorChanged()), this, SLOT(widgetsToData()));
  connect(alphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  connect(widthEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));
  connect(dashEdit_, SIGNAL(valueChanged(const CQChartsLineDash &)),
          this, SLOT(widgetsToData()));
  connect(cornerEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));
}

void
CQChartsStrokeDataEdit::
widgetsToData()
{
  data_.setVisible   (groupBox_  ->isChecked());
  data_.setColor     (colorEdit_ ->color());
  data_.setAlpha     (alphaEdit_ ->value());
  data_.setWidth     (widthEdit_ ->length());
  data_.setDash      (dashEdit_  ->getLineDash());
  data_.setCornerSize(cornerEdit_->length());

  preview_->update();

  emit strokeDataChanged();
}

//------

CQChartsStrokeDataEditPreview::
CQChartsStrokeDataEditPreview(CQChartsStrokeDataEdit *edit) :
 edit_(edit)
{
}

void
CQChartsStrokeDataEditPreview::
paintEvent(QPaintEvent *)
{
  const CQChartsStrokeData &data = edit_->data();

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
CQChartsStrokeDataEditPreview::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("XXXX"), fm.height() + 4);
}
