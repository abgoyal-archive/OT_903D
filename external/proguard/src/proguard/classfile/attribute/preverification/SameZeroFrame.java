
package proguard.classfile.attribute.preverification;

import proguard.classfile.*;
import proguard.classfile.attribute.CodeAttribute;
import proguard.classfile.attribute.preverification.visitor.StackMapFrameVisitor;

public class SameZeroFrame extends StackMapFrame
{
    /**
     * Creates an uninitialized SameZeroFrame.
     */
    public SameZeroFrame()
    {
    }


    /**
     * Creates a SameZeroFrame with the given tag.
     */
    public SameZeroFrame(int tag)
    {
        u2offsetDelta = tag - SAME_ZERO_FRAME;
    }


    // Implementations for StackMapFrame.

    public int getTag()
    {
        return u2offsetDelta < 64 ?
            SAME_ZERO_FRAME + u2offsetDelta :
            SAME_ZERO_FRAME_EXTENDED;
    }


    public void accept(Clazz clazz, Method method, CodeAttribute codeAttribute, int offset, StackMapFrameVisitor stackMapFrameVisitor)
    {
        stackMapFrameVisitor.visitSameZeroFrame(clazz, method, codeAttribute, offset, this);
    }


    // Implementations for Object.

    public String toString()
    {
        return super.toString()+"Var: ..., Stack: (empty)";
    }
}
