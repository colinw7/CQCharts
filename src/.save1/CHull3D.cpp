#include <CHull3D.h>

uint CHull3D::Vertex::count_ = 0;

/*-------------------------------------------------------------------*/

CHull3D::
CHull3D() :
 vertices_(0), edges_(0), faces_(0), vvertices_(0), vedges_(0),
 useLower_(false), debug_(false), check_(false)
{
}

bool
CHull3D::
calc()
{
  reset();

  // initialize triangles
  if (! doubleTriangle())
    return false;

  // build 3d convex hull
  constructHull();

  // order faces
  edgeOrderOnFaces();

  return true;
}

void
CHull3D::
addVertex(double x, double y, double z)
{
  PVertex v = new Vertex(x, y, z);

  v->assignNum();

  v->addTo(&vertices_);
}

void
CHull3D::
addVertex(double x, double y)
{
  addVertex(x, y, x*x + y*y);
}

void
CHull3D::
clearVertices()
{
  if (! vertices_) return;

  PVertex v = vertices_;

  for (;;) {
    PVertex v1 = v->next;

    if (v1 == vertices_)
      break;

    delete v;

    v = v1;
  }

  vertices_ = 0;

  Vertex::resetCount();
}

void
CHull3D::
reset()
{
  if (vertices_) {
    PVertex v = vertices_; do { v->reset(); v = v->next; } while (v != vertices_);
  }

  if (edges_) {
    PEdge e = edges_; do { PEdge e1 = e->next; delete e; e = e1; } while (e != edges_);

    edges_ = 0;
  }

  if (faces_) {
    PFace f = faces_; do { PFace f1 = f->next; delete f; f = f1; } while (f != faces_);

    faces_ = 0;
  }

  if (vvertices_) {
    PVertex v = vvertices_; do { PVertex v1 = v->next; delete v; v = v1; } while (v != vvertices_);

    vvertices_ = 0;
  }

  if (vedges_) {
    PEdge e = vedges_; do { PEdge e1 = e->next; delete e; e = e1; } while (e != vedges_);

    vedges_ = 0;
  }
}

/*---------------------------------------------------------------------
 doubleTriangle builds the initial double triangle.  It first finds 3
 non-collinear points and makes two faces out of them, in opposite order.
 It then finds a fourth point that is not coplanar with that face.  The
 vertices are stored in the face structure in counterclockwise order so
 that the volume between the face and the point is negative. Lastly, the
 3 new faces to the fourth point are constructed and the data structures
 are cleaned up.
---------------------------------------------------------------------*/
bool
CHull3D::
doubleTriangle()
{
  if (! vertices_) return false;

  // Find 3 non-collinear points.
  PVertex v0 = vertices_;
  PVertex v1 = v0->next;
  PVertex v2 = v1->next;

  while (collinear(v0, v1, v2)) {
    v2 = v1;
    v1 = v0;
    v0 = v0->next;

    if (v0 == vertices_) {
      std::cerr << "doubleTriangle: All points collinear!" << std::endl;
      return false;
    }
  }

  //---

  // Mark the vertices as processed.
  v0->setProcessed(true);
  v1->setProcessed(true);
  v2->setProcessed(true);

  // Create the two "twin" faces.
  PFace f0 = makeFace(v0, v1, v2, 0);
  PFace f1 = makeFace(v2, v1, v0, f0);

  /* Link adjacent face fields. */
  f0->edge(0)->setRightFace(f1);
  f0->edge(1)->setRightFace(f1);
  f0->edge(2)->setRightFace(f1);

  f1->edge(0)->setRightFace(f0);
  f1->edge(1)->setRightFace(f0);
  f1->edge(2)->setRightFace(f0);

  // Find a fourth, non-coplanar point to form tetrahedron.
  PVertex v3 = v2->next;

  int vol = volumeSign(f0, v3);

  while (! vol) {
    v3 = v3->next;

    if (v3 == v0) {
      std::cerr << "doubleTriangle: All points coplanar!" << std::endl;
      return false;
    }

    vol = volumeSign(f0, v3);
  }

  // Ensure that v3 will be the first added.
  vertices_ = v3;

  if (debug_) {
    std::cerr << "doubleTriangle: finished. Head repositioned at v3." << std::endl;
    printOut(vertices_);
  }

  return true;
}

