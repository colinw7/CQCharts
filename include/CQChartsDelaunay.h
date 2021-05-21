#ifndef CQChartsDelaunay_H
#define CQChartsDelaunay_H

#include <CQChartsHull3D.h>

/*!
 * \brief Delaunay plot data
 * \ingroup Charts
 */
class CQChartsDelaunay : public CQChartsHull3D {
 public:
  CQChartsDelaunay();

  void clear();

  bool calc();

 private:
  void lowerFaces();

  void calcVoronoi();

  bool faceCenter(PFace f, double *xc, double *yc, double *r);

  PVertex calcEdgePoint(PFace f, PVertex v, PEdge e);

  bool isLeft(double x, double y, PEdge e);

  double normz(PFace f);
};

#endif
