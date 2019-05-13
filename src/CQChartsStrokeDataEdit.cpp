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

  //---

  connectSlots(true);

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

  connectSlots(true);

  if (updateText)
    strokeDataToWidgets();

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

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  groupBox_ = CQUtil::makeWidget<CQGroupBox>("groupBox");

  groupBox_->setCheckable(true);
  groupBox_->setChecked(false);
  groupBox_->setTitle("Visible");

  layout->addWidget(groupBox_);

  //---

  QGridLayout *groupLayout = CQUtil::makeLayout<QGridLayout>(groupBox_, 2, 2);

  // color
  QLabel *colorLabel = CQUtil::makeLabelWidget<QLabel>("Color", "colorLabel");

  colorEdit_ = new CQChartsColorLineEdit;

  groupLayout->addWidget(colorLabel, 0, 0);
  groupLayout->addWidget(colorEdit_, 0, 1);

  // alpha
  QLabel *alphaLabel = CQUtil::makeLabelWidget<QLabel>("Alpha", "alphaLabel");

  alphaEdit_ = new CQChartsAlphaEdit;

  groupLayout->addWidget(alphaLabel, 1, 0);
  groupLayout->addWidget(alphaEdit_, 1, 1);

  // width
  QLabel *widthLabel = CQUtil::makeLabelWidget<QLabel>("Width", "widthLabel");

  widthEdit_ = CQUtil::makeWidget<CQChartsLengthEdit>("widthEdit");

  groupLayout->addWidget(widthLabel, 2, 0);
  groupLayout->addWidget(widthEdit_, 2, 1);

  // dash
  QLabel *dashLabel = CQUtil::makeLabelWidget<QLabel>("Dash", "dashLabel");

  dashEdit_ = new CQChartsLineDashEdit;

  groupLayout->addWidget(dashLabel, 3, 0);
  groupLayout->addWidget(dashEdit_, 3, 1);

  // corner size
  if (config_.cornerSize) {
    QLabel *cornerLabel = CQUtil::makeLabelWidget<QLabel>("Corner", "cornerLabel");

    cornerEdit_ = CQUtil::makeWidget<CQChartsLengthEdit>("cornerEdit");

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

  //---

  connectSlots(true);

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
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    if (b)
      connect(w, from, this, to);
    else
      disconnect(w, from, this, to);
  };

  connectDisconnect(b, groupBox_, SIGNAL(clicked(bool)), SLOT(widgetsToData()));
  connectDisconnect(b, colorEdit_, SIGNAL(colorChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, alphaEdit_, SIGNAL(valueChanged(double)), SLOT(widgetsToData()));
  connectDisconnect(b, widthEdit_, SIGNAL(lengthChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, dashEdit_, SIGNAL(valueChanged(const CQChartsLineDash &)),
                    SLOT(widgetsToData()));

  if (cornerEdit_)
    connectDisconnect(b, cornerEdit_, SIGNAL(lengthChanged()), SLOT(widgetsToData()));
}

void
CQChartsStrokeDataEdit::
dataToWidgets()
{
  connectSlots(false);

  groupBox_ ->setChecked (data_.isVisible());
  colorEdit_->setColor   (data_.color());
  alphaEdit_->setValue   (data_.alpha());
  widthEdit_->setLength  (data_.width());
  dashEdit_ ->setLineDash(data_.dash());

  if (cornerEdit_)
    cornerEdit_->setLength(data_.cornerSize());

  preview_->update();

  connectSlots(true);
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
