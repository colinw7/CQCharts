#ifndef CMATRIX_2D_H
#define CMATRIX_2D_H

#include <CPoint2D.h>
#include <CVector2D.h>
#include <cstring>
#include <sstream>

/* / a b tx \ */
/* | c d ty | */
/* \ 0 0 1  / */

/* / m00 m01 m02 \ */
/* | m10 m11 m12 | */
/* \ m20 m21 m22 / */

class CMatrix2D {
 public:
  enum class Type {
    IDENTITY
  };

 public:
  // constructor/destructor
  CMatrix2D() { }

 ~CMatrix2D() { }

  explicit CMatrix2D(Type type) {
    if (type == Type::IDENTITY)
      setIdentity();
    else
      assert(false && "Bad Matrix Type");
  }

  CMatrix2D(double a, double b, double c, double d) :
   m00_(a), m01_(b), m10_(c), m11_(d) {
    setBottomIdentity();
  }

  CMatrix2D(double a, double b, double c, double d, double tx, double ty) :
   m00_(a), m01_(b), m02_(tx), m10_(c), m11_(d), m12_(ty) {
    setBottomIdentity();
  }

  CMatrix2D(double m00, double m01, double m02, double m10, double m11, double m12,
            double m20, double m21, double m22) :
   m00_(m00), m01_(m01), m02_(m02), m10_(m10), m11_(m11), m12_(m12),
   m20_(m20), m21_(m21), m22_(m22) {
  }

  CMatrix2D(const double *m, int n) {
    if      (n == 4)
      setValues(m[0], m[1], m[2], m[3]);
    else if (n == 6)
      setValues(m[0], m[1], m[2], m[3], m[4], m[5]);
    else
     assert(false && "Invalid size");
  }

  CMatrix2D(const CVector2D &v0, const CVector2D &v1) :
   m00_(v0.getX()), m01_(v1.getX()), m10_(v0.getY()), m11_(v1.getY()) {
    setOuterIdentity ();
    setBottomIdentity();
  }

  CMatrix2D *dup() const {
    return new CMatrix2D(*this);
  }

  //------

  // copy operations
  CMatrix2D(const CMatrix2D &a) :
   m00_(a.m00_), m01_(a.m01_), m02_(a.m02_),
   m10_(a.m10_), m11_(a.m11_), m12_(a.m12_),
   m20_(a.m20_), m21_(a.m21_), m22_(a.m22_) {
  }

  const CMatrix2D &operator=(const CMatrix2D &a) {
    memcpy(&m00_, &a.m00_, 9*sizeof(double));

    return *this;
  }

  //------

  std::string toString() const {
    std::ostringstream ss;

    ss << *this;

    return ss.str();
  }

  // output
  void print(std::ostream &os) const {
    os << "((" << m00_ << "," << m01_ << "," << m02_ << ")" <<
          " (" << m10_ << "," << m11_ << "," << m12_ << ")" <<
          " (" << m20_ << "," << m21_ << "," << m22_ << "))";
  }

  friend std::ostream &operator<<(std::ostream &os, const CMatrix2D &matrix) {
    matrix.print(os);

    return os;
  }

  //------

  static CMatrix2D identity() {
    return CMatrix2D(Type::IDENTITY);
  }

  static CMatrix2D translation(const CPoint2D &point) {
    return translation(point.x, point.y);
  }

  static CMatrix2D translation(double tx, double ty) {
    CMatrix2D matrix;

    matrix.setTranslation(tx, ty);

    return matrix;
  }

  static CMatrix2D scale(const CPoint2D &point) {
    return scale(point.x, point.y);
  }

  static CMatrix2D scale(double s) {
    CMatrix2D matrix;

    matrix.setScale(s, s);

    return matrix;
  }

  static CMatrix2D scale(double sx, double sy) {
    CMatrix2D matrix;

    matrix.setScale(sx, sy);

    return matrix;
  }

  static CMatrix2D rotation(double a) {
    CMatrix2D matrix;

    matrix.setRotation(a);

    return matrix;
  }

  static CMatrix2D skew(double sx, double sy) {
    CMatrix2D matrix;

    matrix.setSkew(sx, sy);

    return matrix;
  }

  static CMatrix2D skewX(double a) {
    CMatrix2D matrix;

    matrix.setSkewX(a);

    return matrix;
  }

