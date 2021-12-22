#include <CQBucketer.h>
#include <CInterval.h>
#include <CMathRound.h>
#include <CMathUtil.h>

CQBucketer::
CQBucketer()
{
}

void
CQBucketer::
setType(const Type &t)
{
  if (type_ != t) {
    // use auto delete for fixed delta if switching from auto to fixed
    if (type_ == Type::REAL_AUTO && t == Type::REAL_RANGE)
      setRDelta(calcDelta());

    type_ = t;

    reset();

    needsCalc_ = true;
  }
}

void
CQBucketer::
reset()
{
  stringInd_.clear();
  indString_.clear();

  rstops_.clear();
  istops_.clear();
}

// get bucket for generic value
int
CQBucketer::
bucket(const QVariant &var) const
{
  int n = INT_MIN; // optional ?

  bool ok;

  if      (type() == Type::STRING) {
    auto str = var.toString();

    return stringBucket(str);
  }
  else if (type() == Type::INTEGER_RANGE) {
    int i = varInt(var, ok);

    if (ok)
      return intBucket(i);
  }
  else if (type() == Type::REAL_RANGE) {
    double r = varReal(var, ok);

    if (ok)
      return realBucket(r);
  }
  else if (type() == Type::REAL_AUTO) {
    double r = varReal(var, ok);

    if (ok)
      return autoRealBucket(r);
  }
  else if (type() == Type::FIXED_STOPS) {
    if      (! rstops_.empty()) {
      double r = varReal(var, ok);

      if (ok)
        return stopsRealBucket(r);
    }
    else if (! istops_.empty()) {
      int i = varInt(var, ok);

      if (ok)
        return stopsIntBucket(i);
    }
  }

  return n;
}

bool
CQBucketer::
bucketValues(int bucket, double &min, double &max) const
{
  if      (type() == Type::INTEGER_RANGE) {
    int imin = 0, imax = 0;

    bool rc = bucketIValues(bucket, imin, imax);

    min = imin;
    max = imax;

    return rc;
  }
  else if (type() == Type::REAL_RANGE) {
    return bucketRValues(bucket, min, max);
  }
  else if (type() == Type::REAL_AUTO) {
    return autoBucketValues(bucket, min, max);
  }
  else if (type() == Type::FIXED_STOPS) {
    if      (! rstops_.empty())
      return rstopsBucketValues(bucket, min, max);
    else if (! istops_.empty())
      return istopsBucketValues(bucket, min, max);
  }

  return false;
}

bool
CQBucketer::
bucketIValues(int bucket, int &min, int &max) const
{
  int idelta = this->idelta();

  if (idelta > 0) {
    min = bucket*idelta + calcIStart();
    max = min + idelta;
  }

  return true;
}

bool
CQBucketer::
bucketRValues(int bucket, double &min, double &max) const
{
  double rdelta = this->rdelta();

  if (rdelta > 0.0) {
    min = bucket*rdelta + calcRStart();
    max = min + rdelta;
  }

  return true;
}

bool
CQBucketer::
autoBucketValues(int bucket, double &min, double &max) const
{
  double rdelta = this->calcDelta();

  if (rdelta > 0.0) {
    min = bucket*rdelta + this->calcMin();
    max = min + rdelta;
  }

  return true;
}

bool
CQBucketer::
rstopsBucketValues(int bucket, double &min, double &max) const
{
  int n = 0;

  min = CMathUtil::getNegInf();

  for (const auto &s : rstops_) {
    max = s;

    if (bucket == n)
      return true;

    min = max;

    ++n;
  }

  max = CMathUtil::getPosInf();

  if (bucket == n)
    return true;

  return false;
}

bool
CQBucketer::
istopsBucketValues(int bucket, double &min, double &max) const
{
  int n = 0;

  min = -INT_MAX;

  for (const auto &s : istops_) {
    max = s;

    if (bucket == n)
      return true;

    min = max;

    ++n;
  }

  max = INT_MAX;

  if (bucket == n)
    return true;

  return false;
}

//----

int
CQBucketer::
varInt(const QVariant &var, bool &ok)
{
  ok = true;

  if (var.type() == QVariant::Int)
    return var.toInt();

  return var.toString().toInt(&ok);
}

double
CQBucketer::
varReal(const QVariant &var, bool &ok)
{
  ok = true;

  if (var.type() == QVariant::Double)
    return var.toReal();

  return var.toString().toDouble(&ok);
}

//----