/*---------------------------------------------------------------------
constructHull adds the vertices to the hull one at a time. The hull
vertices are those in the list marked as on hull.
---------------------------------------------------------------------*/
void
CHull3D::
constructHull()
{
  if (! vertices_) return;

  PVertex v0 = vertices_;

  do {
    PVertex v1 = v0->next;

    if (! v0->isProcessed()) {
      addOne(v0);

      cleanUp(&v1); /* Pass down v1 in case it gets deleted. */

      if (check_) {
        std::cerr << "constructHull: After add of " << v0->num() << " & cleanup" << std::endl;
        checks();
      }

      if (debug_)
        printOut(v0);
    }

    v0 = v1;
  } while (v0 != vertices_);
}

/*------------------------------------------------------------------
  edgeOrderOnFaces: puts e0 between v0 and v1, e1 between v1 and v2,
  e2 between v2 and v0 on each face. This should be unnecessary, alas.
  Not used in code, but useful for other purposes.
------------------------------------------------------------------*/
void
CHull3D::
edgeOrderOnFaces()
{
  PFace f = faces_;

  do {
    for (int i = 0; i < 3; i++) {
      int i1 = (i + 1) % 3;

      if (! (((f->edge(i)->start() == f->vertex(i )) &&
              (f->edge(i)->end  () == f->vertex(i1))) ||
             ((f->edge(i)->end  () == f->vertex(i )) &&
              (f->edge(i)->start() == f->vertex(i1))))) {
        /* Change the order of the edges on the face: */
        for (int j = 0; j < 3; j++) {
          /* find the edge that should be there */
          if (((f->edge(j)->start() == f->vertex(i )) &&
               (f->edge(j)->end  () == f->vertex(i1))) ||
              ((f->edge(j)->end  () == f->vertex(i )) &&
               (f->edge(j)->start() == f->vertex(i1)))) {
            /* Swap it with the one erroneously put into its place: */
            if (debug_)
              std::cerr << "Making a swap in edgeOrderOnFaces: f(" <<
                           f->vertex(0)->num() << "," <<
                           f->vertex(1)->num() << "," <<
                           f->vertex(2)->num() << "): e[" <<
                           i << "] and e[" << j << "]" << std::endl;

            PEdge newEdge = f->edge(i);

            f->setEdge(i, f->edge(j));
            f->setEdge(j, newEdge);
          }
        }
      }
    }

    f = f->next;
  } while (f != faces_);
}

/*---------------------------------------------------------------------
addOne is passed a vertex. It first determines all faces visible from
that point. If none are visible then the point is marked as not
on hull. Next is a loop over edges. If both faces adjacent to an edge
are visible, then the edge is marked for deletion. If just one of the
adjacent faces is visible then a new face is constructed.
---------------------------------------------------------------------*/
bool
CHull3D::
addOne(PVertex p)
{
  if (debug_) {
    std::cerr << "addOne: starting to add v" << p->num() << std::endl;
    printOut(vertices_);
  }

  // set processed
  p->setProcessed(true);

  // Mark faces visible from p
  bool vis = false;

  PFace f = faces_;

  do {
    int vol = volumeSign(f, p);

    if (debug_)
      std::cerr << "faddr: " << (void *) f << " paddr: " <<
                   (void *) p << " vol = " << vol << std::endl;

    if (vol < 0) {
      f->setVisible(true);

      vis = true;
    }

    f = f->next;
  } while (f != faces_);

  // If no faces are visible from p, then p is inside the hull (not on hull).
  if (! vis) {
    p->setOnHull(false);
    return false;
  }

  // Mark edges in interior of visible region for deletion.
  // Erect a new face based on each border edge.
  PEdge e = edges_;

  do {
    PEdge e1 = e->next;

    PFace lf = e->leftFace ();
    PFace rf = e->rightFace();

    if (lf && rf) {
      if      (lf->isVisible() && rf->isVisible())
        /* e interior: mark for deletion. */
        e->setRemoved(true);
      else if (lf->isVisible() || rf->isVisible())
        /* e border: make a new face. */
        e->setConeFace(makeConeFace(e, p));
    }

    e = e1;
  } while (e != edges_);

  return true;
}

