#include <CQChartsContour.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsPlot.h>
#include <QPainter>

static int contourFlags[] = {
  0, 4, // bottom edge to diagonals
  0, 5,
  1, 6, // top edge to diagonals
  1, 7,
  2, 4, // left edge to diagonals
  2, 6,
  3, 5, // right edge to diagonals
  3, 7,

  4, 5, // diagonal to diagonal
  4, 6,
  5, 7,
  6, 7,
};

// 20 colors
static QColor contourColors[] = {
  QColor(0x31,0x82,0xBD),
  QColor(0x6B,0xAE,0xD6),
  QColor(0x9E,0xCA,0xE1),
  QColor(0xC6,0xDB,0xEF),
  QColor(0xE6,0x55,0x0D),
  QColor(0xFD,0x8D,0x3C),
  QColor(0xFD,0xAE,0x6B),
  QColor(0xFD,0xD0,0xA2),
  QColor(0x31,0xA3,0x54),
  QColor(0x74,0xC4,0x76),
  QColor(0xA1,0xD9,0x9B),
  QColor(0xC7,0xE9,0xC0),
  QColor(0x75,0x6B,0xB1),
  QColor(0x9E,0x9A,0xC8),
  QColor(0xBC,0xBD,0xDC),
  QColor(0xDA,0xDA,0xEB),
  QColor(0x63,0x63,0x63),
  QColor(0x96,0x96,0x96),
  QColor(0xBD,0xBD,0xBD),
  QColor(0xD9,0xD9,0xD9),
};

//---

CQChartsContour::
CQChartsContour(CQChartsPlot *plot) :
 plot_(plot)
{
  minX_ = 1E-6;
  minY_ = 1E-6;

  colors_ = ColorArray(&contourColors[0], &contourColors[20]);
}

void
CQChartsContour::
setData(double *x, double *y, double *z, int numX, int numY)
{
  // polulate x and calc min/max
  x_.resize(numX);

  xmin_ = x[0]; xmax_ = xmin_;

  for (int i = 0; i < numX; i++) {
    x_[i] = x[i];

    xmin_ = std::min(xmin_, x[i]);
    xmax_ = std::max(xmax_, x[i]);
  }

  // populate y and calc min/max
  y_.resize(numY);

  ymin_ = y[0]; ymax_ = ymin_;

  for (int i = 0; i < numY; i++) {
    y_[i] = y[i];

    ymin_ = std::min(ymin_, y[i]);
    ymax_ = std::max(ymax_, y[i]);
  }

  // populate z and calc min/max
  int numZ = numX*numY;

  z_.resize(numZ);

  zmin_ = z[0]; zmax_ = zmin_;

  for (int i = 0; i < numZ; i++) {
    z_[i] = z[i];

    zmin_ = std::min(zmin_, z[i]);
    zmax_ = std::max(zmax_, z[i]);
  }
}

void
CQChartsContour::
setNumContourLevels(int n)
{
  if (n < 1)
    return;

  if (n != numLevels_) {
    numLevels_ = n;

    levels_.clear();
  }
}

void
CQChartsContour::
setContourLevels(const ContourLevels &levels)
{
  levels_    = levels;
  numLevels_ = levels_.size();
}

void
CQChartsContour::
setContourColors(const ColorArray &colors)
{
  colors_ = colors;
}

void
CQChartsContour::
drawContour(CQChartsPaintDevice *device)
{
  if (isSolid())
    drawContourSolid(device);
  else
    drawContourLines(device);
}

