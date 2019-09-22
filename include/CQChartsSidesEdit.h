#ifndef CQChartsSidesEdit_H
#define CQChartsSidesEdit_H

#include <CQChartsSides.h>
#include <QFrame>

class QStyleOptionComboBox;

/*!
 * \brief rectangle sides edit
 * \ingroup Charts
 */
class CQChartsSidesEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsSides sides READ sides WRITE setSides)

 public:
  CQChartsSidesEdit(QWidget *parent=nullptr);
 ~CQChartsSidesEdit();

  const CQChartsSides &sides() const;
  void setSides(const CQChartsSides &side);

  QSize sizeHint() const;

 signals:
  void sidesChanged();

 private:
  void paintEvent(QPaintEvent *);

  void mousePressEvent(QMouseEvent *);

  void initStyleOption(QStyleOptionComboBox &opt) const;

 private:
  CQChartsSides sides_;
};

//------

/*!
 * \brief rectangle sides menu widget
 * \ingroup Charts
 */
class CQChartsSidesEditMenuWidget : public QFrame {
  Q_OBJECT

 public:
  CQChartsSidesEditMenuWidget(CQChartsSidesEdit *edit);

  QSize sizeHint() const;

 private:
  void resizeEvent(QResizeEvent *);

  void paintEvent(QPaintEvent *);

  void mouseMoveEvent(QMouseEvent *);

  void mousePressEvent(QMouseEvent *);

  void drawSideRect(QPainter *p, CQChartsSides::Side side, bool on);

 private:
  struct Rect {
    QRect r;
    bool  inside { false };

    Rect(const QRect &r=QRect()) :
     r(r) {
    }
  };

  using SideRect = std::map<CQChartsSides::Side,Rect>;

  CQChartsSidesEdit* edit_ { nullptr };
  SideRect           sideRect_;
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsSides
 * \ingroup Charts
 */
class CQChartsSidesPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsSidesPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "sides"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsSides
 * \ingroup Charts
 */
class CQChartsSidesPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsSidesPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
