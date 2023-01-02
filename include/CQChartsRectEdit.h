#ifndef CQChartsRectEdit_H
#define CQChartsRectEdit_H

#include <CQChartsRect.h>

#include <QFrame>
#include <QPointer>

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

  View *view() const;
  void setView(View *view);

  Plot *plot() const;
  void setPlot(Plot *plot);

  const Rect &rect() const { return rect_; }
  void setRect(const Rect &pos);

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
  using ViewP = QPointer<View>;
  using PlotP = QPointer<Plot>;

  ViewP                 view_;                  //!< associated view
  PlotP                 plot_;                  //!< associated plot
  Rect                  rect_;                  //!< rect data
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

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
