#ifndef _X_MATRIX_HPP
#define _X_MATRIX_HPP

class XMatrix
{
public:
	XMatrix(int rowCount, int colCount,double* data=nullptr) :_rowCount(0), _colCount(0), _data(0)
	{
		resize(rowCount, colCount);
		if (data)
		{
			memcpy(_data, data, elementCount()*sizeof(double));
		}
	}
	~XMatrix()
	{
		clear();
	}

	XMatrix(const XMatrix& other)
	{
		_rowCount = other._rowCount;
		_colCount = other._colCount;
		int count = elementCount();
		_data = new double[count];
		memcpy(_data, other._data, count*sizeof(double));
	}
	XMatrix& operator=(const XMatrix& other)
	{
		clear(); 
		_rowCount = other._rowCount;
		_colCount = other._colCount;
		int count = elementCount();
		_data = new double[count];
		memcpy(_data, other._data, count*sizeof(double));
		return *this;
	}

	XMatrix operator+(const XMatrix& mat)const
	{
		XMatrix rs(_rowCount, _colCount);
		for (int row = 0; row < _rowCount; ++row)
		{
			for (int col = 0; col < _colCount; ++col)
			{
				rs.at(row, col) = at(row, col) + mat.at(row, col);
			}
		}
		return rs;
	}

	XMatrix operator-(const XMatrix& mat)const
	{
		XMatrix rs(_rowCount, _colCount);
		for (int row = 0; row < _rowCount; ++row)
		{
			for (int col = 0; col < _colCount; ++col)
			{
				rs.at(row, col) = at(row, col) - mat.at(row, col);
			}
		}
		return rs;
	}
	

	XMatrix operator*(double d)const
	{
		XMatrix rs(_rowCount, _colCount);
		for (int row = 0; row < _rowCount; ++row)
		{
			for (int col = 0; col < _colCount; ++col)
			{
				rs.at(row, col) = at(row, col)*d;
			}
		}
		return rs;
	}
	friend XMatrix operator*(double d, const XMatrix& mat)
	{
		return mat * 2;
	}
	XMatrix& operator*=(double d)
	{
		*this = (*this)*d;
		return *this;
	}


	int rowCount()const{ return _rowCount; }
	int colCount()const{ return _colCount; }
	int elementCount()const{ return _rowCount*_colCount; }
	int length()const{ return _rowCount*_colCount; }

	(double max)() const
	{
		assert(_data);

		double rs = _data[0];
		int count = elementCount();
		for (int i = 1; i < count; ++i)
		{
			if (_data[i] > rs)
				rs = _data[i];
		}
		return rs;
	}

	(double min)() const
	{
		assert(_data);

		double rs = _data[0];
		int count = elementCount();
		for (int i = 1; i < count; ++i)
		{
			if (_data[i] < rs)
				rs = _data[i];
		}
		return rs;
	}

	void resize(int rowCount, int colCount)
	{
		int preRowCount = _rowCount;
		int preColCount = _colCount;
		double* preData = _data;

		double* data = new double[rowCount*colCount*sizeof(double)];
		if (data)
		{
			int minRowCount = xMin(preRowCount, rowCount);
			int minColCount = xMin(preColCount, colCount);
			for (int row = 0; row < minRowCount; ++row)
			{
				memcpy(&data[row*colCount], &_data[row*preColCount], minColCount*sizeof(double));
			}

			clear();
			_data = data;
			_rowCount = rowCount;
			_colCount = colCount;
		}
	}

	void clear()
	{
		if (_data)
		{
			delete[] _data;
			_data = 0;
			_rowCount = 0;
			_colCount = 0;
		}
	}

	double&  at(int row,int col)
	{
		assert(0 <= row && row < _rowCount);
		assert(0 <= col && col < _colCount);

		return _data[row*_colCount + col];
	}

	double at(int row, int col) const
	{
		assert(0 <= row && row < _rowCount);
		assert(0 <= col && col < _colCount);

		return _data[row*_colCount + col];
	}

	XMatrix roi(int row1, int col1, int row2, int col2)
	{
		assert(0 <= row1 && row1 < _rowCount);
		assert(0 <= row2 && row2 < _rowCount);
		assert(row1 <= row2);
		assert(0 <= col1 && col1 < _colCount);
		assert(0 <= col2 && col2 < _colCount);
		assert(col1 <= col2);

		int rows = row2 - row1 + 1;
		int cols = col2 - col1 + 1;
		XMatrix rs(rows, cols);
		for (int row = 0; row < rows; ++row)
		{
			for (int col = 0; col < cols; ++col)
			{
				rs.at(row, col) = this->at(row1 + row, col1 + col);
			}
		}
		return rs;
	}

	void transpose()
	{
		XMatrix bk = *this;
		int preRows = _rowCount;
		int preCols = _colCount;
		_rowCount = preCols;
		_colCount = preRows;

		for (int row = 0; row < _rowCount; ++row)
		{
			for (int col = 0; col < _colCount; ++col)
			{
				this->at(row, col) = bk.at(col, row);
			}
		}
	}

	void setRow(int idx, const XMatrix& mat)
	{
		assert(0 <= idx && idx < _rowCount);
		assert(_colCount == mat._colCount);
		assert(mat._rowCount == 1);

		for (int col = 0; col < _colCount; ++col)
			this->at(idx, col) = mat.at(0, col);
	}
	void setCol(int idx, const XMatrix& mat)
	{
		assert(0 <= idx && idx < _colCount);
		assert(_rowCount == mat._rowCount);
		assert(mat._colCount == 1);

		for (int row = 0; row < _rowCount; ++row)
			this->at(row, idx) = mat.at(row, 0);
	}

	XMatrix row(int idx)const
	{
		assert(0 <= idx && idx < _rowCount);

		XMatrix rs(1, _colCount);
		for (int col = 0; col < _colCount; ++col)
			rs.at(0, col) = this->at(idx, col);
		return rs;
	}
	XMatrix col(int idx)const
	{
		assert(0 <= idx && idx < _colCount);

		XMatrix rs(_rowCount, 1);
		for (int row = 0; row < _rowCount; ++row)
			rs.at(row, 0) = this->at(row, idx);
		return rs;
	}

	double determinant(int m,int n)const
	{
		return 0;
	}

	bool operator==(const XMatrix& mat)const
	{ 
		if ((_rowCount != mat._rowCount) || (_colCount != mat._colCount))
			return false;

		for (int row = 0; row < _rowCount; ++row)
		{
			for (int col = 0; col < _colCount; ++col)
			{
				if (this->at(row, col) != mat.at(row, col))
					return false;
			}
		}
		return true;
	}
	bool operator!=(const XMatrix& mat)const{ return !(*this == mat); }

private:
	int _rowCount;
	int _colCount;
	double* _data;
};

#endif //_X_MATRIX_HPP
