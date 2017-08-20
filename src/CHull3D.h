#ifndef CHULL3D_H
#define CHULL3D_H

#include <CListLink.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <cmath>
#include <cassert>
#include <iostream>

class CHull3D {
 private:
  /* Define vertex indices. */
  enum { IX = 0, IY = 1, IZ = 2 };

 public:
  /* Define structures for vertices, edges and faces */
  class Face;
  class Edge;
  class Vertex;

  class Vertex : public CListLink<Vertex> {
   private:
    typedef CListLink<Vertex> ListLink;

   public:
    static void resetCount() { count_ = 0; }

    Vertex(double x, double y, double z) {
      v_[0] = x;
      v_[1] = y;
      v_[2] = z;

      num_       = 0;
      duplicate_ = NULL;
      onHull_    = false;
      mark_      = false;

      checkSafe();
    }

    void assignNum() {
      num_ = count_++;
    }

    void reset() {
      duplicate_ = NULL;
      onHull_    = false;
      mark_      = false;
    }

    double x() const { return v_[0]; }
    double y() const { return v_[1]; }
    double z() const { return v_[2]; }

    const double *v() const { return &v_[0]; }

    bool num() const { return num_; }

    Edge *duplicateEdge() const { return duplicate_; }

    void setDuplicateEdge(Edge *e) { duplicate_ = e; }

    bool onHull() const { return onHull_; }

    void setOnHull(bool onHull) { onHull_ = onHull; }

    bool isProcessed() const { return mark_; }

    void setProcessed(bool mark) { mark_ = mark; }

    iterator beginIterator() { return ListLink::begin(); }
    iterator endIterator  () { return ListLink::end  (); }

    void print(std::ostream &os=std::cout) const {
      os << "(" << x() << ", " << y() << ", " << z() << ")";
    }

    friend std::ostream &operator<<(std::ostream &os, const Vertex &v) {
      v.print(os);

      return os;
    }

   private:
    void checkSafe() const {
      /* Range of safe coord values. */
      static int SAFE = INT_MAX;

      if ((fabs(x()) > SAFE) || (fabs(y()) > SAFE) || (fabs(z()) > SAFE)) {
        printf("Coordinate of vertex below might be too large: run with -c flag\n");
        print();
      }
    }

   private:
    static uint count_;

    double v_[3];
    uint   num_;
    Edge*  duplicate_; /* pointer to incident cone edge (or NULL) */
    bool   onHull_;    /* true iff point on hull. */
    bool   mark_;      /* true iff point already processed. */
  };

  typedef Vertex *PVertex;

  //-------

  class Edge : public CListLink<Edge> {
   private:
    typedef CListLink<Edge> ListLink;

   public:
    Edge() :
     newface_(0), removed_(false) {
      setStart(0);
      setEnd  (0);

      setLeftFace (0);
      setRightFace(0);
    }

    Edge(Vertex *s, Vertex *e) :
     newface_(0), removed_(false) {
      setStart(s);
      setEnd  (e);

      setLeftFace (0);
      setRightFace(0);
    }

    Vertex *start() const { return endpts_[0]; }
    Vertex *end  () const { return endpts_[1]; }

    void setStart(Vertex *v) { endpts_[0] = v; }
    void setEnd  (Vertex *v) { endpts_[1] = v; }

    Vertex *endPoint(uint i) const {
      Vertex *v = 0;
      switch (i) {
        case 0 : v = start(); break;
        case 1 : v = end  (); break;
        default: assert(false); break;
      }
      return v;
    }

    void setVertices(Vertex *start, Vertex *end) { setStart(start); setEnd(end); }

    Face *leftFace () const { return adjface_[0]; }
    Face *rightFace() const { return adjface_[1]; }

    void setLeftFace (Face *f) { adjface_[0] = f; }
    void setRightFace(Face *f) { adjface_[1] = f; }

    Face *face(uint i) const {
      Face *f = 0;
      switch (i) {
        case 0 : f = leftFace (); break;
        case 1 : f = rightFace(); break;
        default: assert(false); break;
      }
      return f;
    }

    void setFace(uint i, Face *f) {
      switch (i) {
        case 0 : setLeftFace (f); break;
        case 1 : setRightFace(f); break;
        default: assert(false); break;
      }
    }

    Face *otherFace(const Face *f) { return (f == leftFace() ? rightFace() : leftFace()); }

    Face *coneFace() const { return newface_; }
    void setConeFace(Face *f) { newface_ = f; }

    bool isRemoved() const { return removed_; }
    void setRemoved(bool b) { removed_ = b; }

    iterator beginIterator() { return ListLink::begin(); }
    iterator endIterator  () { return ListLink::end  (); }

    void print(std::ostream &os=std::cout) const {
      os << "(" << *start() << ", " << *end() << ")";
    }

    friend std::ostream &operator<<(std::ostream &os, const Edge &e) {
      e.print(os);

      return os;
    }

   private:
    Vertex* endpts_[2];
    Face*   adjface_[2];
    Face*   newface_; /* pointer to incident cone face. */
    bool    removed_; /* true iff edge should be removed. */
  };

  typedef Edge *PEdge;

  //-------

  class Face : public CListLink<Face> {
   private:
    typedef CListLink<Face> ListLink;

