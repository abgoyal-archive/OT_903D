
package proguard.classfile.attribute.preverification;

import proguard.classfile.*;
import proguard.classfile.attribute.CodeAttribute;
import proguard.classfile.attribute.preverification.visitor.VerificationTypeVisitor;

public class UninitializedThisType extends VerificationType
{
    // Implementations for VerificationType.

    public int getTag()
    {
        return UNINITIALIZED_THIS_TYPE;
    }


    public void accept(Clazz clazz, Method method, CodeAttribute codeAttribute, int instructionOffset, VerificationTypeVisitor verificationTypeVisitor)
    {
        verificationTypeVisitor.visitUninitializedThisType(clazz, method, codeAttribute, instructionOffset, this);
    }


    public void stackAccept(Clazz clazz, Method method, CodeAttribute codeAttribute, int instructionOffset, int stackIndex, VerificationTypeVisitor verificationTypeVisitor)
    {
        verificationTypeVisitor.visitStackUninitializedThisType(clazz, method, codeAttribute, instructionOffset, stackIndex, this);
    }


    public void variablesAccept(Clazz clazz, Method method, CodeAttribute codeAttribute, int instructionOffset, int variableIndex, VerificationTypeVisitor verificationTypeVisitor)
    {
        verificationTypeVisitor.visitVariablesUninitializedThisType(clazz, method, codeAttribute, instructionOffset, variableIndex, this);
    }


    // Implementations for Object.

    public String toString()
    {
        return "u:this";
    }
}
