
package proguard.classfile.visitor;

import proguard.classfile.*;
import proguard.util.*;


public class MemberNameFilter implements MemberVisitor
{
    private final StringMatcher regularExpressionMatcher;
    private final MemberVisitor memberVisitor;


    /**
     * Creates a new MemberNameFilter.
     * @param regularExpression the regular expression against which member
     *                          names will be matched.
     * @param memberVisitor     the <code>MemberVisitor</code> to which visits
     *                          will be delegated.
     */
    public MemberNameFilter(String        regularExpression,
                            MemberVisitor memberVisitor)
    {
        this(new NameParser().parse(regularExpression), memberVisitor);
    }


    /**
     * Creates a new MemberNameFilter.
     * @param regularExpressionMatcher the regular expression against which
     *                                 member names will be matched.
     * @param memberVisitor            the <code>MemberVisitor</code> to which
     *                                 visits will be delegated.
     */
    public MemberNameFilter(StringMatcher regularExpressionMatcher,
                            MemberVisitor memberVisitor)
    {
        this.regularExpressionMatcher = regularExpressionMatcher;
        this.memberVisitor            = memberVisitor;
    }


    // Implementations for MemberVisitor.

    public void visitProgramField(ProgramClass programClass, ProgramField programField)
    {
        if (accepted(programField.getName(programClass)))
        {
            memberVisitor.visitProgramField(programClass, programField);
        }
    }


    public void visitProgramMethod(ProgramClass programClass, ProgramMethod programMethod)
    {
        if (accepted(programMethod.getName(programClass)))
        {
            memberVisitor.visitProgramMethod(programClass, programMethod);
        }
    }


    public void visitLibraryField(LibraryClass libraryClass, LibraryField libraryField)
    {
        if (accepted(libraryField.getName(libraryClass)))
        {
            memberVisitor.visitLibraryField(libraryClass, libraryField);
        }
    }


    public void visitLibraryMethod(LibraryClass libraryClass, LibraryMethod libraryMethod)
    {
        if (accepted(libraryMethod.getName(libraryClass)))
        {
            memberVisitor.visitLibraryMethod(libraryClass, libraryMethod);
        }
    }


    // Small utility methods.

    private boolean accepted(String name)
    {
        return regularExpressionMatcher.matches(name);
    }
}
