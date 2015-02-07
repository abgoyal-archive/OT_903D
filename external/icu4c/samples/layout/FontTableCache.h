
#ifndef __FONTTABLECACHE_H

#define __FONTTABLECACHE_H

#include "layout/LETypes.h"

struct FontTableCacheEntry;

class FontTableCache
{
public:
    FontTableCache();

    virtual ~FontTableCache();

    const void *find(LETag tableTag) const;

protected:
    virtual const void *readFontTable(LETag tableTag) const = 0;
    virtual void freeFontTable(const void *table) const;

private:

    void add(LETag tableTag, const void *table);

    FontTableCacheEntry *fTableCache;
    le_int32 fTableCacheCurr;
    le_int32 fTableCacheSize;
};

#endif

