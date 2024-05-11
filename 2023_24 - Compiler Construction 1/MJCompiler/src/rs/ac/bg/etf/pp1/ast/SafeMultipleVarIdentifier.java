// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:25


package rs.ac.bg.etf.pp1.ast;

public class SafeMultipleVarIdentifier extends SafeVarIdentifiersList {

    private SafeVarIdentifiersList SafeVarIdentifiersList;
    private String identName;

    public SafeMultipleVarIdentifier (SafeVarIdentifiersList SafeVarIdentifiersList, String identName) {
        this.SafeVarIdentifiersList=SafeVarIdentifiersList;
        if(SafeVarIdentifiersList!=null) SafeVarIdentifiersList.setParent(this);
        this.identName=identName;
    }

    public SafeVarIdentifiersList getSafeVarIdentifiersList() {
        return SafeVarIdentifiersList;
    }

    public void setSafeVarIdentifiersList(SafeVarIdentifiersList SafeVarIdentifiersList) {
        this.SafeVarIdentifiersList=SafeVarIdentifiersList;
    }

    public String getIdentName() {
        return identName;
    }

    public void setIdentName(String identName) {
        this.identName=identName;
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
        buffer.append("SafeMultipleVarIdentifier(\n");

        if(SafeVarIdentifiersList!=null)
            buffer.append(SafeVarIdentifiersList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(" "+tab+identName);
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [SafeMultipleVarIdentifier]");
        return buffer.toString();
    }
}
