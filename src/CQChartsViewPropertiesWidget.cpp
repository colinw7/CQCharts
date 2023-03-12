#include <CQChartsViewPropertiesWidget.h>
#include <CQChartsPropertyViewTree.h>
#include <CQChartsPropertyViewTreeFilterEdit.h>
#include <CQChartsKeyEdit.h>
#include <CQChartsKey.h>
#include <CQChartsView.h>

#include <CQChartsWidgetUtil.h>
#include <CQUtil.h>

#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDir>

#include <fstream>

CQChartsViewPropertiesControl::
CQChartsViewPropertiesControl(CQChartsView *view) :
 QFrame()
{
  setObjectName("viewPropertiesControl");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  propertiesWidget_ = new CQChartsViewPropertiesWidget(view);

  layout->addWidget(propertiesWidget_);

  //--

  auto createPushButton = [&](const QString &label, const QString &objName,
                              const QString &tipStr, const char *slotName) {
    auto *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    button->setToolTip(tipStr);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  //---

  auto *editFrame       = CQUtil::makeWidget<QFrame>("editFrame");
  auto *editFrameLayout = CQUtil::makeLayout<QHBoxLayout>(editFrame, 2, 2);

  auto *editKeyButton =
    createPushButton("Edit Key...", "key", "Edit View Key", SLOT(editKeySlot()));
  auto *writeButton =
    createPushButton("Write", "write", "Write View Script", SLOT(writeSlot()));

  editFrameLayout->addWidget(editKeyButton);
  editFrameLayout->addWidget(CQChartsWidgetUtil::createHSpacer(1));
  editFrameLayout->addWidget(writeButton);
  editFrameLayout->addStretch(1);

  layout->addWidget(editFrame);
}

CQChartsView *
CQChartsViewPropertiesControl::
view() const
{
  return propertiesWidget_->view();
}

void
CQChartsViewPropertiesControl::
setView(CQChartsView *view)
{
  propertiesWidget_->setView(view);
}

void
CQChartsViewPropertiesControl::
editKeySlot()
{
  auto *view = propertiesWidget_->view();

  auto *key = (view ? view->key() : nullptr);
  if (! key) return;

  if (editKeyDlg_)
    delete editKeyDlg_;

  editKeyDlg_ = new CQChartsEditKeyDlg(this, key);

  editKeyDlg_->show();
  editKeyDlg_->raise();
}

void
CQChartsViewPropertiesControl::
writeSlot()
{
  auto *view = propertiesWidget_->view();
  if (! view) return;

  auto dir = QDir::current().dirName() + "/view.tcl";

  auto filename = QFileDialog::getSaveFileName(this, "Write View", dir, "Files (*.tcl)");
  if (! filename.length()) return; // cancelled

  auto fs = std::ofstream(filename.toStdString(), std::ofstream::out);

  //---

  view->writeAll(fs);
}

//---

CQChartsViewPropertiesWidget::
CQChartsViewPropertiesWidget(CQChartsView *view)
{
  setObjectName("viewPropertiesWidget");

  CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  setView(view);
}

CQChartsView *
CQChartsViewPropertiesWidget::
view() const
{
  return view_.data();
}

void
CQChartsViewPropertiesWidget::
setView(CQChartsView *view)
{
  if (view_ == view)
    return;

  view_ = view;
  if (! view_) return;

  //---

  auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
  assert(layout);

  propertyTree_ = new CQChartsPropertyViewTree(this, view_->propertyModel());

  propertyTree_->setObjectName("propertyTree");

  propertyTree_->setView(view);

  connect(propertyTree_, SIGNAL(itemSelected(QObject *, const QString &)),
          this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

  connect(propertyTree_, SIGNAL(filterStateChanged(bool, bool)),
          this, SLOT(filterStateSlot(bool, bool)));

  //--

  filterEdit_ = new CQChartsPropertyViewTreeFilterEdit(propertyTree_);

  filterEdit_->setVisible(propertyTree_->isFilterDisplayed());

  //--

  layout->addWidget(filterEdit_);
  layout->addWidget(propertyTree_);
}

void
CQChartsViewPropertiesWidget::
filterStateSlot(bool visible, bool focus)
{
  if (! filterEdit_) return;

  filterEdit_->setVisible(visible);

  if (focus)
    filterEdit_->setFocus();
}
