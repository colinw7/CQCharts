#include <CQGnuDataModel.h>
#include <CQStrParse.h>
#include <CUnixFile.h>

//------

namespace {

inline bool fileToLines(const QString &filename, QStringList &lines) {
  // open file
  CUnixFile file(filename.toStdString());

  if (! file.open())
    return false;

  // read lines
  std::string line;

  while (file.readLine(line))
    lines.push_back(line.c_str());

  return true;
}

}

//------

CQGnuDataModel::
CQGnuDataModel()
{
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
  bool commentHeader   = isCommentHeader  ();
  bool firstLineHeader = isFirstLineHeader();

  SubSet    subSet;
  FieldsSet fieldsSet;

  int bline = 0;

  int setNum    = 0;
  int subSetNum = 0;
  int lineNum   = 0;

  auto comment = commentChars_.value_or("#");

  for (const auto &line : lines_) {
    if ((commentHeader || firstLineHeader) &&
        setNum == 0 && subSetNum == 0 && lineNum == 0) {
      if (! line.length())
        continue;

      //---

      bool setHeader = false;

      auto line1 = line;

      int cpos = line1.indexOf(comment.c_str());

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
        Fields fields;

        parseFileLine(line1, fields);

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

    if (line1.indexOf(comment.c_str()) != -1) {
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
          if (pline.isString(comment.c_str())) {
            line1 = pline.getAt(0, pline.getPos()).simplified();

            std::string cstr =
              pline.getAt(pline.getPos() + comment.length()).simplified().toStdString();

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

    for (int i = 0; i < numColumns; ++i) {
      if (i < int(columnHeaderFields_.size()))
        addColumn(columnHeaderFields_[i].c_str());
      else
        addColumn(QString("%1").arg(i + 1));
    }

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

//---

void
CQGnuDataModel::
parseFileLine(const QString &str, Fields &fields)
{
  bool isSeparator = false;

  std::vector<std::string> strs;

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

      words.addWord(word.toStdString());

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
      while (! line.eof() && line.isChar(separator()))
        line.skipChar();

      isSeparator = true;
    }
    else {
      words.addChar(line.getChar().toLatin1());

      isSeparator = false;
    }
  }

  words.flush(true);

  for (const auto &s : strs)
    fields.push_back(s);
}

//------

void
CQGnuDataModel::
addColumn(const QString &name)
{
  columns_.emplace_back(name);
}

int
CQGnuDataModel::
columnCount(const QModelIndex &) const
{
  return columns_.size();
}

QVariant
CQGnuDataModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  int n = columnCount();

  if (section < 0 || section >= n)
    return QVariant();

  const Column &column = columns_[section];

  if (orientation == Qt::Horizontal) {
    if (role == Qt::DisplayRole) {
      return QVariant(column.name());
    }
    else if (role == Qt::EditRole) {
      return QVariant();
    }
    else if (role == Qt::ToolTipRole) {
      if (column.type() != "")
        return QVariant(QString("%1\n%2").arg(column.name()).arg(column.type()));
      else
        return QVariant(QString("%1").arg(column.name()));
    }
  }

  return QVariant();
}

bool
CQGnuDataModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &, int)
{
  if (section < 0 || orientation != Qt::Horizontal)
    return false;

  return false;
}

QVariant
CQGnuDataModel::
data(const QModelIndex &index, int role) const
{
  if (! index.isValid())
    return QVariant();

  if      (role == Qt::UserRole) {
    const Cells &cells = data_[index.row()];

    if (index.column() < 0 || index.column() >= int(cells.size()))
      return QVariant();

    const QString &cell = cells[index.column()];

    return cell;
  }
  else if (role == Qt::DisplayRole) {
    const Cells &cells = data_[index.row()];

    if (index.column() < 0 || index.column() >= int(cells.size()))
      return QVariant();

    const QString &cell = cells[index.column()];

    return cell;
  }

  return QVariant();
}

QModelIndex
CQGnuDataModel::
index(int row, int column, const QModelIndex &) const
{
  return createIndex(row, column, nullptr);
}

QModelIndex
CQGnuDataModel::
parent(const QModelIndex &index) const
{
  if (! index.isValid())
    return QModelIndex();

  return QModelIndex();
}

int
CQGnuDataModel::
rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  return data_.size();
}

Qt::ItemFlags
CQGnuDataModel::
flags(const QModelIndex &index) const
{
  if (! index.isValid())
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
