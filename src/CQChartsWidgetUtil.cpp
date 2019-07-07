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
  QLabel *qlabel = CQUtil::makeLabelWidget<QLabel>(label, "label" + label);

  qlabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

  playout->addWidget(qlabel, row, 0);
  playout->addWidget(widget, row, 1);

  ++row;
}

}

//------

CQChartsDialogButtons::
CQChartsDialogButtons(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("dialogButtons");

  QHBoxLayout *layout = CQUtil::makeLayout<QHBoxLayout>(this, 2, 2);

  okButton_     = CQUtil::makeLabelWidget<QPushButton>("OK"    , "ok"    );
  applyButton_  = CQUtil::makeLabelWidget<QPushButton>("Apply" , "apply" );
  cancelButton_ = CQUtil::makeLabelWidget<QPushButton>("Cancel", "cancel");

  applyButton_->setDefault(true);

  layout->addStretch(1);
  layout->addWidget (okButton_);
  layout->addWidget (applyButton_);
  layout->addWidget (cancelButton_);

  //connect(okButton_    , SIGNAL(clicked()), this, SIGNAL(okPressed()));
  //connect(applyButton_ , SIGNAL(clicked()), this, SIGNAL(applyPressed()));
  //connect(cancelButton_, SIGNAL(clicked()), this, SIGNAL(cancelPressed()));
}

void
CQChartsDialogButtons::
connect(QWidget *w, const char *okSlot, const char *applySlot, const char *cancelSlot)
{
  QObject::connect(okButton_    , SIGNAL(clicked()), w, okSlot);
  QObject::connect(applyButton_ , SIGNAL(clicked()), w, applySlot);
  QObject::connect(cancelButton_, SIGNAL(clicked()), w, cancelSlot);
}

void
CQChartsDialogButtons::
setToolTips(const QString &okTip, const QString &applyTip, const QString &cancelTip)
{
  okButton_    ->setToolTip(okTip);
  applyButton_ ->setToolTip(applyTip);
  cancelButton_->setToolTip(cancelTip);
}
