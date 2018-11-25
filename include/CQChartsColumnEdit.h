#ifndef CQChartsColumnEdit_H
#define CQChartsColumnEdit_H

#include <CQChartsColumn.h>
#include <QFrame>
#include <QStyleOptionComboBox>

class CQWidgetMenu;
class QLineEdit;
class QPushButton;
class QGroupBox;
class QComboBox;
class QCheckBox;
class QAbstractItemModel;

class CQChartsColumnEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn column          READ column          WRITE setColumn         )
  Q_PROPERTY(QString        placeholderText READ placeholderText WRITE setPlaceholderText)

 public:
  CQChartsColumnEdit(QWidget *parent=nullptr);

  QAbstractItemModel *model() const { return model_; }
  void setModel(QAbstractItemModel *model);

  const CQChartsColumn &column() const;
  void setColumn(const CQChartsColumn &c);

  QString placeholderText() const;
  void setPlaceholderText(const QString &s);

  void paintEvent(QPaintEvent *) override;

  void resizeEvent(QResizeEvent *) override;

 signals:
  void columnChanged();

 private slots:
  void showMenu();

  void updateMenu();

  void textChanged(const QString &str);

  void menuColumnGroupClicked(bool b);
  void menuExprGroupClicked  (bool b);
  void vheaderCheckClicked   (bool b);

  void menuColumnChanged(int i);

  void expressionTextChanged(const QString &str);

  void updateState();

 private:
  void initStyle(QStyleOptionComboBox &opt);

  void connectSlots(bool b);

  void columnToWidgets();
  void widgetsToColumn();

 private:
  CQChartsColumn      column_;
  QLineEdit*          edit_            { nullptr };
  QPushButton*        button_          { nullptr };
  CQWidgetMenu*       menu_            { nullptr };
  QGroupBox*          menuColumnGroup_ { nullptr };
  QComboBox*          columnCombo_     { nullptr };
  QGroupBox*          menuExprGroup_   { nullptr };
  QLineEdit*          expressionEdit_  { nullptr };
  QCheckBox*          vheaderCheck_    { nullptr };
  QAbstractItemModel* model_           { nullptr };
};

//---

#include <QPushButton>

class CQChartsColumnEditMenuButton : public QPushButton {
  Q_OBJECT

 public:
  CQChartsColumnEditMenuButton(QWidget *parent=nullptr);

  void paintEvent(QPaintEvent *) override;
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsColumn
class CQChartsColumnPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsColumnPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsColumn
class CQChartsColumnPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsColumnPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
