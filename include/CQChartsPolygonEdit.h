#ifndef CQChartsPolygonEdit_H
#define CQChartsPolygonEdit_H

#include <CQChartsPolygon.h>
#include <QFrame>
#include <QStyleOptionComboBox>

class CQPoint2DEdit;
class CQWidgetMenu;
class QLineEdit;
class QComboBox;
class QPushButton;
class QScrollArea;

class CQChartsPolygonEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsPolygon polygon         READ polygon         WRITE setPolygon        )
  Q_PROPERTY(QString         placeholderText READ placeholderText WRITE setPlaceholderText)

 public:
  CQChartsPolygonEdit(QWidget *parent=nullptr);

  const CQChartsPolygon &polygon() const;
  void setPolygon(const CQChartsPolygon &pos);

  QString placeholderText() const;
  void setPlaceholderText(const QString &s);

  void paintEvent(QPaintEvent *) override;

  void resizeEvent(QResizeEvent *) override;

 signals:
  void polygonChanged();

 private slots:
  void showMenu();

  void updateMenu();

  void textChanged();

  void unitsChanged();

  void addSlot();
  void removeSlot();

  void updateState();

  void pointSlot();

 private:
  void initStyle(QStyleOptionComboBox &opt);

  void widgetsToPolygon();
  void polygonToWidgets();

  void connectSlots(bool b);

 private:
  using PointEdits = std::vector<CQPoint2DEdit *>;

  CQChartsPolygon polygon_;
  QLineEdit*      edit_         { nullptr };
  QPushButton*    button_       { nullptr };
  CQWidgetMenu*   menu_         { nullptr };
  QFrame*         menuFrame_    { nullptr };
  QComboBox*      unitsCombo_   { nullptr };
  QFrame*         controlFrame_ { nullptr };
  QScrollArea*    scrollArea_   { nullptr };
  QFrame*         pointsFrame_  { nullptr };
  PointEdits      pointEdits_;
};

//---

#include <QPushButton>

class CQChartsPolygonEditMenuButton : public QPushButton {
  Q_OBJECT

 public:
  CQChartsPolygonEditMenuButton(QWidget *parent=nullptr);

  void paintEvent(QPaintEvent *) override;
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsPolygon
class CQChartsPolygonPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsPolygonPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsPolygon
class CQChartsPolygonPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsPolygonPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
