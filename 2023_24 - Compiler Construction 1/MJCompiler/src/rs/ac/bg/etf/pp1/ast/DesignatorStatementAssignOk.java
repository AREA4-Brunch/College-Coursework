// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:25


package rs.ac.bg.etf.pp1.ast;

public class DesignatorStatementAssignOk extends DesignatorStatementAssign {

    private DesignatorAssign DesignatorAssign;

    public DesignatorStatementAssignOk (DesignatorAssign DesignatorAssign) {
        this.DesignatorAssign=DesignatorAssign;
        if(DesignatorAssign!=null) DesignatorAssign.setParent(this);
    }

    public DesignatorAssign getDesignatorAssign() {
        return DesignatorAssign;
    }

    public void setDesignatorAssign(DesignatorAssign DesignatorAssign) {
        this.DesignatorAssign=DesignatorAssign;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(DesignatorAssign!=null) DesignatorAssign.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(DesignatorAssign!=null) DesignatorAssign.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(DesignatorAssign!=null) DesignatorAssign.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("DesignatorStatementAssignOk(\n");

        if(DesignatorAssign!=null)
            buffer.append(DesignatorAssign.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [DesignatorStatementAssignOk]");
        return buffer.toString();
    }
}
