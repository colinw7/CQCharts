#ifndef CQChartsLineDashEdit_H
#define CQChartsLineDashEdit_H

#include <CQChartsLineDash.h>
#include <QFrame>
#include <QAction>
#include <map>

class CQChartsLineDashEditAction;
class CQChartsLineEdit;
class CQPropertyViewTree;
class QMenu;
class QToolButton;
class CQIconCombo;

/*!
 * \brief line dash edit
 * \ingroup Charts
 */
class CQChartsLineDashEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool             editable READ editable    WRITE setEditable)
  Q_PROPERTY(CQChartsLineDash lineDash READ getLineDash WRITE setLineDash)

 public:
  using LineDash = CQChartsLineDash;

 public:
  CQChartsLineDashEdit(QWidget *parent=nullptr);
 ~CQChartsLineDashEdit();

  bool editable() const { return editable_; }
  void setEditable(bool b);

  const LineDash &getLineDash() const { return dash_; }
  void setLineDash(const LineDash &dash);

  void addDashOption(const std::string &id, const LineDash &dash);

  static QIcon dashIcon(const LineDash &dash, bool bg=true);

 private:
  void updateState();

 private Q_SLOTS:
  void dashChangedSlot();
  void menuItemActivated(QAction *);
  void comboItemChanged();

 Q_SIGNALS:
  void valueChanged(const CQChartsLineDash &dash);

 private:
  using Actions = std::map<std::string, CQChartsLineDashEditAction *>;

  bool              editable_ { false };
  LineDash          dash_;
  CQChartsLineEdit* edit_     { nullptr };
  QToolButton*      button_   { nullptr };
  QMenu*            menu_     { nullptr };
  CQIconCombo*      combo_    { nullptr };
  Actions           actions_;
};

//------

#include <CQSwitchLineEdit.h>

class CQChartsSwitchLineDashEdit : public CQSwitchLineEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsLineDash lineDash READ getLineDash WRITE setLineDash)

 public:
  using LineDash = CQChartsLineDash;

 public:
  CQChartsSwitchLineDashEdit(QWidget *parent=nullptr);

  CQChartsLineDashEdit *edit() const { return edit_; }

  LineDash getLineDash() const;
  void setLineDash(const LineDash &l);

  void setPropertyView(CQPropertyViewTree *pv);

  void updatePlacement() override;

 private:
  void connectSlots(bool b);

 Q_SIGNALS:
  void valueChanged(const CQChartsLineDash &dash);
  void altEditingFinished();

 private Q_SLOTS:
  void editSwitched(bool);

  void valueChangedSlot(const CQChartsLineDash &dash);
  void textChangedSlot();

 private:
  static bool s_isAlt;

  CQChartsLineDashEdit *edit_ { nullptr };
  CQPropertyViewTree   *pv_   { nullptr };
};

//------

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

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsLineDash
 * \ingroup Charts
 */
class CQChartsLineDashPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsLineDashPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(ViewItem *item, const QVariant &value) override;

  void draw(ViewItem *item, const ViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

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

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