/*---------------------------------------------------------------------
makeConeFace makes a new face and two new edges between the
edge and the point that are passed to it. It returns a pointer to
the new face.
---------------------------------------------------------------------*/
CHull3D::PFace
CHull3D::
makeConeFace(PEdge e, PVertex p)
{
  PEdge new_edge[2];

  /* Make two new edges (if don't already exist). */
  for (int i = 0; i < 2; ++i) {
    /* If the edge exists, copy it into new_edge. */
    new_edge[i] = e->endPoint(i)->duplicateEdge();

    /* Otherwise (duplicate edge is NULL), so make new edge. */
    if (! new_edge[i]) {
      new_edge[i] = makeEdge();

      new_edge[i]->setVertices(e->endPoint(i), p);

      e->endPoint(i)->setDuplicateEdge(new_edge[i]);
    }
  }

  /* Make the new face. */
  PFace new_face = makeFace();

  new_face->setEdge(0, e);
  new_face->setEdge(1, new_edge[0]);
  new_face->setEdge(2, new_edge[1]);

  makeCcw(new_face, e, p);

  /* Set the adjacent face pointers. */
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      /* Only one NULL link should be set to new_face. */
      if (! new_edge[i]->face(j)) {
        new_edge[i]->setFace(j, new_face);
        break;
      }
    }
  }

  return new_face;
}

/*---------------------------------------------------------------------
makeCcw puts the vertices in the face structure in counterclock wise
order. We want to store the vertices in the same order as in the
visible face. The third vertex is always p.

Although no specific ordering of the edges of a face are used
by the code, the following condition is maintained for each face f:
one of the two endpoints of f->edge(i) matches f->vertex(i).
But note that this does not imply that f->edge(i) is between
f->vertex(i) and f->vertex((i + 1) % 3).  (Thanks to Bob Williamson.)
---------------------------------------------------------------------*/
void
CHull3D::
makeCcw(PFace f, PEdge e, PVertex p)
{
  PFace fv; /* The visible face adjacent to e */

  PFace lf = e->leftFace ();
  PFace rf = e->rightFace();

  if (lf && lf->isVisible())
    fv = lf;
  else
    fv = rf;

  /* Set vertex(0) and vertex(1) of f to have the same orientation
     as do the corresponding vertices of fv. */
  int i = 0;

  while (fv->vertex(i) != e->start())
    ++i;

  /* Orient f the same as fv. */
  int i1 = (i + 1) % 3;

  if (fv->vertex(i1) != e->end()) {
    f->setVertex(0, e->end  ());
    f->setVertex(1, e->start());
  }
  else {
    f->setVertex(0, e->start());
    f->setVertex(1, e->end  ());

    Edge *t = f->edge(1);

    f->setEdge(1, f->edge(2));
    f->setEdge(2, t         );
  }

  /* This swap is tricky. e is edge(0). edge(1) is based on endpt[0],
     edge(2) on endpt[1]. So if e is oriented "forwards," we need to
     move edge(1) to follow [0], because it precedes. */

  f->setVertex(2, p);
}