  static CMatrix2D skewY(double a) {
    CMatrix2D matrix;

    matrix.setSkewY(a);

    return matrix;
  }

  static CMatrix2D reflection(double a) {
    CMatrix2D matrix;

    matrix.setReflection(a);

    return matrix;
  }

  static CMatrix2D reflection(double dx, double dy) {
    CMatrix2D matrix;

    matrix.setReflection(dx, dy);

    return matrix;
  }

  void setIdentity() {
    setInnerIdentity ();
    setOuterIdentity ();
    setBottomIdentity();
  }

  void setTranslation(double tx, double ty) {
    setInnerIdentity ();
    setOuterTranslate(tx, ty);
    setBottomIdentity();
  }

  void setTranslation(const CVector2D &vector) {
    setInnerIdentity ();
    setOuterTranslate(vector.getX(), vector.getY());
    setBottomIdentity();
  }

  void setScale(double s) {
    setInnerScale    (s, s);
    setOuterIdentity ();
    setBottomIdentity();
  }

  void setScale(double sx, double sy) {
    setInnerScale    (sx, sy);
    setOuterIdentity ();
    setBottomIdentity();
  }

  void setScaleTranslation(double sx, double sy, double tx, double ty) {
    setInnerScale    (sx, sy);
    setOuterTranslate(tx, ty);
    setBottomIdentity();
  }

  void setScaleTranslation(double s, double tx, double ty) {
    setInnerScale    (s, s);
    setOuterTranslate(tx, ty);
    setBottomIdentity();
  }

  void setRotation(double a) {
    setInnerRotation (a);
    setOuterIdentity ();
    setBottomIdentity();
  }

  void setRotationTranslation(double a, double tx, double ty) {
    setInnerRotation (a);
    setOuterTranslate(tx, ty);
    setBottomIdentity();
  }

  void setReflection(double a) {
    setUnitInnerReflection(cos(a), sin(a));
    setOuterIdentity      ();
    setBottomIdentity     ();
  }

  void setReflection(double dx, double dy) {
    setInnerReflection(dx, dy);
    setOuterIdentity  ();
    setBottomIdentity ();
  }

  void setSkew(double x, double y) {
    setInnerSkew     (x, y);
    setOuterIdentity ();
    setBottomIdentity();
  }

  void setSkewX(double a) {
    setInnerSkewX    (a);
    setOuterIdentity ();
    setBottomIdentity();
  }

  void setSkewY(double a) {
    setInnerSkewY    (a);
    setOuterIdentity ();
    setBottomIdentity();
  }

  void setValues(double a, double b, double c, double d) {
    m00_ = a, m01_ = b;
    m10_ = c, m11_ = d;

    setOuterIdentity ();
    setBottomIdentity();
  }

  void setValues(double a, double b, double c, double d, double tx, double ty) {
    m00_ = a, m01_ = b, m02_ = tx;
    m10_ = c, m11_ = d, m12_ = ty;

    setBottomIdentity();
  }

  void setValues(double a, double b, double c, double d, double e, double f,
                 double g, double h, double i) {
    m00_ = a, m01_ = b, m02_ = c;
    m10_ = d, m11_ = e, m12_ = f;
    m20_ = g, m21_ = h, m22_ = i;
  }

