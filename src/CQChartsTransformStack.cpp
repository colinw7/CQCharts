#include <CQChartsTransformStack.h>
#include <CMathUtil.h>

CQChartsTransformStack::
CQChartsTransformStack()
{
}

CQChartsTransformStack::
CQChartsTransformStack(const CQChartsTransformStack &t) :
 transformStack_(t.transformStack_), tValid_(t.tValid_), t_(t.t_)
{
}

CQChartsTransformStack::
CQChartsTransformStack(const QTransform &t)
{
  assert(t.isAffine());

  if (CMathUtil::realEq(t.m11(), 1.0) && CMathUtil::realEq(t.m12(), 0.0) &&
      CMathUtil::realEq(t.m21(), 0.0) && CMathUtil::realEq(t.m22(), 1.0)) {
    double dx = t.dx();
    double dy = t.dy();

    translate(QPointF(dx, dy));
  }
  else {
    transformStack_.push_back(Transform(t));
  }

  tValid_ = false;
}

const CQChartsTransformStack &
CQChartsTransformStack::
operator=(const CQChartsTransformStack &t)
{
  transformStack_ = t.transformStack_;
  tValid_         = t.tValid_;
  t_              = t.t_;

  return *this;
}

void
CQChartsTransformStack::
translate(const QPointF &d)
{
  transformStack_.push_back(Transform(TransformType::TRANSLATE, d.x(), d.y()));

  tValid_ = false;
}

void
CQChartsTransformStack::
scale(double s)
{
  transformStack_.push_back(Transform(TransformType::SCALE1, s));

  tValid_ = false;
}

void
CQChartsTransformStack::
scale(double sx, double sy)
{
  transformStack_.push_back(Transform(TransformType::SCALE2, sx, sy));

  tValid_ = false;
}

void
CQChartsTransformStack::
rotate(double a)
{
  transformStack_.push_back(Transform(TransformType::ROTATE, a));

  tValid_ = false;
}

void
CQChartsTransformStack::
rotate(double a, const QPointF &o)
{
  transformStack_.push_back(Transform(a, o));

  tValid_ = false;
}

void
CQChartsTransformStack::
transform(double m00, double m01, double m10, double m11, double tx, double ty)
{
  QTransform t = QTransform(m00, m01, m10, m11, tx, ty);

  transformStack_.push_back(Transform(t));

  tValid_ = false;
}

void
CQChartsTransformStack::
transform(const QTransform &t)
{
  transformStack_.push_back(Transform(t));

  tValid_ = false;
}

void
CQChartsTransformStack::
reset()
{
  transformStack_.clear();

  tValid_ = false;
}

bool
CQChartsTransformStack::
isEmpty() const
{
  return transformStack_.empty();
}

void
CQChartsTransformStack::
append(const CQChartsTransformStack &t)
{
  for (const auto &t : t.transformStack_)
    transformStack_.push_back(t);

  tValid_ = false;
}

const QTransform &
CQChartsTransformStack::
getTransform() const
{
  if (! tValid_) {
    CQChartsTransformStack *th = const_cast<CQChartsTransformStack *>(this);

    th->t_ = QTransform();

    uint num = transformStack_.size();

    for (uint i = 0; i < num; ++i) {
      const Transform &t = transformStack_[i];

      th->t_ = t.calcTransform()*t_;
    }

    th->tValid_ = true;
  }

  return t_;
}

void
CQChartsTransformStack::
multiplyPoint(const QPointF &point1, QPointF &point2) const
{
  point2 = getTransform().map(point1);
}

void
CQChartsTransformStack::
print(std::ostream &os) const
{
  uint num = transformStack_.size();

  for (uint i = 0; i < num; ++i) {
    const Transform &t = transformStack_[i];

    if (i > 0) os << " ";

    os << t.name() << "(";

    t.printParts(os);

    os << ")";
  }
}
