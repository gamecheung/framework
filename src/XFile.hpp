#ifndef _X_FILE_HPP
#define _X_FILE_HPP

#include <fstream>
#include <Windows.h>
#include "XString.hpp"

class XFile{
public:
	virtual ~XFile(){}
	virtual bool isOpen()const = 0;
	virtual bool open(const XString& pathfile = XString(), std::ios_base::openmode mode=0) = 0;
    static bool exists(const XString& file){
		WIN32_FIND_DATA wfd;
		HANDLE hFind = FindFirstFile(file.c_str(), &wfd);
		if (hFind == INVALID_HANDLE_VALUE){
			return false;
		}
		FindClose(hFind);
		return true;
    }
	static size_t size(const XString& file){
		std::ifstream fin(file);
		fin.seekg(0, std::ios::end);
		return (size_t)fin.tellg();
	}
	size_t size()const{ return size(_pathfile); }
	static bool remove(const XString& file){ return (std::remove(file.toStdString().c_str())==0); }
	bool remove(){ return remove(_pathfile); }

	static bool rename(const XString& oldName, const XString& newName){
		return (MoveFile(oldName.c_str(), newName.c_str()) != 0);
	}
	static bool copy(const XString& oldName, const XString& newName ){
		return (CopyFile(oldName.c_str(), newName.c_str(), TRUE)!=0);
	}
protected:
	XString _pathfile;
};

class XIFile:public XFile{
public:	
	XIFile(const XString& pathfile = XString()){
		_pathfile = pathfile;
		_fin.imbue(std::locale(""));
		open();
	}
	bool isOpen()const{ return _fin.is_open(); }
	bool open(const XString& pathfile = XString(),
		std::ios_base::openmode mode = std::ios_base::in){
		if (!pathfile.isEmpty())
			_pathfile = pathfile;
		if (isOpen())
			close();

		_fin.open(_pathfile,mode);
		return isOpen();
	}
	void close(){ return _fin.close(); }
	XString readAll(){
		std::wstring retval((std::istreambuf_iterator<wchar_t>(_fin)),
		            std::istreambuf_iterator<wchar_t>());
		return retval;
	}
	XString readLine(){
		XString text;
		std::getline(_fin, text);
		return text;
	}
	bool atEnd()const{ return _fin.eof(); }
private:
	std::wifstream _fin;
};

class XOFile:public XFile{
public:
	XOFile(const XString& pathfile=XString()) {
		_pathfile = pathfile;
		_fout.imbue(std::locale(""));
		open();
	}
	bool isOpen()const{ return _fout.is_open(); }
	bool open(const XString& pathfile = XString(), 
		std::ios_base::openmode mode = std::ios_base::out)
	{
		if (!pathfile.isEmpty())
			_pathfile = pathfile;
		if (isOpen())
			close();

		_fout.open(_pathfile,mode);
		return isOpen();
	}
	void close(){ _fout.close(); }
	void flush(){ _fout.flush(); }
	void write(const XString& text){ _fout << text; }
	void writeLine(const XString& text){ _fout << text << L"\n"; }

	XOFile& operator<<(const XString& str)
	{
		_fout << str;
		return *this;
	}

private:
	std::wofstream	_fout;
};

#endif //_X_FILE_HPP
