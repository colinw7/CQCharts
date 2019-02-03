#include <CQChartsBoxDataEdit.h>

#include <CQChartsShapeDataEdit.h>
#include <CQChartsSidesEdit.h>
#include <CQChartsRoundedPolygon.h>
#include <CQRealSpin.h>

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

CQChartsBoxDataEdit::
CQChartsBoxDataEdit(QWidget *parent) :
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

  // margin
  QLabel *marginLabel = new QLabel("Margin");

  marginEdit_ = new CQRealSpin;

  connect(marginEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(marginLabel, 0, 0);
  groupLayout->addWidget(marginEdit_, 0, 1);

  // padding
  QLabel *paddingLabel = new QLabel("Padding");

  paddingEdit_ = new CQRealSpin;

  connect(paddingEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));

  groupLayout->addWidget(paddingLabel, 1, 0);
  groupLayout->addWidget(paddingEdit_, 1, 1);

  // shape
  shapeEdit_ = new CQChartsShapeDataEdit;

  //shapeEdit_->setTitle("Shape");

  connect(shapeEdit_, SIGNAL(shapeDataChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(shapeEdit_, 2, 0, 1, 2);

  // sides
  sidesEdit_ = new CQChartsSidesEdit;

  connect(sidesEdit_, SIGNAL(sidesChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(sidesEdit_, 3, 0, 1, 2);

  //---

  preview_ = new CQChartsBoxDataEditPreview(this);

  groupLayout->addWidget(preview_, 4, 1);

  //---

  groupLayout->setRowStretch(5, 1);

  //---

  dataToWidgets();
}

void
CQChartsBoxDataEdit::
dataToWidgets()
{
  disconnect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  disconnect(marginEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  disconnect(paddingEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  disconnect(shapeEdit_, SIGNAL(shapeDataChanged()), this, SLOT(widgetsToData()));
  disconnect(sidesEdit_, SIGNAL(sidesChanged()), this, SLOT(widgetsToData()));

  groupBox_   ->setChecked(data_.visible);
  marginEdit_ ->setValue(data_.margin);
  paddingEdit_->setValue(data_.padding);
  shapeEdit_  ->setData(data_.shape);
  sidesEdit_  ->setSides(data_.borderSides);

  preview_->update();

  connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  connect(marginEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  connect(paddingEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  connect(shapeEdit_, SIGNAL(shapeDataChanged()), this, SLOT(widgetsToData()));
  connect(sidesEdit_, SIGNAL(sidesChanged()), this, SLOT(widgetsToData()));
}

void
CQChartsBoxDataEdit::
widgetsToData()
{
  data_.visible     = groupBox_   ->isChecked();
  data_.margin      = marginEdit_ ->value();
  data_.padding     = paddingEdit_->value();
  data_.shape       = shapeEdit_  ->data();
  data_.borderSides = sidesEdit_  ->sides();

  preview_->update();

  emit boxDataChanged();
}

//------

CQChartsBoxDataEditPreview::
CQChartsBoxDataEditPreview(CQChartsBoxDataEdit *edit) :
 edit_(edit)
{
}

void
CQChartsBoxDataEditPreview::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing);

  const CQChartsBoxData &data = edit_->data();

  QPen pen;

  double width = CQChartsUtil::limitLineWidth(data.shape.border().width().value());

  CQChartsUtil::setPen(pen, data.shape.border().isVisible(), data.shape.border().color().color(),
                       data.shape.border().alpha(), width, data.shape.border().dash());

  QBrush brush;

  CQChartsUtil::setBrush(brush, data.shape.background().isVisible(),
                         data.shape.background().color().color(), data.shape.background().alpha(),
                         data.shape.background().pattern());

  painter.setPen  (pen);
  painter.setBrush(brush);

  double cxs = data.shape.border().cornerSize().value();
  double cys = data.shape.border().cornerSize().value();

  CQChartsRoundedPolygon::draw(&painter, rect(), cxs, cys);
}

QSize
CQChartsBoxDataEditPreview::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("XXXX"), fm.height() + 4);
}
