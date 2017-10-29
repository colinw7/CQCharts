#ifndef CDELAUNAY_H
#define CDELAUNAY_H

#include <CHull3D.h>

class CDelaunay : public CHull3D {
 public:
  CDelaunay();

  bool calc();

 private:
  void lowerFaces();

  void calcVoronoi();

  bool faceCenter(PFace f, double *xc, double *yc);

  PVertex calcEdgePoint(PFace f, PVertex v, PEdge e);

  bool isLeft(double x, double y, PEdge e);

  double normz(PFace f);
};

#endif
