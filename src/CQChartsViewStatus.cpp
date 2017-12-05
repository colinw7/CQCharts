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

  layout->addWidget (statusLabel_);
  layout->addStretch(1);
  layout->addWidget (posLabel_);
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

void
CQChartsViewStatus::
setPosText(const QString &s)
{
  posLabel_->setText(s);

  update();
}

QSize
CQChartsViewStatus::
sizeHint() const
{
  QFontMetricsF fm(font());

  return QSize(fm.width("XX"), fm.height() + 4);
}
