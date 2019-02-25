#ifndef CQChartsFillUnderPosEdit_H
#define CQChartsFillUnderPosEdit_H

#include <CQChartsFillUnderPos.h>
#include <QFrame>
#include <QStyleOptionComboBox>

class CQWidgetMenu;
class CQRealSpin;
class CQFillUnderPosChooser;
class CQCheckBox;

class QLineEdit;
class QPushButton;
class QComboBox;
class QSpinBox;

class CQChartsFillUnderPosEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsFillUnderPos fillUnderPos    READ fillUnderPos    WRITE setFillUnderPos   )
  Q_PROPERTY(QString              placeholderText READ placeholderText WRITE setPlaceholderText)

 public:
  CQChartsFillUnderPosEdit(QWidget *parent=nullptr);

  const CQChartsFillUnderPos &fillUnderPos() const;
  void setFillUnderPos(const CQChartsFillUnderPos &fillUnderPos);

  QString placeholderText() const;
  void setPlaceholderText(const QString &s);

  void paintEvent(QPaintEvent *) override;

  void resizeEvent(QResizeEvent *) override;

 signals:
  void fillUnderPosChanged();

 private slots:
  void showMenu();

  void updateMenu();

  void textChanged(const QString &str);

  void widgetsToFillUnderPos();

  void updateState();

 private:
  void initStyle(QStyleOptionComboBox &opt);

  void fillUnderPosToWidgets(bool updateText);

  void connectSlots(bool b);

 private:
  CQChartsFillUnderPos fillUnderPos_;
  QComboBox*           xtypeCombo_ { nullptr };
  CQRealSpin*          xposEdit_   { nullptr };
  QComboBox*           ytypeCombo_ { nullptr };
  CQRealSpin*          yposEdit_   { nullptr };
};

//---

#include <QPushButton>

class CQChartsFillUnderPosEditMenuButton : public QPushButton {
  Q_OBJECT

 public:
  CQChartsFillUnderPosEditMenuButton(QWidget *parent=nullptr);

  void paintEvent(QPaintEvent *) override;
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsFillUnderPos
class CQChartsFillUnderPosPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsFillUnderPosPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsFillUnderPos
class CQChartsFillUnderPosPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsFillUnderPosPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
