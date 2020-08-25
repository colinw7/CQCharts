#include <CQChartsPlotPropertyEdit.h>
#include <CQChartsPlot.h>

#include <CQPropertyView.h>
#include <CQPropertyViewEditor.h>
#include <CQUtil.h>

#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QHBoxLayout>

CQChartsPlotPropertyEdit::
CQChartsPlotPropertyEdit(CQChartsPlot *plot, const QString &propertyName) :
 plot_(plot), propertyName_(propertyName)
{
  layout_ = new QHBoxLayout(this);

  init();
}

void
CQChartsPlotPropertyEdit::
setPlot(CQChartsPlot *p)
{
  plot_ = p;

  init();
}

void
CQChartsPlotPropertyEdit::
setPropertyName(const QString &s)
{
  propertyName_ = s;

  init();
}

void
CQChartsPlotPropertyEdit::
init()
{
  if (widget_) {
    delete widget_;

    widget_ = nullptr;
  }

  if (! plot_ || propertyName_ == "")
    return;

  widget_ = createEditor();

  if (widget_)
    layout_->addWidget(widget_);
}

QWidget *
CQChartsPlotPropertyEdit::
createEditor()
{
  if (! plot_)
    return nullptr;

  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(plot_, propertyName_, &propInfo))
    typeName = propInfo.typeName();

  QVariant var;

  if (! CQUtil::getProperty(plot_, propertyName_, var))
    var = QVariant();

  if (propInfo.isEnumType()) {
    int value = var.toInt();

    QString str;

    if (CQUtil::getPropInfoEnumValueName(propInfo, value, str))
      var = str;
  }

  QWidget *widget = nullptr;

  auto *factory = CQPropertyViewMgrInst->getEditor(typeName);

  if (factory) {
    widget = factory->createEdit(this);

    factory->setValue(widget, var);

    factory->connect(widget, this, SLOT(updateValue()));
  }
  else if (propInfo.isEnumType()) {
    QString valueStr = var.toString();

    const QStringList &names = propInfo.enumNames();

    auto *combo = new QComboBox(this);

    combo->setObjectName("combo");

    combo->addItems(names);
    combo->setCurrentIndex(combo->findText(valueStr));

    connect(combo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateValue()));

    widget = combo;
  }
  // bool - create toggle
  // TODO: use button press (no need to edit) see CQCheckTree.cpp
  else if (typeName == "bool") {
    auto *check = new QCheckBox(this);

    check->setObjectName("check");

    check->setChecked(var.toBool());

    check->setText(check->isChecked() ? "true" : "false");

    check->setAutoFillBackground(true);
    //check->setLayoutDirection(Qt::RightToLeft);

    connect(check, SIGNAL(stateChanged(int)), this, SLOT(updateValue()));

    widget = check;
  }
  else if (var.type() == QVariant::UserType) {
    QString valueStr;

    if (! CQUtil::userVariantToString(var, valueStr)) {
      //std::cerr << "Failed to convert to string" << std::endl;
    }

    auto edit = new QLineEdit(this);

    edit->setText(valueStr);

    widget = edit;
  }
  else {
    QString valueStr;

    if (! CQUtil::variantToString(var, valueStr)) {
      //std::cerr << "Failed to convert to string" << std::endl;
    }

    auto edit = new QLineEdit(this);

    edit->setText(valueStr);

    widget = edit;
  }

  return widget;
}

void
CQChartsPlotPropertyEdit::
updateValue()
{
  if (! plot_)
    return;

  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(plot_, propertyName_, &propInfo))
    typeName = propInfo.typeName();

  auto *factory = CQPropertyViewMgrInst->getEditor(typeName);

  QVariant var;

  if      (factory) {
    var = factory->getValue(widget_);
  }
  else if (propInfo.isEnumType()) {
    auto *combo = qobject_cast<QComboBox *>(widget_);
    assert(combo);

    var = combo->currentText();
  }
  else if (typeName == "bool") {
    auto *check = qobject_cast<QCheckBox *>(widget_);
    assert(check);

    check->setText(check->isChecked() ? "true" : "false");

    var = (check->isChecked() ? "1" : "0");
  }
  else if (propInfo.type() == QVariant::UserType) {
    auto *edit = qobject_cast<QLineEdit *>(widget_);
    assert(edit);

    QString text = edit->text();

    var = text;

    if (! CQUtil::userVariantFromString(var, text))
      return;
  }
  else {
    auto *edit = qobject_cast<QLineEdit *>(widget_);
    assert(edit);

    var = edit->text();
  }

  CQUtil::setProperty(plot_, propertyName_, var);
}
