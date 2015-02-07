
package proguard.classfile.attribute.preverification;

import proguard.classfile.*;
import proguard.classfile.attribute.CodeAttribute;
import proguard.classfile.attribute.preverification.visitor.*;

public class MoreZeroFrame extends StackMapFrame
{
    public int                additionalVariablesCount;
    public VerificationType[] additionalVariables;


    /**
     * Creates an uninitialized MoreZeroFrame.
     */
    public MoreZeroFrame()
    {
    }


    /**
     * Creates a MoreZeroFrame with the given tag.
     */
    public MoreZeroFrame(int tag)
    {
        additionalVariablesCount = tag + 1 - MORE_ZERO_FRAME;
    }


    /**
     * Creates a MoreZeroFrame with the given additional variables.
     */
    public MoreZeroFrame(VerificationType[] additionalVariables)
    {
        this(additionalVariables.length, additionalVariables);
    }


    /**
     * Creates a MoreZeroFrame with the given additional variables.
     */
    public MoreZeroFrame(int                additionalVariablesCount,
                         VerificationType[] additionalVariables)
    {
        this.additionalVariablesCount = additionalVariablesCount;
        this.additionalVariables      = additionalVariables;
    }


    /**
     * Applies the given verification type visitor to all variables.
     */
    public void additionalVariablesAccept(Clazz clazz, Method method, CodeAttribute codeAttribute, int offset, VerificationTypeVisitor verificationTypeVisitor)
    {
        for (int index = 0; index < additionalVariablesCount; index++)
        {
            additionalVariables[index].accept(clazz, method, codeAttribute, offset, verificationTypeVisitor);
        }
    }


    // Implementations for StackMapFrame.

    public int getTag()
    {
        return MORE_ZERO_FRAME + additionalVariablesCount - 1;
    }


    public void accept(Clazz clazz, Method method, CodeAttribute codeAttribute, int offset, StackMapFrameVisitor stackMapFrameVisitor)
    {
        stackMapFrameVisitor.visitMoreZeroFrame(clazz, method, codeAttribute, offset, this);
    }


    // Implementations for Object.

    public boolean equals(Object object)
    {
        if (!super.equals(object))
        {
            return false;
        }

        MoreZeroFrame other = (MoreZeroFrame)object;

        if (this.u2offsetDelta            != other.u2offsetDelta ||
            this.additionalVariablesCount != other.additionalVariablesCount)
        {
            return false;
        }

        for (int index = 0; index < additionalVariablesCount; index++)
        {
            VerificationType thisType  = this.additionalVariables[index];
            VerificationType otherType = other.additionalVariables[index];

            if (!thisType.equals(otherType))
            {
                return false;
            }
        }

        return true;
    }


    public int hashCode()
    {
        int hashCode = super.hashCode();

        for (int index = 0; index < additionalVariablesCount; index++)
        {
            hashCode ^= additionalVariables[index].hashCode();
        }

        return hashCode;
    }


    public String toString()
    {
        StringBuffer buffer = new StringBuffer(super.toString()).append("Var: ...");

        for (int index = 0; index < additionalVariablesCount; index++)
        {
            buffer = buffer.append('[')
                           .append(additionalVariables[index].toString())
                           .append(']');
        }

        buffer.append(", Stack: (empty)");

        return buffer.toString();
    }
}
