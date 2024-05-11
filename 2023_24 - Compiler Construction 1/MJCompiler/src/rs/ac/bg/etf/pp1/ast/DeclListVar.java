// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:25


package rs.ac.bg.etf.pp1.ast;

public class DeclListVar extends DeclList {

    private DeclList DeclList;
    private SafeVarDecl SafeVarDecl;

    public DeclListVar (DeclList DeclList, SafeVarDecl SafeVarDecl) {
        this.DeclList=DeclList;
        if(DeclList!=null) DeclList.setParent(this);
        this.SafeVarDecl=SafeVarDecl;
        if(SafeVarDecl!=null) SafeVarDecl.setParent(this);
    }

    public DeclList getDeclList() {
        return DeclList;
    }

    public void setDeclList(DeclList DeclList) {
        this.DeclList=DeclList;
    }

    public SafeVarDecl getSafeVarDecl() {
        return SafeVarDecl;
    }

    public void setSafeVarDecl(SafeVarDecl SafeVarDecl) {
        this.SafeVarDecl=SafeVarDecl;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(DeclList!=null) DeclList.accept(visitor);
        if(SafeVarDecl!=null) SafeVarDecl.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(DeclList!=null) DeclList.traverseTopDown(visitor);
        if(SafeVarDecl!=null) SafeVarDecl.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(DeclList!=null) DeclList.traverseBottomUp(visitor);
        if(SafeVarDecl!=null) SafeVarDecl.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("DeclListVar(\n");

        if(DeclList!=null)
            buffer.append(DeclList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(SafeVarDecl!=null)
            buffer.append(SafeVarDecl.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [DeclListVar]");
        return buffer.toString();
    }
}
