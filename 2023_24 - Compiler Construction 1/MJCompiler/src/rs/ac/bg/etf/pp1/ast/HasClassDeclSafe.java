// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:25


package rs.ac.bg.etf.pp1.ast;

public class HasClassDeclSafe extends ClassDeclSafeVarDeclList {

    private ClassDeclSafeVarDeclList ClassDeclSafeVarDeclList;
    private ClassDeclSafeVarDecl ClassDeclSafeVarDecl;

    public HasClassDeclSafe (ClassDeclSafeVarDeclList ClassDeclSafeVarDeclList, ClassDeclSafeVarDecl ClassDeclSafeVarDecl) {
        this.ClassDeclSafeVarDeclList=ClassDeclSafeVarDeclList;
        if(ClassDeclSafeVarDeclList!=null) ClassDeclSafeVarDeclList.setParent(this);
        this.ClassDeclSafeVarDecl=ClassDeclSafeVarDecl;
        if(ClassDeclSafeVarDecl!=null) ClassDeclSafeVarDecl.setParent(this);
    }

    public ClassDeclSafeVarDeclList getClassDeclSafeVarDeclList() {
        return ClassDeclSafeVarDeclList;
    }

    public void setClassDeclSafeVarDeclList(ClassDeclSafeVarDeclList ClassDeclSafeVarDeclList) {
        this.ClassDeclSafeVarDeclList=ClassDeclSafeVarDeclList;
    }

    public ClassDeclSafeVarDecl getClassDeclSafeVarDecl() {
        return ClassDeclSafeVarDecl;
    }

    public void setClassDeclSafeVarDecl(ClassDeclSafeVarDecl ClassDeclSafeVarDecl) {
        this.ClassDeclSafeVarDecl=ClassDeclSafeVarDecl;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(ClassDeclSafeVarDeclList!=null) ClassDeclSafeVarDeclList.accept(visitor);
        if(ClassDeclSafeVarDecl!=null) ClassDeclSafeVarDecl.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(ClassDeclSafeVarDeclList!=null) ClassDeclSafeVarDeclList.traverseTopDown(visitor);
        if(ClassDeclSafeVarDecl!=null) ClassDeclSafeVarDecl.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(ClassDeclSafeVarDeclList!=null) ClassDeclSafeVarDeclList.traverseBottomUp(visitor);
        if(ClassDeclSafeVarDecl!=null) ClassDeclSafeVarDecl.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("HasClassDeclSafe(\n");

        if(ClassDeclSafeVarDeclList!=null)
            buffer.append(ClassDeclSafeVarDeclList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(ClassDeclSafeVarDecl!=null)
            buffer.append(ClassDeclSafeVarDecl.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [HasClassDeclSafe]");
        return buffer.toString();
    }
}
