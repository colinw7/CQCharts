#include <CQChartsViewStatus.h>
#include <CQChartsWindow.h>
#include <QHBoxLayout>
#include <QLabel>

CQChartsViewStatus::
CQChartsViewStatus(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  setObjectName("status");

  setAutoFillBackground(true);

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  statusLabel_ = new QLabel;

  statusLabel_->setObjectName("status");

  posLabel_ = new QLabel;

  posLabel_->setObjectName("pos");

  selLabel_ = new QLabel;

  selLabel_->setObjectName("sel");

  layout->addWidget (statusLabel_);
  layout->addStretch(1);
  layout->addWidget (posLabel_);
  layout->addWidget (selLabel_);

  //---

  setStatusText("");
  setPosText("");
  setSelText("0");
}

QString
CQChartsViewStatus::
statusText() const
{
  return statusLabel_->text();
}

void
CQChartsViewStatus::
setStatusText(const QString &s)
{
  statusLabel_->setText(s);

  update();
}

QString
CQChartsViewStatus::
posText() const
{
  return posLabel_->text();
}

QString
CQChartsViewStatus::
selText() const
{
  return selLabel_->text();
}

void
CQChartsViewStatus::
setPosText(const QString &s)
{
  posLabel_->setText("<b>Pos:</b> " + s);

  update();
}

void
CQChartsViewStatus::
setSelText(const QString &s)
{
  selLabel_->setText("<b>Sel:</b> " + s);

  update();
}

QSize
CQChartsViewStatus::
sizeHint() const
{
  QFontMetricsF fm(font());

  return QSize(fm.width("XX"), fm.height() + 4);
}
