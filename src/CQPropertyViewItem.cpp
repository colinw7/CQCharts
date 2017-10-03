#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQPropertyViewEditor.h>
#include <CQAlignEdit.h>
#include <CQUtil.h>

#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

CQPropertyViewItem::
CQPropertyViewItem(CQPropertyViewItem *parent, QObject *object, const QString &name) :
 parent_(parent), object_(object), name_(name)
{
  CQUtil::PropInfo propInfo;

  if (CQUtil::getPropInfo(object_, name_, &propInfo) && propInfo.isWritable())
    editable_ = true;
}

CQPropertyViewItem::
~CQPropertyViewItem()
{
  for (auto &child : children_)
    delete child;
}

void
CQPropertyViewItem::
addChild(CQPropertyViewItem *row)
{
  children_.push_back(row);
}

// handle click
bool
CQPropertyViewItem::
click()
{
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  if (propInfo.isEnumType())
    return false;

  if (typeName == "bool" && propInfo.isWritable()) {
    if (! setData(! var.toBool()))
      std::cerr << "Failed to set property " << name_.toStdString() << std::endl;

    emit valueChanged(object_, name_);

    return true;
  }

  return false;
}

/*! get property value
*/
QVariant
CQPropertyViewItem::
getEditorData() const
{
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  if (propInfo.isEnumType()) {
    int ind = var.toInt();

    QString str;

    if (enumIndToString(propInfo, ind, str))
      var = str;
  }

  return var;
}

/*! create editor widget for property
*/
QWidget *
CQPropertyViewItem::
createEditor(QWidget *parent)
{
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  if (propInfo.isEnumType()) {
    int ind = var.toInt();

    QString str;

    if (enumIndToString(propInfo, ind, str))
      var = str;
  }

  CQPropertyViewEditorFactory *editor = editor_;

  if (! editor)
    editor = CQPropertyViewEditorMgr::instance()->getEditor(typeName);

  if      (editor) {
    widget_ = editor->createEdit(parent);

    editor->setValue(widget_, var);

    editor->connect(widget_, this, SLOT(updateValue()));
  }
  else if (propInfo.isEnumType()) {
    QString valueStr = var.toString();

    const QStringList &names = propInfo.enumNames();

    QComboBox *combo = new QComboBox(parent);

    combo->addItems(names);
    combo->setCurrentIndex(combo->findText(valueStr));

    connect(combo, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(updateValue()));

    widget_ = combo;
  }
  // bool - create toggle
  // TODO: use button press (no need to edit) see CQCheckTree.cpp
  else if (typeName == "bool") {
    QCheckBox *check = new QCheckBox(parent);

    check->setChecked(var.toBool());

    check->setText(check->isChecked() ? "true" : "false");

    check->setAutoFillBackground(true);
    //check->setLayoutDirection(Qt::RightToLeft);

    connect(check, SIGNAL(stateChanged(int)), this, SLOT(updateValue()));

    widget_ = check;
  }
  else {
    QLineEdit *edit = new QLineEdit(parent);

    QString valueStr;

    if (! CQUtil::variantToString(var, valueStr))
      std::cerr << "Failed to convert to string" << std::endl;

    edit->setText(valueStr);

    connect(edit, SIGNAL(editingFinished()), this, SLOT(updateValue()));

    widget_ = edit;
  }

  return widget_;
}

/*! set property from variant
*/
void
CQPropertyViewItem::
setEditorData(const QVariant &value)
{
  CQUtil::PropInfo propInfo;

  if (CQUtil::getPropInfo(object_, name_, &propInfo) && propInfo.isWritable()) {
    QString typeName = propInfo.typeName();

    if     (typeName == "Qt::Alignment") {
      if (! this->setData(value))
        std::cerr << "Failed to set property " << name_.toStdString() << std::endl;
    }
    else if (propInfo.isEnumType()) {
      QString name = CQUtil::variantToString(value);

      const QStringList &names = propInfo.enumNames();

      for (int i = 0; i < names.size(); ++i) {
        if (name == names[i]) {
          QVariant v(i);

          if (! this->setData(v))
            std::cerr << "Failed to set property " << name_.toStdString() << std::endl;
        }
      }
    }
    else {
      if (! this->setData(value))
        std::cerr << "Failed to set property " << name_.toStdString() << std::endl;
    }

    emit valueChanged(object_, name_);
  }
}

