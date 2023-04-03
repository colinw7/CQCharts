#include <CQCsvModel.h>
#include <CCsv.h>

#include <CQModelUtil.h>
#include <CQStrParse.h>

#include <QColor>
#include <QBuffer>
#include <QDataStream>
#include <iostream>

namespace {

QDataStream::Version dataStreamVersion() {
#if QT_VERSION >= 0x050000
  return QDataStream::Qt_5_0;
#else
  return QDataStream::Qt_4_0;
#endif
}

}

CQCsvModel::
CQCsvModel()
{
  setObjectName("csvModel");

  setDataType(CQBaseModelDataType::CSV);

  // default read only
  setReadOnly(true);
}

bool
CQCsvModel::
load(const QString &filename)
{
  filename_ = filename;

  //---

  // parse file into array of fields
  CCsv csv(filename_.toStdString());

  csv.setCommentHeader  (isCommentHeader());
  csv.setFirstLineHeader(isFirstLineHeader());
  csv.setSeparator      (separator().toLatin1());

  if (! csv.load())
    return false;

  //---

  // get header data and rows from csv
  const auto &header = csv.header();
  const auto &data   = csv.data();

  //---

  hheader_.clear();
  vheader_.clear();
  data_   .clear();

  //---

  int numColumns = 0;

  // add header to model
  if (! header.empty()) {
    int i = 0;

    for (const auto &f : header) {
      if (i == 0 && isFirstColumnHeader()) {
        //vheader_.push_back(f.c_str());
      }
      else
        hheader_.push_back(f.c_str());

      ++i;
    }

    numColumns = std::max(numColumns, int(hheader_.size()));
  }

  //---

  // expand horizontal header to max number of columns
  for (const auto &fields : data) {
    int numFields = int(fields.size());

    if (isFirstColumnHeader())
      --numFields;

    numColumns = std::max(numColumns, numFields);
  }

  while (int(hheader_.size()) < numColumns)
    hheader_.push_back("");

  //---

  // add fields to model
  int nr = 0;

  for (const auto &fields : data) {
    // get row vertical header and cells
    Cells   cells;
    QString vheader;

    int i = 0;

    for (const auto &f : fields) {
      if (i == 0 && isFirstColumnHeader())
        vheader = f.c_str();
      else
        cells.push_back(f.c_str());

      ++i;
    }

    //---

    // skip row if not accepted by model
    if (! acceptsRow(cells))
      continue;

    //---

    // add row vertical header and cells to model
    if (isFirstColumnHeader())
      vheader_.push_back(vheader);

    data_.push_back(cells);

    //---

    // stop if hit maximum rows
    ++nr;

    if (maxRows() > 0 && nr >= maxRows())
      break;
  }

  //---

  // expand vertical header to number of rows
  int numRows = int(data_.size());

  while (int(vheader_.size()) < numRows)
    vheader_.push_back("");

  //---

  // if columns specified filter and reorder data by columns
  if (columns().length())
    applyFilterColumns(columns());

  //---

  // clear column types
  resetColumnTypes();

  //---

  // process meta data
  meta_ = csv.meta();

  if (! meta_.empty()) {
    for (const auto &fields : meta_) {
      int numFields = int(fields.size());

      // handle column data:
      //   column <column_name> <value_type> <value>
      if      (fields[0] == "column") {
        std::string name, type, value;

        if (numFields == 4) {
          name  = fields[1];
          type  = fields[2];
          value = fields[3];
        }
        else {
          std::cerr << "Invalid column data\n";
          continue;
        }

        int icolumn = modelColumnNameToInd(name.c_str());

        if      (type == "type") {
          auto columnType = nameType(value.c_str());

          if (columnType != CQBaseModelType::NONE)
            setColumnType(icolumn, columnType);
        }
        else if (type == "min") {
          auto columnType = this->columnType(icolumn);

          if      (columnType == CQBaseModelType::INTEGER) {
            bool ok;

            int min = QString(value.c_str()).toInt(&ok);

            if (! ok)
              std::cerr << "Invalid integer column min '" << value << "'\n";

            setColumnMin(icolumn, min);
          }
          else if (columnType == CQBaseModelType::REAL) {
            bool ok;

            double min = QString(value.c_str()).toDouble(&ok);

            if (! ok)
              std::cerr << "Invalid real column min '" << value << "'\n";

            setColumnMin(icolumn, min);
          }
        }
        else if (type == "max") {
          auto columnType = this->columnType(icolumn);

          if      (columnType == CQBaseModelType::INTEGER) {
            bool ok;

            int max = QString(value.c_str()).toInt(&ok);

            if (! ok)
              std::cerr << "Invalid integer column max '" << value << "'\n";

            setColumnMax(icolumn, max);
          }
          else if (columnType == CQBaseModelType::REAL) {
            bool ok;

            double max = QString(value.c_str()).toDouble(&ok);

            if (! ok)
              std::cerr << "Invalid real column max '" << value << "'\n";

            setColumnMax(icolumn, max);
          }
        }
        else if (type == "sum") {
          auto columnType = this->columnType(icolumn);

          if      (columnType == CQBaseModelType::INTEGER) {
            bool ok;

            int sum = QString(value.c_str()).toInt(&ok);

            if (! ok)
              std::cerr << "Invalid integer column sum '" << value << "'\n";

            setColumnSum(icolumn, sum);
          }
          else if (columnType == CQBaseModelType::REAL) {
            bool ok;

            double sum = QString(value.c_str()).toDouble(&ok);

            if (! ok)
              std::cerr << "Invalid real column sum '" << value << "'\n";

            setColumnSum(icolumn, sum);
          }
        }
        else if (type == "title") {
          setColumnTitle(icolumn, value.c_str());
        }
        else {
          setColumnNameValue(icolumn, type.c_str(), value.c_str());
        }
      }
      // handle cell data:
      //   cell <cell_index> <role> <value>
      else if (fields[0] == "cell") {
        if (numFields != 4) {
          std::cerr << "Invalid cell data\n";
          continue;
        }

        auto indStr  = QString::fromStdString(fields[1]);
        auto roleStr = QString::fromStdString(fields[2]);
        auto value   = QString::fromStdString(fields[3]);

        // get role
        int role = CQModelUtil::nameToRole(this, roleStr);

        if (role < 0) {
          std::cerr << "Invalid role '" << fields[2] << "'\n";
          continue;
        }

        // get cell index
        int row = -1, col = -1;

        if (! CQModelUtil::stringToRowCol(indStr, row, col)) {
          std::cerr << "Invalid index '" << fields[1] << "'\n";
          continue;
        }

        // get value(s)
        // <value> | <type>:<value>
        QVariant var;

        CQStrParse parse(value);

        parse.skipSpace();

        int i1 = parse.getPos();

        while (! parse.eof() && parse.isAlnum())
          parse.skipChar();

        int i2 = parse.getPos();

        parse.skipSpace();

        if (parse.isChar(':')) {
          parse.skipChar();

          parse.skipSpace();

          auto lhs = value.mid(i1, i2 - i1);
          auto rhs = value.mid(parse.getPos());

          int type = QMetaType::type(lhs.toLatin1().constData());

          if (type < 0) {
            std::cerr << "Invalid type '" << lhs.toStdString() << "'\n";
            continue;
          }

          if (type == QMetaType::QColor) {
            var = QColor(rhs);
          }
          else {
            QByteArray ba;

            // write current string to buffer
            QBuffer obuffer(&ba);
            obuffer.open(QIODevice::WriteOnly);

            QDataStream out(&obuffer);
            out.setVersion(dataStreamVersion());

            out << rhs;

            // create user type data from data stream using registered DataStream methods
            QBuffer ibuffer(&ba);
            ibuffer.open(QIODevice::ReadOnly);

            QDataStream in(&ibuffer);
            in.setVersion(dataStreamVersion());

            var = QVariant(type, nullptr);

            // const cast is safe since we operate on a newly constructed variant
            if (! QMetaType::load(in, type, const_cast<void *>(var.constData()))) {
              std::cerr << "Invalid data '" << rhs.toStdString() <<
                           "' for type '" << lhs.toStdString() << "'\n";
              continue;
            }
          }
        }
        else {
          var = value;
        }

        if (row < 0 || col < 0) {
          std::cerr << "Invalid index '" << fields[1] << "'\n";
          continue;
        }

        bool readOnly = isReadOnly();

        if (readOnly)
          setReadOnly(false);

        setData(index(row, col, QModelIndex()), var, role);

        if (readOnly)
          setReadOnly(readOnly);
      }
      // handle global data
      //  3: global,<name>,<value>
      //  4: global,<name>,<key>,<value>
      else if (fields[0] == "global") {
        QString name, key, value;

        if      (numFields == 3) {
          name  = QString::fromStdString(fields[1]);
          value = QString::fromStdString(fields[2]);
        }
        else if (numFields == 4) {
          name  = QString::fromStdString(fields[1]);
          key   = QString::fromStdString(fields[2]);
          value = QString::fromStdString(fields[3]);
        }
        else {
          std::cerr << "Invalid global data\n";
          continue;
        }

        setMetaNameValue(name, key, value);
      }
      else {
        std::cerr << "Unknown meta data '" << fields[0] << "\n";
        continue;
      }
    }
  }

  return true;
}

