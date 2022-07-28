#ifndef CQChartsMainArgs_H
#define CQChartsMainArgs_H

#include <optional>

/*!
 * \brief Charts Main Arguments
 * \ingroup Charts
 */
class CQChartsMainArgs {
 public:
  using OptReal   = std::optional<double>;
  using OptInt    = std::optional<int>;
  using OptString = std::optional<QString>;

 public:
  CQChartsMainArgs(int &argc, char **argv) :
   argc_(argc), argv_(argv) {
  }

  bool eof() const { return i_ >= argc_; };

  bool isOpt() const { return argv_[i_][0] == '-'; }

  QString arg() const { return argv_[i_]; }
  QString opt() const { return &argv_[i_][1]; }

  void next() { ++i_; }

  bool parseOpt(QString &s) {
    ++i_;

    if (i_ >= argc_)
      return false;

    s = argv_[i_];

    return true;
  }

  bool parseOpt(int &i) {
    ++i_;

    if (i_ >= argc_)
      return false;

    i = atoi(argv_[i_]);

    return true;
  }

  bool parseOpt(double &r) {
    ++i_;

    if (i_ >= argc_)
      return false;

    r = std::stod(argv_[i_]);

    return true;
  }

  bool parseOpt(OptString &s) {
    ++i_;

    if (i_ >= argc_)
      return false;

    s = argv_[i_];

    return true;
  }

  bool parseOpt(OptReal &r) {
    ++i_;

    if (i_ >= argc_)
      return false;

    r = std::stod(argv_[i_]);

    return true;
  }

 private:
  int    i_    { 1 };
  int    argc_ { 0 };
  char **argv_ { nullptr };
};

#endif
