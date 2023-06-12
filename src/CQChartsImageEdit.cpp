#include <CQChartsImageEdit.h>
#include <CQCharts.h>
#include <CQChartsPlot.h>
#include <CQChartsObjUtil.h>
#include <CQChartsLineEdit.h>

#include <CQPropertyView.h>
#include <CQIconButton.h>
#include <CQUtil.h>

#include <QHBoxLayout>
#include <QFileDialog>
#include <QPainter>

CQChartsImageEdit::
CQChartsImageEdit(QWidget *parent) :
 CQChartsFrame(parent)
{
  setObjectName("imageName");

  setToolTip("Image Name");

  //---

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  edit_   = CQUtil::makeWidget<CQChartsLineEdit>("edit");
  button_ = CQUtil::makeWidget<CQIconButton>("button");

  button_->setIcon("FILE_DIALOG");

  layout->addWidget(edit_);
  layout->addWidget(button_);

  connectSlots(true);
}

void
CQChartsImageEdit::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b,
    edit_  , SIGNAL(editingFinished()), this, SLOT(editChanged()));
  CQUtil::connectDisconnect(b,
    button_, SIGNAL(clicked()), this, SLOT(fileSlot()));
}

const CQChartsImage &
CQChartsImageEdit::
image() const
{
  return image_;
}

void
CQChartsImageEdit::
setImage(const CQChartsImage &image)
{
  connectSlots(false);

  image_ = image;

  edit_->setText(image_.toString());

  connectSlots(true);
}

void
CQChartsImageEdit::
editChanged()
{
  connectSlots(false);

  image_ = CQChartsImage(edit_->text());

  edit_->setText(image_.toString());

  connectSlots(true);

  Q_EMIT imageChanged();
}

void
CQChartsImageEdit::
fileSlot()
{
  auto dir = QDir::current().dirName();

  dir += "/" + image_.filename();

  auto filename = QFileDialog::getOpenFileName(this, "Open File", dir, "Image (*.png *.jpg)");
  if (! filename.length()) return;

  connectSlots(false);

  image_ = CQChartsImage(filename);

  edit_->setText(image_.toString());

  connectSlots(true);

  Q_EMIT imageChanged();
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsImagePropertyViewType::
CQChartsImagePropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsImagePropertyViewType::
getEditor() const
{
  return new CQChartsImagePropertyViewEditor;
}

bool
CQChartsImagePropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

QString
CQChartsImagePropertyViewType::
tip(const QVariant &value) const
{
  auto str = CQChartsImage::fromVariant(value).toString();

  return str;
}

//------

CQChartsImagePropertyViewEditor::
CQChartsImagePropertyViewEditor()
{
}

QWidget *
CQChartsImagePropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *item = CQPropertyViewMgrInst->editItem();

  auto *obj = (item ? item->object() : nullptr);

  CQChartsObjUtil::ObjData objData;

  CQChartsObjUtil::getObjData(obj, objData);

  //---

  auto *edit = new CQChartsImageEdit(parent);

  if (objData.charts)
    edit->setCharts(objData.charts);

  return edit;
}

void
CQChartsImagePropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsImageEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(imageChanged()), obj, method);
}

QVariant
CQChartsImagePropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsImageEdit *>(w);
  assert(edit);

  return CQChartsImage::toVariant(edit->image());
}

void
CQChartsImagePropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsImageEdit *>(w);
  assert(edit);

  auto image = CQChartsImage::fromVariant(var);

  edit->setImage(image);
}
