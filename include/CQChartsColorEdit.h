#ifndef CQChartsColorEdit_H
#define CQChartsColorEdit_H

#include <CQChartsColor.h>
#include <QFrame>
#include <QStyleOptionComboBox>

class CQWidgetMenu;
class CQRealSpin;
class CQColorChooser;
class CQCheckBox;

class QLineEdit;
class QPushButton;
class QComboBox;
class QSpinBox;

class CQChartsColorEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsColor color           READ color           WRITE setColor          )
  Q_PROPERTY(QString       placeholderText READ placeholderText WRITE setPlaceholderText)

 public:
  CQChartsColorEdit(QWidget *parent=nullptr);

  const CQChartsColor &color() const;
  void setColor(const CQChartsColor &c);

  QString placeholderText() const;
  void setPlaceholderText(const QString &s);

  void paintEvent(QPaintEvent *) override;

  void resizeEvent(QResizeEvent *) override;

 signals:
  void colorChanged();

 private slots:
  void showMenu();

  void updateMenu();

  void textChanged(const QString &str);

  void widgetsToColor();

  void updateState();

 private:
  void initStyle(QStyleOptionComboBox &opt);

  void colorToWidgets(bool updateText);

  void connectSlots(bool b);

 private:
  CQChartsColor   color_;
  QLineEdit*      edit_       { nullptr };
  QPushButton*    button_     { nullptr };
  CQWidgetMenu*   menu_       { nullptr };
  QComboBox*      typeCombo_  { nullptr };
  QSpinBox*       indEdit_    { nullptr };
  CQRealSpin*     valueEdit_  { nullptr };
  CQColorChooser* colorEdit_  { nullptr };
  CQCheckBox*     scaleCheck_ { nullptr };
};

//---

#include <QPushButton>

class CQChartsColorEditMenuButton : public QPushButton {
  Q_OBJECT

 public:
  CQChartsColorEditMenuButton(QWidget *parent=nullptr);

  void paintEvent(QPaintEvent *) override;
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsColor
class CQChartsColorPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsColorPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsColor
class CQChartsColorPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsColorPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
