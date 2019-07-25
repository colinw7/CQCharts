#ifndef CQChartsHull3D_H
#define CQChartsHull3D_H

#include <cassert>
#include <sys/types.h>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <cmath>
#include <iostream>

#include <iterator>
#include <cstddef>

/*!
 * input iterator supports:
 *  . Default construct
 *  . Copy construct
 *  . Copy assign
 *  . Increment pre/post
 *  . ==, !=
 *  . * (dereference to value)
 *
 * single pass with a shared underlying iterator value for a class
 *
 * i.e  II i1, i2;
 *      i1 = o.begin();
 *      i2 = i1;
 *      ++i1
 *      assert(*i1 == *i2);
 *
*/
template<class STATE, class T, class DIST=ptrdiff_t>
class CInputIterator : public std::iterator<std::input_iterator_tag,T,DIST> {
 public:
  enum { IS_PTR = std::is_pointer<T>::value };

  typedef typename std::remove_reference<T>::type     NonRefT;
  typedef typename std::remove_pointer<NonRefT>::type NonPtrT;
  typedef typename std::remove_cv<NonPtrT>::type      BareT;

  typedef typename std::conditional<IS_PTR,const BareT *,const BareT &>::type ContentsT;

  CInputIterator() :
   state_() {
  }

  CInputIterator(const STATE &state) :
   state_(state) {
  }

  CInputIterator(const CInputIterator &i) :
   state_(i.state_) {
  }

 ~CInputIterator() { }

  const CInputIterator &operator=(const CInputIterator &i) {
    state_ = i.state_;
    return *this;
  }

  const CInputIterator &operator++() { state_.next(); return *this; }

  const CInputIterator &operator++(int) { ++(*this); }

  friend bool operator==(const CInputIterator &lhs, const CInputIterator &rhs) {
    return lhs.state_ == rhs.state_;
  }

  friend bool operator!=(const CInputIterator &lhs, const CInputIterator &rhs) {
    return ! (lhs == rhs);
  }

  ContentsT operator*() const {
    return state_.contents();
  }

 protected:
  STATE state_;
};

//------

//! linked list
template<typename T>
class CListLink {
 public:
  typedef CListLink<T> ListLink;

  CListLink() {
    next = 0;
    prev = 0;
  }

  void addTo(T **head) {
    T *th = (T *) this;

    if (*head) {
      th     ->next       = *head;
      th     ->prev       = (*head)->prev;
      (*head)->prev       = th;
      th     ->prev->next = th;
    }
    else {
      *head = th;

      (*head)->next = (*head)->prev = th;
    }
  }

  void removeFrom(T **head) {
    T *th = (T *) this;

    if (*head) {
      if      (*head == (*head)->next)
        *head = 0;
      else if (th == *head)
        (*head) = (*head)->next;

      th->next->prev = th->prev;
      th->prev->next = th->next;
    }
  }

  //! iterator state
  class IteratorState {
   public:
    IteratorState(const ListLink *list=0) :
     list_(list), p_(list), end_(false) {
      end_ = (list_ == 0);
    }

    void next() {
      assert(p_ != 0);

      p_ = p_->next;

      end_ = (p_ == list_);
    }

    const T *contents() const {
      assert(p_ != 0);

      return (const T *) p_;
    }

    friend bool operator==(const IteratorState &lhs, const IteratorState &rhs) {
      if (lhs.end_ == rhs.end_) return true;
      if (lhs.end_ != rhs.end_) return false;

      return (lhs.p_ == rhs.p_);
    }

   private:
    const ListLink* list_;
    const ListLink* p_;
    bool            end_;
  };

  typedef CInputIterator<IteratorState, T *> iterator;

  iterator begin() const { return iterator(IteratorState(this)); }
  iterator end  () const { return iterator(IteratorState(0));}

 public:
  T* next { nullptr };
  T* prev { nullptr };
};

//---

//! Charts convex hull data
class CQChartsHull3D {
 private:
  /* Define vertex indices. */
  enum { IX = 0, IY = 1, IZ = 2 };

 public:
  /* Define structures for vertices, edges and faces */
  class Face;
  class Edge;
  class Vertex;

