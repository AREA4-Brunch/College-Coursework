// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:25


package rs.ac.bg.etf.pp1.ast;

public class ClassDeclExtendsNoStaticVarDeclErr extends ClassDecl {

    private ClassName ClassName;
    private StaticInitList StaticInitList;
    private ClassDeclSafeVarDeclList ClassDeclSafeVarDeclList;
    private OptClassBody OptClassBody;

    public ClassDeclExtendsNoStaticVarDeclErr (ClassName ClassName, StaticInitList StaticInitList, ClassDeclSafeVarDeclList ClassDeclSafeVarDeclList, OptClassBody OptClassBody) {
        this.ClassName=ClassName;
        if(ClassName!=null) ClassName.setParent(this);
        this.StaticInitList=StaticInitList;
        if(StaticInitList!=null) StaticInitList.setParent(this);
        this.ClassDeclSafeVarDeclList=ClassDeclSafeVarDeclList;
        if(ClassDeclSafeVarDeclList!=null) ClassDeclSafeVarDeclList.setParent(this);
        this.OptClassBody=OptClassBody;
        if(OptClassBody!=null) OptClassBody.setParent(this);
    }

    public ClassName getClassName() {
        return ClassName;
    }

    public void setClassName(ClassName ClassName) {
        this.ClassName=ClassName;
    }

    public StaticInitList getStaticInitList() {
        return StaticInitList;
    }

    public void setStaticInitList(StaticInitList StaticInitList) {
        this.StaticInitList=StaticInitList;
    }

    public ClassDeclSafeVarDeclList getClassDeclSafeVarDeclList() {
        return ClassDeclSafeVarDeclList;
    }

    public void setClassDeclSafeVarDeclList(ClassDeclSafeVarDeclList ClassDeclSafeVarDeclList) {
        this.ClassDeclSafeVarDeclList=ClassDeclSafeVarDeclList;
    }

    public OptClassBody getOptClassBody() {
        return OptClassBody;
    }

    public void setOptClassBody(OptClassBody OptClassBody) {
        this.OptClassBody=OptClassBody;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(ClassName!=null) ClassName.accept(visitor);
        if(StaticInitList!=null) StaticInitList.accept(visitor);
        if(ClassDeclSafeVarDeclList!=null) ClassDeclSafeVarDeclList.accept(visitor);
        if(OptClassBody!=null) OptClassBody.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(ClassName!=null) ClassName.traverseTopDown(visitor);
        if(StaticInitList!=null) StaticInitList.traverseTopDown(visitor);
        if(ClassDeclSafeVarDeclList!=null) ClassDeclSafeVarDeclList.traverseTopDown(visitor);
        if(OptClassBody!=null) OptClassBody.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(ClassName!=null) ClassName.traverseBottomUp(visitor);
        if(StaticInitList!=null) StaticInitList.traverseBottomUp(visitor);
        if(ClassDeclSafeVarDeclList!=null) ClassDeclSafeVarDeclList.traverseBottomUp(visitor);
        if(OptClassBody!=null) OptClassBody.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("ClassDeclExtendsNoStaticVarDeclErr(\n");

        if(ClassName!=null)
            buffer.append(ClassName.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(StaticInitList!=null)
            buffer.append(StaticInitList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(ClassDeclSafeVarDeclList!=null)
            buffer.append(ClassDeclSafeVarDeclList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(OptClassBody!=null)
            buffer.append(OptClassBody.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [ClassDeclExtendsNoStaticVarDeclErr]");
        return buffer.toString();
    }
}