void
CQChartsContour::
drawContourLines(CQChartsPaintDevice *device)
{
  // fill background
  CQChartsGeom::BBox bbox(xmin_, ymin_, xmax_, ymax_);

  device->setBrush(backgroundColor());

  device->fillRect(bbox);

  //---

  // draw contour points (optional ?)
  device->setPen(gridPointColor());

  for (auto y : y_)
    for (auto x : x_)
      drawPoint(device, x, y);

  //---

  ContourLevels levels;

  initLevels(levels);

  //---

  device->setPen(QColor(0, 0, 0));

  double xc[8];
  double yc[8];
  int    flag[8];

  for (uint l = 0; l < levels.size(); l++) {
    double level = levels[l];

    QColor c = getLevelColor(l);

    if (c.isValid()) {
      device->setPen  (c);
      device->setBrush(c);
    }

    for (uint i = 0; i < x_.size() - 1; i++) {
      double xi1 = x_[i + 0];
      double xi2 = x_[i + 1];

      double xm = (xi1 + xi2)/2.0;

      int i1 =  i     *x_.size();
      int i2 = (i + 1)*x_.size();

      for (uint j = 0; j < y_.size() - 1; j++) {
        double yj1 = y_[j + 0];
        double yj2 = y_[j + 1];

        double ym = (yj1 + yj2)/2.0;

        double z1 = z_[i1 + j    ];
        double z2 = z_[i2 + j    ];
        double z3 = z_[i1 + j + 1];
        double z4 = z_[i2 + j + 1];

        double zm = (z1 + z2 + z3 + z4)/4.0;

        //---

        // get point on lower side for specified value (if in range)
        if ((level >= z1 && level <= z2) || (level <= z1 && level >= z2)) {
          flag[0] = true;

          if (z1 != z2)
            xc[0] = xi1 + (level - z1)*(xi2 - xi1)/(z2 - z1);
          else
            xc[0] = xi1;

          yc[0] = yj1;
        }
        else
          flag[0] = false;

        // get point on upper side for specified value (if in range)
        if ((level >= z3 && level <= z4) || (level <= z3 && level >= z4)) {
          flag[1] = true;

          if (z3 != z4)
            xc[1] = xi1 + (level - z3)*(xi2 - xi1)/(z4 - z3);
          else
            xc[1] = xi1;

          yc[1] = yj2;
        }
        else
          flag[1] = false;

        // get point on left side for specified value (if in range)
        if ((level >= z1 && level <= z3) || (level <= z1 && level >= z3)) {
          flag[2] = true;

          xc[2] = xi1;

          if (z1 != z3)
            yc[2] = yj1 + (level - z1)*(yj2 - yj1)/(z3 - z1);
          else
            yc[2] = yj1;
        }
        else
          flag[2] = false;

        // get point on right side for specified value (if in range)
        if ((level >= z2 && level <= z4) || (level <= z2 && level >= z4)) {
          flag[3] = true;

          xc[3] = xi2;

          if (z2 != z4)
            yc[3] = yj1 + (level - z2)*(yj2 - yj1)/(z4 - z2);
          else
            yc[3] = yj1;
        }
        else
          flag[3] = false;

        //---

        // get point on left half of diagonal up line
        if ((level >= z1 && level <= zm) || (level <= z1 && level >= zm)) {
          flag[4] = true;

          if (z1 != zm) {
            xc[4] = xi1 + (level - z1)*(xm - xi1)/(zm - z1);
            yc[4] = yj1 + (level - z1)*(ym - yj1)/(zm - z1);
          }
          else {
            xc[4] = xi1;
            yc[4] = yj1;
          }
        }
        else
          flag[4] = false;

        // get point on right half of diagonal down line
        if ((level >= z2 && level <= zm) || (level <= z2 && level >= zm)) {
          flag[5] = true;

          if (z2 != zm) {
            xc[5] = xi2 + (level - z2)*(xm - xi2)/(zm - z2);
            yc[5] = yj1 + (level - z2)*(ym - yj1)/(zm - z2);
          }
          else {
            xc[5] = xi2;
            yc[5] = yj1;
          }
        }
        else
          flag[5] = false;

        // get point on left half of diagonal down line
        if ((level >= z3 && level <= zm) || (level <= z3 && level >= zm)) {
          flag[6] = true;

          if (z3 != zm) {
            xc[6] = xi1 + (level - z3)*(xm - xi1)/(zm - z3);
            yc[6] = yj2 + (level - z3)*(ym - yj2)/(zm - z3);
          }
          else {
            xc[6] = xi1;
            yc[6] = yj2;
          }
        }
        else
          flag[6] = false;

        // get point on right half of diagonal up line
        if ((level >= z4 && level <= zm) || (level <= z4 && level >= zm)) {
          flag[7] = true;

          if (z4 != zm) {
            xc[7] = xi2 + (level - z4)*(xm - xi2)/(zm - z4);
            yc[7] = yj2 + (level - z4)*(ym - yj2)/(zm - z4);
          }
          else {
            xc[7] = xi2;
            yc[7] = yj2;
          }
        }
        else
          flag[7] = false;

        //---

        // connect lines
        for (int k = 0; k < 12; k++) {
          int f1 = contourFlags[2*k + 0];
          int f2 = contourFlags[2*k + 1];

          if (flag[f1] && flag[f2])
            drawLine(device, xc[f1], yc[f1], xc[f2], yc[f2]);
        }
      }
    }
  }
}

void
CQChartsContour::
drawContourSolid(CQChartsPaintDevice *device)
{
  ContourLevels levels;

  initLevels(levels);

  //---

  minX_ = plot_->pixelToWindowWidth (1);
  minY_ = plot_->pixelToWindowHeight(1);

  for (uint i = 0; i < x_.size() - 1; i++) {
    double x1 = x_[i + 0];
    double x2 = x_[i + 1];

    int i1 =  i     *x_.size();
    int i2 = (i + 1)*x_.size();

    for (uint j = 0; j < y_.size() - 1; j++) {
      double y1 = y_[j + 0];
      double y2 = y_[j + 1];

      double z1 = z_[i1 + j    ];
      double z2 = z_[i2 + j    ];
      double z3 = z_[i1 + j + 1];
      double z4 = z_[i2 + j + 1];

      fillContourBox(device, x1, y1, x2, y2, z1, z2, z3, z4, levels);
    }
  }
}

QColor
CQChartsContour::
getLevelColor(int l) const
{
  if (plot_)
    return plot_->interpPaletteColor(CQChartsUtil::ColorInd(l, numLevels_));

  if (colors_.empty())
    return QColor();

  return colors_[l % colors_.size()];
}

