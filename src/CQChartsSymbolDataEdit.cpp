#include <CQChartsSymbolDataEdit.h>
#include <CQChartsSymbolEdit.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsStrokeDataEdit.h>
#include <CQChartsFillDataEdit.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

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

  setToolTip("Symbol Data");

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

  auto tip = QString("%1 (%2)").arg(symbolData().toString());

  edit_->setToolTip(tip);

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
  connectBaseSlots(b);

  CQChartsWidgetUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(symbolDataChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsSymbolDataLineEdit::
drawPreview(QPainter *painter, const QRect &rect)
{
  auto data = this->symbolData();

  double s  = rect.height()/2.0 - 4.0;
  double is = std::round(s);

  int xl = 3;

  data.setSize(CQChartsLength::pixel(s));

  QRect rect1(int(rect.left() + xl), int(rect.center().y() - is), int(2.0*is), int(2.0*is));

  CQChartsSymbolDataEditPreview::draw(painter, data, rect1, plot(), view());

  //---

  auto str = QString("%1 %2").
    arg(symbolData().symbol().toString()).arg(symbolData().size().toString());

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
  auto data = CQChartsSymbolData::fromVariant(value);

  CQChartsSymbolDataEditPreview::draw(painter, data, rect, plot, view);
}

QString
CQChartsSymbolDataPropertyViewType::
tip(const QVariant &value) const
{
  auto str = CQChartsSymbolData::fromVariant(value).toString();

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
  auto *edit = qobject_cast<CQChartsSymbolDataLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(symbolDataChanged()), obj, method);
}

QVariant
CQChartsSymbolDataPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsSymbolDataLineEdit *>(w);
  assert(edit);

  return CQChartsSymbolData::toVariant(edit->symbolData());
}

void
CQChartsSymbolDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsSymbolDataLineEdit *>(w);
  assert(edit);

  auto data = CQChartsSymbolData::fromVariant(var);

  edit->setSymbolData(data);
}

//------

CQChartsSymbolDataEdit::
CQChartsSymbolDataEdit(QWidget *parent, bool optional) :
 CQChartsEditBase(parent)
{
  setObjectName("symbolDataEdit");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  if (optional) {
    groupBox_ = CQUtil::makeLabelWidget<CQGroupBox>("Visible", "groupBox");

    groupBox_->setCheckable(true);
    groupBox_->setChecked(false);
    groupBox_->setTitle("Visible");

    layout->addWidget(groupBox_);
  }

  //---

  auto *groupLayout = CQUtil::makeLayout<QGridLayout>(groupBox_, 2, 2);

  if (! optional)
    layout->addLayout(groupLayout);

  // symbol
  auto *symbolLabel = CQUtil::makeLabelWidget<QLabel>("Symbol", "symbolLabel");

  symbolEdit_ = new CQChartsSymbolLineEdit;

  groupLayout->addWidget(symbolLabel, 0, 0);
  groupLayout->addWidget(symbolEdit_, 0, 1);

  // size
  auto *sizeLabel = CQUtil::makeLabelWidget<QLabel>("Size", "sizeLabel");

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

  auto connectDisconnect = [&](QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(connected_, w, from, this, to);
  };

  if (groupBox_)
    connectDisconnect(groupBox_, SIGNAL(clicked(bool)), SLOT(widgetsToData()));

  connectDisconnect(symbolEdit_, SIGNAL(symbolChanged()), SLOT(widgetsToData()));
  connectDisconnect(sizeEdit_, SIGNAL(lengthChanged()), SLOT(widgetsToData()));
  connectDisconnect(strokeEdit_, SIGNAL(strokeDataChanged()), SLOT(widgetsToData()));
  connectDisconnect(fillEdit_, SIGNAL(fillDataChanged()), SLOT(widgetsToData()));
}

void
CQChartsSymbolDataEdit::
dataToWidgets()
{
  connectSlots(false);

  if (groupBox_)
    groupBox_->setChecked(data_.isVisible());

  symbolEdit_->setSymbol(data_.symbol());
  sizeEdit_  ->setLength(data_.size());
  strokeEdit_->setData  (data_.stroke());
  fillEdit_  ->setData  (data_.fill());

  preview_->update();

  connectSlots(true);
}

void
CQChartsSymbolDataEdit::
widgetsToData()
{
  if (groupBox_)
    data_.setVisible(groupBox_->isChecked());

  data_.setSymbol(symbolEdit_->symbol());
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
  setToolTip("Symbol Preview");
}

void
CQChartsSymbolDataEditPreview::
draw(QPainter *painter)
{
  const auto &data = edit_->data();

  draw(painter, data, rect(), edit_->plot(), edit_->view());
}

void
CQChartsSymbolDataEditPreview::
draw(QPainter *painter, const CQChartsSymbolData &data, const QRect &rect,
     CQChartsPlot *plot, CQChartsView *view)
{
  // set pen
  auto pc = interpColor(plot, view, data.stroke().color());

  QPen pen;

  double width = data.stroke().width().value();

  width = (plot ? plot->limitLineWidth(width) : view->limitLineWidth(width));

  CQChartsUtil::setPen(pen, data.stroke().isVisible(), pc,
                       data.stroke().alpha(), width, data.stroke().dash());

  painter->setPen(pen);

  //---

  // set brush
  auto fc = interpColor(plot, view, data.fill().color());

  QBrush brush;

  CQChartsBrushData brushData;

  brushData.setVisible(data.fill().isVisible());
  brushData.setColor  (fc);
  brushData.setAlpha  (data.fill().alpha());
  brushData.setPattern(data.fill().pattern());

  CQChartsDrawUtil::setBrush(brush, brushData);

  painter->setBrush(brush);

  //---

  // draw symbol
  CQChartsGeom::Point p(rect.center().x(), rect.center().y());

  double size = data.size().value();

  CQChartsPixelPaintDevice device(painter);

  auto symbolSize = CQChartsLength::pixel(size);

  if (data.symbol().isValid())
    CQChartsDrawUtil::drawSymbol(&device, data.symbol(), p, symbolSize);
}
