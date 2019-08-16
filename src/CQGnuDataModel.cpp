#include <CQGnuDataModel.h>
#include <CQStrParse.h>
#include <QFile>
#include <QTextStream>

//------

namespace {

inline bool fileToLines(const QString &filename, QStringList &lines) {
  // open file
  QFile file(filename);

  if (! file.open(QIODevice::ReadOnly))
    return false;

  // read lines
  QTextStream in(&file);

  while (! in.atEnd()) {
    QString line = in.readLine();

    lines.push_back(line);
  }

  file.close();

  return true;
}

}

//------

CQGnuDataModel::
CQGnuDataModel()
{
  setObjectName("gnuDataModel");

  setDataType(CQBaseModelDataType::GNUPLOT);
}

bool
CQGnuDataModel::
load(const QString &filename)
{
  filename_ = filename;

  //---

  // open file
  if (! fileToLines(filename_, lines_))
    return false;

  //---

  set_.clear();

  maxNumFields_ = 0;

  // TODO: skip comments and blank lines at start of file

  // process each line in file
  bool commentHeader     = isCommentHeader    ();
  bool firstLineHeader   = isFirstLineHeader  ();
  bool firstColumnHeader = isFirstColumnHeader();

  SubSet    subSet;
  FieldsSet fieldsSet;

  int bline = 0;

  int setNum    = 0;
  int subSetNum = 0;
  int lineNum   = 0;

  auto comment = commentChars_.value_or("#");

  for (const auto &line : lines_) {
    if ((commentHeader || firstLineHeader) && setNum == 0 && subSetNum == 0 && lineNum == 0) {
      if (! line.length())
        continue;

      //---

      bool setHeader = false;

      auto line1 = line;

      int cpos = line1.indexOf(comment);

      if (cpos >= 0) {
        cpos += comment.length();

        line1 = line1.mid(cpos).simplified();

        setHeader = commentHeader;
      }
      else {
        setHeader = firstLineHeader;
      }

      //---

      if (setHeader) {
        QString field0;
        Fields  fields;

        parseFileLine(line1, field0, fields, firstColumnHeader);

        //if (firstColumnHeader)
        //  rowHeaderFields_.push_back(field0);

        columnHeaderFields_ = fields;

        firstLineHeader = false;
        commentHeader   = false;

        continue;
      }
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

    if (line1.indexOf(comment) != -1) {
      CQStrParse pline(line1);

      while (! pline.eof()) {
        if (isParseStrings() && pline.isChar('\"')) {
          pline.skipChar();

          while (! pline.eof() && ! pline.isChar('"')) {
            QChar c = pline.getChar();

            if (c == '\\') {
              if (! pline.eof())
                pline.skipChar();
            }
          }

          if (pline.isChar('"'))
            pline.skipChar();
        }
        else {
          if (pline.isString(comment)) {
            line1 = pline.getAt(0, pline.getPos()).simplified();

            QString cstr = pline.getAt(pline.getPos() + comment.length()).simplified();

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
    QString field0;
    Fields  fields;

    parseFileLine(line1, field0, fields, firstColumnHeader);

    if (firstColumnHeader)
      rowHeaderFields_.push_back(field0);

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

    for (int i = 0; i < numColumns; ++i) {
      if (i < int(columnHeaderFields_.size()))
        addColumnHeader(columnHeaderFields_[i]);
      else
        addColumnHeader(QString("%1").arg(i + 1));
    }

    if (firstColumnHeader) {
      int numRows = fieldsSet.size();

      for (int i = 0; i < numRows; ++i) {
        if (i < int(rowHeaderFields_.size()))
          addRowHeader(rowHeaderFields_[i]);
        else
          addRowHeader(QString("%1").arg(i + 1));
      }
    }

    for (const auto &fields : fieldsSet) {
      Cells cells;

      for (const auto &f : fields)
        cells.push_back(f);

      data_.push_back(cells);
    }
  }

  //---

  resetColumnTypes();

  return true;
}

//------

namespace {

struct Words {
  typedef std::vector<QString> Fields;

  // takes references to fileds to populate
  Words(Fields &fields) :
   fields_(fields) {
    fields_.clear();
  }

 ~Words() {
    flush();
  }

  void addChar(const QChar &c) {
    word_ += c;
  }

  void addWord(const QString &word) {
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

  Fields& fields_;
  QString word_;
  bool    force_ { false };
};

}

//---

void
CQGnuDataModel::
parseFileLine(const QString &str, QString &field0, Fields &fields, bool firstColumnHeader)
{
  bool isSeparator = false;
  bool isPartial   = false;

  std::vector<QString> strs;

  Words words(strs);

  CQStrParse line(str);

  while (! line.eof()) {
    if (isParseStrings() && line.isChar('\"')) {
      line.skipChar();

      QString word;

      while (! line.eof() && ! line.isChar('"')) {
        QChar c = line.getChar();

        if (c == '\\') {
          if (! line.eof()) {
            c = line.getChar();

            switch (c.toLatin1()) {
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

      if (isKeepQuotes())
        word = "\"" + word + "\"";

      words.addWord(word);

      isSeparator = false;
      isPartial   = false;
    }
    else if (separator() == '\0' && line.isSpace()) {
      words.flush(false);

      line.skipSpace();

      isSeparator = true;
      isPartial   = false;
    }
    else if (line.isChar(separator())) {
      words.flush(isSeparator);

      // TODO: skip all generic separators ?
      while (! line.eof() && line.isChar(separator()))
        line.skipChar();

      isSeparator = true;
      isPartial   = false;
    }
    else {
      words.addChar(line.getChar());

      isSeparator = false;
      isPartial   = true;
    }
  }

  if (isPartial)
    words.flush(true);

  for (const auto &s : strs) {
    if (firstColumnHeader) {
      field0 = s;

      firstColumnHeader = false;
    }
    else
      fields.push_back(s);
  }
}

//------

void
CQGnuDataModel::
addColumnHeader(const QString &name)
{
  hheader_.emplace_back(name);
}

void
CQGnuDataModel::
addRowHeader(const QString &name)
{
  vheader_.emplace_back(name);
}
