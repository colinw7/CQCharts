#ifndef CQChartsFont_H
#define CQChartsFont_H

#include <cassert>
#include <QFont>
#include <iostream>

class QObject;

/*!
 * \brief class to contain charts font.
 * \ingroup Charts
 *
 * A charts font can be:
 *  . An explicit font
 *  . An derived modified inherited font
 */
class CQChartsFont {
 public:
  enum class Type {
    NONE,
    FONT,
    INHERITED
  };

  enum class SizeType {
    NONE,
    EXPLICIT,
    INCREMENT,
    DECREMENT
  };

  struct InheritData {
    bool     normal   { false };          //!< make normal
    bool     bold     { false };          //!< make bold
    bool     italic   { false };          //!< make italic
    SizeType sizeType { SizeType::NONE }; //!< size type
    double   size     { 0.0 };            //!< size

    static int cmp(const InheritData &lhs, const InheritData &rhs) {
      if (lhs.normal   != rhs.normal  ) return (lhs.normal   < rhs.normal   ? -1 : 1);
      if (lhs.bold     != rhs.bold    ) return (lhs.bold     < rhs.bold     ? -1 : 1);
      if (lhs.italic   != rhs.italic  ) return (lhs.italic   < rhs.italic   ? -1 : 1);
      if (lhs.sizeType != rhs.sizeType) return (lhs.sizeType < rhs.sizeType ? -1 : 1);
      if (lhs.size     != rhs.size    ) return (lhs.size     < rhs.size     ? -1 : 1);

      return 0;
    }
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsFont() { }

  explicit CQChartsFont(Type type) :
   type_(type) {
  }

  explicit CQChartsFont(const QFont &font) :
   type_(Type::FONT), font_(font) {
  }

  explicit CQChartsFont(const InheritData &data) :
   type_(Type::INHERITED), data_(data) {
  }

  explicit CQChartsFont(const QString &str) {
    setFontStr(str);
  }

  bool isValid() const {
    if      (type_ == Type::FONT)
      return true;
    else if (type_ == Type::INHERITED)
      return true;
    else
      return false;
  }

  Type type() const { return type_; }
  void setType(Type type) { type_  = type; }

  const QFont &font() const { return font_; }
  void setFont(const QFont &font) { type_ = Type::FONT; font_ = font; }

  const InheritData &data() const { return data_; }
  void setData(const InheritData &data) { type_ = Type::INHERITED; data_ = data; }

  //---

  void setNormal() { data_.normal = true; }
  void setBold  () { data_.bold   = true; }
  void setItalic() { data_.italic = true; }

  void incFontSize(double inc) {
    if (data_.sizeType == SizeType::DECREMENT) {
      data_.size = -data_.size + inc;

      if (data_.size >= 0)
        data_.sizeType = SizeType::INCREMENT;
      else {
        data_.sizeType = SizeType::DECREMENT;
        data_.size     = -data_.size;
      }
    }
    else {
      if (data_.sizeType == SizeType::NONE)
        data_.sizeType = SizeType::INCREMENT;

      data_.size += inc;
    }
  }

  void decFontSize(double dec) {
    if (data_.sizeType == SizeType::INCREMENT) {
      data_.size = data_.size - dec;

      if (data_.size >= 0)
        data_.sizeType = SizeType::INCREMENT;
      else {
        data_.sizeType = SizeType::DECREMENT;
        data_.size     = -data_.size;
      }
    }
    else {
      if (data_.sizeType == SizeType::NONE)
        data_.sizeType = SizeType::DECREMENT;

      data_.size += dec;
    }
  }

  void setFontSize(double size) {
    data_.sizeType = SizeType::EXPLICIT;
    data_.size     = size;
  }

  //---

  double pointSizeF(double parentSize=-1) const;
  void setPointSizeF(double s);

  //---

  CQChartsFont calcFont(const CQChartsFont &parentFont) const;

  QFont calcFont(const QFont &parentFont=QFont()) const;

  //---

  QString fontStr() const;
  bool setFontStr(const QString &str);

  //---

  QString toString() const { return fontStr(); }

  bool fromString(const QString &s) { return setFontStr(s); }

  //---

  static int cmp(const QFont &lhs, const QFont &rhs) {
    if (lhs.key() < rhs.key()) return -1;
    if (lhs.key() > rhs.key()) return  1;

    return 0;
  }

  static int cmp(const CQChartsFont &lhs, const CQChartsFont &rhs) {
    if (lhs.type_ != rhs.type_) return (lhs.type_ < rhs.type_ ? -1 : 1);

    if (lhs.type_ == Type::FONT) {
      return cmp(lhs.font_, rhs.font_);
    }
    else {
      return InheritData::cmp(lhs.data_, rhs.data_);
    }

    return 0;
  }

  friend bool operator==(const CQChartsFont &lhs, const CQChartsFont &rhs) {
    return (cmp(lhs, rhs) == 0);
  }

  friend bool operator!=(const CQChartsFont &lhs, const CQChartsFont &rhs) {
    return (cmp(lhs, rhs) != 0);
  }

  friend bool operator<(const CQChartsFont &lhs, const CQChartsFont &rhs) {
    return (cmp(lhs, rhs) < 0);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsFont &c) {
    c.print(os);

    return os;
  }

  //---

 private:
  Type        type_  { Type::INHERITED }; //!< font type (font, inherited)
  QFont       font_;                      //!< specific font
  InheritData data_;                      //!< inherit data
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsFont)

#endif
