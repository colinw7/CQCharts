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
  using TitleLocation = CQChartsTitleLocation;
  using TextBoxObj    = CQChartsTextBoxObj;
  using Position      = CQChartsPosition;
  using Rect          = CQChartsRect;
  using ColorInd      = CQChartsUtil::ColorInd;

 public:
  CQChartsTitle(Plot *plot);
 ~CQChartsTitle();

  QString calcId() const override;

  //---

  void setVisible(bool b) override { TextBoxObj::setVisible(b); redraw(); }

  void setSelected(bool b) override;

  //---

  const TitleLocation &location() const { return location_; }
  void setLocation(const TitleLocation &l);

  const Position &absolutePosition() const { return absolutePosition_; }
  void setAbsolutePosition(const Position &p);

  const Rect &absoluteRectangle() const { return absoluteRectangle_; }
  void setAbsoluteRectangle(const Rect &r);

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

  TextBoxObj *subTitle() const { return subTitle_; }

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

  // Implement edit interface

  bool editPress (const Point &) override;
  bool editMove  (const Point &) override;
  bool editMotion(const Point &) override;

  void editMoveBy(const Point &d) override;

  //---

  bool isDrawn() const;

  void draw(PaintDevice *device);

  //---

  void setEditHandlesBBox() const override;

 private:
  void init();

  void updateLocation();

  void textBoxDataInvalidate() override;

 private:
  struct FitData {
    bool horizontal { true };
    bool vertical   { true };
  };

  TitleLocation location_;                      //!< location type
  Position      absolutePosition_;              //!< position (relative to plot box)
  Rect          absoluteRectangle_;             //!< rect (relative to plot box)
  bool          insidePlot_        { false };   //!< is placed inside plot
  bool          expandWidth_       { false };   //!< is width expanded to plot
  Point         position_          { 0, 0 };    //!< position
  TextBoxObj*   subTitle_          { nullptr }; //!< subtitle text box
  Size          textSize_;                      //!< main size (without padding, border)
  Size          subTitleTextSize_;              //!< subtitle size (without padding, border)
  Size          allTextSize_;                   //!< combined size (without padding, border)
  Size          size_;                          //!< size (width padding, border)
  FitData       fitData_;                       //!< fit data
};

#endif
