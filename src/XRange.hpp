#ifndef _X_RANGE_HPP
#define _X_RANGE_HPP

template <typename Type>
class XRange{
public:
	XRange(){}
	XRange(const Type& begin, const Type& end) :m_begin(begin), m_end(end){}

	void setBegin(const Type& begin){ m_begin = begin; }
	void setEnd(const Type& end){ m_end = end; }
	void set(const Type& begin, const Type& end){ m_begin = begin; m_end = end; }
	Type begin()const{ return m_begin; }
	Type end()const{ return m_end; }

private:
	Type	m_begin;
	Type	m_end;
};

#endif //_X_RANGE_HPP
