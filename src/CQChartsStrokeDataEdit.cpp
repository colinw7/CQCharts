#include <CQChartsStrokeDataEdit.h>
#include <CQChartsColorEdit.h>
#include <CQChartsAlphaEdit.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsLineDashEdit.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

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

  setToolTip("Stroke Data");

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
  connectBaseSlots(b);

  CQChartsWidgetUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(strokeDataChanged()), this, SLOT(menuEditChanged()));
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
  auto data = value.value<CQChartsStrokeData>();

  CQChartsStrokeDataEditPreview::draw(painter, data, rect, plot, view);
}

QString
CQChartsStrokeDataPropertyViewType::
tip(const QVariant &value) const
{
  auto str = value.value<CQChartsStrokeData>().toString();

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
  auto *edit = qobject_cast<CQChartsStrokeDataLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(strokeDataChanged()), obj, method);
}

QVariant
CQChartsStrokeDataPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsStrokeDataLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->strokeData());
}

void
CQChartsStrokeDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsStrokeDataLineEdit *>(w);
  assert(edit);

  auto data = var.value<CQChartsStrokeData>();

  edit->setStrokeData(data);
}

//------

CQChartsStrokeDataEdit::
CQChartsStrokeDataEdit(QWidget *parent, const CQChartsStrokeDataEditConfig &config) :
 CQChartsEditBase(parent), config_(config)
{
  setObjectName("strokeDataEdit");

  setToolTip("Stroke Data");

  //---

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
  auto *colorLabel = CQUtil::makeLabelWidget<QLabel>("Color", "colorLabel");

  colorEdit_ = new CQChartsColorLineEdit;

  groupLayout->addWidget(colorLabel, 0, 0);
  groupLayout->addWidget(colorEdit_, 0, 1);

  // alpha
  auto *alphaLabel = CQUtil::makeLabelWidget<QLabel>("Alpha", "alphaLabel");

  alphaEdit_ = new CQChartsAlphaEdit;

  groupLayout->addWidget(alphaLabel, 1, 0);
  groupLayout->addWidget(alphaEdit_, 1, 1);

  // width
  auto *widthLabel = CQUtil::makeLabelWidget<QLabel>("Width", "widthLabel");

  widthEdit_ = CQUtil::makeWidget<CQChartsLengthEdit>("widthEdit");

  groupLayout->addWidget(widthLabel, 2, 0);
  groupLayout->addWidget(widthEdit_, 2, 1);

  // dash
  auto *dashLabel = CQUtil::makeLabelWidget<QLabel>("Dash", "dashLabel");

  dashEdit_ = new CQChartsLineDashEdit;

  groupLayout->addWidget(dashLabel, 3, 0);
  groupLayout->addWidget(dashEdit_, 3, 1);

  // corner size
  if (config_.cornerSize) {
    auto *cornerLabel = CQUtil::makeLabelWidget<QLabel>("Corner", "cornerLabel");

    cornerEdit_ = CQUtil::makeWidget<CQChartsLengthEdit>("cornerEdit");

    cornerEdit_->setToolTip("Corner length");

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

  auto connectDisconnect = [&](QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(connected_, w, from, this, to);
  };

  connectDisconnect(groupBox_, SIGNAL(clicked(bool)), SLOT(widgetsToData()));
  connectDisconnect(colorEdit_, SIGNAL(colorChanged()), SLOT(widgetsToData()));
  connectDisconnect(alphaEdit_, SIGNAL(alphaChanged()), SLOT(widgetsToData()));
  connectDisconnect(widthEdit_, SIGNAL(lengthChanged()), SLOT(widgetsToData()));
  connectDisconnect(dashEdit_, SIGNAL(valueChanged(const CQChartsLineDash &)),
                    SLOT(widgetsToData()));

  if (cornerEdit_)
    connectDisconnect(cornerEdit_, SIGNAL(lengthChanged()), SLOT(widgetsToData()));
}

void
CQChartsStrokeDataEdit::
dataToWidgets()
{
  connectSlots(false);

  groupBox_ ->setChecked (data_.isVisible());
  colorEdit_->setColor   (data_.color());
  alphaEdit_->setAlpha   (data_.alpha());
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
  data_.setAlpha  (alphaEdit_->alpha());
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
  setToolTip("Stroke Preview");
}

void
CQChartsStrokeDataEditPreview::
draw(QPainter *painter)
{
  const auto &data = edit_->data();

  draw(painter, data, rect(), edit_->plot(), edit_->view());
}

void
CQChartsStrokeDataEditPreview::
draw(QPainter *painter, const CQChartsStrokeData &data, const QRect &rect,
     CQChartsPlot *plot, CQChartsView *view)
{
  // set pen
  auto pc = interpColor(plot, view, data.color());

  double width = CQChartsUtil::limitLineWidth(data.width().value());

  QPen pen;

  CQChartsUtil::setPen(pen, data.isVisible(), pc, data.alpha(), width,
                       data.dash(), data.lineCap(), data.lineJoin());

  painter->setPen(pen);

  //---

  // draw line
  QPoint p1(rect.left (), rect.center().y());
  QPoint p2(rect.right(), rect.center().y());

  painter->drawLine(p1, p2);
}
