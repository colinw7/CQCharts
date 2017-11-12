#ifndef CCsv_H
#define CCsv_H

#include <cstdio>
#include <string>
#include <vector>
#include <cassert>

class CCsv {
 public:
  typedef std::vector<int>         Inds;
  typedef std::vector<std::string> Fields;
  typedef std::vector<Fields>      Data;

 public:
  CCsv(const std::string &filename) :
   filename_(filename) {
  }

  const std::string &filename() const { return filename_; }

  const Fields &header() const { return header_; }

  const Data &data() const { return data_; }

  bool isCommentHeader() const { return commentHeader_; }
  void setCommentHeader(bool b) { commentHeader_ = b; }

  bool isFirstLineHeader() const { return firstLineHeader_; }
  void setFirstLineHeader(bool b) { firstLineHeader_ = b; }

  bool load() {
    bool commentHeader   = isCommentHeader  ();
    bool firstLineHeader = isFirstLineHeader();

    data_.clear();

    if (! open())
      return false;

    std::string line;

    while (readLine(line)) {
      if (line.empty())
        continue;

      std::string comment;

      if (isComment(line, comment)) {
        if (commentHeader) {
          Fields strs;

          if (! stringToFields(comment, strs))
            continue;

          header_ = strs;

          commentHeader   = false;
          firstLineHeader = false;
        }

        continue;
      }

      //---

      Fields strs;

      if (! stringToFields(line, strs))
        continue;

      //---

      if (firstLineHeader) {
        header_ = strs;

        commentHeader   = false;
        firstLineHeader = false;

        continue;
      }

      //---

      data_.push_back(strs);
    }

    close();

    return true;
  }

  bool getFields(const Inds &inds, Data &data) {
    if (! load())
      return false;

    for (const auto &fields : data_) {
      Fields fields1;

      if (! inds.empty()) {
        for (uint i = 0; i < inds.size(); ++i) {
          int ind = inds[i];

          if (ind >= 1 && ind <= int(fields.size()))
            fields1.push_back(fields[ind - 1]);
          else
            fields1.push_back("");
        }
      }
      else {
        fields1 = fields;
      }

      data.push_back(fields1);
    }

    return true;
  }

  bool getFields(Data &data) {
    if (! load())
      return false;

    data = data_;

    return true;
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

  bool isComment(const std::string &line, std::string &comment) {
    int i = 0;

    skipSpace(line, i);

    if (line[i] != '#')
      return false;

    ++i;

    skipSpace(line, i);

    comment = line.substr(i);

    return true;
  }

  bool readLine(std::string &line) {
    line = "";

    if (feof(fp_)) return false;

    int c = fgetc(fp_);

    if (c == EOF)
      return false;

    while (! feof(fp_) && c != '\n') {
      line += c;

      c = fgetc(fp_);
    }

    return true;
  }

  bool stringToFields(std::string &line, Fields &strs) {
    std::vector<Fields> strsArray;

    // keep reading lines until propertly terminated
    bool terminated = true;

    while (true) {
      Fields strs1;

      if (! stringToSubFields(line, strs1, terminated))
        return false;

      strsArray.push_back(strs1);

      if (terminated)
        break;

      if (! readLine(line))
        break;
    }

    //---

    int na = strsArray.size();

    if (na == 0)
      return false;

    //---

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

  bool stringToSubFields(const std::string &str, Fields &strs, bool &terminated) {
    static char sep    = ',';
    static char dquote = '\"';

    str_ = str;
    len_ = str_.size();
    pos_ = 0;

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

  void skipSpace(const std::string &str, int &i) {
    int len = str.size();

    while (i < len && isspace(str[i]))
      ++i;
  }

 private:
  std::string         filename_;
  Fields              header_;
  Data                data_;
  bool                commentHeader_   { true };
  bool                firstLineHeader_ { false };
  mutable FILE*       fp_              { 0 };
  mutable std::string str_;
  mutable int         len_             { 0 };
  mutable int         pos_             { 0 };
};

#endif
