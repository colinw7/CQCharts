#ifndef CVECTOR_2D_H
#define CVECTOR_2D_H

#include <CPoint2D.h>
//#include <CMathGen.h>

#include <cassert>

class CVector2D {
 public:
  enum Type {
    ZERO,
    UNIT,
    UNIT_X,
    UNIT_Y
  };

 public:
  // constructor/destructor
  explicit CVector2D(double x=0, double y=0) : x_(x), y_(y) { }

 ~CVector2D() { }

  //------

  // copy operations
  CVector2D(const CVector2D &vector) :
   x_(vector.x_), y_(vector.y_), normalized_(vector.normalized_) {
  }

  CVector2D &operator=(const CVector2D &vector) {
    x_ = vector.x_; y_ = vector.y_;

    normalized_ = false;

    return *this;
  }

  //------

  // output
  void print(std::ostream &os) const {
    os << "(" << x_ << "," << y_ << ")";
  }

  friend std::ostream &operator<<(std::ostream &os, const CVector2D &vector) {
    vector.print(os);

    return os;
  }

  //------

  // accessors

  // get
  double x() const { return x_; }
  double y() const { return y_; }

  double getX() const { return x_; }
  double getY() const { return y_; }

  void getXY(double *x, double *y) const { *x = x_; *y = y_; }

  const double *getValues() const { return &x_; }

  // Reference routine would break encapsulation

  double operator[](uint i) const {
    assert(i < 2);

    return (&x_)[i];
  }

  // set
  void setX(double x) {
    x_          = x;
    normalized_ = false;
  }

  void setY(double y) {
    y_          = y;
    normalized_ = false;
  }

  void setXY(double x, double y) {
    x_          = x;
    y_          = y;
    normalized_ = false;
  }

  void iset(uint i, double v) {
    assert(i < 2);

    (&x_)[i] = v;

    normalized_ = false;
  }

  // more get accessors
  bool getNormalized() const { return normalized_; }

  //------

  double length() const {
    if (normalized_)
      return 1.0;

    return ::sqrt(lengthSqr());
  }

#if 0
  double fastLength() const {
    if (normalized_)
      return 1.0;

    return CMathGen::fastDistance(int(x_), int(y_));
  }
#endif

  double modulus() const {
    return length();
  }

  double lengthSqr() const {
    if (normalized_)
      return 1.0;

    return (x_*x_ + y_*y_);
  }

  //------

  // angle
  double angle() const {
    return atan2(y_, x_);
  }

  //------

  // comparison
  int cmp(const CVector2D &v) const {
    if      (x_ < v.x_) return -1;
    else if (x_ > v.x_) return 1;
    else if (y_ < v.y_) return -1;
    else if (y_ > v.y_) return 1;
    else                return 0;
  }

  friend bool operator==(const CVector2D &lhs, const CVector2D &rhs) {
    return lhs.cmp(rhs) == 0;
  }

  friend bool operator!=(const CVector2D &lhs, const CVector2D &rhs) {
    return lhs.cmp(rhs) != 0;
  }

  friend bool operator< (const CVector2D &lhs, const CVector2D &rhs) {
    return lhs.cmp(rhs) < 0;
  }

  friend bool operator<=(const CVector2D &lhs, const CVector2D &rhs) {
    return lhs.cmp(rhs) <= 0;
  }

  friend bool operator> (const CVector2D &lhs, const CVector2D &rhs) {
    return lhs.cmp(rhs) > 0;
  }

  friend bool operator>=(const CVector2D &lhs, const CVector2D &rhs) {
    return lhs.cmp(rhs) >= 0;
  }

  bool eq(const CVector2D &rhs) const {
    return fabs(x_ - rhs.x_) < 1E-6 &&
           fabs(y_ - rhs.y_) < 1E-6;
  }

  //------

  explicit CVector2D(const CPoint2D &point) :
   x_(point.x), y_(point.y), normalized_(false) {
  }

  CVector2D(const CPoint2D &point1, const CPoint2D &point2) :
   x_(point2.x - point1.x), y_(point2.y - point1.y),
   normalized_(false) {
  }

  CVector2D(const CVector2D &vector1, const CVector2D &vector2) :
   x_(vector2.x_ - vector1.x_), y_(vector2.y_ - vector1.y_),
   normalized_(false) {
  }

  //------

