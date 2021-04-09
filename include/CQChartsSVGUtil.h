#ifndef CQChartsSVGUtil_H
#define CQChartsSVGUtil_H

#include <CQChartsPath.h>
#include <CQChartsStyle.h>

/*!
 * \brief SVG Utilities
 * \ingroup Charts
 */
namespace CQChartsSVGUtil {
  using Paths  = std::vector<CQChartsPath>;
  using Styles = std::vector<CQChartsStyle>;
  using BBox   = CQChartsGeom::BBox;

  bool svgFileToPaths(const QString &filename, Paths &paths, Styles &styles, BBox &bbox);

  bool stringToPath(const QString &str, QPainterPath &path);
  QString pathToString(const QPainterPath &path);

  bool stringToPenBrush(const QString &str, QPen &pen, QBrush &brush);
  QString penBrushToString(QPen &pen, QBrush &brush);
}

#endif
