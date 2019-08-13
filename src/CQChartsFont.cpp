#include <CQChartsFont.h>
#include <CQChartsUtil.h>
#include <CQPropertyView.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsFont, toString, fromString)

int CQChartsFont::metaTypeId;

void
CQChartsFont::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsFont);

  CQPropertyViewMgrInst->setUserName("CQChartsFont", "font");
}

QString
CQChartsFont::
fontStr() const
{
  if (! isValid())
    return "none";

  if (type() == Type::INHERITED) {
    QString str;

    auto addWord = [&](const QString &word) {
      if (str.length()) str += " ";
      str += word;
    };

    if (data_.normal)
      addWord("normal");

    if (data_.bold)
      addWord("bold");

    if (data_.italic)
      addWord("italic");

    if (data_.sizeType != SizeType::NONE) {
      QString word;

      if      (data_.sizeType == SizeType::INCREMENT)
        word += "+";
      else if (data_.sizeType == SizeType::DECREMENT)
        word += "-";

      assert(data_.size >= 0);

      word += QString("%1").arg(data_.size);

      addWord(word);
    }

    if (str == "")
      str = "inherited";

    return str;
  }
  else {
    return font().toString();
  }
}

bool
CQChartsFont::
setFontStr(const QString &str)
{
  InheritData inheritData;

  bool valid = false;

  CQStrParse parse(str);

  parse.skipSpace();

  while (! parse.eof()) {
    if      (parse.isWord("normal")) {
      parse.skipWord("normal");

      inheritData.normal = true;
      valid              = true;
    }
    else if (parse.isWord("bold")) {
      parse.skipWord("bold");

      inheritData.bold = true;
      valid            = true;
    }
    else if (parse.isWord("italic")) {
      parse.skipWord("italic");

      inheritData.italic = true;
      valid              = true;
    }
    else if (parse.isChar('+') || parse.isChar('-') || parse.isDigit()) {
      SizeType sizeType = SizeType::EXPLICIT;

      if      (parse.isChar('+'))
        sizeType = SizeType::INCREMENT;
      else if (parse.isChar('-'))
        sizeType = SizeType::DECREMENT;

      double size;

      if (! parse.readReal(&size))
        break;

      inheritData.sizeType = sizeType;
      inheritData.size     = std::abs(size);
      valid                = true;
    }
    else if (parse.isWord("inherited")) {
      parse.skipWord("inherited");

      valid = true;
    }
    else {
      valid = false;
      break;
    }

    parse.skipSpace();
  }

  if (valid) {
    type_ = Type::INHERITED;
    data_ = inheritData;
  }
  else {
    QFont f;

    if (! f.fromString(str))
      return false;

    setFont(f);
  }

  return true;
}

double
CQChartsFont::
pointSizeF(double parentSize) const
{
  if      (type_ == Type::FONT) {
    return font_.pointSizeF();
  }
  else if (type_ == Type::INHERITED) {
    if (data_.sizeType == SizeType::EXPLICIT)
      return data_.size;
    else {
      double s = (parentSize > 0 ? parentSize : QFont().pointSizeF());

      if      (data_.sizeType == SizeType::INCREMENT)
        s += data_.size;
      else if (data_.sizeType == SizeType::DECREMENT)
        s -= data_.size;

      return s;
    }
  }
  else {
    return QFont().pointSizeF();
  }
}

void
CQChartsFont::
setPointSizeF(double s)
{
  if      (type_ == Type::FONT) {
    if (s > 0.0)
      font_.setPointSizeF(s);
  }
  else if (type_ == Type::INHERITED) {
    data_.sizeType = SizeType::EXPLICIT;
    data_.size     = s;
  }
}

CQChartsFont
CQChartsFont::
calcFont(const CQChartsFont &parentFont) const
{
  CQChartsFont font;

  if      (type_ == Type::FONT)
    font = CQChartsFont(font_);
  else if (type_ == Type::NONE)
    font = parentFont;
  else if (type_ == Type::INHERITED) {
    if (parentFont.type_ == Type::INHERITED) {
      font = parentFont;

      if (data_.normal)
        font.setNormal();
      else {
        if (data_.bold)
          font.setBold();

        if (data_.italic)
          font.setItalic();
      }

      if      (data_.sizeType == SizeType::INCREMENT)
        font.incFontSize(data_.size);
      else if (data_.sizeType == SizeType::DECREMENT)
        font.decFontSize(data_.size);
      else if (data_.sizeType == SizeType::EXPLICIT)
        font.setFontSize(data_.size);
    }
    else
      font = CQChartsFont(calcFont(parentFont.font_));
  }

  return font;
}

QFont
CQChartsFont::
calcFont(const QFont &parentFont) const
{
  QFont font;

  if      (type_ == Type::FONT)
    font = font_;
  else if (type_ == Type::NONE)
    font = parentFont;
  else if (type_ == Type::INHERITED) {
    font = parentFont;

    if (data_.normal)
      font.setBold(false);
    else {
      if (data_.bold)
        font.setBold(true);

      if (data_.italic)
        font.setItalic(true);
    }

    double pointSize = 0.0;

    if      (data_.sizeType == SizeType::INCREMENT)
      pointSize = font.pointSizeF() + data_.size;
    else if (data_.sizeType == SizeType::DECREMENT)
      pointSize = font.pointSizeF() - data_.size;
    else if (data_.sizeType == SizeType::EXPLICIT)
      pointSize = data_.size;

    if (pointSize > 0.0)
      font.setPointSizeF(pointSize);
  }

  return font;
}
