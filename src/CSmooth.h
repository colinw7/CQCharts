#ifndef CSMOOTH_H
#define CSMOOTH_H

#include <CPoint2D.h>
#include <C2Bezier2D.h>
#include <C3Bezier2D.h>

// smooth curve through set of points
class CSmooth {
 public:
  typedef std::vector<CPoint2D> Points;

 private:
  class Segment {
   public:
    Segment() { }

    virtual ~Segment() { }

    virtual bool inside(const CSmooth *smooth, double x) const = 0;

    virtual double interp(const CSmooth *smooth, double x) const = 0;

    virtual CPoint2D controlPoint1(const CSmooth *smooth) const = 0;
    virtual CPoint2D controlPoint2(const CSmooth *smooth) const = 0;
  };

  class Line : public Segment {
   public:
    Line(int i) : i_(i) { }

    bool inside(const CSmooth *smooth, double x) const override {
      const CPoint2D &p1 = smooth->point(i_    );
      const CPoint2D &p2 = smooth->point(i_ + 1);

      return (x >= p1.x && x <= p2.x);
    }

    double interp(const CSmooth *smooth, double x) const override {
      const CPoint2D &p1 = smooth->point(i_    );
      const CPoint2D &p2 = smooth->point(i_ + 1);

      if (p1.x == p2.x) return p1.y;

      double m = (x - p1.x)/(p2.x - p1.x);
      double y = (1 - m)*p1.y + m*p2.y;

      return y;
    }

    CPoint2D controlPoint1(const CSmooth *smooth) const override { return smooth->point(i_    ); }
    CPoint2D controlPoint2(const CSmooth *smooth) const override { return smooth->point(i_ + 1); }

   private:
    int i_;
  };

  class Curve2 : public Segment {
   public:
    Curve2(int i, const CPoint2D &mp) : i_(i), mp_(mp) { }

    bool inside(const CSmooth *smooth, double x) const override {
      const CPoint2D &p1 = smooth->point(i_    );
      const CPoint2D &p2 = smooth->point(i_ + 1);

      return (x >= p1.x && x <= p2.x);
    }

    double interp(const CSmooth *smooth, double x) const override {
      const CPoint2D &p1 = smooth->point(i_    );
      const CPoint2D &p3 = smooth->point(i_ + 1);

      C2Bezier2D bezier(p1, mp_, p3);

      double t = (x - p1.x)/(p3.x - p1.x);

      CPoint2D pi;

      bezier.calc(t, pi);

      return pi.y;
    }

    CPoint2D controlPoint1(const CSmooth *) const override { return mp_; }
    CPoint2D controlPoint2(const CSmooth *) const override { return mp_; }

   private:
    int      i_;
    CPoint2D mp_;
  };

  class Curve3 : public Segment {
   public:
    Curve3(int i, const CPoint2D &mp1, const CPoint2D &mp2) : i_(i), mp1_(mp1), mp2_(mp2) { }

    bool inside(const CSmooth *smooth, double x) const override {
      const CPoint2D &p1 = smooth->point(i_    );
      const CPoint2D &p2 = smooth->point(i_ + 1);

      return (x >= p1.x && x <= p2.x);
    }

    double interp(const CSmooth *smooth, double x) const override {
      const CPoint2D &p1 = smooth->point(i_    );
      const CPoint2D &p3 = smooth->point(i_ + 1);

      C3Bezier2D bezier(p1, mp1_, mp2_, p3);

      double t = (x - p1.x)/(p3.x - p1.x);

      CPoint2D pi;

      bezier.calc(t, pi);

      return pi.y;
    }

    CPoint2D controlPoint1(const CSmooth *) const override { return mp1_; }
    CPoint2D controlPoint2(const CSmooth *) const override { return mp2_; }

   private:
    int      i_;
    CPoint2D mp1_;
    CPoint2D mp2_;
  };

 public:
  CSmooth(const Points &points=Points()) :
   points_(points) {
    reset();

    smooth();
  }

