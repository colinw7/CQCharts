#include <CQChartsLoader.h>
#include <CQFilename.h>

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

CQChartsLoader::
CQChartsLoader(CQCharts *charts) :
 charts_(charts)
{
  QVBoxLayout *layout = new QVBoxLayout(this);

  QGridLayout *promptGrid = new QGridLayout;

  layout->addLayout(promptGrid);

  typeCombo_ = new QComboBox;

  typeCombo_->addItem("CSV");
  typeCombo_->addItem("Json");
  typeCombo_->addItem("Data");

  promptGrid->addWidget(new QLabel("Type"), 0, 0);
  promptGrid->addWidget(typeCombo_        , 0, 1);

  fileEdit_ = new CQFilename;

  promptGrid->addWidget(new QLabel("File"), 1, 0);
  promptGrid->addWidget(fileEdit_         , 1, 1);

  //---

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
