#ifndef CQChartsLineDashEdit_H
#define CQChartsLineDashEdit_H

#include <CQChartsLineDash.h>
#include <QFrame>
#include <QAction>
#include <map>

class QMenu;
class QLineEdit;
class QToolButton;
class CQIconCombo;
class CQChartsLineDashEditAction;

class CQChartsLineDashEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool editable READ editable WRITE setEditable)

 public:
  CQChartsLineDashEdit(QWidget *parent=0);

  bool editable() const { return editable_; }
  void setEditable(bool b);

  void setLineDash(const CQChartsLineDash &dash);
  const CQChartsLineDash &getLineDash() const { return dash_; }

  void addDashOption(const std::string &id, const CQChartsLineDash &dash);

  static QIcon dashIcon(const CQChartsLineDash &dash);

 private:
  void updateState();

 private slots:
  void dashChangedSlot();
  void menuItemActivated(QAction *);
  void comboItemChanged();

 signals:
  void valueChanged(const CQChartsLineDash &dash);

 private:
  typedef std::map<std::string, CQChartsLineDashEditAction *> Actions;

  bool             editable_ { false };
  CQChartsLineDash dash_;
  QLineEdit*       edit_     { nullptr };
  QToolButton*     button_   { nullptr };
  QMenu*           menu_     { nullptr };
  CQIconCombo*     combo_    { nullptr };
  Actions          actions_;
};

//---

class CQChartsLineDashEditAction : public QAction {
 public:
  CQChartsLineDashEditAction(CQChartsLineDashEdit *parent, const std::string &id,
                             const CQChartsLineDash &dash, const QIcon &icon);

 private:
  void init();

 private:
  CQChartsLineDashEdit* parent_ { nullptr };
  std::string           id_;
  CQChartsLineDash      dash_;
};

//---

#include <CQPropertyViewType.h>

// type for CQChartsLineDash
class CQChartsLineDashPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsLineDashPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//------

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsLineDash
class CQChartsLineDashPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsLineDashPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
