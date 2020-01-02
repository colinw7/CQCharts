#ifndef CQChartsGeomPointEdit_H
#define CQChartsGeomPointEdit_H

#include <CQChartsGeom.h>

class CQChartsLineEdit;

/*!
 * \brief geometry point edit
 * \ingroup Charts
 */
class CQChartsGeomPointEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsGeom::Point point READ getValue WRITE setValue)

 public:
  using Point = CQChartsGeom::Point;

 public:
  CQChartsGeomPointEdit(QWidget *parent, const Point &value=Point(0,0));
  CQChartsGeomPointEdit(const Point &value=Point(0, 0));

 ~CQChartsGeomPointEdit() { }

  const Point &getValue() const;
  void setValue(const Point &point);

 private:
  void init(const Point &value);

 private slots:
  void editingFinishedI();

 signals:
  void valueChanged();

 private:
  void updateRange();

  void pointToWidget();
  bool widgetToPoint();

 private:
  Point             point_          { 0, 0 };
  CQChartsLineEdit *edit_           { nullptr };
  mutable bool      disableSignals_ { false };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsGeomPoint
 * \ingroup Charts
 */
class CQChartsGeomPointPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsGeomPointPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "geom_point"; }

 private:
  QString valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const;
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsGeomPoint
 * \ingroup Charts
 */
class CQChartsGeomPointPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsGeomPointPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