/*---------------------------------------------------------------------
dumpPS: prints out the vertices and the faces. Uses the indices
corresponding to the order in which the vertices were input.
Output is in PostScript format.
---------------------------------------------------------------------*/
void
CHull3D::
dumpPS(const char *filename)
{
  FILE *fp = fopen(filename, "w");

  if (fp == 0) {
    std::cerr << "Invalid filename " << filename << std::endl;
    return;
  }

  //------

  /* Counters for Euler's formula. */
  uint numV = 0, numE = 0, numF = 0;

  /*-- find X min & max --*/
  double xmin, ymin, zmin, xmax, ymax, zmax;

  getRange(&xmin, &ymin, &zmin, &xmax, &ymax, &zmax);

  /* PostScript header */
  fprintf(fp, "%%!PS\n");
  fprintf(fp, "%%%%BoundingBox: %g %g %g %g\n", xmin, ymin, xmax, ymax);
  fprintf(fp, ".00 .00 setlinewidth\n");
  fprintf(fp, "%g %g translate\n", -xmin + 72, -ymin + 72);
  /* The +72 shifts the figure one inch from the lower left corner */

  // Vertices.
  if (vertices_) {
    PVertex v = vertices_;

    do {
      if (v->isProcessed()) numV++;

      v = v->next;
    } while (v != vertices_);

    fprintf(fp, "\n%%%% Vertices:\tV = %u\n", numV);
    fprintf(fp, "%%%% index:\tx\ty\tz\n");

    do {
      fprintf(fp, "%%%% %5d:\t%g\t%g\t%g\n", v->num(), v->x(), v->y(), v->z());
      fprintf(fp, "newpath\n");
      fprintf(fp, "%g\t%g 2 0 360 arc\n", v->x(), v->y());
      fprintf(fp, "closepath stroke\n\n");

      v = v->next;
    } while (v != vertices_);
  }

  // Faces. (visible faces only)
  if (faces_) {
    PFace f = faces_;

    do {
      ++numF;

      f  = f ->next;
    } while (f != faces_);

    fprintf(fp, "\n%%%% Faces:\tF = %u\n", numF);
    fprintf(fp, "%%%% Visible faces only: \n");

    do {
      bool show = true;

      if (useLower_) {
        /* Print face only if it is lower */
        show = f->isLower();
      }
      else {
        /* print face only if it is visible: if normal vector >= 0 */
        double nz = f->normalZDirection();

        show = (nz >= 1E-6);
      }

      if (show) {
        fprintf(fp, "%%%% nums: %u %u %u\n",
                f->vertex(0)->num(), f->vertex(1)->num(), f->vertex(2)->num());

        fprintf(fp, "newpath\n");
        fprintf(fp, "%g\t%g\tmoveto\n", f->vertex(0)->x(), f->vertex(0)->y());
        fprintf(fp, "%g\t%g\tlineto\n", f->vertex(1)->x(), f->vertex(1)->y());
        fprintf(fp, "%g\t%g\tlineto\n", f->vertex(2)->x(), f->vertex(2)->y());
        fprintf(fp, "closepath stroke\n\n");
      }

      f = f->next;
    } while (f != faces_);

    /* prints a list of all faces */
    fprintf(fp, "%%%% List of all faces: \n");
    fprintf(fp, "%%%%\tv0\tv1\tv2\t(vertex indices)\n");

    do {
      fprintf(fp, "%%%%\t%u\t%u\t%u\n",
              f->vertex(0)->num(), f->vertex(1)->num(), f->vertex(2)->num());

      f = f->next;
    } while (f != faces_);
  }

  // Edges.
  if (edges_) {
    PEdge e = edges_;

    do {
      numE++;

      e = e->next;
    } while (e != edges_);

    fprintf(fp, "\n%%%% Edges:\tE = %u\n", numE);
    /* Edges not printed out (but easily added). */
  }

  fprintf(fp, "\nshowpage\n\n");
  fprintf(fp, "%%EOF\n");

  fclose(fp);

  checkEuler(numV, numE, numF);
}

void
CHull3D::
getRange(double *xmin, double *ymin, double *zmin, double *xmax, double *ymax, double *zmax)
{
  /*-- find X min & max --*/
  if (! vertices_) return;

  PVertex v = vertices_;

  *xmin = v->x(), *xmax = *xmin;
  *ymin = v->y(), *ymax = *ymin;
  *zmin = v->y(), *zmax = *ymin;

  do {
    *xmin = std::min(*xmin, v->x());
    *xmax = std::max(*xmax, v->x());

    *ymin = std::min(*ymin, v->y());
    *ymax = std::max(*ymax, v->y());

    *zmin = std::min(*zmin, v->z());
    *zmax = std::max(*zmax, v->z());

    v = v->next;
  } while (v != vertices_);
}