  CVector2D(Type type) {
    if      (type == ZERO  ) { x_ = 0.0; y_ = 0.0; }
    else if (type == UNIT  ) { x_ = 1.0; y_ = 1.0; }
    else if (type == UNIT_X) { x_ = 1.0; y_ = 0.0; }
    else if (type == UNIT_Y) { x_ = 0.0; y_ = 1.0; }
  }

  //------

  CPoint2D point() const {
    return CPoint2D(x_, y_);
  }

  //------

  CVector2D &zero() {
    x_ = 0.0; y_ = 0.0;

    normalized_ = false;

    return *this;
  }

  bool isZero() const {
    return realEq(lengthSqr(), 0);
  }

  bool isUnit() const {
    return realEq(lengthSqr(), 1);
  }

  //------

  CVector2D &normalize() {
    if (normalized_)
      return *this;

    double len = length();

    double factor = 0.0;

    if (len > 0.0)
      factor = 1.0/len;

    x_ *= factor;
    y_ *= factor;

    normalized_ = true;

    return *this;
  }

  CVector2D normalized() const {
    if (normalized_)
      return *this;

    double len = length();

    double factor = 0.0;

    if (len > 0.0)
      factor = 1.0/len;

    return CVector2D(x_*factor, y_*factor, true);
  }

  CVector2D unit() const {
    return normalized();
  }

  //------

  CVector2D perpendicular() const {
    return CVector2D(y_, -x_, normalized_);
  }

  CVector2D unitPerpendicular() const {
    return perpendicular().normalize();
  }

  double dotPerpendicular(const CVector2D &v) const {
    return x_*v.y_ - y_*v.x_;
  }

  //------

  CVector2D & setMagnitude(double magnitude) {
    double factor = 0.0;

    if (normalized_)
      factor = magnitude;
    else {
      double len = length();

      if (len > 0.0)
        factor = magnitude/len;
    }

    x_ *= factor; y_ *= factor;

    normalized_ = false;

    return *this;
  }

  //------

  double getDistance(const CVector2D &vector) {
    CVector2D diff = *this - vector;

    return diff.length();
  }

  double getDistanceSqr(const CVector2D &vector) {
    CVector2D diff = *this - vector;

    return diff.lengthSqr();
  }

  //------

  void incX(double x = 1.0) {
    x_ += x;

    normalized_ = false;
  }

  void incY(double y = 1.0) {
    y_ += y;

    normalized_ = false;
  }

  void decX(double x = 1.0) {
    x_ -= x;

    normalized_ = false;
  }

  void decY(double y = 1.0) {
    y_ -= y;

    normalized_ = false;
  }

  //------

  double minComponent() {
    return std::min(x_, y_);
  }

  double maxComponent() {
    return std::max(x_, y_);
  }

  double minAbsComponent() {
    return std::min(::fabs(x_), ::fabs(y_));
  }

  double maxAbsComponent() {
    return std::max(::fabs(x_), ::fabs(y_));
  }

  //------

  static CVector2D min(const CVector2D &lhs, const CVector2D &rhs) {
    return CVector2D(std::min(lhs.x_, rhs.x_), std::min(lhs.y_, rhs.y_));
  }

  static CVector2D max(const CVector2D &lhs, const CVector2D &rhs) {
    return CVector2D(std::max(lhs.x_, rhs.x_), std::max(lhs.y_, rhs.y_));
  }

  //------

  CVector2D &operator=(const CPoint2D &point) {
    x_ = point.x; y_ = point.y;

    normalized_ = false;

    return *this;
  }

  //------

  // operators

  // unary +/-
  CVector2D operator+() const {
    return CVector2D(x_, y_, normalized_);
  }

  CVector2D operator-() const {
    return CVector2D(-x_, -y_, normalized_);
  }

  // addition
  CVector2D &operator+=(const CVector2D &rhs) {
    x_ += rhs.x_; y_ += rhs.y_;

    normalized_ = false;

    return *this;
  }

  CVector2D operator+(const CVector2D &rhs) const {
    return CVector2D(x_ + rhs.x_, y_ + rhs.y_);
  }

  // subtraction
  CVector2D &operator-=(const CVector2D &rhs) {
    x_ -= rhs.x_; y_ -= rhs.y_;

    normalized_ = false;

    return *this;
  }

  CVector2D operator-(const CVector2D &rhs) const {
    return CVector2D(x_ - rhs.x_, y_ - rhs.y_);
  }

