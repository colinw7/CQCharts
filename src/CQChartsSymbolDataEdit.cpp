#include <CQChartsSymbolDataEdit.h>

#include <CQChartsSymbolEdit.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsStrokeDataEdit.h>
#include <CQChartsFillDataEdit.h>

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

CQChartsSymbolDataEdit::
CQChartsSymbolDataEdit(QWidget *parent) :
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

  // symbol
  QLabel *symbolLabel = new QLabel("Symbol");

  symbolEdit_ = new CQChartsSymbolEdit;

  connect(symbolEdit_, SIGNAL(symbolChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(symbolLabel, 0, 0);
  groupLayout->addWidget(symbolEdit_, 0, 1);

  // size
  QLabel *sizeLabel = new QLabel("Size");

  sizeEdit_ = new CQChartsLengthEdit;

  connect(sizeEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(sizeLabel, 1, 0);
  groupLayout->addWidget(sizeEdit_, 1, 1);

  // stroke
  strokeEdit_ = new CQChartsStrokeDataEdit;

  strokeEdit_->setTitle("Stroke");

  connect(strokeEdit_, SIGNAL(strokeDataChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(strokeEdit_, 2, 0, 1, 2);

  // fill
  fillEdit_ = new CQChartsFillDataEdit;

  fillEdit_->setTitle("Fill");

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
dataToWidgets()
{
  disconnect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  disconnect(symbolEdit_, SIGNAL(symbolChanged()), this, SLOT(widgetsToData()));
  disconnect(sizeEdit_, SIGNAL(lengthChanged()), this, SLOT(widgetsToData()));
  disconnect(strokeEdit_, SIGNAL(strokeDataChanged()), this, SLOT(widgetsToData()));
  disconnect(fillEdit_, SIGNAL(fillDataChanged()), this, SLOT(widgetsToData()));

  groupBox_->setChecked(data_.visible);

  symbolEdit_->setSymbol (data_.type);
  sizeEdit_  ->setLength (data_.size);
  strokeEdit_->setData   (data_.stroke);
  fillEdit_  ->setData   (data_.fill);

  preview_->update();

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
  data_.visible = groupBox_->isChecked();

  data_.type   = symbolEdit_->symbol();
  data_.size   = sizeEdit_  ->length();
  data_.stroke = strokeEdit_->data();
  data_.fill   = fillEdit_  ->data();

  preview_->update();

  emit symbolDataChanged();
}

//------

CQChartsSymbolDataEditPreview::
CQChartsSymbolDataEditPreview(CQChartsSymbolDataEdit *edit) :
 edit_(edit)
{
}

void
CQChartsSymbolDataEditPreview::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing);

  const CQChartsSymbolData &data = edit_->data();

  CQChartsGeom::Point p(rect().center().x(), rect().center().y());

  double size = data.size.value();

  QPen pen;

  double width = CQChartsUtil::limitLineWidth(data.stroke.width().value());

  CQChartsUtil::setPen(pen, data.stroke.isVisible(), data.stroke.color().color(),
                       data.stroke.alpha(), width, data.stroke.dash());

  QBrush brush;

  CQChartsUtil::setBrush(brush, data.fill.isVisible(), data.fill.color().color(),
                         data.fill.alpha(), data.fill.pattern());

  painter.setPen  (pen);
  painter.setBrush(brush);

  CQChartsSymbol2DRenderer srenderer(&painter, p, size);

  if (data.fill.isVisible()) {
    CQChartsPlotSymbolMgr::fillSymbol(data.type, &srenderer);

    if (data.stroke.isVisible())
      CQChartsPlotSymbolMgr::strokeSymbol(data.type, &srenderer);
  }
  else {
    if (data.stroke.isVisible())
      CQChartsPlotSymbolMgr::drawSymbol(data.type, &srenderer);
  }
}

QSize
CQChartsSymbolDataEditPreview::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("XXXX"), fm.height() + 4);
}
