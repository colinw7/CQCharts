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

  QHBoxLayout *layout = new QHBoxLayout(this);

  okButton_     = new QPushButton("OK"    ); okButton_    ->setObjectName("ok");
  applyButton_  = new QPushButton("Apply" ); applyButton_ ->setObjectName("apply");
  cancelButton_ = new QPushButton("Cancel"); cancelButton_->setObjectName("cancel");

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
