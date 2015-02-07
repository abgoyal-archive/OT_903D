
package proguard.classfile.attribute.annotation;

import proguard.classfile.*;
import proguard.classfile.attribute.visitor.AttributeVisitor;

public class RuntimeInvisibleAnnotationsAttribute extends AnnotationsAttribute
{
    /**
     * Creates an uninitialized RuntimeInvisibleAnnotationsAttribute.
     */
    public RuntimeInvisibleAnnotationsAttribute()
    {
    }


    /**
     * Creates an initialized RuntimeInvisibleAnnotationsAttribute.
     */
    public RuntimeInvisibleAnnotationsAttribute(int          u2attributeNameIndex,
                                                int          u2annotationsCount,
                                                Annotation[] annotations)
    {
        super(u2attributeNameIndex, u2annotationsCount, annotations);
    }


// Implementations for Attribute.

    public void accept(Clazz clazz, AttributeVisitor attributeVisitor)
    {
        attributeVisitor.visitRuntimeInvisibleAnnotationsAttribute(clazz, this);
    }


    public void accept(Clazz clazz, Field field, AttributeVisitor attributeVisitor)
    {
        attributeVisitor.visitRuntimeInvisibleAnnotationsAttribute(clazz, field, this);
    }


    public void accept(Clazz clazz, Method method, AttributeVisitor attributeVisitor)
    {
        attributeVisitor.visitRuntimeInvisibleAnnotationsAttribute(clazz, method, this);
    }
}
