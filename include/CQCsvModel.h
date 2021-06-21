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
  Q_PROPERTY(int   headerRole        READ headerRole          WRITE setHeaderRole       )
  Q_PROPERTY(int   dataRole          READ dataRole            WRITE setDataRole         )

 public:
  struct ConfigData {
    bool        commentHeader     { false };           //!< first comment line has column names
    bool        firstLineHeader   { false };           //!< first non-comment line has column names
    bool        firstColumnHeader { false };           //!< first column in each line is row name
    QChar       separator         { ',' };             //!< field separator
    int         maxRows           { -1 };              //!< max rows
    QStringList columns;                               //!< specific columns (and order)
    int         headerRole        { Qt::DisplayRole }; //!< header data role
    int         dataRole          { Qt::DisplayRole }; //!< data role

    ConfigData() { }
  };

  using MetaFields = std::vector<std::string>;
  using MetaData   = std::vector<MetaFields>;

 public:
  CQCsvModel();

  //---

  //! get/set use first line comment for horizontal header
  bool isCommentHeader() const { return configData_.commentHeader; }
  void setCommentHeader(bool b) { configData_.commentHeader = b; }

  //! get/set use first line as horizontal header
  bool isFirstLineHeader() const { return configData_.firstLineHeader; }
  void setFirstLineHeader(bool b) { configData_.firstLineHeader = b; }

  //! get/set use column for vertical header
  bool isFirstColumnHeader() const { return configData_.firstColumnHeader; }
  void setFirstColumnHeader(bool b) { configData_.firstColumnHeader = b; }

  //! get/set file separator (default ',')
  const QChar &separator() const { return configData_.separator; }
  void setSeparator(const QChar &v) { configData_.separator = v; }

  //! get/set max rows to read
  int maxRows() const { return configData_.maxRows; }
  void setMaxRows(int i) { configData_.maxRows = i; }

  //! get/set column names/numbers to read (also specifies order)
  const QStringList &columns() const { return configData_.columns; }
  void setColumns(const QStringList &v) { configData_.columns = v; }

  int headerRole() const { return configData_.headerRole; }
  void setHeaderRole(int i) { configData_.headerRole = i; }

  int dataRole() const { return configData_.dataRole; }
  void setDataRole(int i) { configData_.dataRole = i; }

  //---

  //! load CSV from specified file
  bool load(const QString &filename);

  //---

  //! save model to CSV file
  void save(std::ostream &os);

  static void save(QAbstractItemModel *model, std::ostream &os,
                   const ConfigData &configData=ConfigData(), const MetaData &meta=MetaData());

  //---

  //! encode string (suitable for CSV value)
  static QString encodeString(const QString &str, const QChar &separator=',');

 protected:
  //! encode variant (suitable for CSV value)
  static std::string encodeVariant(const QVariant &var, const QChar &separator=',');

 protected:
  ConfigData configData_; //!< config data
  MetaData   meta_;       //!< meta data
};

#endif