void
CQChartsContour::
initLevels(ContourLevels &levels) const
{
  levels = levels_;

  // calc levels from specified number
  if (levels.empty()) {
    double zmin = z_[0];
    double zmax = z_[0];

    int numLevels = numContourLevels();

    for (uint i = 0; i < x_.size(); i++) {
      for (uint j = 0; j < y_.size(); j++) {
        double zm = z_[i*y_.size() + j];

        if (zm < zmin) zmin = zm;
        if (zm > zmax) zmax = zm;
      }
    }

    levels.resize(numLevels);

    for (int i = 0; i < numLevels; i++)
      levels[i] = zmin + ((double) i)*(zmax - zmin)/(numLevels - 1);
  }
}

void
CQChartsContour::
fillContourBox(CQChartsPaintDevice *device, double x1, double y1, double x2, double y2,
               double z1, double z2, double z3, double z4, const ContourLevels &levels)
{
  // get box corner values
  if      (std::abs(x2 - x1) <= minX_ && std::abs(y2 - y1) <= minY_) {
    double z1234 = (z1 + z2 + z3 + z4)/4.0;

    z1 = z1234;
    z2 = z1234;
    z3 = z1234;
    z4 = z1234;
  }
  else if (std::abs(x2 - x1) <= minX_) {
    double z12 = (z1 + z2)/2.0;
    double z34 = (z3 + z4)/2.0;

    z1 = z12;
    z2 = z12;
    z3 = z34;
    z4 = z34;
  }
  else if (std::abs(y2 - y1) <= minY_) {
    double z13 = (z1 + z3)/2.0;
    double z24 = (z2 + z4)/2.0;

    z1 = z13;
    z2 = z13;
    z3 = z24;
    z4 = z24;
  }

  //---

  // get consistent level for four values
  int l = 0;

  if      (z1 < levels[0] && z2 < levels[0] && z3 < levels[0] && z4 < levels[0])
    l = 0;
  else if (z1 > levels[levels.size() - 1] && z2 > levels[levels.size() - 1] &&
           z3 > levels[levels.size() - 1] && z4 > levels[levels.size() - 1])
    l = levels.size();
  else {
    for (l = 1; l < int(levels.size()); l++) {
      if (z1 >= levels[l - 1] && z2 >= levels[l - 1] &&
          z3 >= levels[l - 1] && z4 >= levels[l - 1] &&
          z1 <= levels[l    ] && z2 <= levels[l    ] &&
          z3 <= levels[l    ] && z4 <= levels[l    ])
        break;

      if (z1 >= levels[l - 1] && z3 >= levels[l - 1] &&
          z2 >= levels[l - 1] && z4 >= levels[l - 1] &&
          z1 <= levels[l    ] && z3 <= levels[l    ] &&
          z2 <= levels[l    ] && z4 <= levels[l    ])
        break;
    }

    if (l >= int(levels.size()))
      l = -1;
  }

  //---

  // if consistent level then fill
  if (l >= 0 && l <= int(levels.size())) {
    QColor c = getLevelColor(l);

    device->setPen  (c);
    device->setBrush(c);

    double px[4], py[4];

    px[0] = x1; py[0] = y1;
    px[1] = x2; py[1] = y1;
    px[2] = x2; py[2] = y2;
    px[3] = x1; py[3] = y2;

    fillPolygon(device, px, py, 4);

    return;
  }

  //---

  // sub-divide and draw
  // TODO: iterate instead of recurse
  double x12 = (x1 + x2)/2.0;
  double y12 = (y1 + y2)/2.0;
  double z12 = (z1 + z2)/2.0;
  double z24 = (z2 + z4)/2.0;
  double z13 = (z1 + z3)/2.0;
  double z34 = (z3 + z4)/2.0;

  double z1234 = (z1 + z2 + z3 + z4)/4.0;

  fillContourBox(device, x1 , y1 , x12, y12, z1, z12, z13, z1234, levels);
  fillContourBox(device, x12, y1 , x2 , y12, z12, z2, z1234, z24, levels);
  fillContourBox(device, x1 , y12, x12, y2 , z13, z1234, z3, z34, levels);
  fillContourBox(device, x12, y12, x2 , y2 , z1234, z24, z34, z4, levels);
}

void
CQChartsContour::
fillPolygon(CQChartsPaintDevice *device, const double *x, const double *y, int n)
{
  QPainterPath path;

  for (int i = 0; i < n; ++i) {
    if (i == 0)
      path.moveTo(x[i], y[i]);
    else
      path.lineTo(x[i], y[i]);
  }

  path.closeSubpath();

  device->drawPath(path);
}

void
CQChartsContour::
drawPoint(CQChartsPaintDevice *device, double x, double y)
{
  device->drawPoint(CQChartsGeom::Point(x, y));
}

void
CQChartsContour::
drawLine(CQChartsPaintDevice *device, double x1, double y1, double x2, double y2)
{
  device->drawLine(CQChartsGeom::Point(x1, y1), CQChartsGeom::Point(x2, y2));
}
