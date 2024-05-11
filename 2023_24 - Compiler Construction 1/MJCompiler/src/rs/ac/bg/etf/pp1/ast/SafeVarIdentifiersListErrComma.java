// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:25


package rs.ac.bg.etf.pp1.ast;

public class SafeVarIdentifiersListErrComma extends SafeVarIdentifiersList {

    private SafeVarIdentifiersList SafeVarIdentifiersList;

    public SafeVarIdentifiersListErrComma (SafeVarIdentifiersList SafeVarIdentifiersList) {
        this.SafeVarIdentifiersList=SafeVarIdentifiersList;
        if(SafeVarIdentifiersList!=null) SafeVarIdentifiersList.setParent(this);
    }

    public SafeVarIdentifiersList getSafeVarIdentifiersList() {
        return SafeVarIdentifiersList;
    }

    public void setSafeVarIdentifiersList(SafeVarIdentifiersList SafeVarIdentifiersList) {
        this.SafeVarIdentifiersList=SafeVarIdentifiersList;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(SafeVarIdentifiersList!=null) SafeVarIdentifiersList.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(SafeVarIdentifiersList!=null) SafeVarIdentifiersList.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(SafeVarIdentifiersList!=null) SafeVarIdentifiersList.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("SafeVarIdentifiersListErrComma(\n");

        if(SafeVarIdentifiersList!=null)
            buffer.append(SafeVarIdentifiersList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [SafeVarIdentifiersListErrComma]");
        return buffer.toString();
    }
}
