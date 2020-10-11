#ifndef CQChartsNoDataObj_H
#define CQChartsNoDataObj_H

#include <CQChartsPlotObj.h>
#include <CQChartsFont.h>

/*!
 * \brief no data object
 * \ingroup Charts
 */
class CQChartsNoDataObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsFont font READ font WRITE setFont)

 public:
  using Font = CQChartsFont;

 public:
  CQChartsNoDataObj(Plot *plot);

  QString typeName() const override { return "no_data"; }

  QString calcId() const override { return typeName(); }

  const Font &font() const { return font_; }
  void setFont(const Font &f) { font_ = f; }

  void draw(PaintDevice *device) const override;

 private:
  Font font_; //!< label font
};

#endif
