#ifndef CQChartsGeomPointEdit_H
#define CQChartsGeomPointEdit_H

#include <CQChartsGeom.h>

class CQChartsView;
class CQChartsPlot;
class CQChartsLineEdit;
class CQChartsRegionButton;

/*!
 * \brief geometry point edit
 * \ingroup Charts
 */
class CQChartsGeomPointEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsGeom::Point point READ getValue WRITE setValue)

 public:
  using View  = CQChartsView;
  using Plot  = CQChartsPlot;
  using Point = CQChartsGeom::Point;

 public:
  CQChartsGeomPointEdit(QWidget *parent, const Point &value=Point(0, 0));
  CQChartsGeomPointEdit(const Point &value=Point(0, 0));

 ~CQChartsGeomPointEdit() { }

  const Point &getValue() const { return point_; }
  void setValue(const Point &point);

  const View *view() const { return view_; }
  void setView(View *view);

  const Plot *plot() const { return plot_; }
  void setPlot(Plot *p);

  void setRegion(const Point &p);

 private:
  void init(const Point &value);

 private Q_SLOTS:
  void editingFinishedI();

  void regionSlot(const CQChartsGeom::Point &p);

 Q_SIGNALS:
  void valueChanged();

  void regionChanged();

 private:
  void updateRange();

  void pointToWidget();
  bool widgetToPoint();

 private:
  Point                 point_          { 0, 0 };
  View*                 view_           { nullptr };
  Plot*                 plot_           { nullptr };
  CQChartsLineEdit*     edit_           { nullptr };
  CQChartsRegionButton* regionButton_   { nullptr };
  mutable bool          disableSignals_ { false };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsGeomPoint
 * \ingroup Charts
 */
class CQChartsGeomPointPropertyViewType : public CQPropertyViewType {
 public:
  using PropertyItem = CQPropertyViewItem;
  using Delegate     = CQPropertyViewDelegate;
  using StyleOption  = QStyleOptionViewItem;

 public:
  CQChartsGeomPointPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(PropertyItem *item, const QVariant &value) override;

  void draw(PropertyItem *item, const Delegate *delegate, QPainter *painter,
            const StyleOption &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "geom_point"; }

 private:
  QString valueString(PropertyItem *item, const QVariant &value, bool &ok) const;
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
