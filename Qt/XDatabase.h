#ifndef _X_DATABASE_H
#define _X_DATABASE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QSqlDatabase>


#define DEF_FLD_ENUM(key,val) key,
#define DEF_FLD1(key,val)	fields.append(Field{#key,#val});
#define DEF_FLD2(key,val)	Field fld##key()const{return field(key);}

#define DEF_TBL(TblName,FldList)	\
	class Tbl##TblName : public XDbTable{ \
	public:enum{FldList(DEF_FLD_ENUM) Count}; Tbl##TblName(){name=#TblName,FldList(DEF_FLD1)} FldList(DEF_FLD2) };

//#define FLDS_LIST(_) \
//	_(SampleNo,INT)	\
//	_(Barcode,TEXT)	\
//	_(SampleType,TEXT)
//DEF_TBL(Sample, FLDS_LIST)


class XDbTable
{
public:
	typedef struct{ QString name; QString value; }Field;
	QString			name;
	QList<Field>	fields;

protected:
	const Field& field(int idx) const{ return fields.at(idx); }
};


class XDatabase : public QObject
{
	Q_OBJECT

public:
	XDatabase(QObject* parent = nullptr);
	~XDatabase();

	bool open(const QString& fileName);

private:
	QSqlDatabase	m_db;
};

#endif //_X_DATABASE_H
