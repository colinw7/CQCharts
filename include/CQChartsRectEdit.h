#ifndef CQChartsRectEdit_H
#define CQChartsRectEdit_H

#include <CQChartsRect.h>
#include <QFrame>

class CQChartsView;
class CQChartsPlot;
class CQChartsGeomBBoxEdit;
class CQChartsUnitsEdit;

/*!
 * \brief rectangle edit
 * \ingroup Charts
 */
class CQChartsRectEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsRect rect READ rect WRITE setRect)

 public:
  using View = CQChartsView;
  using Plot = CQChartsPlot;
  using Rect = CQChartsRect;
  using BBox = CQChartsGeom::BBox;

 public:
  CQChartsRectEdit(QWidget *parent=nullptr);

  const Rect &rect() const { return rect_; }
  void setRect(const Rect &pos);

  const View *view() const { return view_; }
  void setView(View *view);

  const Plot *plot() const { return plot_; }
  void setPlot(Plot *plot);

  void setRegion(const BBox &bbox);

 Q_SIGNALS:
  void rectChanged();

  void regionChanged();

 private Q_SLOTS:
  void editChanged();
  void unitsChanged();

  void editRegionSlot();

 private:
  void rectToWidgets();
//void widgetsToRect();

  void connectSlots(bool b);

 private:
  Rect                  rect_;                  //!< rect data
  View*                 view_      { nullptr }; //!< associated view
  Plot*                 plot_      { nullptr }; //!< associated plot
  CQChartsGeomBBoxEdit* edit_      { nullptr }; //!< rect edit
  CQChartsUnitsEdit*    unitsEdit_ { nullptr }; //!< units edit
  bool                  connected_ { false };   //!< is connected
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsRect
 * \ingroup Charts
 */
class CQChartsRectPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsRectPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "rectangle"; }

 private:
  QString valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const;
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsRect
 * \ingroup Charts
 */
class CQChartsRectPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsRectPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
