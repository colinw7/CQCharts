#include <CQFilename.h>
#include <CQPixmapCache.h>

#include <QFileDialog>
#include <QDir>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>

#include <svg/file_dialog_svg.h>

CQFilename::
CQFilename(QWidget *parent) :
 QFrame(parent), pattern_("Files (*.*)")
{
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  edit_   = new QLineEdit;
  button_ = new QToolButton;

  button_->setIcon(CQPixmapCacheInst->getIcon("FILE_DIALOG"));

  layout->addWidget(edit_);
  layout->addWidget(button_);

  connect(edit_, SIGNAL(returnPressed()), this, SLOT(acceptSlot()));

  connect(button_, SIGNAL(clicked()), this, SLOT(fileSlot()));
}

void
CQFilename::
acceptSlot()
{
  QString fileName = edit_->text();

  emit filenameChanged(fileName);
}

void
CQFilename::
fileSlot()
{
  QString dir = QDir::current().dirName();

  QString fileName;

  if (isSave()) {
    fileName = QFileDialog::getSaveFileName(this, "Save File", dir, pattern_);
  }
  else {
    fileName = QFileDialog::getOpenFileName(this, "Open File", dir, pattern_);
  }

  if (! fileName.length())
    return;

  edit_->setText(fileName);

  emit filenameChanged(fileName);
}

QString
CQFilename::
name() const
{
  return edit_->text();
}

void
CQFilename::
setName(const QString &name)
{
  edit_->setText(name);
}