void
CQBucketer::
bucketMinMax(int bucket, QVariant &min, QVariant &max) const
{
  if      (type() == Type::STRING) {
    min = bucket;
    max = bucket;
  }
  else if (type() == Type::INTEGER_RANGE) {
    int imin = 0, imax = 0;

    bucketIValues(bucket, imin, imax);

    min = imin;
    max = imax;
  }
  else if (type() == Type::REAL_RANGE) {
    double rmin = 0.0, rmax = 0.0;

    bucketRValues(bucket, rmin, rmax);

    if (isIntegral()) {
      int imin = CMathRound::RoundNearest(rmin);
      int imax = CMathRound::RoundNearest(rmax);

      min = imin;
      max = imax;
    }
    else {
      min = rmin;
      max = rmax;
    }
  }
  else if (type() == Type::REAL_AUTO) {
    double rmin = 0.0, rmax = 0.0;

    autoBucketValues(bucket, rmin, rmax);

    if (isIntegral()) {
      int imin = CMathRound::RoundNearest(rmin);
      int imax = CMathRound::RoundNearest(rmax);

      min = imin;
      max = imax;
    }
    else {
      min = rmin;
      max = rmax;
    }
  }
  else if (type() == Type::FIXED_STOPS) {
  }
  else {
    min = bucket;
    max = bucket;
  }
}

QString
CQBucketer::
bucketName(int bucket, NameFormat format) const
{
  return bucketName(bucket, Formatter(), format);
}

QString
CQBucketer::
bucketName(int bucket, const Formatter &formatter, NameFormat format) const
{
  if      (type() == Type::STRING) {
    return bucketString(bucket);
  }
  else if (type() == Type::INTEGER_RANGE) {
    int imin = 0, imax = 0;

    bucketIValues(bucket, imin, imax);

    return bucketName(imin, imax, formatter, format);
  }
  else if (type() == Type::REAL_RANGE) {
    double rmin = 0.0, rmax = 0.0;

    bucketRValues(bucket, rmin, rmax);

    if (isIntegral()) {
      int imin = CMathRound::RoundNearest(rmin);
      int imax = CMathRound::RoundNearest(rmax);

      if (imax > imin)
        return bucketName(imin, imax, formatter, format);
      else
        return formatter.formatInt(imin);
    }

    return bucketName(rmin, rmax, formatter, format);
  }
  else if (type() == Type::REAL_AUTO) {
    double rstart = this->calcMin();
    double rend   = this->calcMax();

    if (bucket < 0 && isUnderflow())
      return bucketName(CMathUtil::getNaN(), rstart, formatter, format);

    if (bucket == INT_MAX && isOverflow())
      return bucketName(rend, CMathUtil::getNaN(), formatter, format);

    double rmin = 0.0, rmax = 0.0;

    autoBucketValues(bucket, rmin, rmax);

    if (isIntegral()) {
      int imin = CMathRound::RoundNearest(rmin);
      int imax = CMathRound::RoundNearest(rmax);

      if (imax > imin)
        return bucketName(imin, imax, formatter, format);
      else
        return formatter.formatInt(imin);
    }

    return bucketName(rmin, rmax, formatter, format);
  }
  else if (type() == Type::FIXED_STOPS) {
    double rmin = 0.0, rmax = 0.0;

    if (! bucketValues(bucket, rmin, rmax))
      return QString("%1").arg(bucket);

    return bucketName(rmin, rmax, formatter, format);
  }
  else
    return QString("%1").arg(bucket);
}

QString
CQBucketer::
bucketName(int imin, int imax, NameFormat format)
{
  return bucketName(imin, imax, Formatter(), format);
}

QString
CQBucketer::
bucketName(int imin, int imax, const Formatter &formatter, NameFormat format)
{
  static QChar arrowChar(0x2192);

  auto lhs = formatter.formatInt(imin);
  auto rhs = formatter.formatInt(imax);

  if      (format == NameFormat::DASH) {
    if (imax > imin + 1)
      return QString("%1-%2").arg(lhs).arg(rhs);
    else
      return QString("%1").arg(lhs);
  }
  else if (format == NameFormat::ARROW) {
    if (imax > imin + 1)
      return QString("%1%2%3").arg(lhs).arg(arrowChar).arg(rhs);
    else
      return QString("%1").arg(lhs);
  }
  else if (format == NameFormat::BRACKETED)
    return QString("[%1,%2)").arg(lhs).arg(rhs);
  else {
    assert(false);
    return "";
  }
}