/*---------------------------------------------------------------------
volumeSign returns the sign of the volume of the tetrahedron determined by f
and p. volumeSign is +1 iff p is on the negative side of f, where the
positive side is determined by the rh-rule. So the volume is positive if
the ccw normal to f points outside the tetrahedron. The final
fewer-multiplications form is due to Bob Williamson.
---------------------------------------------------------------------*/
int
CHull3D::
volumeSign(PFace f, PVertex p)
{
  PVertex fv1 = f->vertex(0), fv2 = f->vertex(1), fv3 = f->vertex(2);

  double ax = fv1->x() - p->x(), ay = fv1->y() - p->y(), az = fv1->z() - p->z();
  double bx = fv2->x() - p->x(), by = fv2->y() - p->y(), bz = fv2->z() - p->z();
  double cx = fv3->x() - p->x(), cy = fv3->y() - p->y(), cz = fv3->z() - p->z();

  double vol = ax*(by*cz - bz*cy) + ay*(bz*cx - bx*cz) + az*(bx*cy - by*cx);

  /* The volume should be an integer. */
  if      (vol >  1E-6) return  1;
  else if (vol < -1E-6) return -1;
  else                  return  0;
}

/*--------------------------------------------------------------------
makeFace creates a new face structure and initializes all of its
flags to NULL and sets all the flags to off.  It returns a pointer
to the empty cell.
---------------------------------------------------------------------*/
CHull3D::PFace
CHull3D::
makeFace()
{
  PFace f = new Face;

  f->addTo(&faces_);

  return f;
}

/*---------------------------------------------------------------------
makeFace creates a new face structure from three vertices (in ccw
order).  It returns a pointer to the face.
---------------------------------------------------------------------*/
CHull3D::PFace
CHull3D::
makeFace(PVertex v0, PVertex v1, PVertex v2, PFace fold)
{
  PEdge e0, e1, e2;

  /* Create edges of the initial triangle. */
  if (! fold) {
    e0 = makeEdge();
    e1 = makeEdge();
    e2 = makeEdge();
  }
  else { /* Copy from fold, in reverse order. */
    e0 = fold->edge(2);
    e1 = fold->edge(1);
    e2 = fold->edge(0);
  }

  e0->setVertices(v0, v1);
  e1->setVertices(v1, v2);
  e2->setVertices(v2, v0);

  /* Create face for triangle. */

  PFace f = makeFace();

  f->setEdge  (0, e0); f->setEdge  (1, e1); f->setEdge  (2, e2);
  f->setVertex(0, v0); f->setVertex(1, v1); f->setVertex(2, v2);

  /* Link edges to face. */
  e0->setLeftFace(f);
  e1->setLeftFace(f);
  e2->setLeftFace(f);

  return f;
}

/*---------------------------------------------------------------------
makeEdge creates a new cell and initializes all pointers to NULL
and sets all flags to off.  It returns a pointer to the empty cell.
---------------------------------------------------------------------*/
CHull3D::PEdge
CHull3D::
makeEdge()
{
  PEdge e = new Edge;

  e->addTo(&edges_);

  return e;
}

/*---------------------------------------------------------------------
cleanUp goes through each data structure list and clears all
flags and NULLs out some pointers.  The order of processing
(edges, faces, vertices) is important.
---------------------------------------------------------------------*/
void
CHull3D::
cleanUp(PVertex *pvnext)
{
  cleanEdges();

  cleanFaces();

  cleanVertices(pvnext);
}

