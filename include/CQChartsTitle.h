#ifndef CQChartsTitle_H
#define CQChartsTitle_H

#include <CQChartsTextBoxObj.h>
#include <CQChartsTitleLocation.h>
#include <CQChartsPosition.h>
#include <CQChartsRect.h>
#include <CQChartsGeom.h>
#include <vector>

class CQChartsPlot;
class CQChartsEditHandles;
class CQPropertyViewModel;
class CQChartsPaintDevice;
class QPainter;

/*!
 * \brief Title Object
 * \ingroup Charts
 */
class CQChartsTitle : public CQChartsTextBoxObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsTitleLocation location          READ location     WRITE setLocation  )
  Q_PROPERTY(CQChartsPosition      absolutePosition  READ absolutePosition
                                                     WRITE setAbsolutePosition)
  Q_PROPERTY(CQChartsRect          absoluteRectangle READ absoluteRectangle
                                                     WRITE setAbsoluteRectangle)
  Q_PROPERTY(bool                  insidePlot        READ isInsidePlot WRITE setInsidePlot)

 public:
  CQChartsTitle(CQChartsPlot *plot);
 ~CQChartsTitle();

  QString calcId() const override;

  void setSelected(bool b) override;

  //---

  const CQChartsTitleLocation &location() const { return location_; }
  void setLocation(const CQChartsTitleLocation &l) { location_ = l; redraw(); }

  const CQChartsPosition &absolutePosition() const { return absolutePosition_; }
  void setAbsolutePosition(const CQChartsPosition &p) { absolutePosition_ = p; redraw(); }

  const CQChartsRect &absoluteRectangle() const { return absoluteRectangle_; }
  void setAbsoluteRectangle(const CQChartsRect &r) { absoluteRectangle_ = r; redraw(); }

  bool isInsidePlot() const { return insidePlot_; }
  void setInsidePlot(bool b) { insidePlot_ = b; }

  //---

  const CQChartsGeom::Point &position() const { return position_; }
  void setPosition(const CQChartsGeom::Point &p) { position_ = p; }

  CQChartsEditHandles *editHandles() { return editHandles_; }

  //---

  QString locationStr() const;
  void setLocationStr(const QString &s);

  //---

  CQChartsGeom::Point absolutePlotPosition() const;
  void setAbsolutePlotPosition(const CQChartsGeom::Point &p);

  CQChartsGeom::BBox absolutePlotRectangle() const;
  void setAbsolutePlotRectangle(const CQChartsGeom::BBox &r);

  //---

  const CQChartsGeom::BBox &bbox() const { return bbox_; }
  void setBBox(const CQChartsGeom::BBox &b) { bbox_ = b; }

  //---

  void boxDataInvalidate() override { redraw(); }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path,
                     const QString &desc="") override;

  //---

  CQChartsGeom::Size calcSize();

  //---

  void redraw(bool wait=true);

  //---

  bool contains(const CQChartsGeom::Point &p) const override;

  //---

  virtual bool selectPress  (const CQChartsGeom::Point &, CQChartsSelMod) { return false; }
  virtual bool selectMove   (const CQChartsGeom::Point &) { return false; }
  virtual bool selectRelease(const CQChartsGeom::Point &) { return false; }

  virtual bool editPress  (const CQChartsGeom::Point &);
  virtual bool editMove   (const CQChartsGeom::Point &);
  virtual bool editMotion (const CQChartsGeom::Point &);
  virtual bool editRelease(const CQChartsGeom::Point &);

  virtual void editMoveBy(const CQChartsGeom::Point &d);

  //---

  bool isDrawn() const;

  void draw(CQChartsPaintDevice *device);

  void drawEditHandles(QPainter *painter) const;

 private:
  void updateLocation();

 private:
  CQChartsTitleLocation      location_;                      //!< location type
  CQChartsPosition           absolutePosition_;              //!< position (relative to plot box)
  CQChartsRect               absoluteRectangle_;             //!< rect (relative to plot box)
  bool                       insidePlot_        { false };   //!< is placed inside plot
  CQChartsGeom::Point        position_          { 0, 0 };    //!< position
  CQChartsGeom::Size         size_;                          //!< size
  mutable CQChartsGeom::BBox bbox_;                          //!< bbox
  CQChartsEditHandles*       editHandles_       { nullptr }; //!< edit handles
};

#endif