  void setValues(const double *v, int n) {
    if      (n == 4)
      setValues(v[0], v[1], v[2], v[3]);
    else if (n == 6)
      setValues(v[0], v[1], v[2], v[3], v[4], v[5]);
    else if (n == 9)
      setValues(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8]);
    else
      assert(false && "Invalid Size");
  }

  void getValues(double *a, double *b, double *c, double *d) const {
    if (a) *a = m00_;
    if (b) *b = m01_;
    if (c) *c = m10_;
    if (d) *d = m11_;
  }

  void getValues(double *a, double *b, double *c, double *d, double *tx, double *ty) const {
    if (a ) *a  = m00_;
    if (b ) *b  = m01_;
    if (c ) *c  = m10_;
    if (d ) *d  = m11_;
    if (tx) *tx = m02_;
    if (ty) *ty = m12_;
  }

  void getValues(double *v, int n) const {
    if      (n == 4) {
      v[0] = m00_; v[1] = m01_;
      v[2] = m10_; v[3] = m11_;
    }
    else if (n == 6) {
      v[0] = m00_; v[1] = m01_;
      v[2] = m10_; v[3] = m11_;
      v[4] = m02_; v[5] = m12_;
    }
    else if (n == 9) {
      v[0] = m00_; v[1] = m01_; v[2] = m02_;
      v[3] = m10_; v[4] = m11_; v[5] = m12_;
      v[6] = m20_; v[7] = m21_; v[8] = m22_;
    }
    else
      assert(false && "Invalid Size");
  }

  //---------

  void setColumn(int c, double x, double y) {
    switch (c) {
      case 0: m00_ = x; m10_ = y; break;
      case 1: m01_ = x; m11_ = y; break;
    }
  }

  void setColumn(int c, const CPoint2D &point) {
    switch (c) {
      case 0: m00_ = point.x; m10_ = point.y; break;
      case 1: m01_ = point.x; m11_ = point.y; break;
    }
  }

  void setColumn(int c, const CVector2D &vector) {
    switch (c) {
      case 0: vector.getXY(&m00_, &m10_); break;
      case 1: vector.getXY(&m01_, &m11_); break;
    }
  }

  void setColumns(CVector2D &u, CVector2D &v) {
    setColumn(0, u);
    setColumn(1, v);
  }

  void getColumn(int c, double *x, double *y) const {
    switch (c) {
      case 0: if (x) *x = m00_; if (y) *y = m10_; break;
      case 1: if (x) *x = m01_; if (y) *y = m11_; break;
    }
  }

  void getColumn(int c, CVector2D &vector) {
    switch (c) {
      case 0: vector = CVector2D(m00_, m10_); break;
      case 1: vector = CVector2D(m01_, m11_); break;
    }
  }

  void getColumns(CVector2D &u, CVector2D &v) {
    getColumn(0, u);
    getColumn(1, v);
  }

  //------

  void setRow(int r, double x, double y) {
    switch (r) {
      case 0: m00_ = x; m01_ = y; break;
      case 1: m10_ = x; m11_ = y; break;
    }
  }

  void setRow(int r, const CPoint2D &point) {
    switch (r) {
      case 0: m00_ = point.x; m01_ = point.y; break;
      case 1: m10_ = point.x; m11_ = point.y; break;
    }
  }

  void setRow(int r, const CVector2D &vector) {
    switch (r) {
      case 0: vector.getXY(&m00_, &m01_); break;
      case 1: vector.getXY(&m10_, &m11_); break;
    }
  }

  void getRow(int r, double *x, double *y) const {
    switch (r) {
      case 0: if (x) *x = m00_; if (y) *y = m01_; break;
      case 1: if (x) *x = m10_; if (y) *y = m11_; break;
    }
  }

  //------

  void multiplyPoint(double xi, double yi, double *xo, double *yo) const {
    *xo = m00_*xi + m01_*yi + m02_;
    *yo = m10_*xi + m11_*yi + m12_;
  }

  void multiplyPoint(const CPoint2D &point1, CPoint2D &point2) const {
    point2.x = m00_*point1.x + m01_*point1.y + m02_;
    point2.y = m10_*point1.x + m11_*point1.y + m12_;
  }

  void multiplyVector(const CVector2D &ivector, CVector2D &ovector) const {
    double ix, iy;

    ivector.getXY(&ix, &iy);

    double ox = m00_*ix + m01_*iy;
    double oy = m10_*ix + m11_*iy;

    ovector.setXY(ox, oy);
  }

  void preMultiplyPoint(double xi, double yi, double *xo, double *yo) const {
    *xo = m00_*xi + m10_*yi;
    *yo = m01_*xi + m11_*yi;
  }

  void preMultiplyPoint(const CPoint2D &ipoint, CPoint2D &opoint) const {
    opoint.x = m00_*ipoint.x + m10_*ipoint.y;
    opoint.y = m01_*ipoint.x + m11_*ipoint.y;
  }

  void preMultiplyVector(const CVector2D &ivector, CVector2D &ovector) const {
    double ix, iy;

    ivector.getXY(&ix, &iy);

    double ox = m00_*ix + m10_*iy;
    double oy = m01_*ix + m11_*iy;

    ovector.setXY(ox, oy);
  }

  const CMatrix2D &translate(double x, double y) {
    m02_ += x;
    m12_ += y;

    return *this;
  }

  const CMatrix2D &transpose() {
    std::swap(m10_, m01_);
    std::swap(m20_, m02_);
    std::swap(m21_, m12_);

    return *this;
  }

  CMatrix2D transposed() const {
    return CMatrix2D(m00_, m10_, m20_, m01_, m11_, m21_, m02_, m12_, m22_);
  }

  bool invert(CMatrix2D &imatrix) const {
    double d = determinant();

    if (::fabs(d) == 0.0)
      return false;

    double id = 1.0/d;

    imatrix.m00_ =  id*calcDeterminant(m11_, m12_, m21_, m22_);
    imatrix.m10_ = -id*calcDeterminant(m10_, m12_, m20_, m22_);
    imatrix.m20_ =  id*calcDeterminant(m10_, m11_, m20_, m21_);

    imatrix.m01_ = -id*calcDeterminant(m01_, m02_, m21_, m22_);
    imatrix.m11_ =  id*calcDeterminant(m00_, m02_, m20_, m22_);
    imatrix.m21_ = -id*calcDeterminant(m00_, m01_, m20_, m21_);

    imatrix.m02_ =  id*calcDeterminant(m01_, m02_, m11_, m12_);
    imatrix.m12_ = -id*calcDeterminant(m00_, m02_, m10_, m12_);
    imatrix.m22_ =  id*calcDeterminant(m00_, m01_, m10_, m11_);

    return true;
  }

  CMatrix2D inverse() const {
    CMatrix2D imatrix;

    if (! invert(imatrix))
      assert(false && "Divide by zero");

    return imatrix;
  }

  double determinant() const {
    return (m00_*calcDeterminant(m11_, m12_, m21_, m22_) -
            m01_*calcDeterminant(m10_, m12_, m20_, m22_) +
            m02_*calcDeterminant(m10_, m11_, m20_, m21_));
  }

  const CMatrix2D &normalize() {
    double d = determinant();

    double id = 1.0/d;

    m00_ *= id;
    m01_ *= id;
    m10_ *= id;
    m11_ *= id;

    return *this;
  }

  void setTransform(double xmin1, double ymin1, double xmax1, double ymax1,
                    double xmin2, double ymin2, double xmax2, double ymax2) {
    double sx = (xmax2 - xmin2)/(xmax1 - xmin1);
    double sy = (ymax2 - ymin2)/(ymax1 - ymin1);

    double tx = -xmin1*sx + xmin2;
    double ty = -ymin1*sy + ymin2;

    setInnerScale    (sx, sy);
    setOuterTranslate(tx, ty);
    setBottomIdentity();
  }

  static CMatrix2D *newIdentityMatrix() {
    CMatrix2D *m = new CMatrix2D();

    m->setIdentity();

    return m;
  }

  static bool solveAXeqB(const CMatrix2D &a, CPoint2D &x, const CPoint2D &b) {
    double det_a = a.determinant();

    if (::fabs(det_a) <= 0.0)
      return false;

    double idet_a = 1.0/det_a;

    CMatrix2D t(a);

    t.setColumn(0, b.x, b.y);

    double det_t = t.determinant();

    x.x = det_t*idet_a;

    t = a;

    t.setColumn(1, b.x, b.y);

    det_t = t.determinant();

    x.y = det_t*idet_a;

    return true;
  }

  //------

  bool isIdentity() const {
    return isInnerIdentity() && isTranslateIdentity() &&
           realEq(m20_, 0.0) && realEq(m21_, 0.0) && realEq(m22_, 1.0);
  }

  bool isInnerIdentity() const {
    return realEq(m00_, 1.0) && realEq(m01_, 0.0) &&
           realEq(m10_, 0.0) && realEq(m11_, 1.0);
  }

  bool isTranslateIdentity() const {
    return realEq(m02_, 0.0) && realEq(m12_, 0.0);
  }

  bool isInnerRotation() const {
    return ! realEq(m01_, 0.0) && ! realEq(m10_, 0.0);
  }

  double getAngle() const {
    return asin(m10_);
  }

  void getSize(double *sx, double *sy) const {
    *sx = fabs(m00_ + m01_);
    *sy = fabs(m10_ + m11_);
  }

  void getTranslate(double *tx, double *ty) const {
    *tx = m02_;
    *ty = m12_;
  }

  //------

  void zero() { memset(&m00_, 0, 9*sizeof(double)); }

  const CMatrix2D &operator+=(const CMatrix2D &b) {
    m00_ += b.m00_; m01_ += b.m01_; m02_ += b.m02_;
    m10_ += b.m10_; m11_ += b.m11_; m12_ += b.m12_;
    m20_ += b.m20_; m21_ += b.m21_; m22_ += b.m22_;

    return *this;
  }

  CMatrix2D operator+(const CMatrix2D &b) const {
    CMatrix2D c = *this;

    c += b;

    return c;
  }

  const CMatrix2D &operator-=(const CMatrix2D &b) {
    m00_ -= b.m00_; m01_ -= b.m01_, m02_ -= b.m02_;
    m10_ -= b.m10_; m11_ -= b.m11_; m12_ -= b.m12_;
    m20_ -= b.m20_; m21_ -= b.m21_; m22_ -= b.m22_;

    return *this;
  }

  CMatrix2D operator-(const CMatrix2D &b) const {
    CMatrix2D c = *this;

    c -= b;

    return c;
  }

  const CMatrix2D &operator*=(const CMatrix2D &b) {
    CMatrix2D a = *this;

    m00_ = a.m00_*b.m00_ + a.m01_*b.m10_ + a.m02_*b.m20_;
    m01_ = a.m00_*b.m01_ + a.m01_*b.m11_ + a.m02_*b.m21_;
    m02_ = a.m00_*b.m02_ + a.m01_*b.m12_ + a.m02_*b.m22_;

    m10_ = a.m10_*b.m00_ + a.m11_*b.m10_ + a.m12_*b.m20_;
    m11_ = a.m10_*b.m01_ + a.m11_*b.m11_ + a.m12_*b.m21_;
    m12_ = a.m10_*b.m02_ + a.m11_*b.m12_ + a.m12_*b.m22_;

    m20_ = a.m20_*b.m00_ + a.m21_*b.m10_ + a.m22_*b.m20_;
    m21_ = a.m20_*b.m01_ + a.m21_*b.m11_ + a.m22_*b.m21_;
    m22_ = a.m20_*b.m02_ + a.m21_*b.m12_ + a.m22_*b.m22_;

    return *this;
  }

  CMatrix2D operator*(const CMatrix2D &b) const {
    CMatrix2D c = *this;

    c *= b;

    return c;
  }

  const CMatrix2D &operator*=(double s) {
    CMatrix2D a = *this;

    m00_ = a.m00_*s; m01_ = a.m01_*s; m02_ = a.m02_*s;
    m10_ = a.m10_*s; m11_ = a.m11_*s; m12_ = a.m12_*s;
    m20_ = a.m20_*s; m21_ = a.m21_*s; m22_ = a.m22_*s;

    return *this;
  }

  CMatrix2D operator*(double s) const {
    CMatrix2D c = *this;

    c *= s;

    return c;
  }

  friend CPoint2D operator*(const CMatrix2D &m, const CPoint2D &p) {
    CPoint2D p1;

    m.multiplyPoint(p, p1);

    return p1;
  }

  friend CPoint2D operator*(const CPoint2D &p, const CMatrix2D &m) {
    CPoint2D p1;

    m.preMultiplyPoint(p, p1);

    return p1;
  }

  friend CVector2D operator*(const CMatrix2D &m, const CVector2D &v) {
    CVector2D v1;

    m.multiplyVector(v, v1);

    return v1;
  }

  friend CVector2D operator*(const CVector2D &v, const CMatrix2D &m) {
    CVector2D v1;

    m.preMultiplyVector(v, v1);

    return v1;
  }

  const CMatrix2D &operator/=(const CMatrix2D &b) {
    CMatrix2D bi;

    if (! b.invert(bi)) {
      assert(false && "Divide by zero");
      return *this;
    }

    return (*this) *= bi;
  }

  CMatrix2D operator/(const CMatrix2D &b) const {
    CMatrix2D c = *this;

    c /= b;

    return c;
  }

  //------

  void setValue(uint i, double value) {
    (&m00_)[i] = value;
  }

  void setValue(uint i, uint j, double value) {
    assert(i < 3 && j < 3);

    double &m = (&m00_)[3*j + i];

    m = value;
  }

  const double &getValue(uint i) const {
    assert(i < 9);

    return (&m00_)[i];
  }

  const double &getValue(uint i, uint j) const {
    assert(i < 3 && j < 3);

    const double &m = (&m00_)[3*j + i];

    return m;
  }

  const double &operator[](uint i) const {
    assert(i < 9);

    return (&m00_)[i];
  }

  double &operator[](uint i) {
    assert(i < 9);

    return (&m00_)[i];
  }

  //------

  void setInnerIdentity() {
    m00_ = 1.0, m01_ = 0.0;
    m10_ = 0.0, m11_ = 1.0;
  }

  void setInnerScale(double sx, double sy) {
    m00_ = sx , m01_ = 0.0;
    m10_ = 0.0, m11_ = sy ;
  }

  void setInnerRotation(double a) {
    double c = ::cos(a);
    double s = ::sin(a);

    m00_ =  c, m01_ = -s;
    m10_ =  s, m11_ =  c;
  }

  void setInnerSkew(double x, double y) {
    double tx = ::tan(x);
    double ty = ::tan(y);

    m00_ = 1 , m01_ = tx;
    m10_ = ty, m11_ = 1 ;
  }

  void setInnerSkewX(double x) {
    double tx = ::tan(x);

    m00_ = 1, m01_ = tx;
    m10_ = 0, m11_ = 1 ;
  }

  void setInnerSkewY(double y) {
    double ty = ::tan(y);

    m00_ = 1 , m01_ = 0;
    m10_ = ty, m11_ = 1;
  }

  void setInnerReflection(double dx, double dy) {
    double l = sqrt(dx*dx + dy*dy);

    setUnitInnerReflection(dx/l, dy/l);
  }

  void setUnitInnerReflection(double dx, double dy) {
    //m00_ = (dx*dx - dy*dy)/l; m01_ = 2*dx*dy/l;
    //m10_ = m01_             ; m11_ = -m00_;

    m00_ = 2.0*dx*dx - 1.0; m01_ = 2.0*dx*dy;
    m10_ = m01_           ; m11_ = 2.0*dy*dy - 1.0;
  }

  void setOuterIdentity() {
    m02_ = 0.0; m12_ = 0.0;
  }

  void setOuterTranslate(double tx, double ty) {
    m02_ = tx; m12_ = ty;
  }

  void setBottomIdentity() {
    m20_ = 0.0, m21_ = 0.0, m22_ = 1.0;
  }

  //---

  int cmp(const CMatrix2D &m) const {
    { if (m00_ < m.m00_) return -1; } { if (m00_ > m.m00_) return 1; }
    { if (m10_ < m.m10_) return -1; } { if (m10_ > m.m10_) return 1; }
    { if (m01_ < m.m01_) return -1; } { if (m01_ > m.m01_) return 1; }
    { if (m11_ < m.m11_) return -1; } { if (m11_ > m.m11_) return 1; }
    { if (m02_ < m.m02_) return -1; } { if (m02_ > m.m02_) return 1; }
    { if (m12_ < m.m12_) return -1; } { if (m12_ > m.m12_) return 1; }
    { if (m20_ < m.m20_) return -1; } { if (m20_ > m.m20_) return 1; }
    { if (m21_ < m.m21_) return -1; } { if (m21_ > m.m21_) return 1; }
    { if (m22_ < m.m22_) return -1; } { if (m22_ > m.m22_) return 1; }

    return 0;
  }

  friend bool operator< (const CMatrix2D &lhs, const CMatrix2D &rhs) { return lhs.cmp(rhs) <  0; }
  friend bool operator<=(const CMatrix2D &lhs, const CMatrix2D &rhs) { return lhs.cmp(rhs) <= 0; }
  friend bool operator> (const CMatrix2D &lhs, const CMatrix2D &rhs) { return lhs.cmp(rhs) >  0; }
  friend bool operator>=(const CMatrix2D &lhs, const CMatrix2D &rhs) { return lhs.cmp(rhs) >= 0; }
  friend bool operator==(const CMatrix2D &lhs, const CMatrix2D &rhs) { return lhs.cmp(rhs) == 0; }
  friend bool operator!=(const CMatrix2D &lhs, const CMatrix2D &rhs) { return lhs.cmp(rhs) != 0; }

 private:
  static bool realEq(double r1, double r2) {
    return (fabs((r1) - (r2)) < 1E-5);
  }

  static double calcDeterminant(double m00, double m01, double m10, double m11) {
    return m00*m11 - m01*m10;
  }

 private:
  double m00_ { 0.0 }, m01_ { 0.0 }, m02_ { 0.0 };
  double m10_ { 0.0 }, m11_ { 0.0 }, m12_ { 0.0 };
  double m20_ { 0.0 }, m21_ { 0.0 }, m22_ { 0.0 };
};

#endif