  //! hull vertex
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
      duplicate_ = nullptr;
      onHull_    = false;
      mark_      = false;

#ifdef CQChartsHull3D_CheckSafe
      checkSafe();
#endif
    }

    void assignNum() {
      num_ = count_++;
    }

    void reset() {
      duplicate_ = nullptr;
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

#if 0
    void print(std::ostream &os=std::cout) const {
      os << "(" << x() << ", " << y() << ", " << z() << ")";
    }

    friend std::ostream &operator<<(std::ostream &os, const Vertex &v) {
      v.print(os);

      return os;
    }
#endif

   private:
#ifdef CQChartsHull3D_CheckSafe
    void checkSafe() const {
      /* Range of safe coord values. */
      static int SAFE = INT_MAX;

      if ((fabs(x()) > SAFE) || (fabs(y()) > SAFE) || (fabs(z()) > SAFE)) {
        std::cout << "Coordinate of vertex below might be too large\n";
        print();
      }
    }
#endif

   private:
    static uint count_;

    double v_[3];                  //!< vertices
    uint   num_       { 0 };       //!< vertex number
    Edge*  duplicate_ { nullptr }; //!< pointer to incident cone edge (or NULL)
    bool   onHull_    { false };   //!< true iff point on hull.
    bool   mark_      { false };   //!< true iff point already processed.
  };

  typedef Vertex *PVertex;

  //-------

  //! hull edge
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

#if 0
    void print(std::ostream &os=std::cout) const {
      os << "(" << *start() << ", " << *end() << ")";
    }

    friend std::ostream &operator<<(std::ostream &os, const Edge &e) {
      e.print(os);

      return os;
    }
#endif

   private:
    Vertex* endpts_[2];
    Face*   adjface_[2];
    Face*   newface_; /* pointer to incident cone face. */
    bool    removed_; /* true iff edge should be removed. */
  };

  typedef Edge *PEdge;

  //-------

  //! hull face
  class Face : public CListLink<Face> {
   private:
    typedef CListLink<Face> ListLink;

   public:
    Face() {
      for (uint i = 0; i < 3; ++i) {
        edge_  [i] = nullptr;
        vertex_[i] = nullptr;
      }

      visible_ = false;
      lower_   = false;

      color_ = -1;

      vv_ = nullptr;
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

#if 0
    void print(std::ostream &os=std::cout) const {
      os << "(" << *vertex_[0] << ", " << *vertex_[1] << ", " << *vertex_[2] << ")";
    }

    friend std::ostream &operator<<(std::ostream &os, const Face &f) {
      f.print(os);

      return os;
    }
#endif

   private:
    void subVec(const double *a, const double *b, double c[3]) {
      for (int i = 0; i < 2; i++)
        c[i] = a[i] - b[i];
    }

   private:
    Edge*   edge_  [3];           //!< edges
    Vertex* vertex_[3];           //!< vertices
    bool    visible_ { false };   //!< true iff face visible from new point
    bool    lower_   { false };   //!< true iff is lower face
    int     color_   { -1 };      //!< face color
    Vertex* vv_      { nullptr }; //!< voronoi vertex
  };

  typedef Face *PFace;

  //-------

 public:
  CQChartsHull3D();

  virtual ~CQChartsHull3D();

  bool getUseLower() const { return useLower_; }
  void setUseLower(bool lower) { useLower_ = lower; }

  void setDebug(bool debug=true) { debug_ = debug; }
  void setCheck(bool check=true) { check_ = check; }

  void addVertex(double x, double y, double z);
  void addVertex(double x, double y);

  void clearVertices();

  bool calc();

  void getRange(double *xmin, double *ymin, double *zmin, double *xmax, double *ymax, double *zmax);

  //void dumpPS(const char *filename);

  typedef Vertex::iterator VertexIterator;
  typedef Edge::iterator   EdgeIterator;
  typedef Face::iterator   FaceIterator;

  VertexIterator verticesBegin() { assert(vertices_); return vertices_->beginIterator(); }
  VertexIterator verticesEnd  () { assert(vertices_); return vertices_->endIterator  (); }

  EdgeIterator edgesBegin() { assert(edges_); return edges_->beginIterator(); }
  EdgeIterator edgesEnd  () { assert(edges_); return edges_->endIterator  (); }

  FaceIterator facesBegin() { assert(faces_); return faces_->begin(); }
  FaceIterator facesEnd  () { assert(faces_); return faces_->end  (); }

  EdgeIterator voronoiEdgesBegin() { return vedges_->beginIterator(); }
  EdgeIterator voronoiEdgesEnd  () { return vedges_->endIterator  (); }

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

#if 0
  void    checks();
  void    consistency();
  void    convexity();
  void    checkEuler(uint numV, uint numE, uint numF);
  void    checkEndpts();
#endif

#if 0
  void    printOut(PVertex v);
  void    printVertices();
  void    printEdges();
  void    printFaces();
#endif

 protected:
  /* Global variable definitions */
  PVertex vertices_  { nullptr };
  PEdge   edges_     { nullptr };
  PFace   faces_     { nullptr };
  PVertex vvertices_ { nullptr };
  PEdge   vedges_    { nullptr };
  bool    useLower_  { false };
  bool    debug_     { false };
  bool    check_     { false };
};

#endif
