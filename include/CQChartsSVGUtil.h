#ifndef CQChartsSVGUtil_H
#define CQChartsSVGUtil_H

#include <CQChartsPath.h>
#include <CQChartsStyle.h>
#include <CQChartsSVGStyleData.h>

/*!
 * \brief SVG Utilities
 * \ingroup Charts
 */
namespace CQChartsSVGUtil {
  using Paths     = std::vector<CQChartsPath>;
  using Styles    = std::vector<CQChartsStyle>;
  using BBox      = CQChartsGeom::BBox;
  using StyleData = CQChartsSVGStyleData;

  bool svgFileToPaths(const QString &filename, Paths &paths, Styles &styles, BBox &bbox);

  bool stringToPath(const QString &str, QPainterPath &path);
  QString pathToString(const QPainterPath &path);

  bool stringToPenBrush(const QString &str, QPen &pen, QBrush &brush);

  bool stringToStyle(CQChartsPlot *plot, const QString &str, StyleData &data);

  QString penBrushToString(QPen &pen, QBrush &brush);
}

#endif
