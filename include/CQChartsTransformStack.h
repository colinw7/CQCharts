#ifndef CQChartsTransformStack_H
#define CQChartsTransformStack_H

#include <QTransform>
#include <cassert>

class CQChartsTransformStack {
 public:
  enum TransformType {
    NONE,
    TRANSLATE,
    SCALE1,
    SCALE2,
    ROTATE,
    ROTATE_ORIGIN,
    TRANSFORM
  };

 public:
  class Transform {
   public:
    enum { NUM_VALUES = 6 };

   public:
    Transform(TransformType type, double v) :
     type_(type) {
      memset(&v_[0], 0, NUM_VALUES*sizeof(double));

      v_[0] = v;
    }

    Transform(TransformType type, double v1, double v2) :
     type_(type) {
      memset(&v_[0], 0, NUM_VALUES*sizeof(double));

      v_[0] = v1;
      v_[1] = v2;
    }

    Transform(double a, const QPointF &p) :
     type_(TransformType::ROTATE_ORIGIN) {
      memset(&v_[0], 0, NUM_VALUES*sizeof(double));

      v_[0] = a;
      v_[1] = p.x();
      v_[2] = p.y();
    }

    Transform(const QTransform &t) :
     type_(TransformType::TRANSFORM) {
      v_[0] = t.m11();
      v_[1] = t.m12();
      v_[2] = t.m21();
      v_[3] = t.m22();
      v_[4] = t.dx();
      v_[5] = t.dy();
    }

    TransformType type() const { return type_; }

    double angle() const {
      assert(type_ == TransformType::ROTATE || type_ == TransformType::ROTATE_ORIGIN);
      return value(0);
    }

    double xscale() const {
      assert(type_ == TransformType::SCALE1 || type_ == TransformType::SCALE2);
      return value(0);
    }

    double yscale() const {
      assert(type_ == TransformType::SCALE1 || type_ == TransformType::SCALE2);
      return value(1);
    }

    double dx() const { assert(type_ == TransformType::TRANSLATE); return value(0); }
    double dy() const { assert(type_ == TransformType::TRANSLATE); return value(1); }

    double xo() const { assert(type_ == TransformType::ROTATE_ORIGIN); return value(0); }
    double yo() const { assert(type_ == TransformType::ROTATE_ORIGIN); return value(1); }

    double value(int i) const { return v_[i]; }

    const double *values() const { return &v_[0]; }

    QTransform rotateTransform() const { QTransform t; t.rotate(v_[0]); return t; }

    QTransform rotateOriginTransform() const {
      QTransform t1 = QTransform::fromTranslate(-v_[1], -v_[2]);
      QTransform t2 = rotateTransform();
      QTransform t3 = QTransform::fromTranslate( v_[1],  v_[2]);

      return t3*t2*t1;
    }

    QTransform valueTransform() const {
      return QTransform(v_[0], v_[1], v_[2], v_[3], v_[4], v_[5]);
    }

    QTransform calcTransform() const {
      switch (type_) {
        case TransformType::TRANSLATE    : return QTransform::fromTranslate(v_[0], v_[1]);
        case TransformType::SCALE1       : return QTransform::fromScale    (v_[0], v_[0]);
        case TransformType::SCALE2       : return QTransform::fromScale    (v_[0], v_[1]);
        case TransformType::ROTATE       : return rotateTransform      ();
        case TransformType::ROTATE_ORIGIN: return rotateOriginTransform();
        case TransformType::TRANSFORM    : return valueTransform       ();
        default                          : assert(false); return QTransform();
      }
    }

    std::string name() const {
      switch (type_) {
        case TransformType::TRANSLATE    : return "translate";
        case TransformType::SCALE1       : return "scale";
        case TransformType::SCALE2       : return "scale";
        case TransformType::ROTATE       : return "rotate";
        case TransformType::ROTATE_ORIGIN: return "rotate";
        case TransformType::TRANSFORM    : return "transform";
        default                          : assert(false); return "";
      }
    }

    void printValues(std::ostream &os, int n) const {
      for (int i = 0; i < n; ++i) {
        if (i > 0) os << ",";

        os << v_[i];
      }
    }

    void printParts(std::ostream &os) const {
      switch (type_) {
        case TransformType::TRANSLATE    : printValues(os, 2); return;
        case TransformType::SCALE1       : printValues(os, 1); return;
        case TransformType::SCALE2       : printValues(os, 2); return;
        case TransformType::ROTATE       : printValues(os, 1); return;
        case TransformType::ROTATE_ORIGIN: printValues(os, 3); return;
        case TransformType::TRANSFORM    : printValues(os, 6); return;
        default                          : assert(false)     ; return;
      }
    }

    private:
    TransformType type_;
    double        v_[NUM_VALUES];
  };

  typedef std::vector<Transform> TransformStack;

  //---

 public:
  CQChartsTransformStack();

  CQChartsTransformStack(const CQChartsTransformStack &t);

  explicit CQChartsTransformStack(const QTransform &t);

  const CQChartsTransformStack &operator=(const CQChartsTransformStack &t);

  const TransformStack &transformStack() const { return transformStack_; }

  void translate(const QPointF &d);

  void scale(double s);
  void scale(double sx, double sy);

  void rotate(double a);
  void rotate(double a, const QPointF &o);

  void transform(double m00, double m01, double m10, double m11, double tx, double ty);
  void transform(const QTransform &t);

  void reset();

  bool isEmpty() const;

  void append(const CQChartsTransformStack &t);

  const QTransform &getTransform() const;

  void multiplyPoint(const QPointF &point1, QPointF &point2) const;

  friend std::ostream &operator<<(std::ostream &os, const CQChartsTransformStack &ts) {
    ts.print(os);

    return os;
  }

  void print(std::ostream &os) const;

 private:
  TransformStack transformStack_;
  bool           tValid_ { false };
  QTransform     t_;
};

#endif
