#ifndef _X_UNIT_TEST_HPP
#define _X_UNIT_TEST_HPP

#include <iostream>

class XUnitTest{
public:
    XUnitTest(){ 
        std::wcout.imbue(std::locale("")); 
    }
    ~XUnitTest(){}
    void run(){ 
        _rs = true; 
        std::wcout << "============ start test ============\n\n";
        test(); 
        std::wcout << "\n============ start end  ============\n";
    }

protected:
    virtual void test() = 0;
    bool test(const bool rs, const wchar_t* msg = nullptr){
        if (!_rs) return false;
        if (msg) std::wcout << (rs ? "PASS" : "FAIL") << ": " << msg << "\n";
        if (!rs) _rs = rs;
        return rs;
    }

private:
    bool    _rs;
};

#define X_RUN_TEST(Cls) Cls##Test().run()
#define X_TEST(op,rs)   {op;XUnitTest::test(rs,L#op);}
//#define X_TEST_COMPARE(op,rs)   XUnitTest::test(op==rs,L#op)

#endif //_X_UNIT_TEST_HPP
