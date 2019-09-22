#include <CQChartsSymbolDataEdit.h>

#include <CQChartsSymbolEdit.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsStrokeDataEdit.h>
#include <CQChartsFillDataEdit.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <CQCharts.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQGroupBox.h>
#include <CQUtil.h>

#include <QLabel>
#include <QVBoxLayout>

CQChartsSymbolDataLineEdit::
CQChartsSymbolDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("symbolDataLineEdit");

  //---

  menuEdit_ = dataEdit_ = new CQChartsSymbolDataEdit;

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);

  symbolDataToWidgets();
}

const CQChartsSymbolData &
CQChartsSymbolDataLineEdit::
symbolData() const
{
  return dataEdit_->data();
}

void
CQChartsSymbolDataLineEdit::
setSymbolData(const CQChartsSymbolData &symbolData)
{
  updateSymbolData(symbolData, /*updateText*/ true);
}

void
CQChartsSymbolDataLineEdit::
updateSymbolData(const CQChartsSymbolData &symbolData, bool updateText)
{
  connectSlots(false);

  dataEdit_->setData(symbolData);

  connectSlots(true);

  if (updateText)
    symbolDataToWidgets();

  emit symbolDataChanged();
}

void
CQChartsSymbolDataLineEdit::
textChanged()
{
  CQChartsSymbolData symbolData(edit_->text());

  if (! symbolData.isValid())
    return;

  updateSymbolData(symbolData, /*updateText*/ false);
}

void
CQChartsSymbolDataLineEdit::
symbolDataToWidgets()
{
  connectSlots(false);

  if (symbolData().isValid())
    edit_->setText(symbolData().toString());
  else
    edit_->setText("");

  setToolTip(symbolData().toString());

  connectSlots(true);
}

void
CQChartsSymbolDataLineEdit::
menuEditChanged()
{
  symbolDataToWidgets();

  emit symbolDataChanged();
}

void
CQChartsSymbolDataLineEdit::
connectSlots(bool b)
{
  CQChartsLineEditBase::connectSlots(b);

  if (b)
    connect(dataEdit_, SIGNAL(symbolDataChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(dataEdit_, SIGNAL(symbolDataChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsSymbolDataLineEdit::
drawPreview(QPainter *painter, const QRect &rect)
{
  CQChartsSymbolData data = this->symbolData();

  double s  = rect.height()/2.0 - 4.0;
  double is = int(s + 0.5);

  int xl = 3;

  data.setSize(CQChartsLength(s, CQChartsUnits::PIXEL));

  QRect rect1(rect.left() + xl, rect.center().y() - is, 2*is, 2*is);

  CQChartsSymbolDataEditPreview::draw(painter, data, rect1, plot(), view());

  //---

  QString str = QString("%1 %2").
    arg(symbolData().type().toString()).arg(symbolData().size().toString());

  drawCenteredText(painter, str);
}

//------

CQPropertyViewEditorFactory *
CQChartsSymbolDataPropertyViewType::
getEditor() const
{
  return new CQChartsSymbolDataPropertyViewEditor;
}

void
CQChartsSymbolDataPropertyViewType::
drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
            CQChartsPlot *plot, CQChartsView *view)
{
  CQChartsSymbolData data = value.value<CQChartsSymbolData>();

  CQChartsSymbolDataEditPreview::draw(painter, data, rect, plot, view);
}

QString
CQChartsSymbolDataPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsSymbolData>().toString();

  return str;
}

//------

CQChartsLineEditBase *
CQChartsSymbolDataPropertyViewEditor::
createPropertyEdit(QWidget *parent)
{
  return new CQChartsSymbolDataLineEdit(parent);
}

void
CQChartsSymbolDataPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsSymbolDataLineEdit *edit = qobject_cast<CQChartsSymbolDataLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(symbolDataChanged()), obj, method);
}

QVariant
CQChartsSymbolDataPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsSymbolDataLineEdit *edit = qobject_cast<CQChartsSymbolDataLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->symbolData());
}

void
CQChartsSymbolDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsSymbolDataLineEdit *edit = qobject_cast<CQChartsSymbolDataLineEdit *>(w);
  assert(edit);

  CQChartsSymbolData data = var.value<CQChartsSymbolData>();

  edit->setSymbolData(data);
}

//------

