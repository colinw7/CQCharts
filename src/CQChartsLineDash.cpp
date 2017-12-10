#include <CQChartsLineDash.h>
#include <CQUtil.h>

CQUTIL_DCL_META_STREAM(CQChartsLineDash, toString, fromString)

CQChartsLineDash::
CQChartsLineDash()
{
  init();
}

CQChartsLineDash::
CQChartsLineDash(const CQChartsLineDash &dash)
{
  init();

  copy(dash);
}

CQChartsLineDash::
CQChartsLineDash(const Lengths &lengths, double offset)
{
  init();

  setOffset(offset);

  QLengths qlengths;

  int num_lengths = lengths.size();

  for (int i = 0; i < num_lengths; ++i) {
    if (lengths[i] > 1E-5)
      qlengths.push_back(lengths[i]);
  }

  pen_.setDashPattern(qlengths);
}

CQChartsLineDash::
CQChartsLineDash(ushort pattern)
{
  init();

  setDashes(pattern);
}

CQChartsLineDash::
CQChartsLineDash(const std::string &str)
{
  fromString(str);
}

CQChartsLineDash &
CQChartsLineDash::
operator=(const CQChartsLineDash &dash)
{
  return copy(dash);
}

bool
CQChartsLineDash::
operator==(const CQChartsLineDash &dash) const
{
  if (getOffset() != dash.getOffset()) return false;

  QLengths qlengths1 =      getLengths();
  QLengths qlengths2 = dash.getLengths();

  if (qlengths1.size() != qlengths2.size())
    return false;

  int num_lengths = qlengths1.size();

  for (int i = 0; i < num_lengths; ++i) {
    if (qlengths1[i] != qlengths2[i])
      return false;
  }

  return true;
}

void
CQChartsLineDash::
scale(double factor)
{
  assert(factor > 0);

  setOffset(getOffset()*factor);

  QLengths qlengths = getLengths();

  int num_lengths = qlengths.size();

  for (int i = 0; i < num_lengths; ++i)
    qlengths[i] *= factor;

  pen_.setDashPattern(qlengths);
}

CQChartsLineDash &
CQChartsLineDash::
copy(const CQChartsLineDash &dash)
{
  if (&dash == this)
    return *this;

  pen_ = dash.pen_;

  return *this;
}

void
CQChartsLineDash::
setDashes(const Lengths &lengths, double offset)
{
  QLengths qlengths;

  for (const auto &len : lengths) {
    if (len > 1E-5)
      qlengths.push_back(len);
  }

  setOffset(offset);

  pen_.setDashPattern(qlengths);
}

void
CQChartsLineDash::
setDashes(ushort pattern)
{
  int bits[16];

  for (uint i = 0; i < 16; ++i) {
    uint mask = 1 << i;

    bits[i] = (pattern & mask) ? 1 : 0;
  }

  QLengths qlengths;

  double dash_value[16];

  uint num_lengths = 0;

  uint i = 0;

  while (i < 16) {
    qlengths.push_back(0);

    qlengths  [num_lengths] = 0;
    dash_value[num_lengths] = bits[i];

    while (i < 16 && bits[i] == dash_value[num_lengths]) {
      ++qlengths[num_lengths];

      ++i;
    }

    ++num_lengths;
  }

  double offset = 0.0;

  if (dash_value[0] == 0) {
    offset = qlengths[0];

    for (uint i = 1; i < num_lengths; ++i)
      qlengths[i - 1] = qlengths[i];

    qlengths[num_lengths - 1] = offset;
  }

  if (num_lengths == 1) {
    offset      = 0.0;
    num_lengths = 0;
  }

  if (num_lengths & 1) {
    offset += qlengths[num_lengths - 1];

    qlengths[0] += qlengths[num_lengths - 1];

    --num_lengths;
  }

  setOffset(offset);

  pen_.setDashPattern(qlengths);
}

std::string
CQChartsLineDash::
toString() const
{
  if (isSolid()) return "solid";

  std::stringstream ss;

  QLengths qlengths = getLengths();

  uint num_lengths = qlengths.size();

  for (uint i = 0; i < num_lengths; ++i) {
    if (i > 0) ss << ", ";

    ss << qlengths[i];
  }

  return ss.str();
}

bool
CQChartsLineDash::
fromString(const std::string &str)
{
  if (str == "solid") {
    init();

    return true;
  }

  Lengths lengths;

  uint pos = 0;
  uint len = str.size();

  while (pos < len && isspace(str[pos])) ++pos;

  while (pos < len) {
    if (pos < len && ! isdigit(str[pos]))
      return false;

    double num = 0;

    while (pos < len && isdigit(str[pos]))
      num = num*10 + (str[pos++] - '0');

    if (pos < len && str[pos] == '.') {
      double dec = 0;

      double f = 0.1;

      ++pos;

      while (pos < len && isdigit(str[pos])) {
        dec += (str[pos++] - '0')*f;

        f /= 10.0;
      }

      num += f;
    }

    while (pos < len && isspace(str[pos])) ++pos;

    if (pos < len && str[pos] == ',') {
      ++pos;

      while (pos < len && isspace(str[pos])) ++pos;
    }

    lengths.push_back(num);
  }

  setDashes(lengths);

  return true;
}

void
CQChartsLineDash::
init()
{
  setOffset(0.0);

  QLengths qlengths;

  pen_.setDashPattern(qlengths);
}
