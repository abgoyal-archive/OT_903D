#ifndef TOUPPTRN_H
#define TOUPPTRN_H

#include "unicode/utypes.h"

#if !UCONFIG_NO_TRANSLITERATION

#include "unicode/translit.h"
#include "casetrn.h"

U_NAMESPACE_BEGIN

class UppercaseTransliterator : public CaseMapTransliterator {

 public:

    /**
     * Constructs a transliterator.
     * @param loc the given locale.
     */
    UppercaseTransliterator();

    /**
     * Destructor.
     */
    virtual ~UppercaseTransliterator();

    /**
     * Copy constructor.
     */
    UppercaseTransliterator(const UppercaseTransliterator&);

    /**
     * Transliterator API.
     * @return a copy of the object.
     */
    virtual Transliterator* clone(void) const;

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    virtual UClassID getDynamicClassID() const;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    U_I18N_API static UClassID U_EXPORT2 getStaticClassID();

private:
    /**
     * Assignment operator.
     */
    UppercaseTransliterator& operator=(const UppercaseTransliterator&);
};

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_TRANSLITERATION */

#endif