  // scalar multiplication/division
  CVector2D &operator*=(const CVector2D &rhs) {
    x_ *= rhs.x_; y_ *= rhs.y_;

    normalized_ = false;

    return *this;
  }

  CVector2D &operator*=(double rhs) {
    x_ *= rhs; y_ *= rhs;

    normalized_ = false;

    return *this;
  }

  CVector2D operator*(const CVector2D &rhs) {
    CVector2D t(*this);

    t *= rhs;

    return t;
  }

  friend CVector2D operator*(const CVector2D &lhs, double rhs) {
    return CVector2D(lhs.x_*rhs, lhs.y_*rhs);
  }

  friend CVector2D operator*(double lhs, const CVector2D &rhs) {
    return CVector2D(lhs*rhs.x_, lhs*rhs.y_);
  }

  CVector2D &operator/=(double rhs) {
    double irhs = 1.0/rhs;

    x_ *= irhs; y_ *= irhs;

    normalized_ = false;

    return *this;
  }

  CVector2D operator/(double rhs) const {
    double irhs = 1.0/rhs;

    return CVector2D(x_*irhs, y_*irhs);
  }

  //------

  // dot product
  double dotProduct(const CVector2D &vector) const {
    return (x_*vector.x_ + y_*vector.y_);
  }

  double dotProduct(const CPoint2D &point) const {
    return dotProduct(CVector2D(point.x, point.y));
  }

  double dotProduct(double x, double y) const {
    return dotProduct(CVector2D(x, y));
  }

  static double dotProduct(const CVector2D &vector1, const CVector2D &vector2) {
    return vector1.dotProduct(vector2);
  }

  static double dotProduct(const CVector2D &vector1, double x2, double y2) {
    return vector1.dotProduct(CVector2D(x2, y2));
  }

  double dotProductSelf() const {
    return dotProduct(*this);
  }

  static double absDotProduct(const CVector2D &vector1, const CVector2D &vector2) {
    return ::fabs(dotProduct(vector1, vector2));
  }

  //------

  // cross product
  CVector2D crossProduct(const CVector2D &vector) const {
    return CVector2D(y_*vector.x_ - x_*vector.y_,
                     x_*vector.y_ - y_*vector.x_,
                     normalized_ && vector.normalized_);
  }

  CVector2D crossProduct(const CPoint2D &point) const {
    return crossProduct(CVector2D(point.x, point.y));
  }

  CVector2D crossProduct(double x, double y) const {
    return crossProduct(CVector2D(x, y));
  }

  static CVector2D crossProduct(const CVector2D &vector1, const CVector2D &vector2) {
    return vector1.crossProduct(vector2);
  }

  CVector2D unitCrossProduct(const CVector2D &vector) const {
    return crossProduct(vector).normalize();
  }

  // Note: area of parallelogram is
  // CVector v1(x2 - x1, y2 - y1), v2(x4 - x1, y4 - y1);
  // area = v1.crossProduct(v2).length();

  //------

  friend CPoint2D operator+(const CPoint2D &lhs, const CVector2D &rhs) {
    return CPoint2D(lhs.x + rhs.x_, lhs.y + rhs.y_);
  }

  friend CPoint2D operator+=(CPoint2D &lhs, const CVector2D &rhs) {
    lhs.x += rhs.x_; lhs.y += rhs.y_;

    return lhs;
  }

  friend CPoint2D operator-(const CPoint2D &lhs, const CVector2D &rhs) {
    return CPoint2D(lhs.x - rhs.x_, lhs.y - rhs.y_);
  }

  friend CPoint2D operator-=(CPoint2D &lhs, const CVector2D &rhs) {
    lhs.x -= rhs.x_; lhs.y -= rhs.y_;

    return lhs;
  }

/*
  friend CVector2D operator-(const CPoint2D &lhs, const CPoint2D &rhs) {
    return CVector2D(lhs.x - rhs.x, lhs.y - rhs.y);
  }
*/

  //------

  CVector2D normal(const CVector2D &vector2) const {
    return unitCrossProduct(vector2);
  }

  static CVector2D normal(const CVector2D &vector1, const CVector2D &vector2) {
    return vector1.normal(vector2);
  }

  //------

  double cosIncluded(const CVector2D &vector1) const {
    double dot = dotProduct(vector1);

    if (! normalized_)
      dot /= length();

    if (! vector1.normalized_)
      dot /= vector1.length();

    return dot;
  }