   public:
    Face() {
      for (uint i = 0; i < 3; ++i) {
        edge_  [i] = 0;
        vertex_[i] = 0;
      }

      visible_ = false;
      lower_   = false;

      color_ = -1;

      vv_ = 0;
    }

    Edge *edge(uint i) const { return edge_[i]; }
    void setEdge(uint i, Edge *e) { edge_[i] = e; }

    Vertex *vertex(uint i) const { return vertex_[i]; }
    void setVertex(uint i, Vertex *v) { vertex_[i] = v; }

    bool isVisible() const { return visible_; }
    void setVisible(bool v) { visible_ = v; }

    bool isLower() const { return lower_; }
    void setLower(bool lower) { lower_ = lower; }

    int color() const { return color_; }
    void setColor(int color) { color_ = color; }

    void setVoronoi(PVertex v) { vv_ = v; }

    PVertex getVoronoi() const { return vv_; }

    double normalZDirection() {
      double a[3], b[3];

      subVec(vertex_[1]->v(), vertex_[0]->v(), a);
      subVec(vertex_[2]->v(), vertex_[1]->v(), b);

      return a[0]*b[1] - a[1]*b[0];
    }

    void getCenter(double *x, double *y, double *z) const {
      double x1 = vertex_[0]->x(), y1 = vertex_[0]->y(), z1 = vertex_[0]->z();
      double x2 = vertex_[1]->x(), y2 = vertex_[1]->y(), z2 = vertex_[1]->z();
      double x3 = vertex_[2]->x(), y3 = vertex_[2]->y(), z3 = vertex_[2]->z();

      *x = (x1 + x2 + x3)/3;
      *y = (y1 + y2 + y3)/3;
      *z = (z1 + z2 + z3)/3;
    }

    Face *getFace(int i) const {
      return edge_[i]->otherFace(this);
    }

    void print(std::ostream &os=std::cout) const {
      os << "(" << *vertex_[0] << ", " << *vertex_[1] << ", " << *vertex_[2] << ")";
    }

    friend std::ostream &operator<<(std::ostream &os, const Face &f) {
      f.print(os);

      return os;
    }

   private:
    void subVec(const double *a, const double *b, double c[3]) {
      for (int i = 0; i < 2; i++)
        c[i] = a[i] - b[i];
    }

   private:
    Edge*   edge_[3];
    Vertex* vertex_[3];
    bool    visible_; // true iff face visible from new point
    bool    lower_;   // true iff is lower face
    int     color_;
    Vertex* vv_;      // voronoi vertex
  };

  typedef Face *PFace;

  //-------

 public:
  CHull3D();

 ~CHull3D() { }

  bool getUseLower() const { return useLower_; }
  void setUseLower(bool lower) { useLower_ = lower; }

  void setDebug(bool debug=true) { debug_ = debug; }
  void setCheck(bool check=true) { check_ = check; }

  void addVertex(double x, double y, double z);
  void addVertex(double x, double y);

  void clearVertices();

  bool calc();

  void getRange(double *xmin, double *ymin, double *zmin, double *xmax, double *ymax, double *zmax);

  void dumpPS(const char *filename);

  typedef Vertex::iterator VertexIterator;
  typedef Edge::iterator   EdgeIterator;
  typedef Face::iterator   FaceIterator;

  VertexIterator verticesBegin() { return vertices_->beginIterator(); }
  VertexIterator verticesEnd  () { return vertices_->endIterator(); }

  EdgeIterator edgesBegin() { return edges_->beginIterator(); }
  EdgeIterator edgesEnd  () { return edges_->endIterator(); }

  FaceIterator facesBegin() { return faces_->begin(); }
  FaceIterator facesEnd  () { return faces_->end(); }

  EdgeIterator voronoiEdgesBegin() { return vedges_->beginIterator(); }
  EdgeIterator voronoiEdgesEnd  () { return vedges_->endIterator(); }

 protected:
  /* Function declarations */
  void    reset();
  bool    doubleTriangle();
  void    constructHull();
  void    edgeOrderOnFaces();
  bool    addOne(PVertex p);
  PFace   makeConeFace(PEdge e, PVertex p);
  void    makeCcw(PFace f, PEdge e, PVertex p);
  int     volumeSign(PFace f, PVertex p);

  PFace   makeFace();
  PFace   makeFace(PVertex v0, PVertex v1, PVertex v2, PFace f);
  PEdge   makeEdge();

  void    cleanUp(PVertex *pvnext);
  void    cleanEdges();
  void    cleanFaces();
  void    cleanVertices(PVertex *pvnext);

  void    crossProduct(double x1, double y1, double z1, double x2, double y2, double z2,
                       double *x3, double *y3, double *z3);
  bool    collinear(PVertex a, PVertex b, PVertex c);

  void    checks();
  void    consistency();
  void    convexity();
  void    checkEuler(uint numV, uint numE, uint numF);
  void    checkEndpts();

  void    printOut(PVertex v);
  void    printVertices();
  void    printEdges();
  void    printFaces();

 protected:
  /* Global variable definitions */
  PVertex vertices_;
  PEdge   edges_;
  PFace   faces_;
  PVertex vvertices_;
  PEdge   vedges_;
  bool    useLower_;
  bool    debug_;
  bool    check_;
};

#endif
