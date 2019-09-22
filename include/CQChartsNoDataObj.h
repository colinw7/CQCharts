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
  CQChartsNoDataObj(CQChartsPlot *plot);

  QString typeName() const override { return "no_data"; }

  QString calcId() const override { return typeName(); }

  const CQChartsFont &font() const { return font_; }
  void setFont(const CQChartsFont &f) { font_ = f; }

  void getSelectIndices(Indices &) const override { }

  void draw(CQChartsPaintDevice *) override;

 private:
  CQChartsFont font_; //!< label font
};

#endif
