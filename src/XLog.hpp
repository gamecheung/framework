#ifndef _X_LOG_HPP
#define _X_LOG_HPP

#include "XString.hpp"
#include "XFile.hpp"
#include "XDateTime.hpp"
#include "XMutexLocker.hpp"
#include "XList.hpp"

class XLog{
public:
	enum Level{Info,Warn,Error};
	enum Type{DayType,MonthType,YearType};
	
	XLog() :m_enabled(true), m_timeFormat(L"hh:mm:ss"){}
	XLog(const XString& fileBaseName, const Type type = DayType)
		:m_timeFormat(L"hh:mm:ss"), m_enabled(true)
	{
		open(fileBaseName, type);
	}

	~XLog()
	{
		m_file.close();
		if (m_file.size() == 0){
			m_file.remove();
		}
	}


	bool open(const XString& fileBaseName, const Type type = DayType)
	{ 
		m_type = type;
		m_fileBaseName = fileBaseName;
		m_fileOpenDate = XDate::currentDate();
		return m_file.open(genFullFileName(),std::ios_base::out|std::ios_base::app);
	}
	void writeInfo(const XString& text)
	{ write(XDateTime::currentDateTime(), Info, text ); }
	void writeWarn(const XString& text)
	{ write( XDateTime::currentDateTime(), Warn, text ); }
	void writeError(const XString& text)
	{ write(XDateTime::currentDateTime(), Error, text ); }
	
	void setTimeFormat(const XString& timeFormat){ m_timeFormat = timeFormat; }
	const XString& timeFormat()const{ return m_timeFormat; }

	void setEnabled(bool enable){ m_enabled = enable; }
	bool isEnabled()const{ return m_enabled; }

	static XString& dir()
	{
		static XString s_dir = L".";
		return s_dir;
	}
	static void setDir(const XString& dir){ XLog::dir() = dir; }

protected:
	virtual void write(const XDateTime& dateTime, Level level,const XString& text)
	{
		if (!m_enabled)
			return;

		XMutexLocker mutexLocker(&m_mutex);

		XString strLevel;
		switch (level)
		{
		case Info:strLevel = L"Info"; break;
		case Warn:strLevel = L"Warn"; break;
		case Error:strLevel = L"Error"; break;
		}
		m_file.write(XString("[%1] %2: %3\n").arg(dateTime.toString(m_timeFormat)).arg(strLevel).arg(text));
	}

private:
	XString genFullFileName()const
	{
		switch (m_type)
		{
		case XLog::DayType:
			return XString("%1/%2_%3.log").arg(dir()).arg(m_fileBaseName).arg(XDate::currentDate().toString("yyyy-MM-dd"));
			break;
		case XLog::MonthType:
			return XString("%1/%2_%3.log").arg(dir()).arg(m_fileBaseName).arg(XDate::currentDate().toString("yyyy-MM"));
			break;
		case XLog::YearType:
			return XString("%1/%2_%3.log").arg(dir()).arg(m_fileBaseName).arg(XDate::currentDate().toString("yyyy"));
			break;
		default:
			break;
		}
		return m_fileBaseName;
	}

	void maybeReOpenFile(const XDate& date)
	{
		if (needReOpenFile(date))
		{
			open(m_fileBaseName, m_type);
		}
	}
	bool needReOpenFile(const XDate& date)const
	{
		switch (m_type)
		{
		case XLog::DayType:
			if (m_fileOpenDate != date)
				return true;
			else
				return false;

		case XLog::MonthType:
			if (m_fileOpenDate.year() == date.year() && m_fileOpenDate.month() == date.month())
				return true;
			else
				return false;

		case XLog::YearType:
			if (m_fileOpenDate.year() == date.year())
				return true;
			else
				return false;

		default:
			break;
		}
		return false;
	}


private:
	XOFile		m_file;
	XString		m_fileBaseName;
	XDate		m_fileOpenDate;
	Type		m_type;
	std::mutex	m_mutex;
	XString		m_timeFormat;
	bool		m_enabled;
};

#endif //_X_LOG_HPP
