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
#include <CQGroupBox.h>
#include <CQUtil.h>

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

  menuEdit_ = dataEdit_ = new CQChartsStrokeDataEdit;

  menu_->setWidget(dataEdit_);

  connect(dataEdit_, SIGNAL(strokeDataChanged()), this, SLOT(menuEditChanged()));

  //---

  strokeDataToWidgets();
}

const CQChartsStrokeData &
CQChartsStrokeDataLineEdit::
strokeData() const
{
  return dataEdit_->data();
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

  dataEdit_->setData(strokeData);

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

  setToolTip(strokeData().toString());

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
    connect(dataEdit_, SIGNAL(strokeDataChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(dataEdit_, SIGNAL(strokeDataChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsStrokeDataLineEdit::
drawPreview(QPainter *painter, const QRect &rect)
{
  CQChartsStrokeDataEditPreview::draw(painter, strokeData(), rect, plot(), view());
}

//------

CQPropertyViewEditorFactory *
CQChartsStrokeDataPropertyViewType::
getEditor() const
{
  return new CQChartsStrokeDataPropertyViewEditor;
}

void
CQChartsStrokeDataPropertyViewType::
drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
            CQChartsPlot *plot, CQChartsView *view)
{
  CQChartsStrokeData data = value.value<CQChartsStrokeData>();

  CQChartsStrokeDataEditPreview::draw(painter, data, rect, plot, view);
}

QString
CQChartsStrokeDataPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsStrokeData>().toString();

  return str;
}

//------

CQChartsLineEditBase *
CQChartsStrokeDataPropertyViewEditor::
createPropertyEdit(QWidget *parent)
{
  return new CQChartsStrokeDataLineEdit(parent);
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
CQChartsStrokeDataEdit(QWidget *parent, const CQChartsStrokeDataEditConfig &config) :
 CQChartsEditBase(parent), config_(config)
{
  setObjectName("strokeDataEdit");

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  //---

  groupBox_ = CQUtil::makeWidget<CQGroupBox>("groupBox");

  groupBox_->setCheckable(true);
  groupBox_->setChecked(false);
  groupBox_->setTitle("Visible");

  connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));

  layout->addWidget(groupBox_);

  //---

  QGridLayout *groupLayout = new QGridLayout(groupBox_);

  // color
  QLabel *colorLabel = new QLabel("Color");
  colorLabel->setObjectName("colorLabel");

  colorEdit_ = new CQChartsColorLineEdit;

  connect(colorEdit_, SIGNAL(colorChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(colorLabel, 0, 0);
  groupLayout->addWidget(colorEdit_, 0, 1);

  // alpha
  QLabel *alphaLabel = new QLabel("Alpha");
  alphaLabel->setObjectName("alphaLabel");

  alphaEdit_ = new CQChartsAlphaEdit;

  connect(alphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(alphaLabel, 1, 0);
  groupLayout->addWidget(alphaEdit_, 1, 1);

  // width
  QLabel *widthLabel = new QLabel("Width");
  widthLabel->setObjectName("widthLabel");

  widthEdit_ = CQUtil::makeWidget<CQChartsLengthEdit>("widthEdit");

  connect(widthEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(widthLabel, 2, 0);
  groupLayout->addWidget(widthEdit_, 2, 1);

  // dash
  QLabel *dashLabel = new QLabel("Dash");
  dashLabel->setObjectName("dashLabel");

  dashEdit_ = new CQChartsLineDashEdit;

  connect(dashEdit_, SIGNAL(valueChanged(const CQChartsLineDash &)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(dashLabel, 3, 0);
  groupLayout->addWidget(dashEdit_, 3, 1);

  // corner size
  if (config_.cornerSize) {
    QLabel *cornerLabel = new QLabel("Corner");
    cornerLabel->setObjectName("cornerLabel");

    cornerEdit_ = CQUtil::makeWidget<CQChartsLengthEdit>("cornerEdit");

    connect(cornerEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));

    groupLayout->addWidget(cornerLabel, 4, 0);
    groupLayout->addWidget(cornerEdit_, 4, 1);
  }

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
setData(const CQChartsStrokeData &d)
{
  data_ = d;

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

  if (cornerEdit_)
    disconnect(cornerEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));

  groupBox_ ->setChecked (data_.isVisible());
  colorEdit_->setColor   (data_.color());
  alphaEdit_->setValue   (data_.alpha());
  widthEdit_->setLength  (data_.width());
  dashEdit_ ->setLineDash(data_.dash());

  if (cornerEdit_)
    cornerEdit_->setLength(data_.cornerSize());

  preview_->update();

  connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  connect(colorEdit_, SIGNAL(colorChanged()), this, SLOT(widgetsToData()));
  connect(alphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  connect(widthEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));
  connect(dashEdit_, SIGNAL(valueChanged(const CQChartsLineDash &)),
          this, SLOT(widgetsToData()));

  if (cornerEdit_)
    connect(cornerEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));
}

void
CQChartsStrokeDataEdit::
widgetsToData()
{
  data_.setVisible(groupBox_ ->isChecked());
  data_.setColor  (colorEdit_->color());
  data_.setAlpha  (alphaEdit_->value());
  data_.setWidth  (widthEdit_->length());
  data_.setDash   (dashEdit_ ->getLineDash());

  if (cornerEdit_)
    data_.setCornerSize(cornerEdit_->length());

  preview_->update();

  emit strokeDataChanged();
}

//------

CQChartsStrokeDataEditPreview::
CQChartsStrokeDataEditPreview(CQChartsStrokeDataEdit *edit) :
 CQChartsEditPreview(edit), edit_(edit)
{
}

void
CQChartsStrokeDataEditPreview::
draw(QPainter *painter)
{
  const CQChartsStrokeData &data = edit_->data();

  draw(painter, data, rect(), edit_->plot(), edit_->view());
}

void
CQChartsStrokeDataEditPreview::
draw(QPainter *painter, const CQChartsStrokeData &data, const QRect &rect,
     CQChartsPlot *plot, CQChartsView *view)
{
  // set pen
  QColor pc = interpColor(plot, view, data.color());

  double width = CQChartsUtil::limitLineWidth(data.width().value());

  QPen pen;

  CQChartsUtil::setPen(pen, data.isVisible(), pc, data.alpha(), width, data.dash());

  painter->setPen(pen);

  //---

  // draw line
  QPoint p1(rect.left (), rect.center().y());
  QPoint p2(rect.right(), rect.center().y());

  painter->drawLine(p1, p2);
}
