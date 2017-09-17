#include <CQPropertyItem.h>
#include <CQPropertyDelegate.h>
#include <CQPropertyEditor.h>
#include <CQUtil.h>

#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

CQPropertyItem::
CQPropertyItem(CQPropertyItem *parent, QObject *object, const QString &name) :
 parent_(parent), object_(object), name_(name)
{
  CQUtil::PropInfo propInfo;

  if (CQUtil::getPropInfo(object_, name_, &propInfo) && propInfo.isWritable())
    editable_ = true;
}

void
CQPropertyItem::
addChild(CQPropertyItem *row)
{
  children_.push_back(row);
}

// handle click
bool
CQPropertyItem::
click()
{
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();;

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
CQPropertyItem::
getEditorData() const
{
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  if (propInfo.isEnumType()) {
    int ind = var.toInt();

    const QStringList &names = propInfo.enumNames();

    if (ind >= 0 && ind < names.count())
      var = QString(names[ind]);
  }

  return var;
}

/*! create editor widget for property
*/
QWidget *
CQPropertyItem::
createEditor(QWidget *parent)
{
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  if (propInfo.isEnumType()) {
    int ind = var.toInt();

    const QStringList &names = propInfo.enumNames();

    if (ind >= 0 && ind < names.count())
      var = QVariant(names[ind]);
  }

  CQPropertyEditorFactory *editor = editor_;

  if (! editor)
    editor = CQPropertyEditorMgr::instance()->getEditor(typeName);

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
CQPropertyItem::
setEditorData(const QVariant &value)
{
  CQUtil::PropInfo propInfo;

  if (CQUtil::getPropInfo(object_, name_, &propInfo) && propInfo.isWritable()) {
    if (propInfo.isEnumType()) {
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
CQPropertyItem::
updateValue()
{
  QString typeName;

  CQUtil::PropInfo propInfo;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  CQPropertyEditorFactory *editor = editor_;

  if (! editor)
    editor = CQPropertyEditorMgr::instance()->getEditor(typeName);

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
CQPropertyItem::
isWritable() const
{
  return true;
}

QVariant
CQPropertyItem::
data() const
{
  QVariant var;

  if (! CQUtil::getProperty(object_, name_, var))
    var = QVariant();

  return var;
}

bool
CQPropertyItem::
setData(const QVariant &value)
{
  if (! CQUtil::setProperty(object_, name_, value))
    return false;

  return true;
}

bool
CQPropertyItem::
paint(const CQPropertyDelegate *delegate, QPainter *painter,
      const QStyleOptionViewItem &option, const QModelIndex &index)
{
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  if (propInfo.isEnumType())
    return false;

  QString str;

  if (! CQUtil::variantToString(var, str))
    return false;

  delegate->drawString(painter, option, str, index);

  return true;
}