void
CQCsvModel::
save(std::ostream &os)
{
  save(this, os, configData_, meta_);
}

void
CQCsvModel::
save(QAbstractItemModel *model, std::ostream &os,
     const ConfigData &configData, const MetaData &meta)
{
  int nc = model->columnCount();
  int nr = model->rowCount();

  //---

  if (meta.size()) {
    os << "#META_DATA\n";

    for (const auto &values : meta) {
      os << "# ";

      int i = 0;

      for (const auto &v : values) {
        if (i > 0)
          os << ",";

        os << v;

        ++i;
      }

      os << "\n";
    }

    os << "#END_META_DATA\n";
  }

  //---

  bool hasHHeader = false;

  if (configData.firstLineHeader) {
    for (int c = 0; c < nc; ++c) {
      auto var = model->headerData(c, Qt::Horizontal, configData.headerRole);

      if (var.isValid()) {
        hasHHeader = true;
        break;
      }
    }
  }

  bool hasVHeader = false;

  if (configData.firstColumnHeader) {
    for (int r = 0; r < nr; ++r) {
      auto var = model->headerData(r, Qt::Vertical, configData.headerRole);

      if (var.isValid()) {
        hasVHeader = true;
        break;
      }
    }
  }

  //---

  // output horizontal header on first line if enabled and model has horizontal header data
  if (configData.firstLineHeader && hasHHeader) {
    bool output = false;

    // if vertical header then add empty cell
    if (configData.firstColumnHeader && hasVHeader)
      output = true;

    for (int c = 0; c < nc; ++c) {
      auto var = model->headerData(c, Qt::Horizontal, configData.headerRole);

      if (output)
        os << configData.separator.toLatin1();

      os << encodeVariant(var, configData.separator);

      output = true;
    }

    os << "\n";
  }

  //---

  // output rows
  for (int r = 0; r < nr; ++r) {
    bool output = false;

    // output vertical header value if enabled and model has vertical header data
    if (configData.firstColumnHeader && hasVHeader) {
      auto var = model->headerData(r, Qt::Vertical, configData.headerRole);

      os << encodeVariant(var, configData.separator);

      output = true;
    }

    //---

    for (int c = 0; c < nc; ++c) {
      auto ind = model->index(r, c);

      auto var = model->data(ind, configData.dataRole);

      if (output)
        os << configData.separator.toLatin1();

      os << encodeVariant(var, configData.separator);

      output = true;
    }

    os << "\n";
  }
}

