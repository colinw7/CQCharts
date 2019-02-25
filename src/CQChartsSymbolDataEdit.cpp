#include <CQChartsSymbolDataEdit.h>

#include <CQChartsSymbolEdit.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsStrokeDataEdit.h>
#include <CQChartsFillDataEdit.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQGroupBox.h>

#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

CQChartsSymbolDataLineEdit::
CQChartsSymbolDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("symbolDataLineEdit");

  //---

  menuEdit_ = dataEdit_ = new CQChartsSymbolDataEdit;

  menu_->setWidget(dataEdit_);

  connect(dataEdit_, SIGNAL(symbolDataChanged()), this, SLOT(menuEditChanged()));

  //---

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

  if (updateText)
    symbolDataToWidgets();

  connectSlots(true);

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

  //---

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  if (optional) {
    groupBox_ = new CQGroupBox;

    groupBox_->setObjectName("groupBox");
    groupBox_->setCheckable(true);
    groupBox_->setChecked(false);
    groupBox_->setTitle("Visible");

    connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));

    layout->addWidget(groupBox_);
  }

  //---

  QGridLayout *groupLayout = new QGridLayout(groupBox_);

  if (! optional)
    layout->addLayout(groupLayout);

  // symbol
  QLabel *symbolLabel = new QLabel("Symbol");
  symbolLabel->setObjectName("symbolLabel");

  symbolEdit_ = new CQChartsSymbolEdit;

  connect(symbolEdit_, SIGNAL(symbolChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(symbolLabel, 0, 0);
  groupLayout->addWidget(symbolEdit_, 0, 1);

  // size
  QLabel *sizeLabel = new QLabel("Size");
  sizeLabel->setObjectName("sizeLabel");

  sizeEdit_ = new CQChartsLengthEdit;

  connect(sizeEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(sizeLabel, 1, 0);
  groupLayout->addWidget(sizeEdit_, 1, 1);

  // stroke
  strokeEdit_ = new CQChartsStrokeDataEdit;

  strokeEdit_->setTitle("Stroke");
  strokeEdit_->setPreview(false);

  connect(strokeEdit_, SIGNAL(strokeDataChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(strokeEdit_, 2, 0, 1, 2);

  // fill
  fillEdit_ = new CQChartsFillDataEdit;

  fillEdit_->setTitle("Fill");
  fillEdit_->setPreview(false);

  connect(fillEdit_, SIGNAL(fillDataChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(fillEdit_, 3, 0, 1, 2);

  //---

  preview_ = new CQChartsSymbolDataEditPreview(this);

  groupLayout->addWidget(preview_, 4, 1);

  //---

  groupLayout->setRowStretch(5, 1);

  //---

  layout->addStretch(1);

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
dataToWidgets()
{
  if (groupBox_)
    disconnect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));

  disconnect(symbolEdit_, SIGNAL(symbolChanged()), this, SLOT(widgetsToData()));
  disconnect(sizeEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));
  disconnect(strokeEdit_, SIGNAL(strokeDataChanged()), this, SLOT(widgetsToData()));
  disconnect(fillEdit_, SIGNAL(fillDataChanged()), this, SLOT(widgetsToData()));

  if (groupBox_)
    groupBox_->setChecked(data_.isVisible());

  symbolEdit_->setSymbol (data_.type());
  sizeEdit_  ->setLength (data_.size());
  strokeEdit_->setData   (data_.stroke());
  fillEdit_  ->setData   (data_.fill());

  preview_->update();

  if (groupBox_)
    connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));

  connect(symbolEdit_, SIGNAL(symbolChanged()), this, SLOT(widgetsToData()));
  connect(sizeEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));
  connect(strokeEdit_, SIGNAL(strokeDataChanged()), this, SLOT(widgetsToData()));
  connect(fillEdit_, SIGNAL(fillDataChanged()), this, SLOT(widgetsToData()));

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
  CQChartsGeom::Point p(rect.center().x(), rect.center().y());

  double size = data.size().value();

  CQChartsSymbol2DRenderer srenderer(painter, p, size);

  if (data.fill().isVisible()) {
    CQChartsPlotSymbolMgr::fillSymbol(data.type(), &srenderer);

    if (data.stroke().isVisible())
      CQChartsPlotSymbolMgr::strokeSymbol(data.type(), &srenderer);
  }
  else {
    if (data.stroke().isVisible())
      CQChartsPlotSymbolMgr::drawSymbol(data.type(), &srenderer);
  }
}
