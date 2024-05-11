// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:26


package rs.ac.bg.etf.pp1.ast;

public class SingleDesignatorStatementListAssign extends DesignatorStatementList {

    private DesignatorStatementAssignNoSemi DesignatorStatementAssignNoSemi;

    public SingleDesignatorStatementListAssign (DesignatorStatementAssignNoSemi DesignatorStatementAssignNoSemi) {
        this.DesignatorStatementAssignNoSemi=DesignatorStatementAssignNoSemi;
        if(DesignatorStatementAssignNoSemi!=null) DesignatorStatementAssignNoSemi.setParent(this);
    }

    public DesignatorStatementAssignNoSemi getDesignatorStatementAssignNoSemi() {
        return DesignatorStatementAssignNoSemi;
    }

    public void setDesignatorStatementAssignNoSemi(DesignatorStatementAssignNoSemi DesignatorStatementAssignNoSemi) {
        this.DesignatorStatementAssignNoSemi=DesignatorStatementAssignNoSemi;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(DesignatorStatementAssignNoSemi!=null) DesignatorStatementAssignNoSemi.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(DesignatorStatementAssignNoSemi!=null) DesignatorStatementAssignNoSemi.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(DesignatorStatementAssignNoSemi!=null) DesignatorStatementAssignNoSemi.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("SingleDesignatorStatementListAssign(\n");

        if(DesignatorStatementAssignNoSemi!=null)
            buffer.append(DesignatorStatementAssignNoSemi.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [SingleDesignatorStatementListAssign]");
        return buffer.toString();
    }
}
