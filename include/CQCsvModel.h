#ifndef CQCsvModel_H
#define CQCsvModel_H

#include <CQDataModel.h>

/*!
 * \bried load csv into data model
 */
class CQCsvModel : public CQDataModel {
  Q_OBJECT

  Q_PROPERTY(QString filename          READ filename            WRITE setFilename         )
  Q_PROPERTY(bool    commentHeader     READ isCommentHeader     WRITE setCommentHeader    )
  Q_PROPERTY(bool    firstLineHeader   READ isFirstLineHeader   WRITE setFirstLineHeader  )
  Q_PROPERTY(bool    firstColumnHeader READ isFirstColumnHeader WRITE setFirstColumnHeader)
  Q_PROPERTY(QChar   separator         READ separator           WRITE setSeparator        )

 public:
  CQCsvModel();

  //! get/set filename
  const QString &filename() const { return filename_; }
  void setFilename(const QString &v) { filename_ = v; }

  //! get/set use first line comment for horizontal header
  bool isCommentHeader() const { return commentHeader_; }
  void setCommentHeader(bool b) { commentHeader_ = b; }

  //! get/set use first line as horizontal header
  bool isFirstLineHeader() const { return firstLineHeader_; }
  void setFirstLineHeader(bool b) { firstLineHeader_ = b; }

  //! get/set use column for vertical header
  bool isFirstColumnHeader() const { return firstColumnHeader_; }
  void setFirstColumnHeader(bool b) { firstColumnHeader_ = b; }

  //! get/set file separator (default ',')
  const QChar &separator() const { return separator_; }
  void setSeparator(const QChar &v) { separator_ = v; }

  //! get/set max rows to read
  int maxRows() const { return maxRows_; }
  void setMaxRows(int i) { maxRows_ = i; }

  //! get/set column names/numbers to read (also specifies order)
  const QStringList &columns() const { return columns_; }
  void setColumns(const QStringList &v) { columns_ = v; }

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
  QString     filename_;                    //!< input filename
  bool        commentHeader_     { false }; //!< first comment line has column names
  bool        firstLineHeader_   { false }; //!< first non-comment line has column names
  bool        firstColumnHeader_ { false }; //!< first column in each line is row name
  QChar       separator_         { ',' };   //!< field separator
  int         maxRows_           { -1 };    //!< max rows
  QStringList columns_;                     //!< specific columns (and order)
};

#endif