/*---------------------------------------------------------------------
cleanEdges runs through the edge list and cleans up the structure.
If there is a new face then it will put that face in place of the
visible face and NULL out new face. It also deletes so marked edges.
---------------------------------------------------------------------*/
void
CHull3D::
cleanEdges()
{
  /* Integrate the new face's into the data structure. */
  /* Check every edge. */
  PEdge e = edges_;

  do {
    if (e->coneFace()) {
      PFace lf = e->leftFace();

      if (lf && lf->isVisible())
        e->setLeftFace(e->coneFace());
      else
        e->setRightFace(e->coneFace());

      e->setConeFace(0);
    }

    e = e->next;
  } while (e != edges_);

  /* Delete any edges marked for deletion. */
  while (edges_ && edges_->isRemoved()) {
    e = edges_;

    e->removeFrom(&edges_);

    delete e;
  }

  e = edges_->next;

  do {
    if (e->isRemoved()) {
      PEdge t = e;

      e = e->next;

      t->removeFrom(&edges_);

      delete t;
    }
    else
      e = e->next;
  } while (e != edges_);
}

/*---------------------------------------------------------------------
cleanFaces runs through the face list and deletes any face marked visible.
---------------------------------------------------------------------*/
void
CHull3D::
cleanFaces()
{
  PFace f; /* Primary pointer into face list. */

  while (faces_ && faces_->isVisible()) {
    f = faces_;

    f->removeFrom(&faces_);

    delete f;
  }

  f = faces_->next;

  do {
    if (f->isVisible()) {
      PFace t = f;

      f = f->next;

      t->removeFrom(&faces_);

      delete t;
    }
    else
      f = f->next;
  } while (f != faces_);
}

/*---------------------------------------------------------------------
cleanVertices runs through the vertex list and deletes the
vertices that are marked as processed but are not incident to any
undeleted edges.

The pointer to vnext, is used to alter vnext in constructHull()
if we are about to delete vnext.
---------------------------------------------------------------------*/
void
CHull3D::
cleanVertices(PVertex *pvnext)
{
  if (! edges_) return;

  // Mark all vertices incident to some undeleted edge as on the hull.
  PEdge e = edges_;

  do {
    e->start()->setOnHull(true);
    e->end  ()->setOnHull(true);

    e = e->next;
  } while (e != edges_);

  /* Delete all vertices that have been processed but are not on the hull. */
  PVertex v;

  while (vertices_ && vertices_->isProcessed() && ! vertices_->onHull()) {
    /* If about to delete vnext, advance it first. */
    v = vertices_;

    if (v == *pvnext) *pvnext = v->next;

    v->removeFrom(&vertices_);

    delete v;
  }

  if (vertices_) {
    v = vertices_->next;

    do {
      if (v->isProcessed() && ! v->onHull()) {
        PVertex t = v;

        v = v->next;

        if (t == *pvnext) *pvnext = t->next;

        t->removeFrom(&vertices_);

        delete t;
      }
      else
        v = v->next;
    } while (v != vertices_);
  }

  // Reset flags
  if (vertices_) {
    v = vertices_;

    do {
      v->setDuplicateEdge(0);
      v->setOnHull       (false);

      v = v->next;
    } while (v != vertices_);
  }
}

void
CHull3D::
crossProduct(double x1, double y1, double z1, double x2, double y2, double z2,
             double *x3, double *y3, double *z3)
{
  *x3 = y1*z2 - z1*y2;
  *y3 = z1*x2 - x1*z2;
  *z3 = x1*y2 - y1*x2;
}

/*---------------------------------------------------------------------
collinear checks to see if the three points given are collinear,
by checking to see if each element of the cross product is zero.
---------------------------------------------------------------------*/
bool
CHull3D::
collinear(PVertex a, PVertex b, PVertex c)
{
  double x1 = b->x() - a->x(), y1 = b->y() - a->y(), z1 = b->z() - a->z();
  double x2 = c->x() - a->x(), y2 = c->y() - a->y(), z2 = c->z() - a->z();

  double x3, y3, z3;

  crossProduct(x1, y1, z1, x2, y2, z2, &x3, &y3, &z3);

  return (fabs(x3) < 1E-6 && fabs(y3) < 1E-6 && fabs(z3) < 1E-6);

#if 0
  return (c->z() - a->z()) * (b->y() - a->y()) -
         (b->z() - a->z()) * (c->y() - a->y()) == 0 &&
         (b->z() - a->z()) * (c->x() - a->x()) -
         (b->x() - a->x()) * (c->z() - a->z()) == 0 &&
         (b->x() - a->x()) * (c->y() - a->y()) -
         (b->y() - a->y()) * (c->x() - a->x()) == 0;
#endif
}