QString
CQBucketer::
bucketName(double rmin, double rmax, NameFormat format)
{
  return bucketName(rmin, rmax, Formatter(), format);
}

QString
CQBucketer::
bucketName(double rmin, double rmax, const Formatter &formatter, NameFormat format)
{
  static QChar arrowChar(0x2192);

  auto lhs = (CMathUtil::isNaN(rmin) ? "-Inf" : formatter.formatReal(rmin));
  auto rhs = (CMathUtil::isNaN(rmax) ?  "Inf" : formatter.formatReal(rmax));

  if      (format == NameFormat::DASH)
    return QString("%1-%2").arg(lhs).arg(rhs);
  else if (format == NameFormat::ARROW)
    return QString("%1%2%3").arg(lhs).arg(arrowChar).arg(rhs);
  else if (format == NameFormat::BRACKETED)
    return QString("[%1,%2)").arg(lhs).arg(rhs);
  else {
    assert(false);
    return "";
  }
}

//----

void
CQBucketer::
autoCalc() const
{
  if (needsCalc_) {
    CInterval interval(rmin(), rmax(), numAuto());

    interval.setIntegral(isIntegral());

    calcMin_   = interval.calcStart    ();
    calcMax_   = interval.calcEnd      ();
    calcDelta_ = interval.calcIncrement();
    calcN_     = interval.calcNumMajor ();

    needsCalc_ = false;
  }
}

//----

int
CQBucketer::
stringBucket(const QString &str) const
{
  auto p = stringInd_.find(str);

  if (p == stringInd_.end()) {
    int ind = stringInd_.size();

    p = stringInd_.insert(p, StringInd::value_type(str, ind));

    indString_[ind] = str;
  }

  return p->second;
}

QString
CQBucketer::
bucketString(int bucket) const
{
  auto p = indString_.find(bucket);

  if (p == indString_.end())
    return "";

  return p->second;
}

//-----

int
CQBucketer::
intBucket(int i) const
{
  int n = INT_MIN; // optional ?

  int idelta = this->idelta();
  int istart = this->calcIStart();

  if      (i == istart)
    return 0;
  else if (idelta > 0)
    n = (i - istart)/idelta;

  return n;
}

int
CQBucketer::
realBucket(double r) const
{
  int n = INT_MIN; // optional ?

  double rdelta = this->rdelta();
  double rstart = this->calcRStart();

  if      (r == rstart)
    return 0;
  else if (rdelta > 0) {
    double nr = (r - rstart)/rdelta;

    if (nr > INT_MAX)
      return INT_MAX;

    n = CMathRound::RoundDown(nr);
  }

  return n;
}

int
CQBucketer::
autoRealBucket(double r) const
{
  autoCalc();

  int n = INT_MIN; // optional ?

  double rstart = this->calcMin();
  double rend   = this->calcMax();
  double rdelta = this->calcDelta();

  if (isUnderflow() && r < rstart)
    return -1;

  if (isOverflow() && r > rend)
    return INT_MAX;

  if      (r == rstart)
    return 0;
  else if (rdelta > 0) {
    double nr = (r - rstart)/rdelta;

    if (nr > INT_MAX)
      return INT_MAX;

    n = CMathRound::RoundDown(nr);
  }

  return n;
}

int
CQBucketer::
stopsRealBucket(double r) const
{
  int n = 0;

  for (const auto &s : rstops_) {
    if (r < s) return n;

    ++n;
  }

  return n;
}

int
CQBucketer::
stopsIntBucket(int i) const
{
  int n = 0;

  for (const auto &s : istops_) {
    if (i < s) return n;

    ++n;
  }

  return n;
}

//----

double
CQBucketer::
calcRStart() const
{
  // adjust start to be less than min
  double rstart = this->rstart();

  if (rmin() < rstart) {
    if (rdelta() > 0) {
      int n = CMathRound::RoundUp((rstart - rmin())/rdelta());
      assert(n > 0);

      rstart -= n*rdelta();
    }
  }

  // round start
  double d = rdelta()/100;

  if (d > 0.0) {
    rstart = d*CMathRound::RoundDownF(rstart/d);
  }

  return rstart;
}

int
CQBucketer::
calcIStart() const
{
  int istart = std::min(imin(), this->istart());

  if (idelta() > 0)
    istart = idelta()*CMathRound::RoundDownF(istart/idelta());

  return istart;
}
