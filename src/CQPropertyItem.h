#ifndef CQPropertyItem_H
#define CQPropertyItem_H

#include <QObject>
#include <QPointer>
#include <QStyleOptionViewItem>
#include <QString>
#include <QVariant>
#include <vector>
#include <cassert>

class CQPropertyDelegate;
class CQPropertyEditorFactory;
class QPainter;
class QWidget;

class CQPropertyItem : public QObject {
  Q_OBJECT

 public:
  typedef std::vector<CQPropertyItem *> Children;

 public:
  CQPropertyItem(CQPropertyItem *parent, QObject *object, const QString &name);

  bool isValid() const { return id_ == 0xFEEDBEEF; }

  CQPropertyItem *parent() const { return parent_; }

  QObject *object() const { return object_; }

  const Children &children() const { return children_; }

  CQPropertyItem *child(int i) const {
    assert(i >= 0 && i < numChildren());

    return children_[i];
  }

  int numChildren() const { return children_.size(); }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  const QString &alias() const { return alias_; }
  void setAlias(const QString &v) { alias_ = v; }

  void addChild(CQPropertyItem *row);

  bool isEditable() const { return editable_; }
  void setEditable(bool b) { editable_ = b; }

  //! set editor
  void setEditorFactory(CQPropertyEditorFactory *editor) { editor_ = editor; }

  //! handle click
  bool click();

  //! create editor widget
  QWidget *createEditor(QWidget *parent);

  //! set widget property from variant
  void setEditorData(const QVariant &value);

  //! get editor widget text
  QVariant getEditorData() const;

  //! is writable
  bool isWritable() const;

  //! get/set data
  QVariant data() const;
  bool setData(const QVariant &value);

  //! paint item
  bool paint(const CQPropertyDelegate *delegate, QPainter *painter,
             const QStyleOptionViewItem &option, const QModelIndex &index);

 signals:
  void valueChanged(QObject *obj, const QString &name);

 private slots:
  //! called when the editor widget is changed
  void updateValue();

 private:
  uint                     id_       { 0xFEEDBEEF };
  CQPropertyItem*          parent_   { nullptr };
  QPointer<QObject>        object_;
  QString                  name_;
  QString                  alias_;
  Children                 children_;
  bool                     editable_ { false };
  QWidget*                 widget_   { nullptr };
  CQPropertyEditorFactory *editor_   { nullptr }; //! editor interface
};

#endif
