// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:25


package rs.ac.bg.etf.pp1.ast;

public class SingleConstAssignment extends ConstAssignmentList {

    private String identName;
    private ConstLiteral ConstLiteral;

    public SingleConstAssignment (String identName, ConstLiteral ConstLiteral) {
        this.identName=identName;
        this.ConstLiteral=ConstLiteral;
        if(ConstLiteral!=null) ConstLiteral.setParent(this);
    }

    public String getIdentName() {
        return identName;
    }

    public void setIdentName(String identName) {
        this.identName=identName;
    }

    public ConstLiteral getConstLiteral() {
        return ConstLiteral;
    }

    public void setConstLiteral(ConstLiteral ConstLiteral) {
        this.ConstLiteral=ConstLiteral;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(ConstLiteral!=null) ConstLiteral.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(ConstLiteral!=null) ConstLiteral.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(ConstLiteral!=null) ConstLiteral.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("SingleConstAssignment(\n");

        buffer.append(" "+tab+identName);
        buffer.append("\n");

        if(ConstLiteral!=null)
            buffer.append(ConstLiteral.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [SingleConstAssignment]");
        return buffer.toString();
    }
}
