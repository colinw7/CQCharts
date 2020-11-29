#include <CQChartsImageEdit.h>
#include <CQCharts.h>
//#include <CQChartsPlot.h>
//#include <CQChartsObjUtil.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsLineEdit.h>

#include <CQPropertyView.h>
#include <CQPixmapCache.h>
#include <CQUtil.h>

#include <QHBoxLayout>
#include <QToolButton>
#include <QFileDialog>
#include <QPainter>

#include <svg/file_dialog_svg.h>

CQChartsImageEdit::
CQChartsImageEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("imageName");

  setToolTip("Image Name");

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  edit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

  button_ = CQUtil::makeWidget<QToolButton>("button");

  button_->setIcon(CQPixmapCacheInst->getIcon("FILE_DIALOG"));

  layout->addWidget(edit_);
  layout->addWidget(button_);

  connectSlots(true);
}

void
CQChartsImageEdit::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    edit_  , SIGNAL(editingFinished()), this, SLOT(editChanged()));
  CQChartsWidgetUtil::connectDisconnect(b,
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

  edit_->setText(image_.fileName());

  connectSlots(true);
}

void
CQChartsImageEdit::
editChanged()
{
  connectSlots(false);

  image_ = CQChartsImage(edit_->text());

  connectSlots(true);

  emit imageChanged();
}

void
CQChartsImageEdit::
fileSlot()
{
  auto dir = QDir::current().dirName();

  auto fileName = QFileDialog::getOpenFileName(this, "Open File", dir, "Image (*.png *.jpg)");
  if (! fileName.length()) return;

  connectSlots(false);

  edit_->setText(fileName);

  image_ = CQChartsImage(edit_->text());

  connectSlots(true);

  emit imageChanged();
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
  auto str = value.value<CQChartsImage>().toString();

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
#if 0
  auto *item = CQPropertyViewMgrInst->editItem();

  auto *obj = (item ? item->object() : nullptr);

  CQChartsPlot *plot   = nullptr;
  CQChartsView *view   = nullptr;
  CQCharts     *charts = nullptr;

  CQChartsObjUtil::getObjPlotViewChart(obj, plot, view, charts);
#endif

  //---

  auto *edit = new CQChartsImageEdit(parent);

#if 0
  if (plot)
    edit->setCharts(plot->charts());
#endif

  return edit;
}

void
CQChartsImagePropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsImageEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(editChanged()), obj, method);
}

QVariant
CQChartsImagePropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsImageEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->image());
}

void
CQChartsImagePropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsImageEdit *>(w);
  assert(edit);

  auto image = var.value<CQChartsImage>();

  edit->setImage(image);
}
