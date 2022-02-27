#include <CQChartsWidgetUtil.h>
#include <CQUtil.h>

#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>

namespace CQChartsWidgetUtil {

void
addGridLabelWidget(QGridLayout *playout, const QString &label, QWidget *widget, int &row)
{
  auto *qlabel = CQUtil::makeLabelWidget<QLabel>(label, "label" + label);

  qlabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

  playout->addWidget(qlabel, row, 0);
  playout->addWidget(widget, row, 1);

  ++row;
}

QFrame *
createHSpacer(int width, const char *name)
{
  auto *spacer = CQUtil::makeWidget<QFrame>(name);

  QFontMetrics fm(spacer->font());

  spacer->setFixedWidth(width*fm.horizontalAdvance("X"));

  return spacer;
}

QFrame *
createHStretch(const char *name)
{
  auto *spacer = CQUtil::makeWidget<QLabel>(name);

  spacer->setText(" ");

  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  return spacer;
}

void setTextColor(QWidget *w, const QColor &c) {
  auto palette = w->palette();

  palette.setColor(QPalette::WindowText, c);
  palette.setColor(QPalette::Text      , c);

  w->setPalette(palette);
}

}

//------

CQChartsDialogButtons::
CQChartsDialogButtons(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("dialogButtons");

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 2, 2);

  okButton_     = CQUtil::makeLabelWidget<QPushButton>("OK"    , "ok"    );
  applyButton_  = CQUtil::makeLabelWidget<QPushButton>("Apply" , "apply" );
  cancelButton_ = CQUtil::makeLabelWidget<QPushButton>("Cancel", "cancel");
  helpButton_   = CQUtil::makeLabelWidget<QPushButton>("Help"  , "help"  );

  applyButton_->setDefault(true);

  layout->addStretch(1);
  layout->addWidget (okButton_);
  layout->addWidget (applyButton_);
  layout->addWidget (cancelButton_);
  layout->addWidget (helpButton_);

  okButton_    ->setToolTip("Apply and Close");
  applyButton_ ->setToolTip("Apply without Close");
  cancelButton_->setToolTip("Close without Apply");
  helpButton_  ->setToolTip("Show help");

  helpButton_->setVisible(false);
}

void
CQChartsDialogButtons::
connect(QWidget *w, const char *okSlot, const char *applySlot, const char *cancelSlot,
        const char *helpSlot)
{
  QObject::connect(okButton_    , SIGNAL(clicked()), w, okSlot);
  QObject::connect(applyButton_ , SIGNAL(clicked()), w, applySlot);
  QObject::connect(cancelButton_, SIGNAL(clicked()), w, cancelSlot);

  if (helpSlot) {
    helpButton_->setVisible(true);

    QObject::connect(helpButton_, SIGNAL(clicked()), w, helpSlot);
  }
}

void
CQChartsDialogButtons::
setToolTips(const QString &okTip, const QString &applyTip, const QString &cancelTip)
{
  okButton_    ->setToolTip(okTip);
  applyButton_ ->setToolTip(applyTip);
  cancelButton_->setToolTip(cancelTip);
}