 ~CSmooth() {
    reset();
  }

  void clearPoints() {
    points_.clear();

    reset();
  }

  void setPoints(const Points &points) {
    points_ = points;

    reset();

    smooth();
  }

  void addPoint(const CPoint2D &p) {
    points_.push_back(p);

    reset();

    smooth();
  }

  void sort() {
    std::sort(points_.begin(), points_.end(),
              [](const CPoint2D &p1, const CPoint2D &p2) { return p1.x < p2.x; });
  }

  double interp(double x) {
    if (segments_.empty()) {
      // single point
      if (points_.size() == 1)
        return points_[0].y;
      // assert ?
      else
        return 0;
    }

    //---

    // find segment for x (must be monotonic ?)
    for (auto &s : segments_) {
      if (s->inside(this, x))
        return s->interp(this, x);
    }

    if (x <= points_[0].x)
      return segments_.front()->interp(this, x);
    else
      return segments_.back ()->interp(this, x);
  }

  const Points &points() { return points_; }

  int numPoints() const { return points_.size(); }

  const CPoint2D &point(int i) const { return points_[i]; }

  CPoint2D controlPoint1(int i) const {
    return segments_[i]->controlPoint1(this);
  }

  CPoint2D controlPoint2(int i) const {
    return segments_[i]->controlPoint2(this);
  }

 private:
  void reset() {
    for (auto &s : segments_)
      delete s;

    segments_.clear();
  }

  void smooth() {
    sort();

    if (points_.size() < 2)
      return;

    if (points_.size() == 2) {
      segments_.push_back(new Line(0));
      return;
    }

    // generate beziers for each point pair
    double g1, c1, g2, c2;

    for (int i1 = 0, i2 = 1, i3 = 2; i3 < int(points_.size()); i1 = i2, i2 = i3++) {
      if (points_[i1].x == points_[i2].x || points_[i2].x == points_[i3].x) {
        segments_.push_back(new Line(i1));
        continue;
      }

      double gl1 = (points_[i2].y - points_[i1].y)/(points_[i2].x - points_[i1].x);
      double gl2 = (points_[i3].y - points_[i2].y)/(points_[i3].x - points_[i2].x);

      g2 = gl1 + gl2;
      c2 = points_[i2].y - points_[i2].x*g2;

      if (i1 > 0) {
        double dx = (points_[i2].x - points_[i1].x)/3;

        double mx1 = points_[i1].x + dx;
        double mx2 = points_[i2].x - dx;

        CPoint2D mp1(mx1, g1*mx1 + c1);
        CPoint2D mp2(mx2, g2*mx2 + c2);

        segments_.push_back(new Curve3(i1, mp1, mp2));
      }
      else {
        double dx = (points_[i2].x - points_[i1].x)/3;

        double mx = points_[i2].x - dx;

        CPoint2D mp(mx, g2*mx + c2);

        segments_.push_back(new Curve2(i1, mp));
      }

      g1 = g2;
      c1 = c2;
    }

    // add last segment
    int i1 = points_.size() - 3;
    int i2 = i1 + 1;
    int i3 = i2 + 1;

    if (points_[i1].x == points_[i2].x || points_[i2].x == points_[i3].x) {
      segments_.push_back(new Line(i2));
      return;
    }

    double gl1 = (points_[i2].y - points_[i1].y)/(points_[i2].x - points_[i1].x);
    double gl2 = (points_[i3].y - points_[i2].y)/(points_[i3].x - points_[i2].x);

    double g = gl1 + gl2;
    double c = points_[i2].y - points_[i2].x*g;

    double mx = (points_[i2].x + points_[i3].x)/2;

    CPoint2D mp(mx, g*mx + c);

    segments_.push_back(new Curve2(i2, mp));
  }

 private:
  typedef std::vector<Segment *> Segments;

  Points   points_;
  Segments segments_;
};

#endif
