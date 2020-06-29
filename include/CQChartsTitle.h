#ifndef CQChartsTitle_H
#define CQChartsTitle_H

#include <CQChartsTextBoxObj.h>
#include <CQChartsTitleLocation.h>
#include <CQChartsPosition.h>
#include <CQChartsRect.h>
#include <CQChartsGeom.h>
#include <vector>

class CQChartsPlot;
class CQPropertyViewModel;
class CQChartsPaintDevice;
class QPainter;

/*!
 * \brief Title Object
 * \ingroup Charts
 */
class CQChartsTitle : public CQChartsTextBoxObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsTitleLocation location READ location WRITE setLocation)

  Q_PROPERTY(CQChartsPosition absolutePosition  READ absolutePosition  WRITE setAbsolutePosition)
  Q_PROPERTY(CQChartsRect     absoluteRectangle READ absoluteRectangle WRITE setAbsoluteRectangle)

  Q_PROPERTY(bool insidePlot  READ isInsidePlot  WRITE setInsidePlot )
  Q_PROPERTY(bool expandWidth READ isExpandWidth WRITE setExpandWidth)

  Q_PROPERTY(bool fitHorizontal READ isFitHorizontal WRITE setFitHorizontal)
  Q_PROPERTY(bool fitVertical   READ isFitVertical   WRITE setFitVertical  )

 public:
  CQChartsTitle(CQChartsPlot *plot);
 ~CQChartsTitle();

  QString calcId() const override;

  //---

  void setVisible(bool b) override { CQChartsTextBoxObj::setVisible(b); redraw(); }

  void setSelected(bool b) override;

  //---

  const CQChartsTitleLocation &location() const { return location_; }
  void setLocation(const CQChartsTitleLocation &l);

  const CQChartsPosition &absolutePosition() const { return absolutePosition_; }
  void setAbsolutePosition(const CQChartsPosition &p);

  const CQChartsRect &absoluteRectangle() const { return absoluteRectangle_; }
  void setAbsoluteRectangle(const CQChartsRect &r);

  bool isInsidePlot() const { return insidePlot_; }
  void setInsidePlot(bool b);

  bool isExpandWidth() const { return expandWidth_; }
  void setExpandWidth(bool b);

  bool isFitHorizontal() const { return fitData_.horizontal; }
  void setFitHorizontal(bool b) { fitData_.horizontal = b; }

  bool isFitVertical() const { return fitData_.vertical; }
  void setFitVertical(bool b) { fitData_.vertical = b; }

  //---

  const Point &position() const { return position_; }
  void setPosition(const Point &p) { position_ = p; }

  //---

  QString locationStr() const;
  void setLocationStr(const QString &s);

  //---

  Point absolutePlotPosition() const;
  void setAbsolutePlotPosition(const Point &p);

  BBox absolutePlotRectangle() const;
  void setAbsolutePlotRectangle(const BBox &r);

  //---

  const BBox &bbox() const { return bbox_; }
  void setBBox(const BBox &b) { bbox_ = b; }

  //---

  void boxDataInvalidate() override { redraw(); }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path,
                     const QString &desc="") override;

  //---

  Size calcSize();

  BBox fitBBox() const;

  //---

  void redraw(bool wait=true);

  //---

  bool contains(const Point &p) const override;

  //---

  virtual bool selectPress  (const Point &, CQChartsSelMod) { return false; }
  virtual bool selectMove   (const Point &) { return false; }
  virtual bool selectRelease(const Point &) { return false; }

  virtual bool editPress  (const Point &);
  virtual bool editMove   (const Point &);
  virtual bool editMotion (const Point &);
  virtual bool editRelease(const Point &) { return true; }

  virtual void editMoveBy(const Point &d);

  //---

  bool isDrawn() const;

  void draw(CQChartsPaintDevice *device);

  void drawEditHandles(QPainter *painter) const override;

 private:
  void updateLocation();

  void textBoxDataInvalidate() override;

 private:
  struct FitData {
    bool horizontal { true };
    bool vertical   { true };
  };

  CQChartsTitleLocation location_;                      //!< location type
  CQChartsPosition      absolutePosition_;              //!< position (relative to plot box)
  CQChartsRect          absoluteRectangle_;             //!< rect (relative to plot box)
  bool                  insidePlot_        { false };   //!< is placed inside plot
  bool                  expandWidth_       { false };   //!< is width expanded to plot
  Point                 position_          { 0, 0 };    //!< position
  Size                  size_;                          //!< size
  mutable BBox          bbox_;                          //!< bbox
  FitData               fitData_;                       //!< fit data
};

#endif
