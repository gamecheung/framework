#ifndef _X_HOLDER_HPP
#define _X_HOLDER_HPP

class XBoolHolder{
public: 
	XBoolHolder(bool* b, bool holdValue = true, bool reset = true)
		:m_b(b), m_holdValue(holdValue)
	{
		if(reset) 
			*m_b=m_holdValue;
	}
	~XBoolHolder(){*m_b= !m_holdValue;}
private:
	bool m_holdValue;
	bool* m_b;
};


#endif //_X_HOLDER_HPP
