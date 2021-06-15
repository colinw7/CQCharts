#ifndef CQCsvModel_H
#define CQCsvModel_H

#include <CQDataModel.h>

/*!
 * \brief load csv into data model
 */
class CQCsvModel : public CQDataModel {
  Q_OBJECT

  Q_PROPERTY(bool  commentHeader     READ isCommentHeader     WRITE setCommentHeader    )
  Q_PROPERTY(bool  firstLineHeader   READ isFirstLineHeader   WRITE setFirstLineHeader  )
  Q_PROPERTY(bool  firstColumnHeader READ isFirstColumnHeader WRITE setFirstColumnHeader)
  Q_PROPERTY(QChar separator         READ separator           WRITE setSeparator        )

 public:
  CQCsvModel();

  //---

  //! get/set use first line comment for horizontal header
  bool isCommentHeader() const { return loadData_.commentHeader; }
  void setCommentHeader(bool b) { loadData_.commentHeader = b; }

  //! get/set use first line as horizontal header
  bool isFirstLineHeader() const { return loadData_.firstLineHeader; }
  void setFirstLineHeader(bool b) { loadData_.firstLineHeader = b; }

  //! get/set use column for vertical header
  bool isFirstColumnHeader() const { return loadData_.firstColumnHeader; }
  void setFirstColumnHeader(bool b) { loadData_.firstColumnHeader = b; }

  //! get/set file separator (default ',')
  const QChar &separator() const { return loadData_.separator; }
  void setSeparator(const QChar &v) { loadData_.separator = v; }

  //! get/set max rows to read
  int maxRows() const { return loadData_.maxRows; }
  void setMaxRows(int i) { loadData_.maxRows = i; }

  //! get/set column names/numbers to read (also specifies order)
  const QStringList &columns() const { return loadData_.columns; }
  void setColumns(const QStringList &v) { loadData_.columns = v; }

  //---

  //! load CSV from specified file
  bool load(const QString &filename);

  //---

  //! save model to CSV file
  void save(std::ostream &os);
  void save(QAbstractItemModel *model, std::ostream &os);

  //! encode string (suitable for CSV value)
  static QString encodeString(const QString &str, const QChar &separator=',');

 protected:
  //! encode variant (suitable for CSV value)
  static std::string encodeVariant(const QVariant &var, const QChar &separator=',');

 protected:
  struct LoadData {
    bool        commentHeader     { false }; //!< first comment line has column names
    bool        firstLineHeader   { false }; //!< first non-comment line has column names
    bool        firstColumnHeader { false }; //!< first column in each line is row name
    QChar       separator         { ',' };   //!< field separator
    int         maxRows           { -1 };    //!< max rows
    QStringList columns;                     //!< specific columns (and order)
  };

  LoadData loadData_;
};

#endif
