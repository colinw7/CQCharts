#include <CQCsvModel.h>
#include <CCsv.h>

#include <CQModelUtil.h>

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
  const CCsv::Fields &header = csv.header();
  const CCsv::Data   &data   = csv.data();

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
    int numFields = fields.size();

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

    if (maxRows_ > 0 && nr >= maxRows_)
      break;
  }

  //---

  // expand vertical header to number of rows
  int numRows = data_.size();

  while (int(vheader_.size()) < numRows)
    vheader_.push_back("");

  //---

  // if columns specified filter and reorder data by columns
  if (columns_.length())
    applyFilterColumns(columns_);

  //---

  // clear column types
  resetColumnTypes();

  //---

  // process meta data
  const CCsv::Data &meta = csv.meta();

  if (! meta.empty()) {
    for (const auto &fields : meta) {
      int numFields = fields.size();

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
          CQBaseModelType columnType = nameType(value.c_str());

          if (columnType != CQBaseModelType::NONE)
            setColumnType(icolumn, columnType);
        }
        else if (type == "min") {
          CQBaseModelType columnType = this->columnType(icolumn);

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
          CQBaseModelType columnType = this->columnType(icolumn);

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
        QString indStr, roleStr, value;

        if (numFields == 4) {
          indStr  = fields[1].c_str();
          roleStr = fields[2].c_str();
          value   = fields[3].c_str();
        }
        else {
          std::cerr << "Invalid cell data\n";
          continue;
        }

        int role = CQModelUtil::nameToRole(roleStr);

        if (role < 0) {
          std::cerr << "Invalid role '" << fields[2] << "'\n";
          continue;
        }

        int row = -1, col = -1;

        auto indStrs = indStr.split(":");

        if (indStrs.length() == 2) {
          bool ok;
          row = indStrs[0].toInt(&ok); if (! ok) row = -1;
          col = indStrs[1].toInt(&ok); if (! ok) col = -1;
        }

        if (row < 0 || col < 0) {
          std::cerr << "Invalid index '" << fields[1] << "'\n";
          continue;
        }

        auto valueStrs = value.split(":");

        QVariant var;

        if (valueStrs.length() == 2) {
          int type = QMetaType::type(valueStrs[0].toLatin1().constData());

          if (type < 0) {
            std::cerr << "Invalid type '" << valueStrs[0].toStdString() << "'\n";
            continue;
          }

          if (type == QMetaType::QColor) {
            var = QColor(valueStrs[1]);
          }
          else {
            QByteArray ba;

            // write current string to buffer
            QBuffer obuffer(&ba);
            obuffer.open(QIODevice::WriteOnly);

            QDataStream out(&obuffer);
            out.setVersion(dataStreamVersion());

            out << valueStrs[1];

            // create user type data from data stream using registered DataStream methods
            QBuffer ibuffer(&ba);
            ibuffer.open(QIODevice::ReadOnly);

            QDataStream in(&ibuffer);
            in.setVersion(dataStreamVersion());

            var = QVariant(type, 0);

            // const cast is safe since we operate on a newly constructed variant
            if (! QMetaType::load(in, type, const_cast<void *>(var.constData()))) {
              std::cerr << "Invalid data '" << valueStrs[1].toStdString() <<
                           "' for type '" << valueStrs[0].toStdString() << "'\n";
              continue;
            }
          }
        }
        else {
          var = valueStrs[1];
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
      //   global <> <name> <value>
      else if (fields[0] == "global") {
        std::string name, value;

        if      (numFields == 3) {
          name  = fields[1];
          value = fields[2];
        }
        else if (numFields == 4) {
          name  = fields[1];
          value = fields[3];
        }
        else {
          std::cerr << "Invalid global data\n";
          continue;
        }

        setNameValue(name.c_str(), value.c_str());
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
  save(this, os);
}

void
CQCsvModel::
save(QAbstractItemModel *model, std::ostream &os)
{
  int nc = model->columnCount();
  int nr = model->rowCount();

  //---

  bool hasHHeader = false;

  if (isFirstLineHeader()) {
    for (int c = 0; c < nc; ++c) {
      QVariant var = model->headerData(c, Qt::Horizontal);

      if (var.isValid()) {
        hasHHeader = true;
        break;
      }
    }
  }

  bool hasVHeader = false;

  if (isFirstColumnHeader()) {
    for (int r = 0; r < nr; ++r) {
      QVariant var = model->headerData(r, Qt::Vertical);

      if (var.isValid()) {
        hasVHeader = true;
        break;
      }
    }
  }

  //---

  // output horizontal header on first line if enabled and model has horizontal header data
  if (isFirstLineHeader() && hasHHeader) {
    bool output = false;

    // if vertical header then add empty cell
    if (isFirstColumnHeader() && hasVHeader)
      output = true;

    for (int c = 0; c < nc; ++c) {
      QVariant var = model->headerData(c, Qt::Horizontal);

      if (output)
        os << separator().toLatin1();

      os << encodeVariant(var, separator());

      output = true;
    }

    os << "\n";
  }

  //---

  // output rows
  for (int r = 0; r < nr; ++r) {
    bool output = false;

    // output vertical header value if enabled and model has vertical header data
    if (isFirstColumnHeader() && hasVHeader) {
      QVariant var = model->headerData(r, Qt::Vertical);

      os << encodeVariant(var, separator());

      output = true;
    }

    //---

    for (int c = 0; c < nc; ++c) {
      QModelIndex ind = model->index(r, c);

      QVariant var = model->data(ind);

      if (output)
        os << separator().toLatin1();

      os << encodeVariant(var, separator());

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

  if      (var.type() == QVariant::Double) {
    double r = var.value<double>();

    str = std::to_string(r);
  }
  else if (var.type() == QVariant::Int) {
    int i = var.value<int>();

    str = std::to_string(i);
  }
  else {
    QString qstr = var.toString();

    str = encodeString(qstr, separator).toStdString();
  }

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
      QString str1 = str;
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
