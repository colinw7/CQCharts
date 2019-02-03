#include <CQChartsTextBoxDataEdit.h>

#include <CQChartsTextDataEdit.h>
#include <CQChartsBoxDataEdit.h>
#include <CQChartsRoundedPolygon.h>

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

CQChartsTextBoxDataEdit::
CQChartsTextBoxDataEdit(QWidget *parent) :
 QFrame(parent)
{
  QGridLayout *layout = new QGridLayout(this);

  //---

  // text
  textEdit_ = new CQChartsTextDataEdit;

  //textEdit_->setTitle("Text");

  connect(textEdit_, SIGNAL(textDataChanged()), this, SLOT(widgetsToData()));

  layout->addWidget(textEdit_, 0, 0, 1, 2);

  // box
  boxEdit_ = new CQChartsBoxDataEdit;

  //boxEdit_->setTitle("Box");

  connect(boxEdit_, SIGNAL(boxDataChanged()), this, SLOT(widgetsToData()));

  layout->addWidget(boxEdit_, 1, 0, 1, 2);

  //---

  preview_ = new CQChartsTextBoxDataEditPreview(this);

  layout->addWidget(preview_, 2, 1);

  //---

  layout->setRowStretch(3, 1);

  //---

  dataToWidgets();
}

void
CQChartsTextBoxDataEdit::
dataToWidgets()
{
  disconnect(textEdit_, SIGNAL(textDataChanged()), this, SLOT(widgetsToData()));
  disconnect(boxEdit_, SIGNAL(boxDataChanged()), this, SLOT(widgetsToData()));

  textEdit_->setData(data_.text);
  boxEdit_ ->setData(data_.box);

  preview_->update();

  connect(textEdit_, SIGNAL(textDataChanged()), this, SLOT(widgetsToData()));
  connect(boxEdit_, SIGNAL(boxDataChanged()), this, SLOT(widgetsToData()));

}

void
CQChartsTextBoxDataEdit::
widgetsToData()
{
  data_.text = textEdit_->data();
  data_.box  = boxEdit_ ->data();

  preview_->update();

  emit textBoxDataChanged();
}

//------

CQChartsTextBoxDataEditPreview::
CQChartsTextBoxDataEditPreview(CQChartsTextBoxDataEdit *edit) :
 edit_(edit)
{
}

void
CQChartsTextBoxDataEditPreview::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing);

  const CQChartsTextBoxData &data = edit_->data();

  const CQChartsBoxData &box = data.box;

  const CQChartsShapeData &shape = box.shape;

  QPen pen;

  double width = CQChartsUtil::limitLineWidth(shape.border().width().value());

  CQChartsUtil::setPen(pen, shape.border().isVisible(), shape.border().color().color(),
                       shape.border().alpha(), width, shape.border().dash());

  QBrush brush;

  CQChartsUtil::setBrush(brush, shape.background().isVisible(), shape.background().color().color(),
                         shape.background().alpha(), shape.background().pattern());

  painter.setPen  (pen);
  painter.setBrush(brush);

  double cxs = shape.border().cornerSize().value();
  double cys = shape.border().cornerSize().value();

  CQChartsRoundedPolygon::draw(&painter, rect(), cxs, cys);
}

QSize
CQChartsTextBoxDataEditPreview::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("XXXX"), fm.height() + 4);
}
