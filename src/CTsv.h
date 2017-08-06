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

  bool load() {
    data_.clear();

    FILE *fp = fopen(filename().c_str(), "r");
    if (! fp) return false;

    while (! feof(fp)) {
      std::string line;

      if (! readLine(fp, line))
        break;

      if (line.empty())
        continue;

      std::string comment;

      if (isComment(line, comment)) {
        if (header_.empty()) {
          Fields fields;

          if (! stringToFields(comment, fields))
            continue;

          header_ = fields;
        }

        continue;
      }

      //---

      Fields fields;

      if (! stringToFields(line, fields))
        continue;

      data_.push_back(fields);
    }

    fclose(fp);

    return true;
  }

 private:
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

  bool readLine(FILE *fp, std::string &line) {
    line = "";

    if (feof(fp)) return false;

    char c = fgetc(fp);

    while (! feof(fp) && c != '\n') {
      line += c;

      c = fgetc(fp);
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
  std::string filename_;
  Data        data_;
  Fields      header_;
  bool        isCommentHeader_ { true };
};

#endif
