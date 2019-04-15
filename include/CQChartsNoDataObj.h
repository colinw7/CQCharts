#ifndef CQChartsNoDataObj_H
#define CQChartsNoDataObj_H

#include <CQChartsPlotObj.h>
#include <CQChartsFont.h>

//! \brief no data object
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

  void addColumnSelectIndex(Indices &, const CQChartsColumn &) const override { }

  void draw(QPainter *) override;

 private:
  CQChartsFont font_;
};

#endif