/*! update property on widget change
*/
void
CQPropertyViewItem::
updateValue()
{
  QString typeName;

  CQUtil::PropInfo propInfo;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  CQPropertyViewEditorFactory *editor = editor_;

  if (! editor)
    editor = CQPropertyViewEditorMgr::instance()->getEditor(typeName);

  if      (editor) {
    QVariant var = editor->getValue(widget_);

    setEditorData(var);
  }
  else if (propInfo.isEnumType()) {
    QComboBox *combo = qobject_cast<QComboBox *>(widget_);
    assert(combo);

    QString text = combo->currentText();

    setEditorData(text);
  }
  else if (typeName == "bool") {
    QCheckBox *check = qobject_cast<QCheckBox *>(widget_);
    assert(check);

    check->setText(check->isChecked() ? "true" : "false");

    QString text = (check->isChecked() ? "1" : "0");

    setEditorData(text);
  }
  else {
    QLineEdit *edit = qobject_cast<QLineEdit *>(widget_);
    assert(edit);

    QString text = edit->text();

    setEditorData(text);
  }
}

bool
CQPropertyViewItem::
isWritable() const
{
  return true;
}

QVariant
CQPropertyViewItem::
data() const
{
  QVariant var;

  if (! CQUtil::getProperty(object_, name_, var))
    var = QVariant();

  return var;
}

bool
CQPropertyViewItem::
setData(const QVariant &value)
{
  if (! CQUtil::setProperty(object_, name_, value))
    return false;

  return true;
}

bool
CQPropertyViewItem::
paint(const CQPropertyViewDelegate *delegate, QPainter *painter,
      const QStyleOptionViewItem &option, const QModelIndex &index)
{
  bool inside = isInside();

  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  if      (typeName == "QColor") {
    delegate->drawColor(painter, option, var.value<QColor>(), index, inside);
  }
  else if (typeName == "QFont") {
    delegate->drawFont(painter, option, var.value<QFont>(), index, inside);
  }
  else if (typeName == "QPointF") {
    delegate->drawPoint(painter, option, var.value<QPointF>(), index, inside);
  }
  else if (typeName == "QSizeF") {
    delegate->drawSize(painter, option, var.value<QSizeF>(), index, inside);
  }
  else if (typeName == "QRectF") {
    delegate->drawRect(painter, option, var.value<QRectF>(), index, inside);
  }
  else if (typeName == "bool") {
    delegate->drawCheck(painter, option, var.toBool(), index, inside);
  }
  else if (typeName == "Qt::Alignment") {
    int i = var.toInt();

    QString str = CQAlignEdit::toString((Qt::Alignment) i);

    delegate->drawString(painter, option, str, index, inside);
  }
  else if (propInfo.isEnumType()) {
    int ind = var.toInt();

    QString str;

    if (enumIndToString(propInfo, ind, str))
      delegate->drawString(painter, option, str, index, inside);
  }
  else {
    QString str;

    if (! CQUtil::variantToString(var, str))
      return false;

    delegate->drawString(painter, option, str, index, inside);
  }

  return true;
}

bool
CQPropertyViewItem::
enumIndToString(const CQUtil::PropInfo &propInfo, int ind, QString &str) const
{
  const QStringList &names = propInfo.enumNames();

  if (ind < 0 || ind >= names.count())
    return false;

  str = names[ind];

  return true;
}

bool
CQPropertyViewItem::
enumStringToInd(const CQUtil::PropInfo &propInfo, const QString &str, int &ind) const
{
  const QStringList &names = propInfo.enumNames();

  for (int i = 0; i < names.size(); ++i) {
    if (str == names[i]) {
      ind = i;
      return true;
    }
  }

  return false;
}
