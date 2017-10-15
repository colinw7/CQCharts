#ifndef CTsv_H
#define CTsv_H

#include <vector>
#include <string>
#include <cstdio>

class CTsv {
 public:
  typedef std::vector<std::string> Fields;
  typedef std::vector<Fields>      Data;

 public:
  CTsv(const std::string &filename) :
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
          Fields fields;

          if (! stringToFields(comment, fields))
            continue;

          header_ = fields;

          commentHeader   = false;
          firstLineHeader = false;
        }

        continue;
      }

      //---

      Fields fields;

      if (! stringToFields(line, fields))
        continue;

      //---

      if (firstLineHeader) {
        header_ = fields;

        commentHeader   = false;
        firstLineHeader = false;

        continue;
      }

      //---

      data_.push_back(fields);
    }

    close();

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

  bool stringToFields(const std::string &str, Fields &fields) {
    int i   = 0;
    int len = str.size();

    skipSpace(str, i);

    while (i < len) {
      // skip to end of word
      int j = i;

      skipToTab(str, i);

      // get (non-empty) word
      if (j == i)
        break;

      std::string str1 = str.substr(j, i - j);

      // add to return list
      fields.push_back(str1);

      if (str[i] == '\t')
        ++i;
    }

    return true;
  }

  void skipSpace(const std::string &str, int &i) {
    int len = str.size();

    while (i < len && isspace(str[i]))
      ++i;
  }

  void skipToTab(const std::string &str, int &i) {
    int len = str.size();

    while (i < len && str[i] != '\t')
      ++i;
  }

 private:
  std::string   filename_;
  Fields        header_;
  Data          data_;
  bool          commentHeader_   { true };
  bool          firstLineHeader_ { false };
  mutable FILE* fp_              { 0 };
};

#endif
