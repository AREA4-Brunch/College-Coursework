// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:25


package rs.ac.bg.etf.pp1.ast;

public class MultipleArrayIdentifier extends VarIdentifiersList {

    private VarIdentifiersList VarIdentifiersList;
    private String identName;

    public MultipleArrayIdentifier (VarIdentifiersList VarIdentifiersList, String identName) {
        this.VarIdentifiersList=VarIdentifiersList;
        if(VarIdentifiersList!=null) VarIdentifiersList.setParent(this);
        this.identName=identName;
    }

    public VarIdentifiersList getVarIdentifiersList() {
        return VarIdentifiersList;
    }

    public void setVarIdentifiersList(VarIdentifiersList VarIdentifiersList) {
        this.VarIdentifiersList=VarIdentifiersList;
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
        if(VarIdentifiersList!=null) VarIdentifiersList.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(VarIdentifiersList!=null) VarIdentifiersList.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(VarIdentifiersList!=null) VarIdentifiersList.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("MultipleArrayIdentifier(\n");

        if(VarIdentifiersList!=null)
            buffer.append(VarIdentifiersList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(" "+tab+identName);
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [MultipleArrayIdentifier]");
        return buffer.toString();
    }
}