std::string
CQCsvModel::
encodeVariant(const QVariant &var, const QChar &separator)
{
  std::string str;

  if      (var.type() == QVariant::Double)
    str = std::to_string(var.toDouble());
  else if (var.type() == QVariant::Int)
    str = std::to_string(var.toInt());
  else if (var.type() == QVariant::LongLong)
    str = std::to_string(var.toLongLong());
  else
    str = encodeString(var.toString(), separator).toStdString();

  return str;
}

QString
CQCsvModel::
encodeString(const QString &str, const QChar &separator)
{
  bool quote = false;

  int i = str.indexOf(separator);

  if (i >= 0)
    quote = true;
  else {
    i = str.indexOf('\n');

    if (i >= 0)
      quote = true;
    else {
      i = str.indexOf('\"');

      if (i >= 0)
        quote = true;
    }
  }

  if (quote) {
    int pos = str.indexOf('\"');

    if (pos >= 0) {
      auto str1 = str;

      QString str2;

      while (pos >= 0) {
        str2 += str1.mid(0, pos) + "\"\"";

        str1 = str1.mid(pos + 1);

        pos = str1.indexOf('\"');
      }

      str2 += str1;

      return str2;
    }
    else {
      return "\"" + str + "\"";
    }
  }
  else
    return str;
}
