#include <CQCheckBox.h>

CQCheckBox::
CQCheckBox(QWidget *parent) :
 QCheckBox(parent)
{
  setObjectName("checkbox");

  connect(this, SIGNAL(stateChanged(int)), this, SLOT(stateSlot(int)));

  stateSlot(isChecked());
}

CQCheckBox::
CQCheckBox(const QString &text, QWidget *parent) :
 QCheckBox(text, parent)
{
  setObjectName("checkbox");

  connect(this, SIGNAL(stateChanged(int)), this, SLOT(stateSlot(int)));
}

void
CQCheckBox::
stateSlot(int b)
{
  setText(b ? "true" : "false");
}
