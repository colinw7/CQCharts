#ifndef CQChartsHtml_H
#define CQChartsHtml_H

/*!
 * \brief Helper class to build HTML string
 * \ingroup Charts
 */
class CQChartsHtml {
 public:
  //! Intermediate class for string part of HTML string
  class Str {
   public:
    enum class Type {
      LT,
      GT,
      AMP,
      NBSP,
      BR
    };

    Str(const QString &str="") : str_(str) { }

    Str(const char *str) : str_(str) { }

    Str(const Type &type) {
      switch (type) {
        case Type::LT  : str_ = "&lt;"  ; break;
        case Type::GT  : str_ = "&gt;"  ; break;
        case Type::AMP : str_ = "&amp;" ; break;
        case Type::NBSP: str_ = "&nbsp;"; break;
        case Type::BR  : str_ = "<br>"  ; break;
      }
    }

    operator QString() { return str_; }

    Str &operator+=(const Str &str) { str_ += str.str_; return *this; }
    Str &operator+=(const char *str) { str_ += str; return *this; }

    friend Str operator+(const char *lhs, const Str &rhs) { return lhs + rhs.str_; }
    friend Str operator+(const Str &lhs, const char *rhs) { return lhs.str_ + rhs; }
    friend Str operator+(const Str &lhs, const Str &rhs) { return lhs.str_ + rhs.str_; }

    static Str angled(const Str &str) { return Str("&lt;") + str + Str("&gt;"); }

    static Str bold  (const Str &str) { return Str("<b>") + str + Str("</b>"); }
    static Str italic(const Str &str) { return Str("<i>") + str + Str("</i>"); }

    static Str img(const Str &src) {
      return Str("<img src=\"@CHARTS_DOC_PATH@/") + src + Str("\"/>"); }

    static Str a(const Str &ref, const Str &str) {
      return Str("<a href=\"") + ref + Str("\">") + str + Str("</a>"); }

   private:
    QString str_;
  };

  //---

  CQChartsHtml() { }

  operator QString() { return str_; }

  CQChartsHtml &h1(const Str &str) { str_ += Str("<h1>") + str + Str("</h1>\n"); return *this; }
  CQChartsHtml &h2(const Str &str) { str_ += Str("<h2>") + str + Str("</h2>\n"); return *this; }
  CQChartsHtml &h3(const Str &str) { str_ += Str("<h3>") + str + Str("</h3>\n"); return *this; }
  CQChartsHtml &h4(const Str &str) { str_ += Str("<h4>") + str + Str("</h4>\n"); return *this; }
  CQChartsHtml &h5(const Str &str) { str_ += Str("<h5>") + str + Str("</h5>\n"); return *this; }
  CQChartsHtml &h6(const Str &str) { str_ += Str("<h6>") + str + Str("</h6>\n"); return *this; }

  CQChartsHtml &p(const Str &str) { str_ += Str("<p>") + str + Str("</p>\n"); return *this; }

  CQChartsHtml &ul(const std::initializer_list<Str> &strs) {
    str_ += Str("<ul>\n");

    for (const auto &s : strs)
      str_ += Str("<li>") + s + Str("</li>\n");

    str_ += Str("</ul>\n");

    return *this;
  }

  CQChartsHtml &pre(const CQChartsHtml &html) {
    str_ += Str("<pre>\n");
    str_ += html.str_;
    str_ += Str("\n</pre>\n");

    return *this;
  }

 private:
  Str str_;
};

#endif
