#ifndef CQChartsObj_H
#define CQChartsObj_H

#include <CQChartsGeom.h>
#include <CQChartsUtil.h>
#include <QObject>
#include <future>
#include <boost/optional.hpp>

class CQCharts;

/*!
 * \brief Charts object base class
 * \ingroup Charts
 */
class CQChartsObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString id       READ id         WRITE setId      )
  Q_PROPERTY(QRectF  rect     READ qrect      WRITE setQRect   )
  Q_PROPERTY(QString tipId    READ tipId      WRITE setTipId   )
  Q_PROPERTY(bool    selected READ isSelected WRITE setSelected)
  Q_PROPERTY(bool    inside   READ isInside   WRITE setInside  )

 public:
  CQChartsObj(CQCharts *charts, const CQChartsGeom::BBox &rect=CQChartsGeom::BBox());

  //---

  CQCharts *charts() const { return charts_; }

  //---

  // unique id of object
  bool hasId() const { return !!id_; }

  const QString &id() const;
  void setId(const QString &s);

  // calculate unique id of object (on demand)
  virtual QString calcId() const { return ""; }

  //---

  //! get set rect
  const CQChartsGeom::BBox &rect() const { return rect_; }
  virtual void setRect(const CQChartsGeom::BBox &r) { rect_ = r; }

  //---

  //! get set qrect (for property)
  QRectF qrect() const { return rect().qrect(); }
  void setQRect(const QRectF &r) { setRect(CQChartsGeom::BBox(r)); }

  //---

  // tip id for object (string to display in tooltip)
  bool hasTipId() const { return !!tipId_; }

  const QString &tipId() const;
  void setTipId(const QString &s) { tipId_ = s; }
  void resetTipId() { tipId_ = OptString(); }

  // calculate tip id (on demand)
  virtual QString calcTipId() const { return calcId(); }

  //---

  //! set/get selected
  bool isSelected() const { return selected_; }
  virtual void setSelected(bool b) { selected_ = b; }

  //---

  //! set/get inside
  bool isInside() const { return inside_; }
  virtual void setInside(bool b) { inside_ = b; }

  //---

 signals:
  //! emitted when object id changed
  void idChanged();

 protected:
  using OptString = boost::optional<QString>;

  CQCharts*          charts_   { nullptr }; //!< charts
  OptString          id_;                   //!< id
  CQChartsGeom::BBox rect_;                 //!< bbox
  OptString          tipId_;                //!< tip id
  bool               selected_ { false };   //!< is selected
  bool               inside_   { false };   //!< is mouse inside
  mutable std::mutex mutex_;                //!< mutex
};

#endif
