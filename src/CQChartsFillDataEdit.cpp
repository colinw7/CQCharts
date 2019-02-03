#include <CQChartsFillDataEdit.h>

#include <CQChartsColorEdit.h>
#include <CQChartsAlphaEdit.h>
#include <CQChartsFillPatternEdit.h>
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

CQChartsFillDataLineEdit::
CQChartsFillDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("fillDataLineEdit");

  //---

  menuEdit_ = new CQChartsFillDataEdit;

  menu_->setWidget(menuEdit_);

  connect(menuEdit_, SIGNAL(fillDataChanged()), this, SLOT(menuEditChanged()));
}

CQChartsPlot *
CQChartsFillDataLineEdit::
plot() const
{
  return menuEdit_->plot();
}

void
CQChartsFillDataLineEdit::
setPlot(CQChartsPlot *plot)
{
  menuEdit_->setPlot(plot);
}

CQChartsView *
CQChartsFillDataLineEdit::
view() const
{
  return menuEdit_->view();
}

void
CQChartsFillDataLineEdit::
setView(CQChartsView *view)
{
  menuEdit_->setView(view);
}

const CQChartsFillData &
CQChartsFillDataLineEdit::
fillData() const
{
  return menuEdit_->data();
}

void
CQChartsFillDataLineEdit::
setFillData(const CQChartsFillData &fillData)
{
  updateFillData(fillData, /*updateText*/ true);
}

void
CQChartsFillDataLineEdit::
updateFillData(const CQChartsFillData &fillData, bool updateText)
{
  connectSlots(false);

  menuEdit_->setData(fillData);

  if (updateText)
    fillDataToWidgets();

  connectSlots(true);

  emit fillDataChanged();
}

void
CQChartsFillDataLineEdit::
textChanged()
{
  CQChartsFillData fillData(edit_->text());

  if (! fillData.isValid())
    return;

  updateFillData(fillData, /*updateText*/ false);
}

void
CQChartsFillDataLineEdit::
fillDataToWidgets()
{
  connectSlots(false);

  if (fillData().isValid())
    edit_->setText(fillData().toString());
  else
    edit_->setText("");

  connectSlots(true);
}

void
CQChartsFillDataLineEdit::
menuEditChanged()
{
  fillDataToWidgets();

  emit fillDataChanged();
}

void
CQChartsFillDataLineEdit::
connectSlots(bool b)
{
  CQChartsLineEditBase::connectSlots(b);

  if (b)
    connect(menuEdit_, SIGNAL(fillDataChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(menuEdit_, SIGNAL(fillDataChanged()), this, SLOT(menuEditChanged()));
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsFillDataPropertyViewType::
CQChartsFillDataPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsFillDataPropertyViewType::
getEditor() const
{
  return new CQChartsFillDataPropertyViewEditor;
}

bool
CQChartsFillDataPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsFillDataPropertyViewType::
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

  CQChartsFillData data = value.value<CQChartsFillData>();

  QColor fc;

  if      (plot)
    fc = plot->charts()->interpColor(data.color(), 0, 1);
  else if (view)
    fc = view->charts()->interpColor(data.color(), 0, 1);
  else
    fc = data.color().color();

  QBrush brush;

  CQChartsUtil::setBrush(brush, data.isVisible(), fc, data.alpha(), data.pattern());

  painter->fillRect(option.rect, brush);
}

QString
CQChartsFillDataPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsFillData>().toString();

  return str;
}

//------

CQChartsFillDataPropertyViewEditor::
CQChartsFillDataPropertyViewEditor()
{
}

QWidget *
CQChartsFillDataPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQPropertyViewItem *item = CQPropertyViewMgrInst->editItem();

  QObject *obj = (item ? item->object() : nullptr);

  CQChartsPlot *plot = qobject_cast<CQChartsPlot *>(obj);
  CQChartsView *view = qobject_cast<CQChartsView *>(obj);

  CQChartsFillDataLineEdit *edit = new CQChartsFillDataLineEdit(parent);

  if      (plot) edit->setPlot(plot);
  else if (view) edit->setView(view);

  return edit;
}

void
CQChartsFillDataPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsFillDataLineEdit *edit = qobject_cast<CQChartsFillDataLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(fillDataChanged()), obj, method);
}

QVariant
CQChartsFillDataPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsFillDataLineEdit *edit = qobject_cast<CQChartsFillDataLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->fillData());
}

void
CQChartsFillDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsFillDataLineEdit *edit = qobject_cast<CQChartsFillDataLineEdit *>(w);
  assert(edit);

  CQChartsFillData data = var.value<CQChartsFillData>();

  edit->setFillData(data);
}

//------

CQChartsFillDataEdit::
CQChartsFillDataEdit(QWidget *parent) :
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

  // pattern
  QLabel *patternLabel = new QLabel("Pattern");

  patternEdit_ = new CQChartsFillPatternEdit;

  connect(patternEdit_, SIGNAL(fillPatternChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(patternLabel, 2, 0);
  groupLayout->addWidget(patternEdit_, 2, 1);

  //---

  preview_ = new CQChartsFillDataEditPreview(this);

  groupLayout->addWidget(preview_, 3, 1);

  //---

  groupLayout->setRowStretch(4, 1);

  //---

  layout->addStretch(1);

  dataToWidgets();
}

void
CQChartsFillDataEdit::
setTitle(const QString &title)
{
  groupBox_->setTitle(title);
}

void
CQChartsFillDataEdit::
setPreview(bool b)
{
  preview_->setVisible(b);
}

void
CQChartsFillDataEdit::
dataToWidgets()
{
  disconnect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  disconnect(colorEdit_, SIGNAL(colorChanged()), this, SLOT(widgetsToData()));
  disconnect(alphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  disconnect(patternEdit_, SIGNAL(fillPatternChanged()), this, SLOT(widgetsToData()));

  groupBox_->setChecked(data_.isVisible());

  colorEdit_  ->setColor      (data_.color());
  alphaEdit_  ->setValue      (data_.alpha());
  patternEdit_->setFillPattern(data_.pattern());

  preview_->update();

  connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  connect(colorEdit_, SIGNAL(colorChanged()), this, SLOT(widgetsToData()));
  connect(alphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  connect(patternEdit_, SIGNAL(fillPatternChanged()), this, SLOT(widgetsToData()));

}

void
CQChartsFillDataEdit::
widgetsToData()
{
  data_.setVisible(groupBox_   ->isChecked());
  data_.setColor  (colorEdit_  ->color());
  data_.setAlpha  (alphaEdit_  ->value());
  data_.setPattern(patternEdit_->fillPattern());

  preview_->update();

  emit fillDataChanged();
}

//------

CQChartsFillDataEditPreview::
CQChartsFillDataEditPreview(CQChartsFillDataEdit *edit) :
 edit_(edit)
{
}

void
CQChartsFillDataEditPreview::
paintEvent(QPaintEvent *)
{
  const CQChartsFillData &data = edit_->data();

  QColor fc;

  if      (edit_->plot())
    fc = edit_->plot()->charts()->interpColor(data.color(), 0, 1);
  else if (edit_->view())
    fc = edit_->view()->charts()->interpColor(data.color(), 0, 1);
  else
    fc = data.color().color();

  QBrush brush;

  CQChartsUtil::setBrush(brush, data.isVisible(), fc, data.alpha(), data.pattern());

  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing);

  painter.fillRect(rect(), brush);
}

QSize
CQChartsFillDataEditPreview::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("XXXX"), fm.height() + 4);
}
