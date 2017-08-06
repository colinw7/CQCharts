#include <CQChartsData.h>
#include <CQChartsUtil.h>
#include <CParseLine.h>
#include <CStrUtil.h>

CQChartsData::
CQChartsData() :
 CQChartsModel()
{
  setColumnHeaders(true);
}

bool
CQChartsData::
load(const QString &filename)
{
  filename_ = filename;

  //---

  // open file
  if (! CQChartsUtil::fileToLines(filename_, lines_))
    return false;

  //---

  set_.clear();

  maxNumFields_ = 0;

  // TODO: skip comments and blank lines at start of file

  // process each line in file
  bool columnHeaders = hasColumnHeaders();

  SubSet    subSet;
  FieldsSet fieldsSet;

  int bline = 0;

  int setNum    = 0;
  int subSetNum = 0;
  int lineNum   = 0;

  auto comment = commentChars_.getValue("#");

  for (const auto &line : lines_) {
    if (columnHeaders && setNum == 0 && subSetNum == 0 && lineNum == 0) {
      if (! line.length())
        continue;

      auto line1 = line;

      int cpos = line1.indexOf(comment.c_str());

      if (cpos >= 0) {
        cpos += comment.length();

        line1 = line1.mid(cpos).simplified();
      }

      Fields fields;

      parseFileLine(line1, fields);

      columnHeaderFields_ = fields;

      columnHeaders = false;

      continue;
    }

    //---

    if (! line.length()) {
      ++bline;

      if      (bline == subSetBlankLines_) {
        lineNum = 0;

        ++subSetNum;

        // start new subset
        if (! fieldsSet.empty()) {
          subSet.push_back(fieldsSet);

          fieldsSet.clear();
        }
      }
      else if (bline == setBlankLines_) {
        lineNum   = 0;
        subSetNum = 0;

        ++setNum;

        // start new set
        if (! subSet.empty()) {
          set_.push_back(subSet);

          subSet.clear();
        }
      }

      continue;
    }

    //---

    bline = 0;

    //---

    bool commentFound = false;

    auto line1 = line;

    if (line1.indexOf(comment.c_str()) != -1) {
      CParseLine pline(line1.toStdString());

      while (pline.isValid()) {
        if (isParseStrings() && pline.isChar('\"')) {
          pline.skipChar();

          while (pline.isValid() && ! pline.isChar('"')) {
            char c = pline.getChar();

            if (c == '\\') {
              if (pline.isValid())
                pline.skipChar();
            }
          }

          if (pline.isChar('"'))
            pline.skipChar();
        }
        else {
          if (pline.isChars(comment)) {
            line1 = CStrUtil::stripSpaces(pline.substr(0, pline.pos())).c_str();

            std::string cstr = CStrUtil::stripSpaces(pline.substr(pline.pos() + comment.length()));

            commentStrs_[setNum][subSetNum][lineNum] = cstr;

            commentFound = true;

            break;
          }
          else
            pline.skipChar();
        }
      }

      if (commentFound && ! line1.length())
        continue;
    }

    //---

    // get fields from line
    Fields fields;

    parseFileLine(line1, fields);

    fieldsSet.push_back(fields);

    maxNumFields_ = std::max(maxNumFields_, int(fields.size()));

    //---

    ++lineNum;
  }

  //---

  if (! fieldsSet.empty())
    subSet.push_back(fieldsSet);

  if (! subSet.empty())
    set_.push_back(subSet);

  //---

  if (set_.size() == 1 && set_[0].size() == 1) {
    const FieldsSet &fieldsSet = set_[0][0];

    // add fields to model
    int numColumns = columnHeaderFields_.size();

    for (const auto &fields : fieldsSet)
      numColumns = std::max(numColumns, int(fields.size()));

    for (const auto &columnHeader : columnHeaderFields_)
      addColumn(columnHeader.c_str());

    for (const auto &fields : fieldsSet) {
      Cells cells;

      for (const auto &f : fields)
        cells.push_back(f.c_str());

      data_.push_back(cells);
    }
  }

  return true;
}

//------

namespace {

struct Words {
  Words(std::vector<std::string> &fields) :
   fields_(fields) {
    fields_.clear();
  }

 ~Words() {
    flush();
  }

  void addChar(char c) {
    word_ += c;
  }

  void addWord(const std::string &word) {
    force_ = true;
    word_  = word;

    flush();
  }

  void flush(bool force=false) {
    if (force || force_ || word_ != "") {
      fields_.push_back(word_);

      force_ = false;
      word_  = "";
    }
  }

  std::vector<std::string> &fields_;
  std::string               word_;
  bool                      force_ { false };
};

}

//------

void
CQChartsData::
parseFileLine(const QString &str, Fields &fields)
{
  bool isSeparator = false;

  std::vector<std::string> strs;

  Words words(strs);

  CParseLine line(str.toStdString());

  while (line.isValid()) {
    if (isParseStrings() && line.isChar('\"')) {
      line.skipChar();

      std::string word;

      while (line.isValid() && ! line.isChar('"')) {
        char c = line.getChar();

        if (c == '\\') {
          if (line.isValid()) {
            c = line.getChar();

            switch (c) {
              case 't' : word += '\t'; break;
              case 'n' : word += '\n'; break;
              default  : word += '?' ; break;
            }
          }
          else
            word += c;
        }
        else
          word += c;
      }

      if (line.isChar('"'))
        line.skipChar();

      words.addWord("\"" + word + "\"");

      isSeparator = false;
    }
    else if (separator() == '\0' && line.isSpace()) {
      words.flush(false);

      line.skipSpace();

      isSeparator = true;
    }
    else if (line.isChar(separator())) {
      words.flush(isSeparator);

      // TODO: skip all generic separators ?
      while (line.isValid() && line.isChar(separator()))
        line.skipChar();

      isSeparator = true;
    }
    else {
      words.addChar(line.getChar());

      isSeparator = false;
    }
  }

  words.flush(true);

  for (const auto &s : strs)
    fields.push_back(s);
}