/*-------------------------------------------------------------------*/
void
CHull3D::
checks()
{
  if (! vertices_)
    return;

  consistency();

  convexity();

  uint numV = 0;

  PVertex v = vertices_;

  if (v) {
    do {
      if (v->isProcessed()) numV++;

      v = v->next;
    } while (v != vertices_);
  }

  uint numE = 0;

  PEdge e = edges_;

  if (e) {
    do {
      numE++;

      e = e->next;
    } while (e != edges_);
  }

  uint numF = 0;

  PFace f = faces_;

  if (f) {
    do {
      numF++;

      f  = f ->next;
    } while (f  != faces_);
  }

  checkEuler(numV, numE, numF);

  checkEndpts();
}

/*---------------------------------------------------------------------
consistency runs through the edge list and checks that all
adjacent faces have their endpoints in opposite order.  This verifies
that the vertices are in counterclockwise order.
---------------------------------------------------------------------*/
void
CHull3D::
consistency()
{
  PEdge e = edges_;

  do {
    /* find index of endpoint[0] in adjacent face[0] */
    PFace lf = e->leftFace();

    int i = 0;

    while (lf && lf->vertex(i) != e->start())
      ++i;

    /* find index of endpoint[0] in adjacent face[1] */
    PFace rf = e->rightFace();

    int j = 0;

    while (rf && rf->vertex(j) != e->start())
      ++j;

    /* check if the endpoints occur in opposite order */
    int i1 = (i  + 1) % 3;
    int i2 = (i1 + 1) % 3;
    int j1 = (j  + 1) % 3;
    int j2 = (j1 + 1) % 3;

    if (! (lf->vertex(i1) == rf->vertex(j2) ||
           lf->vertex(i2) == rf->vertex(j1)))
      break;

    e = e->next;
  } while (e != edges_);

  if (e != edges_)
    fprintf(stderr, "checks: edges are NOT consistent.\n");
  else
    fprintf(stderr, "checks: edges consistent.\n");
}

/*---------------------------------------------------------------------
convexity checks that the volume between every face and every
point is negative.  This shows that each point is inside every face
and therefore the hull is convex.
---------------------------------------------------------------------*/
void
CHull3D::
convexity()
{
  if (! faces_) return;

  PFace f = faces_;

  do {
    PVertex v = vertices_;

    do {
      if (v->isProcessed()) {
        int vol = volumeSign(f, v);

        if (vol < 0)
          break;
      }

      v = v->next;
    } while (v != vertices_);

    f = f->next;
  } while (f != faces_);

  if (f != faces_)
    fprintf(stderr, "checks: NOT convex.\n");
  else if (check_)
    fprintf(stderr, "checks: convex.\n");
}

/*---------------------------------------------------------------------
checkEuler checks Euler's relation, as well as its implications when
all faces are known to be triangles.  Only prints positive information
when debug is true, but always prints negative information.
---------------------------------------------------------------------*/
void
CHull3D::
checkEuler(uint numV, uint numE, uint numF)
{
  if (check_)
    fprintf(stderr, "checks: numV, numE, numF = %u %u %u:\t", numV, numE, numF);

  if ((numV - numE + numF) != 2)
    fprintf(stderr, "checks: numV - numE + numF != 2\n");
  else if (check_)
    fprintf(stderr, "numV - numE + numF = 2\t");

  if (numF != (2 * numV - 4))
    fprintf(stderr, "checks: numF=%u != 2V-4=%u; numV=%u\n", numF, 2*numV-4, numV);
  else if (check_)
    fprintf(stderr, "numF = 2V - 4\t");

  if ((2 * numE) != (3 * numF))
    fprintf(stderr, "checks: 2E=%u != 3F=%u; numE=%u, numF=%u\n", 2*numE, 3*numF, numE, numF);
  else if (check_)
    fprintf(stderr, "2E = 3F\n");
}

