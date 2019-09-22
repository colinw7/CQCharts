#ifndef CQChartsLineDashEdit_H
#define CQChartsLineDashEdit_H

#include <CQChartsLineDash.h>
#include <QFrame>
#include <QAction>
#include <map>

class QMenu;
class CQLineEdit;
class QToolButton;
class CQIconCombo;
class CQChartsLineDashEditAction;

/*!
 * \brief line dash edit
 * \ingroup Charts
 */
class CQChartsLineDashEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool             editable READ editable    WRITE setEditable)
  Q_PROPERTY(CQChartsLineDash lineDash READ getLineDash WRITE setLineDash)

 public:
  CQChartsLineDashEdit(QWidget *parent=0);
 ~CQChartsLineDashEdit();

  bool editable() const { return editable_; }
  void setEditable(bool b);

  void setLineDash(const CQChartsLineDash &dash);
  const CQChartsLineDash &getLineDash() const { return dash_; }

  void addDashOption(const std::string &id, const CQChartsLineDash &dash);

  static QIcon dashIcon(const CQChartsLineDash &dash, bool bg=true);

 private:
  void updateState();

 private slots:
  void dashChangedSlot();
  void menuItemActivated(QAction *);
  void comboItemChanged();

 signals:
  void valueChanged(const CQChartsLineDash &dash);

 private:
  using Actions = std::map<std::string, CQChartsLineDashEditAction *>;

  bool             editable_ { false };
  CQChartsLineDash dash_;
  CQLineEdit*      edit_     { nullptr };
  QToolButton*     button_   { nullptr };
  QMenu*           menu_     { nullptr };
  CQIconCombo*     combo_    { nullptr };
  Actions          actions_;
};

//---

/*!
 * \brief line dash edit action
 * \ingroup Charts
 */
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

/*!
 * \brief type for CQChartsLineDash
 * \ingroup Charts
 */
class CQChartsLineDashPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsLineDashPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "line_dash"; }
};

//------

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsLineDash
 * \ingroup Charts
 */
class CQChartsLineDashPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsLineDashPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
