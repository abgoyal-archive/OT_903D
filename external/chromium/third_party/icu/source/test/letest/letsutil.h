
#ifndef __LETSUTIL_H
#define __LETSUTIL_H

#include "unicode/utypes.h"
#include "unicode/unistr.h"
#include "unicode/ubidi.h"

#include "layout/LETypes.h"
#include "layout/LEScripts.h"
#include "layout/LayoutEngine.h"
#include "layout/LELanguages.h"

#include "OpenTypeLayoutEngine.h"

#include "letest.h"

char *getCString(const UnicodeString *uString);
char *getCString(const LEUnicode16 *uChars);
char *getUTF8String(const UnicodeString *uString);
void freeCString(char *cString);
le_bool getRTL(const UnicodeString &text);
le_int32 getLanguageCode(const char *lang);

#endif