CQChartsSymbolDataEdit::
CQChartsSymbolDataEdit(QWidget *parent, bool optional) :
 CQChartsEditBase(parent)
{
  setObjectName("symbolDataEdit");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  if (optional) {
    groupBox_ = CQUtil::makeLabelWidget<CQGroupBox>("Visible", "groupBox");

    groupBox_->setCheckable(true);
    groupBox_->setChecked(false);
    groupBox_->setTitle("Visible");

    layout->addWidget(groupBox_);
  }

  //---

  QGridLayout *groupLayout = CQUtil::makeLayout<QGridLayout>(groupBox_, 2, 2);

  if (! optional)
    layout->addLayout(groupLayout);

  // symbol
  QLabel *symbolLabel = CQUtil::makeLabelWidget<QLabel>("Symbol", "symbolLabel");

  symbolEdit_ = new CQChartsSymbolEdit;

  groupLayout->addWidget(symbolLabel, 0, 0);
  groupLayout->addWidget(symbolEdit_, 0, 1);

  // size
  QLabel *sizeLabel = CQUtil::makeLabelWidget<QLabel>("Size", "sizeLabel");

  sizeEdit_ = new CQChartsLengthEdit;

  groupLayout->addWidget(sizeLabel, 1, 0);
  groupLayout->addWidget(sizeEdit_, 1, 1);

  // stroke
  strokeEdit_ = new CQChartsStrokeDataEdit;

  strokeEdit_->setTitle("Stroke");
  strokeEdit_->setPreview(false);

  groupLayout->addWidget(strokeEdit_, 2, 0, 1, 2);

  // fill
  fillEdit_ = new CQChartsFillDataEdit;

  fillEdit_->setTitle("Fill");
  fillEdit_->setPreview(false);

  groupLayout->addWidget(fillEdit_, 3, 0, 1, 2);

  //---

  preview_ = new CQChartsSymbolDataEditPreview(this);

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
CQChartsSymbolDataEdit::
setData(const CQChartsSymbolData &d)
{
  data_ = d;

  dataToWidgets();
}

void
CQChartsSymbolDataEdit::
setPlot(CQChartsPlot *plot)
{
  CQChartsEditBase::setPlot(plot);

  strokeEdit_->setPlot(plot);
  fillEdit_  ->setPlot(plot);
}

void
CQChartsSymbolDataEdit::
setView(CQChartsView *view)
{
  CQChartsEditBase::setView(view);

  strokeEdit_->setView(view);
  fillEdit_  ->setView(view);
}

void
CQChartsSymbolDataEdit::
setTitle(const QString &title)
{
  if (groupBox_)
    groupBox_->setTitle(title);

  strokeEdit_->setTitle("Stroke");
  fillEdit_  ->setTitle("Fill");
}

void
CQChartsSymbolDataEdit::
setPreview(bool b)
{
  strokeEdit_->setPreview(b);
  fillEdit_  ->setPreview(b);

  preview_->setVisible(b);
}

void
CQChartsSymbolDataEdit::
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

  if (groupBox_)
    connectDisconnect(b, groupBox_, SIGNAL(clicked(bool)), SLOT(widgetsToData()));

  connectDisconnect(b, symbolEdit_, SIGNAL(symbolChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, sizeEdit_, SIGNAL(lengthChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, strokeEdit_, SIGNAL(strokeDataChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, fillEdit_, SIGNAL(fillDataChanged()), SLOT(widgetsToData()));
}

void
CQChartsSymbolDataEdit::
dataToWidgets()
{
  connectSlots(false);

  if (groupBox_)
    groupBox_->setChecked(data_.isVisible());

  symbolEdit_->setSymbol (data_.type());
  sizeEdit_  ->setLength (data_.size());
  strokeEdit_->setData   (data_.stroke());
  fillEdit_  ->setData   (data_.fill());

  preview_->update();

  connectSlots(true);
}

void
CQChartsSymbolDataEdit::
widgetsToData()
{
  if (groupBox_)
    data_.setVisible(groupBox_->isChecked());

  data_.setType  (symbolEdit_->symbol());
  data_.setSize  (sizeEdit_  ->length());
  data_.setStroke(strokeEdit_->data());
  data_.setFill  (fillEdit_  ->data());

  preview_->update();

  emit symbolDataChanged();
}

//------

CQChartsSymbolDataEditPreview::
CQChartsSymbolDataEditPreview(CQChartsSymbolDataEdit *edit) :
 CQChartsEditPreview(edit), edit_(edit)
{
}

void
CQChartsSymbolDataEditPreview::
draw(QPainter *painter)
{
  const CQChartsSymbolData &data = edit_->data();

  draw(painter, data, rect(), edit_->plot(), edit_->view());
}

void
CQChartsSymbolDataEditPreview::
draw(QPainter *painter, const CQChartsSymbolData &data, const QRect &rect,
     CQChartsPlot *plot, CQChartsView *view)
{
  // set pen
  QColor pc = interpColor(plot, view, data.stroke().color());

  QPen pen;

  double width = CQChartsUtil::limitLineWidth(data.stroke().width().value());

  CQChartsUtil::setPen(pen, data.stroke().isVisible(), pc,
                       data.stroke().alpha(), width, data.stroke().dash());

  painter->setPen(pen);

  //---

  // set brush
  QColor fc = interpColor(plot, view, data.fill().color());

  QBrush brush;

  CQChartsUtil::setBrush(brush, data.fill().isVisible(), fc,
                         data.fill().alpha(), data.fill().pattern());

  painter->setBrush(brush);

  //---

  // draw symbol
  QPointF p(rect.center().x(), rect.center().y());

  double size = data.size().value();

  CQChartsPixelPainter device(painter);

  CQChartsLength symbolSize(CQChartsUnits::PIXEL, size);

  CQChartsDrawUtil::drawSymbol(&device, data.type(), p, symbolSize);
}
