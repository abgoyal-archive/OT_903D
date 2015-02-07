
#include "unicode/uniset.h"
#include "intltest.h"

/** Helper function for TestUnicodeData */
U_CFUNC void U_CALLCONV unicodeDataLineFn(void *context,
                              char *fields[][2], int32_t fieldCount,
                              UErrorCode *pErrorCode);

U_CFUNC void U_CALLCONV
derivedCorePropsLineFn(void *context,
                       char *fields[][2], int32_t fieldCount,
                       UErrorCode *pErrorCode);

class UnicodeTest: public IntlTest {
public:
    UnicodeTest();
    virtual ~UnicodeTest();
    
    void runIndexedTest( int32_t index, UBool exec, const char* &name, char* par = NULL );

    void TestAdditionalProperties();
    void TestBinaryValues();

private:

    friend void U_CALLCONV unicodeDataLineFn(void *context,
                              char *fields[][2], int32_t fieldCount,
                              UErrorCode *pErrorCode);

    friend void U_CALLCONV
    derivedCorePropsLineFn(void *context,
                           char *fields[][2], int32_t fieldCount,
                           UErrorCode *pErrorCode);

    UnicodeSet derivedCoreProps[30];
};

