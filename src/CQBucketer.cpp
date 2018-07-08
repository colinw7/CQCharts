#include <CQBucketer.h>

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

    needsCalc_ = true;
  }
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
    min = bucket*rdelta + calcMin_;
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

QString
CQBucketer::
bucketName(int bucket, bool utfArrow) const
{
  if      (type() == Type::STRING) {
    auto p = indString_.find(bucket);

    if (p == indString_.end())
      return "";

    return (*p).second;
  }
  else if (type() == Type::INTEGER_RANGE) {
    int imin = 0, imax = 0;

    bucketIValues(bucket, imin, imax);

    return bucketName(imin, imax, utfArrow);
  }
  else if (type() == Type::REAL_RANGE) {
    double rmin = 0.0, rmax = 0.0;

    bucketRValues(bucket, rmin, rmax);

    return bucketName(rmin, rmax, utfArrow);
  }
  else if (type() == Type::REAL_AUTO) {
    double rmin = 0.0, rmax = 0.0;

    autoBucketValues(bucket, rmin, rmax);

    return bucketName(rmin, rmax, utfArrow);
  }
  else
    return QString("%1").arg(bucket);
}

QString
CQBucketer::
bucketName(int imin, int imax, bool utfArrow) const
{
  static QChar arrowChar(0x2192);

  QChar dashChar = (utfArrow ? arrowChar : QChar('-'));

  return QString("%1%2%3").arg(imin).arg(dashChar).arg(imax);
}

QString
CQBucketer::
bucketName(double rmin, double rmax, bool utfArrow) const
{
  static QChar arrowChar(0x2192);

  QChar dashChar = (utfArrow ? arrowChar : QChar('-'));

  return QString("%1%2%3").arg(rmin).arg(dashChar).arg(rmax);
}

//----

void
CQBucketer::
autoCalc() const
{
  if (needsCalc_) {
    double length = rmax() - rmin();

    double length1 = length/numAuto();

    // Calculate nearest Power of Ten to Length
    int power = (length1 > 0 ? roundNearest(log10(length1)) : 1);

    // prefer integral values
    if (isIntegral()) {
      if (power < 0) {
        length1 = 1.0;
        power   = 0;
      }
    }

    calcDelta_ = 1;

    if      (power < 0) {
      for (int i = 0; i < -power; i++)
        calcDelta_ /= 10.0;
    }
    else if (power > 0) {
      for (int i = 0; i <  power; i++)
        calcDelta_ *= 10.0;
    }

    // round min value to increment
    if (length1 > 0) {
      int n = roundNearest(length1/calcDelta_);

      if (! n)
        n = 1;

      calcDelta_ = calcDelta_*n;
      calcMin_   = calcDelta_*roundDownF(rmin()/calcDelta_);
    }
    else {
      calcMin_ = rmin();
    }

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

int
CQBucketer::
intBucket(int i) const
{
  int n = INT_MIN; // optional ?

  int idelta = this->idelta();
  int istart = this->calcIStart();

  if (idelta > 0)
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

  if (rdelta > 0)
    n = roundDown((r - rstart)/rdelta);

  return n;
}

int
CQBucketer::
autoRealBucket(double r) const
{
  autoCalc();

  int n = INT_MIN; // optional ?

  double rdelta = calcDelta_;

  if (rdelta > 0)
    n = roundDown((r - calcMin_)/rdelta);

  return n;
}

//----

double
CQBucketer::
calcRStart() const
{
  double rstart = std::min(rmin(), this->rstart());

  double d = rdelta()/100;

  if (d > 0.0) {
    rstart = d*roundDownF(rstart/d);
  }

  return rstart;
}

int
CQBucketer::
calcIStart() const
{
  int istart = std::min(imin(), this->istart());

  if (idelta() > 0)
    istart = idelta()*roundDownF(istart/idelta());

  return istart;
}

int
CQBucketer::
roundNearest(double x) const
{
  double x1;

  if (x <= 0.0)
    x1 = (x - 0.499999);
  else
    x1 = (x + 0.500001);

  if (x1 < INT_MIN || x1 > INT_MAX)
    errno = ERANGE;

  return int(x1);
}

int
CQBucketer::
roundDown(double x) const
{
  double x1;

  if (x >= 0.0)
    x1 = (x       + 1E-6);
  else
    x1 = (x - 1.0 + 1E-6);

  if (x1 < INT_MIN || x1 > INT_MAX)
    errno = ERANGE;

  return int(x1);
}

double
CQBucketer::
roundNearestF(double x) const
{
  double x1;

  if (x <= 0.0)
    x1 = (x - 0.499999);
  else
    x1 = (x + 0.500001);

  return std::trunc(x1);
}

double
CQBucketer::
roundDownF(double x) const
{
  double x1;

  if (x >= 0.0)
    x1 = (x       + 1E-6);
  else
    x1 = (x - 1.0 + 1E-6);

  return std::trunc(x1);
}
