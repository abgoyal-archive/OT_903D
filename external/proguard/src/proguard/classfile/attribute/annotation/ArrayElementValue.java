
package proguard.classfile.attribute.annotation;

import proguard.classfile.*;
import proguard.classfile.attribute.annotation.visitor.ElementValueVisitor;

public class ArrayElementValue extends ElementValue
{
    public int            u2elementValuesCount;
    public ElementValue[] elementValues;


    /**
     * Creates an uninitialized ArrayElementValue.
     */
    public ArrayElementValue()
    {
    }


    /**
     * Creates an initialized ArrayElementValue.
     */
    public ArrayElementValue(int            u2elementNameIndex,
                             int            u2elementValuesCount,
                             ElementValue[] elementValues)
    {
        super(u2elementNameIndex);

        this.u2elementValuesCount = u2elementValuesCount;
        this.elementValues        = elementValues;
    }


    // Implementations for ElementValue.

    public int getTag()
    {
        return ClassConstants.ELEMENT_VALUE_ARRAY;
    }

    public void accept(Clazz clazz, Annotation annotation, ElementValueVisitor elementValueVisitor)
    {
        elementValueVisitor.visitArrayElementValue(clazz, annotation, this);
    }


    /**
     * Applies the given visitor to all nested element values.
     */
    public void elementValuesAccept(Clazz clazz, Annotation annotation, ElementValueVisitor elementValueVisitor)
    {
        for (int index = 0; index < u2elementValuesCount; index++)
        {
            elementValues[index].accept(clazz, annotation, elementValueVisitor);
        }
    }
}
