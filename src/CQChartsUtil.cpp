#include <CQChartsUtil.h>
#include <QFontMetrics>

namespace {

void findStringSplits1(const QString &str, std::vector<int> &splits) {
  for (int i = 1; i < str.length(); ++i) {
    if (str[i].isSpace())
      splits.push_back(i);
  }
}

void findStringSplits2(const QString &str, std::vector<int> &splits) {
  for (int i = 1; i < str.length(); ++i) {
    if (str[i].isPunct())
      splits.push_back(i);
  }
}

void findStringSplits3(const QString &str, std::vector<int> &splits) {
  for (int i = 1; i < str.length(); ++i) {
    if (str[i - 1].isLower() && str[i].isUpper())
      splits.push_back(i);
  }
}

}

//------

namespace CQChartsUtil {

bool
formatStringInRect(const QString &str, const QFont &font, const QRectF &rect, QStringList &strs)
{
  QString sstr = str.simplified();

  //---

  QFontMetricsF fm(font);

  double w = fm.width(sstr);

  if (w < rect.width()) {
    strs.push_back(sstr);
    return false;
  }

  //---

  // get potential split points
  std::vector<int> splits;

  findStringSplits1(sstr, splits);

  if (splits.empty()) {
    findStringSplits2(sstr, splits);

    if (splits.empty())
      findStringSplits3(sstr, splits);

    if (splits.empty()) {
      strs.push_back(sstr);
      return false;
    }
  }

  //---

  // get split closest to middle
  int target   = sstr.length()/2;
  int bestDist = target*2 + 1;
  int bestInd  = -1;

  for (std::size_t i = 0; i < splits.size(); ++i) {
    int dist = std::abs(splits[i] - target);

    if (bestInd < 0 || dist < bestDist) {
      bestDist = dist;
      bestInd  = i;
    }
  }

  if (bestInd < 0) {
    strs.push_back(sstr);
    return false;
  }

  //---

  // split at best and measure
  int split = splits[bestInd];

  QString str1 = sstr.mid(0, split).simplified();
  QString str2 = sstr.mid(split   ).simplified();

  double w1 = fm.width(str1);
  double w2 = fm.width(str2);

  // both fit so we are done
  if (w1 <= rect.width() && w2 <= rect.width()) {
    strs.push_back(str1);
    strs.push_back(str2);

    return true;
  }

  //---

  // if one or both still wider then divide rect and refit
  if      (w1 > rect.width() && w2 > rect.width()) {
    double splitHeight = rect.height()/2.0;

    QRect rect1(rect.left(), rect.top(), rect.width(), splitHeight);
    QRect rect2(rect.left(), rect.top() + splitHeight, rect.width(), rect.height() - splitHeight);

    QStringList strs1, strs2;

    formatStringInRect(str1, font, rect1, strs1);
    formatStringInRect(str2, font, rect2, strs2);

    strs += strs1;
    strs += strs2;
  }
  else if (w1 > rect.width()) {
    double splitHeight = rect.height() - fm.height();

    QRect rect1(rect.left(), rect.top(), rect.width(), splitHeight);

    QStringList strs1;

    formatStringInRect(str1, font, rect1, strs1);

    strs += strs1;

    strs.push_back(str2);
  }
  else {
    double splitHeight = rect.height() - fm.height();

    QRect rect2(rect.left(), rect.top() + fm.height(), rect.width(), splitHeight);

    QStringList strs2;

    formatStringInRect(str2, font, rect2, strs2);

    strs.push_back(str1);

    strs += strs2;
  }

  return true;
}

}