/*===================================================================
These functions are used whenever the debug flag is set.
They print out the entire contents of each data structure.
printing is to standard error.
=====================================================================*/

/*-------------------------------------------------------------------*/
void
CHull3D::
printOut(PVertex v)
{
  fprintf(stderr, "\nHead vertex %u = %6p :\n", v->num(), (void *) v);

  printVertices();
  printEdges();
  printFaces();
}

/*-------------------------------------------------------------------*/
void
CHull3D::
printVertices()
{
  PVertex temp = vertices_;

  fprintf(stderr, "Vertex List\n");

  if (vertices_) {
    do {
      fprintf(stderr,"  addr %6p\t", (void *) vertices_);
      fprintf(stderr,"  num %4d", vertices_->num());
      fprintf(stderr,"   (%6g,%6g,%6g)",vertices_->x(), vertices_->y(), vertices_->z());
      fprintf(stderr,"   active:%3d", vertices_->onHull());
      fprintf(stderr,"   dup:%5p", (void *) vertices_->duplicateEdge());
      fprintf(stderr,"   mark:%2d\n", vertices_->isProcessed());

      vertices_ = vertices_->next;
    } while (vertices_ != temp);
  }
}

/*-------------------------------------------------------------------*/
void
CHull3D::
printEdges()
{
  if (! edges_) return;

  PEdge temp = edges_;

  fprintf(stderr, "Edge List\n");

  if (edges_) {
    do {
      fprintf(stderr, "  addr: %6p\t", (void *) edges_);
      fprintf(stderr, "adj: ");

      for (int i = 0; i < 2; ++i)
        fprintf(stderr, "%6p", (void *) edges_->face(i));

      fprintf(stderr, "  endpts:");

      for (int i = 0; i < 2; ++i)
        fprintf(stderr, "%4d", edges_->endPoint(i)->num());

      fprintf(stderr, "  del: %3d\n", edges_->isRemoved());

      edges_ = edges_->next;
    } while (edges_ != temp);
  }
}

/*-------------------------------------------------------------------*/
void
CHull3D::
printFaces()
{
  if (! faces_) return;

  PFace temp = faces_;

  fprintf(stderr, "Face List\n");

  if (faces_) {
    do {
      fprintf(stderr, "  addr: %10p ", (void *) faces_);
      fprintf(stderr, "  edges:");

      for (int i = 0; i < 3; ++i)
        fprintf(stderr, "%10p ", (void *) faces_->edge(i));

      fprintf(stderr, "  vert:");

      for (int i = 0; i < 3; ++i)
        fprintf(stderr, "%4d", faces_->vertex(i)->num());

      fprintf(stderr, "  vis: %d\n", faces_->isVisible());

      faces_ = faces_->next;
    } while (faces_ != temp);
  }
}

/*-------------------------------------------------------------------
Checks that, for each face, for each i={0,1,2}, the [i]th vertex of
that face is either the [0]th or [1]st endpoint of the [ith] edge of
the face.
-------------------------------------------------------------------*/
void
CHull3D::
checkEndpts()
{
  if (faces_) {
    PFace fstart = faces_;

    bool error = false;

    do {
      for (int i = 0; i < 3; ++i) {
        PVertex v = faces_->vertex(i);
        PEdge   e = faces_->edge(i);

        if (v != e->start() && v != e->end()) {
          error = true;

          fprintf(stderr,"CheckEndpts: Error!\n");
          fprintf(stderr,"  addr: %8p;", (void *) faces_);
          fprintf(stderr,"  edges:");
          fprintf(stderr,"(%3d,%3d)", e->start()->num(), e->end()->num());
          fprintf(stderr,"\n");
        }
      }

      faces_= faces_->next;
    } while (faces_ != fstart);

    if (error)
      fprintf(stderr,"Checks: ERROR found and reported above.\n");
    else
      fprintf(stderr,"Checks: All endpts of all edges of all faces check.\n");
  }
}
