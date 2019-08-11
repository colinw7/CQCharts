#include <CQCsvModel.h>
#include <CCsv.h>

#include <iostream>

CQCsvModel::
CQCsvModel()
{
  setObjectName("csvModel");

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
  if (columns_.length()) {
    Data  data    = data_;
    Cells hheader = hheader_;

    std::map<int,int> columnMap;

    int nc1 = hheader_.size();
    int nc2 = columns_.length();

    for (int c = 0; c < nc1; ++c)
      columnMap[c] = -1;

    for (int c = 0; c < nc2; ++c) {
      const QString &name = columns_[c];

      // get index for matching column name
      int ind = -1;

      for (int c1 = 0; c1 < nc1; ++c1) {
        if (hheader[c1] == name) {
          ind = c1;
          break;
        }
      }

      // if name not found, try and convert column name to number
      if (ind == -1) {
        bool ok;

        int ind1 = name.toInt(&ok);

        if (ok && ind1 >= 0 && ind1 < nc1)
          ind = ind1;
      }

      if (ind == -1) {
        std::cerr << "Invalid column name '" << name.toStdString() << "'\n";
        continue;
      }

      columnMap[ind] = c;
    }

    // get new number of columns
    nc2 = 0;

    for (int c = 0; c < nc1; ++c) {
      int c1 = columnMap[c];

      if (c1 >= 0 && c1 < nc1)
        ++nc2;
    }

    // remap horizontal header and row data
    hheader_.clear(); hheader_.resize(nc2);

    int nr = data.size();

    for (int r = 0; r < nr; ++r) {
      Cells &cells1 = data [r]; // old data
      Cells &cells2 = data_[r]; // new data

      cells2.clear(); cells2.resize(nc2);

      for (int c = 0; c < nc1; ++c) {
        int c1 = columnMap[c];

        if (c1 < 0 || c1 >= nc1)
          continue;

        hheader_[c1] = hheader[c];
        cells2  [c1] = cells1 [c];
      }
    }
  }

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
        else {
          std::cerr << "Invalid column type '" << type << "'\n";
          continue;
        }
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
        os << ",";

      os << encodeVariant(var, separator());

      output = true;
    }

    os << "\n";
  }

  //--

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
        os << ",";

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
