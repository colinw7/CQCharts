#ifndef CQChartsLineEditBase_H
#define CQChartsLineEditBase_H

#include <QFrame>

class CQChartsEditBase;
class CQChartsPlot;
class CQChartsView;
class CQChartsColor;

class CQPropertyViewTree;
class CQWidgetMenu;

class QHBoxLayout;
class QLineEdit;
class QPushButton;
class QStyleOptionComboBox;

class CQChartsLineEditBase : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString text            READ text            WRITE setText           )
  Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText)
  Q_PROPERTY(bool    editable        READ isEditable      WRITE setEditable       )

 public:
  CQChartsLineEditBase(QWidget *parent=nullptr);

  QString text() const;
  void setText(const QString &s);

  QString placeholderText() const;
  void setPlaceholderText(const QString &s);

  bool isEditable() const { return editable_; }
  void setEditable(bool b);

  const CQPropertyViewTree *propertyViewTree() const { return propertyViewTree_; }
  void setPropertyViewTree(CQPropertyViewTree *tree) { propertyViewTree_ = tree; }

  CQChartsPlot *plot() const;
  virtual void setPlot(CQChartsPlot *plot);

  CQChartsView *view() const;
  virtual void setView(CQChartsView *view);

  CQChartsEditBase *menuEdit() const { return menuEdit_; }

  void paintEvent(QPaintEvent *) override;

  void resizeEvent(QResizeEvent *) override;

  virtual void drawPreview(QPainter *painter, const QRect &rect);

  void drawBackground(QPainter *painter) const;

  void drawCenteredText(QPainter *painter, const QString &text) const;

  QColor interpColor(const CQChartsColor &color) const;

 protected slots:
  void menuButtonSlot();

  void showMenuSlot();
  void hideMenuSlot();

  void textChangedSlot();

 protected:
  friend class CQChartsLineEditMenuButton;

  virtual void textChanged() = 0;

  virtual void updateMenu();

  virtual void connectSlots(bool b);

  void initStyle(QStyleOptionComboBox &opt);

 protected:
  bool                editable_         { true };
  CQPropertyViewTree* propertyViewTree_ { nullptr };
  CQChartsEditBase*   menuEdit_         { nullptr };
  QHBoxLayout*        layout_           { nullptr };
  QLineEdit*          edit_             { nullptr };
  QPushButton*        button_           { nullptr };
  CQWidgetMenu*       menu_             { nullptr };
  mutable QColor      tc_;
};

//---

#include <QLineEdit>

class CQChartsLineEditEdit : public QLineEdit {
  Q_OBJECT

 public:
  CQChartsLineEditEdit(CQChartsLineEditBase *edit);

  void paintEvent(QPaintEvent *) override;

 private:
  CQChartsLineEditBase *edit_ { nullptr };
};

//---

#include <QPushButton>

class CQChartsLineEditMenuButton : public QPushButton {
  Q_OBJECT

 public:
  CQChartsLineEditMenuButton(CQChartsLineEditBase *edit);

  void paintEvent(QPaintEvent *) override;

 private:
  CQChartsLineEditBase *edit_ { nullptr };
};

#endif
