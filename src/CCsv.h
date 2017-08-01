#ifndef CCSV_H
#define CCSV_H

#include <cstdio>
#include <string>
#include <vector>
#include <cassert>

class CCsv {
 public:
  typedef std::vector<int>         Inds;
  typedef std::vector<std::string> Fields;
  typedef std::vector<Fields>      FieldsArray;

 public:
  CCsv(const std::string &filename) :
   filename_(filename) {
  }

  bool isSkipFirst() const { return skipFirst_; }
  void setSkipFirst(bool b) { skipFirst_ = b; }

  bool isSkipComments() const { return skipComments_; }
  void setSkipComments(bool b) { skipComments_ = b; }

  template<typename FUNC>
  bool load(const FUNC &func) {
    if (! open())
      return false;

    std::string line;

    if (isSkipFirst())
      (void) readLine(line);

    while (readLine(line)) {
      Fields strs;

      if (! readLineStrings(line, strs))
        continue;

      //---

      try {
        func(strs);
      } catch (...) {
        continue;
      }
    }

    close();

    return true;
  }

  bool getFields(const Inds &inds, FieldsArray &fieldsArray) {
    if (! open())
      return false;

    std::string line;

    if (isSkipFirst())
      (void) readLine(line);

    while (readLine(line)) {
      Fields strs;

      if (! readLineStrings(line, strs))
        continue;

      //---

      Fields fields;

      if (! inds.empty()) {
        for (uint i = 0; i < inds.size(); ++i) {
          int ind = inds[i];

          if (ind >= 1 && ind <= int(strs.size()))
            fields.push_back(strs[ind - 1]);
          else
            fields.push_back("");
        }
      }
      else {
        fields = strs;
      }

      fieldsArray.push_back(fields);
    }

    close();

    return true;
  }

  bool getFields(FieldsArray &fieldsArray) {
    Inds inds;

    return getFields(inds, fieldsArray);
  }

 private:
  bool open() const {
    fp_ = fopen(filename_.c_str(), "r");
    if (! fp_) return false;

    return true;
  }

  void close() const {
    if (fp_)
      fclose(fp_);

    fp_ = 0;
  }

  bool readLine(std::string &line) {
    line = "";

    if (feof(fp_)) return false;

    char c = fgetc(fp_);

    if (c == EOF)
      return false;

    while (! feof(fp_) && c != '\n') {
      line += c;

      c = fgetc(fp_);
    }

    return true;
  }

  bool readLineStrings(std::string &line, Fields &strs) {
    std::vector<Fields> strsArray;

    bool terminated = true;

    while (true) {
      Fields strs1;

      if (! stringToFields(line, strs1, terminated))
        return false;

      strsArray.push_back(strs1);

      if (terminated)
        break;

      if (! readLine(line))
        break;
    }

    int na = strsArray.size();

    if (na == 0)
      return false;

    for (int i = 0; i < na; ++i) {
      const Fields &strs1 = strsArray[i];

      if (i > 0) {
        std::string ls = strs.back() + '\n';

        strs.pop_back();

        int ns1 = strs1.size();

        strs.push_back(ls + strs1[0]);

        for (int j = 1; j < ns1; ++j)
          strs.push_back(strs1[j]);
      }
      else {
        for (const auto &s : strs1)
          strs.push_back(s);
      }
    }

    return true;
  }

  bool stringToFields(const std::string &str, Fields &strs, bool &terminated) {
    static char sep    = ',';
    static char dquote = '\"';

    str_ = str;
    len_ = str_.size();
    pos_ = 0;

    if (terminated) {
      if (isSkipComments() && isCommentLine())
        return false;
    }

    while (pos_ < len_) {
      std::string str1;

      while (pos_ < len_ && str_[pos_] != sep) {
        if (! terminated || str_[pos_] == dquote) {
          if (terminated)
            ++pos_;

          // skip string (might not be terminated)
          std::string pstr;

          terminated = parseString(pstr);

          str1 += pstr;
        }
        else {
          // skip to field separator
          int j = pos_;

          while (pos_ < len_ && str_[pos_] != sep)
            ++pos_;

          str1 += str_.substr(j, pos_ - j);

          break;
        }
      }

      // skip field separator
      if (pos_ < len_ && str_[pos_] == sep)
        ++pos_;

      // add to return list
      strs.push_back(str1);
    }

    return true;
  }

  bool isCommentLine() {
    // skip space
    skipSpace();

    return (pos_ < len_ && str_[pos_] == '#');
  }

  void skipSpace() {
    while (pos_ < len_ && isspace(str_[pos_]))
      ++pos_;
  }

  bool parseString(std::string &str) {
    static char dquote = '\"';

    bool terminated = false;

    while (pos_ < len_) {
      if (str_[pos_] == dquote) {
        ++pos_;

        if (pos_ < len_ && str_[pos_] == dquote) {
          str += dquote;

          ++pos_;
        }
        else {
          terminated = true;
          break;
        }
      }
      else
        str += str_[pos_++];
    }

    return terminated;
  }

 private:
  std::string         filename_;
  bool                skipFirst_    { false };
  bool                skipComments_ { true };
  mutable FILE*       fp_           { 0 };
  mutable std::string str_;
  mutable int         len_          { 0 };
  mutable int         pos_          { 0 };
};

#endif
