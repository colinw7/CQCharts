#include <CQChartsPlotPropertyEdit.h>
#include <CQChartsLineEditBase.h>
#include <CQChartsPlot.h>

#include <CQPropertyView.h>
#include <CQPropertyViewEditor.h>
#include <CQUtil.h>

#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include <QHBoxLayout>

CQChartsPlotPropertyEditGroup::
CQChartsPlotPropertyEditGroup(CQChartsPlot *plot) :
 plot_(plot)
{
  setObjectName("editGroup");

  setAutoFillBackground(true);

  setFrameShape(QFrame::StyledPanel);

  layout_ = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  auto *widgetFrame = CQUtil::makeWidget<QFrame>("frame");

  widgetLayout_ = CQUtil::makeLayout<QVBoxLayout>(widgetFrame, 0, 0);

  widgetFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  layout_->addWidget(widgetFrame);

  auto *spacer = CQUtil::makeWidget<QFrame>("spacer");

  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout_->addWidget(spacer);
}

void
CQChartsPlotPropertyEditGroup::
setPlot(CQChartsPlot *p)
{
  plot_ = p;

  for (auto &edit : edits_)
    edit->setPlot(p);
}

void
CQChartsPlotPropertyEditGroup::
addSlot(const QString &s)
{
  auto strs = s.split(" ", QString::SkipEmptyParts);

  if (strs.size() == 0)
    return;

  QString label;

  if (strs.size() > 1)
    label = strs[1];

  auto propertyName = strs[0];

  auto *edit = new CQChartsPlotPropertyEdit;

  edit->setObjectName(QString("edit%1").arg(edits_.size() + 1));

  if (label == "")
    label = propertyName;

  edit->setLabel(label);

  edit->setPropertyName(propertyName);

  addEdit(edit);
}

void
CQChartsPlotPropertyEditGroup::
addEdit(CQChartsPlotPropertyEdit *edit)
{
  edit->setPlot(plot_);

  widgetLayout_->addWidget(edit);

  edits_.push_back(edit);
}

QSize
CQChartsPlotPropertyEditGroup::
sizeHint() const
{
  int w = 0, h = 0;

  for (auto &edit : edits_) {
    auto s = edit->sizeHint();

    w  = std::max(w, s.width());
    h += s.height() + 2;
  }

  return QSize(w + 4, h + 2);
}

//------

CQChartsPlotPropertyEdit::
CQChartsPlotPropertyEdit(CQChartsPlot *plot, const QString &propertyName) :
 plot_(plot), propertyName_(propertyName)
{
  setObjectName("edit");

  layout_ = CQUtil::makeLayout<QHBoxLayout>(this, 2, 2);

  init();
}

void
CQChartsPlotPropertyEdit::
setPlot(CQChartsPlot *p)
{
  if (p != plot_) {
    plot_ = p;

    init(/*rebuild*/true);
  }
}

void
CQChartsPlotPropertyEdit::
setPropertyName(const QString &s)
{
  if (s != propertyName_) {
    propertyName_ = s;

    init(/*rebuild*/true);
  }
}

void
CQChartsPlotPropertyEdit::
setLabel(const QString &s)
{
  if (s != label_) {
    label_ = s;

    init();
  }
}

void
CQChartsPlotPropertyEdit::
init(bool rebuild)
{
  if (rebuild) {
    if (widget_) {
      delete widget_;

      widget_ = nullptr;
    }
  }

  if (! plot_ || propertyName_ == "")
    return;

  if (! labelWidget_) {
    labelWidget_ = CQUtil::makeWidget<QLabel>("label");

    layout_->addWidget(labelWidget_);

    widgetLayout_ = CQUtil::makeLayout<QHBoxLayout>(0, 0);

    layout_->addLayout(widgetLayout_);

    layout_->addStretch(1);
  }

  labelWidget_->setText(label_);

  if (rebuild) {
    widget_ = createEditor();

    if (widget_)
      widgetLayout_->addWidget(widget_);
  }
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

    setWidgetPlot(widget);

    factory->setValue(widget, var);

    factory->connect(widget, this, SLOT(updateValue()));
  }
  else if (propInfo.isEnumType()) {
    auto valueStr = var.toString();

    const auto &names = propInfo.enumNames();

    auto *combo = CQUtil::makeWidget<QComboBox>(this, "combo");

    combo->addItems(names);
    combo->setCurrentIndex(combo->findText(valueStr));

    connect(combo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateValue()));

    widget = combo;
  }
  // bool - create toggle
  // TODO: use button press (no need to edit) see CQCheckTree.cpp
  else if (typeName == "bool") {
    auto *check = CQUtil::makeWidget<QCheckBox>(this, "check");

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

    auto edit = CQUtil::makeWidget<QLineEdit>(this, "edit");

    edit->setText(valueStr);

    widget = edit;
  }
  else {
    QString valueStr;

    if (! CQUtil::variantToString(var, valueStr)) {
      //std::cerr << "Failed to convert to string" << std::endl;
    }

    auto edit = CQUtil::makeWidget<QLineEdit>(this, "edit");

    edit->setText(valueStr);

    widget = edit;
  }

  return widget;
}

void
CQChartsPlotPropertyEdit::
setWidgetPlot(QWidget *widget)
{
  auto *lineEdit = qobject_cast<CQChartsLineEditBase *>(widget);

  if (lineEdit)
    lineEdit->setPlot(plot_);
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

    auto text = edit->text();

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

QSize
CQChartsPlotPropertyEdit::
sizeHint() const
{
  auto ls = (labelWidget_ ? labelWidget_->sizeHint() : QSize());
  auto ws = (widget_      ? widget_     ->sizeHint() : QSize());

  return QSize(ls.width() + ws.width() + 6, std::max(ls.height(), ws.height()) + 4);
}
