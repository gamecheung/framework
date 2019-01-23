#ifndef _X_ICON_MANAGER_H
#define _X_ICON_MANAGER_H

#include <QIcon>
#include <QPainter>
#include "../XGlobal.h"

#define DEF_XICON(name,fileName) const XIcon& name##(){static XIcon icon = XIcon(#name,fileName);return icon;}
#define DEF_XICO2(name,ico,merge) const XIcon& name##(){static XIcon icon=XIcon(#name,ico##(),merge##()); return icon;}

class XIcon
{
public:
	XIcon(const QString& name, const QString& fileName,const XIcon* merge=nullptr) 
		:m_name(name), m_fileName(fileName),m_merge(merge){}
	XIcon(const QString& name, const XIcon& other, const XIcon& merge)
		:m_name(name), m_fileName(other.fileName()), m_merge(&merge){}
	QIcon toIcon()const{ return QIcon(toPixmap()); }
	QPixmap toPixmap()const
	{
		QPixmap pixmap(QString("%1/%2").arg(rootDir()).arg(m_fileName));
		if (m_merge)
		{
			QPainter painter(&pixmap);
			painter.drawPixmap(0, 0, m_merge->toPixmap());
		}
		return pixmap;
	}

	const QString& name()const{ return m_name; }
	const QString& fileName()const{ return m_fileName; }

	operator QIcon()const{ return toIcon(); }

	static void setRootDir(const QString& dir){ rootDir() = dir; }
	static QString& rootDir(){ static QString dir; return dir; }

private:
	QString m_name;
	QString m_fileName;
	const XIcon* m_merge;
};

class XIconManager
{
public:
	static void setRootDir(const QString& dir){ XIcon::setRootDir(dir); }
};

#endif //_X_ICON_MANAGER_H
