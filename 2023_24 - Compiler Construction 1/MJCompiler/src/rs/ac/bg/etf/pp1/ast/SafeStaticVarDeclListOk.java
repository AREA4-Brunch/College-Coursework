// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:25


package rs.ac.bg.etf.pp1.ast;

public class SafeStaticVarDeclListOk extends SafeStaticVarDeclList {

    private SafeStaticVarDeclList SafeStaticVarDeclList;
    private ClassDeclSafeVarDecl ClassDeclSafeVarDecl;

    public SafeStaticVarDeclListOk (SafeStaticVarDeclList SafeStaticVarDeclList, ClassDeclSafeVarDecl ClassDeclSafeVarDecl) {
        this.SafeStaticVarDeclList=SafeStaticVarDeclList;
        if(SafeStaticVarDeclList!=null) SafeStaticVarDeclList.setParent(this);
        this.ClassDeclSafeVarDecl=ClassDeclSafeVarDecl;
        if(ClassDeclSafeVarDecl!=null) ClassDeclSafeVarDecl.setParent(this);
    }

    public SafeStaticVarDeclList getSafeStaticVarDeclList() {
        return SafeStaticVarDeclList;
    }

    public void setSafeStaticVarDeclList(SafeStaticVarDeclList SafeStaticVarDeclList) {
        this.SafeStaticVarDeclList=SafeStaticVarDeclList;
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
        if(SafeStaticVarDeclList!=null) SafeStaticVarDeclList.accept(visitor);
        if(ClassDeclSafeVarDecl!=null) ClassDeclSafeVarDecl.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(SafeStaticVarDeclList!=null) SafeStaticVarDeclList.traverseTopDown(visitor);
        if(ClassDeclSafeVarDecl!=null) ClassDeclSafeVarDecl.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(SafeStaticVarDeclList!=null) SafeStaticVarDeclList.traverseBottomUp(visitor);
        if(ClassDeclSafeVarDecl!=null) ClassDeclSafeVarDecl.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("SafeStaticVarDeclListOk(\n");

        if(SafeStaticVarDeclList!=null)
            buffer.append(SafeStaticVarDeclList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(ClassDeclSafeVarDecl!=null)
            buffer.append(ClassDeclSafeVarDecl.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [SafeStaticVarDeclListOk]");
        return buffer.toString();
    }
}
