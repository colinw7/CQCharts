#include <CQBucketer.h>
#include <CInterval.h>
#include <CMathRound.h>

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
}

// get bucket for generic value
int
CQBucketer::
bucket(const QVariant &var) const
{
  int n = INT_MIN; // optional ?

  bool ok;

  if      (type() == Type::STRING) {
    QString str = var.toString();

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

  return n;
}

bool
CQBucketer::
bucketValues(int bucket, double &min, double &max) const
{
  if      (type() == Type::INTEGER_RANGE) {
    int imin = 0, imax = 0;

    bucketIValues(bucket, imin, imax);

    min = imin;
    max = imax;
  }
  else if (type() == Type::REAL_RANGE)
    bucketRValues(bucket, min, max);
  else if (type() == Type::REAL_AUTO)
    autoBucketValues(bucket, min, max);
  else
    return false;

  return true;
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
  else {
    min = bucket;
    max = bucket;
  }
}

QString
CQBucketer::
bucketName(int bucket, NameFormat format) const
{
  if      (type() == Type::STRING) {
    return bucketString(bucket);
  }
  else if (type() == Type::INTEGER_RANGE) {
    int imin = 0, imax = 0;

    bucketIValues(bucket, imin, imax);

    return bucketName(imin, imax, format);
  }
  else if (type() == Type::REAL_RANGE) {
    double rmin = 0.0, rmax = 0.0;

    bucketRValues(bucket, rmin, rmax);

    if (isIntegral()) {
      int imin = CMathRound::RoundNearest(rmin);
      int imax = CMathRound::RoundNearest(rmax);

      if (imax > imin)
        return bucketName(imin, imax, format);
      else
        return QString("%1").arg(imin);
    }

    return bucketName(rmin, rmax, format);
  }
  else if (type() == Type::REAL_AUTO) {
    double rmin = 0.0, rmax = 0.0;

    autoBucketValues(bucket, rmin, rmax);

    if (isIntegral()) {
      int imin = CMathRound::RoundNearest(rmin);
      int imax = CMathRound::RoundNearest(rmax);

      if (imax > imin)
        return bucketName(imin, imax, format);
      else
        return QString("%1").arg(imin);
    }

    return bucketName(rmin, rmax, format);
  }
  else
    return QString("%1").arg(bucket);
}

QString
CQBucketer::
bucketName(int imin, int imax, NameFormat format)
{
  static QChar arrowChar(0x2192);

  if      (format == NameFormat::DASH) {
    if (imax > imin + 1)
      return QString("%1-%2").arg(imin).arg(imax);
    else
      return QString("%1").arg(imin);
  }
  else if (format == NameFormat::ARROW) {
    if (imax > imin + 1)
      return QString("%1%2%3").arg(imin).arg(arrowChar).arg(imax);
    else
      return QString("%1").arg(imin);
  }
  else if (format == NameFormat::BRACKETED)
    return QString("(%1,%2]").arg(imin).arg(imax);
  else
    assert(false);
}

QString
CQBucketer::
bucketName(double rmin, double rmax, NameFormat format)
{
  static QChar arrowChar(0x2192);

  if      (format == NameFormat::DASH)
    return QString("%1-%2").arg(rmin).arg(rmax);
  else if (format == NameFormat::ARROW)
    return QString("%1%2%3").arg(rmin).arg(arrowChar).arg(rmax);
  else if (format == NameFormat::BRACKETED)
    return QString("(%1,%2]").arg(rmin).arg(rmax);
  else
    assert(false);
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

  double rdelta = this->calcDelta();
  double rstart = this->calcMin();

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
