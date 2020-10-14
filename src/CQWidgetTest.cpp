#include <CQWidgetTest.h>
#include <QApplication>
#include <QPushButton>
#include <QToolButton>
#include <QCheckBox>
#include <QComboBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QDialog>
#include <QTextEdit>
#include <iostream>

CQWidgetTest::
CQWidgetTest()
{
  addType(TestType::OBJECT_NAME);
}

void
CQWidgetTest::
addType(const TestType &type)
{
  testTypes_.insert(type);
}

void
CQWidgetTest::
exec()
{
  const auto &widgets = qApp->allWidgets();

  for (const auto &w : widgets)
    test(w);
}

void
CQWidgetTest::
test(QWidget *w)
{
  if (testTypes_.find(TestType::OBJECT_NAME) != testTypes_.end()) {
    if (isObjectNameType(w)) {
      if (w->objectName() == "") {
        std::cerr << "Missing object name for '" << widgetPath(w).toStdString() << "'\n";
        setObjectName(genObjectName(w));
      }
    }
  }

  if (testTypes_.find(TestType::TOOLTIP) != testTypes_.end()) {
    if (isToolTipType(w)) {
      if (w->toolTip() == "")
        std::cerr << "Missing tooltip for '" << widgetPath(w).toStdString() << "'\n";
    }
  }
}

bool
CQWidgetTest::
isToolTipType(QWidget *w) const
{
  QString className = w->metaObject()->className();

  auto *button   = qobject_cast<QAbstractButton  *>(w);
  auto *comboBox = qobject_cast<QComboBox        *>(w);
  auto *lineEdit = qobject_cast<QLineEdit        *>(w);
  auto *spinBox  = qobject_cast<QAbstractSpinBox *>(w);

  if (lineEdit && qobject_cast<QAbstractSpinBox *>(lineEdit->parentWidget()))
    return false;

  if (button && qobject_cast<QTabBar *>(button->parentWidget()))
    return false;

  if (button && className == "QLineEditIconButton")
    return false;

  if (button && className == "QTableCornerButton")
    return false;

  if (button || comboBox || lineEdit || spinBox )
    return true;

  return false;
}

bool
CQWidgetTest::
isObjectNameType(QWidget *w) const
{
  QString className = w->metaObject()->className();

  auto *button     = qobject_cast<QAbstractButton     *>(w);
  auto *comboBox   = qobject_cast<QComboBox           *>(w);
  auto *lineEdit   = qobject_cast<QLineEdit           *>(w);
  auto *spinBox    = qobject_cast<QAbstractSpinBox    *>(w);
  auto *model      = qobject_cast<QAbstractItemModel  *>(w);
  auto *view       = qobject_cast<QAbstractItemView   *>(w);
  auto *dialog     = qobject_cast<QDialog             *>(w);
  auto *frame      = qobject_cast<QFrame              *>(w);
  auto *scrollArea = qobject_cast<QAbstractScrollArea *>(w);
  auto *tabWidget  = qobject_cast<QTabWidget          *>(w);
  auto *textEdit   = qobject_cast<QTextEdit           *>(w);

  if (lineEdit && qobject_cast<QAbstractSpinBox *>(lineEdit->parentWidget()))
    return false;

  if (button && qobject_cast<QTabBar *>(button->parentWidget()))
    return false;

  if (button && className == "QLineEditIconButton")
    return false;

  if (button && className == "QTableCornerButton")
    return false;

  if (view && className == "QComboBoxListView")
    return false;

  if (view && className == "QHeaderView")
    return false;

  if (frame && className == "QLabel")
    return false;

  if (frame && className == "QComboBoxPrivateContainer")
    return false;

  if (button || comboBox || lineEdit || spinBox || model || view || dialog || frame ||
      scrollArea || tabWidget || textEdit)
    return true;

  return false;
}

QString
CQWidgetTest::
widgetPath(QWidget *w) const
{
  auto *parent = w->parentWidget();

  QString path;

  if (parent) {
    path = widgetPath(parent);

    if (path == "")
      path = "*";
  }

  auto name = w->objectName();

  if (name == "")
    name = w->metaObject()->className();

  if (parent)
    return path + "|" + name;
  else
    return name;
}

QString
CQWidgetTest::
genObjectName(QWidget *w)
{
  QString className = w->metaObject()->className();

  int n = 0;

  auto p = classNameCount_.find(className);

  if (p == classNameCount_.end()) {
    classNameCount_[className] = 0;

    return className;
  }

  n = (*p).second++;

  return QString("%1_%2").arg(className).arg(n);
}