  static double cosIncluded(const CVector2D &vector1, const CVector2D &vector2) {
    return vector1.cosIncluded(vector2);
  }

  //------

  void getBarycentrics(const CVector2D &vector1, const CVector2D &vector2,
                       const CVector2D &vector3, double barycentrics[3]) const {
    // compute the vectors relative to V2 of the triangle
    CVector2D diff[3] = {
      vector1 - vector3,
      vector2 - vector3,
      *this   - vector3
    };

    // If the vertices have large magnitude, the linear system of equations
    // for computing barycentric coordinates can be ill-conditioned.  To avoid
    // this, uniformly scale the triangle edges to be of order 1.  The scaling
    // of all differences does not change the barycentric coordinates.
    double maxval = 0.0;

    for (int i = 0; i < 2; ++i)
      for (int j = 0; j < 2; ++j)
        maxval = std::max(maxval, std::fabs(diff[i][j]));

    // scale down only large data
    if (maxval > 1.0) {
      double imaxval = 1.0/maxval;

      for (int i = 0; i < 3; i++)
        diff[i] *= imaxval;
    }

    double det = diff[0].dotPerpendicular(diff[1]);
    if (::fabs(det) > 1E-6 ) {
      double idet = 1.0/det;

      barycentrics[0] = diff[2].dotPerpendicular(diff[1])*idet;
      barycentrics[1] = diff[0].dotPerpendicular(diff[2])*idet;
      barycentrics[2] = 1.0 - barycentrics[0] - barycentrics[1];
    }
    else {
      // The triangle is a sliver. Determine the longest edge and
      // compute barycentric coordinates with respect to that edge.
      CVector2D v12 = vector1 - vector2;

      double max_length = v12.lengthSqr();

      int max_ind = 2;

      double fSqrLength = diff[1].lengthSqr();

      if (fSqrLength > max_length ) {
        max_ind    = 1;
        max_length = fSqrLength;
      }

      fSqrLength = diff[0].lengthSqr();

      if (fSqrLength > max_length) {
        max_ind    = 2;
        max_length = fSqrLength;
      }

      if (max_length > 1E-6) {
        double imax_length = 1.0/max_length;

        if      (max_ind == 0) {
          // P-V2 = t(V0-V2)
          barycentrics[0] = diff[2].dotProduct(diff[0])*imax_length;
          barycentrics[1] = 0.0;
          barycentrics[2] = 1.0 - barycentrics[0];
        }
        else if (max_ind == 1) {
          // P-V2 = t(V1-V2)
          barycentrics[0] = 0.0;
          barycentrics[1] = diff[2].dotProduct(diff[1])*imax_length;
          barycentrics[2] = 1.0 - barycentrics[1];
        }
        else {
          // P-V1 = t(V0-V1)
          diff[2] = *this - vector2;
          barycentrics[0] = diff[2].dotProduct(v12)*imax_length;
          barycentrics[1] = 1.0 - barycentrics[0];
          barycentrics[2] = 0.0;
        }
      }
      else {
        // triangle is a nearly a point, just return equal weights
        barycentrics[0] = 0.333333333;
        barycentrics[1] = barycentrics[0];
        barycentrics[2] = barycentrics[0];
      }
    }
  }

  //------

  void orthonormalize(CVector2D &u, CVector2D &v) {
    // If the input vectors are v0 and v1, then the Gram-Schmidt
    // orthonormalization produces vectors u0 and u1 as follows,
    //
    //   u0 = v0/|v0|
    //   u1 = (v1-(u0*v1)u0)/|v1-(u0*v1)u0|
    //
    // where |A| indicates length of vector A and A*B indicates dot
    // product of vectors A and B.

    // compute u0
    u.normalize();

    // compute u1
    double d = u.dotProduct(v);

    v -= u*d;

    v.normalize();
  }

  //------

  void generateOrthonormalBasis(CVector2D &u, CVector2D &v) {
    v.normalize();

    u = v.perpendicular();
  }

  //------

 private:
  static bool realEq(double r1, double r2, double tol=1E-6) {
    return (std::fabs(r1 - r2) < tol);
  }

 private:
  CVector2D(double x, double y, bool normalized) :
   x_(x), y_(y), normalized_(normalized) {
  }

 private:
  double x_          { 0 };
  double y_          { 0 };
  bool   normalized_ { false };
};

#endif
