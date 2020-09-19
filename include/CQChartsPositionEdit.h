#ifndef CQChartsPositionEdit_H
#define CQChartsPositionEdit_H

#include <CQChartsPosition.h>
#include <QFrame>

class CQChartsPlot;
class CQChartsGeomPointEdit;
class CQChartsUnitsEdit;

/*!
 * \brief position edit
 * \ingroup Charts
 */
class CQChartsPositionEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition position READ position WRITE setPosition)

 public:
  using Plot     = CQChartsPlot;
  using Position = CQChartsPosition;
  using Point    = CQChartsGeom::Point;

 public:
  CQChartsPositionEdit(QWidget *parent=nullptr);

  const Position &position() const { return position_; }
  void setPosition(const Position &pos);

  const Plot *plot() const { return plot_; }
  void setPlot(Plot *plot);

  void setRegion(const Point &p);

 signals:
  void positionChanged();

  void regionChanged();

 private slots:
  void editChanged();
  void unitsChanged();

  void editRegionSlot();

 private:
  void positionToWidgets();
//void widgetsToPosition();

  void connectSlots(bool b);

 private:
  Position               position_;              //!< position data
  Plot*                  plot_      { nullptr }; //!< associated plot
  CQChartsGeomPointEdit* edit_      { nullptr }; //!< position edit
  CQChartsUnitsEdit*     unitsEdit_ { nullptr }; //!< units edit
  bool                   connected_ { false };   //!< is connected
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsPosition
 * \ingroup Charts
 */
class CQChartsPositionPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsPositionPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "position"; }

 private:
  QString valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const;
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsPosition
 * \ingroup Charts
 */
class CQChartsPositionPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsPositionPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
