#include <CQChartsDelaunay.h>

/*-------------------------------------------------------------------*/

CQChartsDelaunay::
CQChartsDelaunay() :
 CQChartsHull3D()
{
  setUseLower(true);
}

bool
CQChartsDelaunay::
calc()
{
  reset();

  // initialize triangles
  if (! doubleTriangle())
    return false;

  // build 3d convex hull
  constructHull();

  // get delaunay points from lower faces
  lowerFaces();

  // calc voronoi graph
  calcVoronoi();

  return true;
}

void
CQChartsDelaunay::
lowerFaces()
{
  PFace f = faces_;

  uint numLower = 0; /* Total number of lower faces. */

  do {
    if (normz(f) < 0) {
      numLower++;

      f->setLower(true);
    }
    else
      f->setLower(false);

    f = f->next;
  } while (f != faces_);
}

void
CQChartsDelaunay::
calcVoronoi()
{
  for (PVertex v = vertices_, vn = nullptr; v && vn != vertices_; v = vn) {
    v->clearVoronoi();
  }

  //---

  // get center of circle for each face
  PFace f = faces_;

  do {
    if (f->isLower()) {
      double xc, yc, r;

      if (faceCenter(f, &xc, &yc, &r)) {
        PVertex v = new Vertex(xc, yc, r);

        f->setVoronoi(v);

        v->addTo(&vvertices_);

        PVertex v1 = f->vertex(0);
        PVertex v2 = f->vertex(1);
        PVertex v3 = f->vertex(2);

        v1->addVoronoi(v);
        v2->addVoronoi(v);
        v3->addVoronoi(v);
      }
    }

    f = f->next;
  } while (f != faces_);

  //---

  f = faces_;

  do {
    f->clearVoronoiEdges();

    // get center point
    PVertex v = f->getVoronoi();

    if (v) {
      // get face edges
      PEdge e1 = f->edge(0);
      PEdge e2 = f->edge(1);
      PEdge e3 = f->edge(2);

      // get face on other side of edge
      PFace f1 = e1->otherFace(f);
      PFace f2 = e2->otherFace(f);
      PFace f3 = e3->otherFace(f);

      // get center points of outside faces
      PVertex v1 = (f1 ? f1->getVoronoi() : nullptr);
      PVertex v2 = (f2 ? f2->getVoronoi() : nullptr);
      PVertex v3 = (f3 ? f3->getVoronoi() : nullptr);

      //----

      // calc edges for each face pair
      if (! v1) { v1 = calcEdgePoint(f, v, e1); v1->addTo(&vvertices_); }
      if (! v2) { v2 = calcEdgePoint(f, v, e2); v2->addTo(&vvertices_); }
      if (! v3) { v3 = calcEdgePoint(f, v, e3); v3->addTo(&vvertices_); }

      PEdge pe1 = new Edge(v, v1);
      pe1->setLeftFace(f1); pe1->setRightFace(f);

      f->addVoronoiEdge(pe1);
      pe1->addTo(&vedges_);

      //--

      PEdge pe2 = new Edge(v, v2);
      pe2->setLeftFace(f2); pe2->setRightFace(f);

      f->addVoronoiEdge(pe2);
      pe2->addTo(&vedges_);

      //--

      PEdge pe3 = new Edge(v, v3);
      pe3->setLeftFace(f3); pe3->setRightFace(f);

      f->addVoronoiEdge(pe3);
      pe3->addTo(&vedges_);
    }

    f = f->next;
  } while (f != faces_);
}

bool
CQChartsDelaunay::
faceCenter(PFace f, double *xc, double *yc, double *r)
{
  PVertex v1 = f->vertex(0);
  PVertex v2 = f->vertex(1);
  PVertex v3 = f->vertex(2);

  double A = v2->x() - v1->x();
  double B = v2->y() - v1->y();
  double C = v3->x() - v1->x();
  double D = v3->y() - v1->y();

  double E = A*(v1->x() + v2->x()) + B*(v1->y() + v2->y());
  double F = C*(v1->x() + v3->x()) + D*(v1->y() + v3->y());

  double G = 2*(A*(v3->y() - v2->y()) - B*(v3->x() - v2->x()));

  if (std::abs(G) < 1E-6) return false;

  *xc = (D*E - B*F)/G;
  *yc = (A*F - C*E)/G;

  double dx = v1->x() - *xc;
  double dy = v1->y() - *yc;

  *r = std::hypot(dx, dy);

  return true;
}

CQChartsDelaunay::PVertex
CQChartsDelaunay::
calcEdgePoint(PFace f, PVertex v, PEdge e)
{
  double fx, fy, fz;

  f->getCenter(&fx, &fy, &fz);

  PVertex v1 = e->start(), v2 = e->end();

  double xe = (v1->x() + v2->x())/2, ye = (v1->y() + v2->y())/2;

  double dx = xe - v->x();
  double dy = ye - v->y();

  double xe1 = v->x() + 100*dx, ye1 = v->y() + 100*dy;
  double xe2 = v->x() - 100*dx, ye2 = v->y() - 100*dy;

//bool l1 = isLeft(v->x(), v->y(), e);
  bool l2 = isLeft(xe1   , ye1   , e);
  bool l3 = isLeft(xe2   , ye2   , e);
  bool l4 = isLeft(fx    , fy    , e);

  if      (l2 != l4)
    return new Vertex(xe1, ye1, 0);
  else if (l3 != l4)
    return new Vertex(xe2, ye2, 0);
  else
    return new Vertex(xe , ye , 0);
}

bool
CQChartsDelaunay::
isLeft(double x, double y, PEdge e)
{
  PVertex v1 = e->start(), v2 = e->end();

  double area2 = (v1->x() - x)*(v2->y() - y) - (v2->x() - x)*(v1->y() - y);

  return (area2 > 0);
}

/*---------------------------------------------------------------------
Computes the z-coordinate of the vector normal to face f.
---------------------------------------------------------------------*/
double
CQChartsDelaunay::
normz(PFace f)
{
  PVertex a = f->vertex(0);
  PVertex b = f->vertex(1);
  PVertex c = f->vertex(2);

  return (b->x() - a->x())*(c->y() - a->y()) - (b->y() - a->y())*(c->x() - a->x());
}
