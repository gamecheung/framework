#ifndef _XCOMBO_BOX_HELPER_H
#define _XCOMBO_BOX_HELPER_H

#include <QComboBox>
#include <QHash>


class XComboBoxHelper : public QObject{
	Q_OBJECT
public:
	~XComboBoxHelper(){instances()->remove(m_obj);}
	static XComboBoxHelper* getInstance(QComboBox* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XComboBoxHelper(obj);
		}
		return (*instances())[obj];
	}

	void setCurrentData(const QVariant& data)
	{
		m_obj->setCurrentIndex(m_obj->findData(data));
	}

private:
    XComboBoxHelper(QComboBox* obj) :QObject(obj), m_obj(obj){ 
        Q_ASSERT(m_obj);
        // code here
    }
	static QHash<QComboBox*, XComboBoxHelper*>* instances(){
		static QHash<QComboBox*, XComboBoxHelper*> s_instances;
		return &s_instances;
	}
private:
	QComboBox*	m_obj;
};

#define XCOMBO_BOX_HELPER(obj)  XComboBoxHelper::getInstance(obj)

#endif // _XCOMBO_BOX_HELPER
