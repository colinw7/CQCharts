#include <CQChartsLoader.h>
#include <CQFilename.h>

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

CQChartsLoader::
CQChartsLoader(CQCharts *charts) :
 charts_(charts)
{
  QVBoxLayout *layout = new QVBoxLayout(this);

  //---

  QGridLayout *promptGrid = new QGridLayout;

  layout->addLayout(promptGrid);

  //---

  // Type Combo
  typeCombo_ = new QComboBox;

  typeCombo_->setObjectName("typeCombo");

  typeCombo_->addItem("CSV");
  typeCombo_->addItem("TSV");
  typeCombo_->addItem("Json");
  typeCombo_->addItem("Data");
  typeCombo_->addItem("Expr");

  promptGrid->addWidget(new QLabel("Type"), 0, 0);
  promptGrid->addWidget(typeCombo_        , 0, 1);

  //---

  // File Prompt
  fileEdit_ = new CQFilename;

  promptGrid->addWidget(new QLabel("File"), 1, 0);
  promptGrid->addWidget(fileEdit_         , 1, 1);

  //---

  // Option Checks
  QHBoxLayout *optionLayout = new QHBoxLayout;

  commentHeaderCheck_ = new QCheckBox("Comment Header");

  optionLayout->addWidget(commentHeaderCheck_);

  firstLineHeaderCheck_ = new QCheckBox("First Line Header");

  optionLayout->addWidget(firstLineHeaderCheck_);

  firstColumnHeaderCheck_ = new QCheckBox("First Column Header");

  optionLayout->addWidget(firstColumnHeaderCheck_);

  optionLayout->addStretch(1);

  layout->addLayout(optionLayout);

  //---

  // Number Edit
  numberEdit_ = new QLineEdit;

  numberEdit_->setObjectName("numberEdit");
  numberEdit_->setText("100");

  promptGrid->addWidget(new QLabel("Num Rows"), 2, 0);
  promptGrid->addWidget(numberEdit_           , 2, 1);

  //---

  layout->addStretch(1);

  //---

  // Bottom Buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout;

  QPushButton *okButton     = new QPushButton("OK");
  QPushButton *cancelButton = new QPushButton("Cancel");

  connect(okButton    , SIGNAL(clicked()), this, SLOT(okSlot()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelSlot()));

  buttonLayout->addStretch(1);

  buttonLayout->addWidget(okButton);
  buttonLayout->addWidget(cancelButton);

  layout->addLayout(buttonLayout);
}

CQChartsLoader::
~CQChartsLoader()
{
}

bool
CQChartsLoader::
isCommentHeader() const
{
  return commentHeaderCheck_->isChecked();
}

bool
CQChartsLoader::
isFirstLineHeader() const
{
  return firstLineHeaderCheck_->isChecked();
}

bool
CQChartsLoader::
isFirstColumnHeader() const
{
  return firstColumnHeaderCheck_->isChecked();
}

int
CQChartsLoader::
numRows() const
{
  bool ok;

  int n = numberEdit_->text().toInt(&ok);

  if (! ok)
    n = 100;

  return n;
}

void
CQChartsLoader::
okSlot()
{
  QString filename = fileEdit_->name();

  emit loadFile(typeCombo_->currentText(), filename);

  hide();
}

void
CQChartsLoader::
cancelSlot()
{
  hide();
}
