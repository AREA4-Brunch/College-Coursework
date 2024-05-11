// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:26


package rs.ac.bg.etf.pp1.ast;

public class MultipleDesignatorStatementListAssign extends DesignatorStatementList {

    private DesignatorStatementList DesignatorStatementList;
    private DesignatorStatementAssignNoSemi DesignatorStatementAssignNoSemi;

    public MultipleDesignatorStatementListAssign (DesignatorStatementList DesignatorStatementList, DesignatorStatementAssignNoSemi DesignatorStatementAssignNoSemi) {
        this.DesignatorStatementList=DesignatorStatementList;
        if(DesignatorStatementList!=null) DesignatorStatementList.setParent(this);
        this.DesignatorStatementAssignNoSemi=DesignatorStatementAssignNoSemi;
        if(DesignatorStatementAssignNoSemi!=null) DesignatorStatementAssignNoSemi.setParent(this);
    }

    public DesignatorStatementList getDesignatorStatementList() {
        return DesignatorStatementList;
    }

    public void setDesignatorStatementList(DesignatorStatementList DesignatorStatementList) {
        this.DesignatorStatementList=DesignatorStatementList;
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
        if(DesignatorStatementList!=null) DesignatorStatementList.accept(visitor);
        if(DesignatorStatementAssignNoSemi!=null) DesignatorStatementAssignNoSemi.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(DesignatorStatementList!=null) DesignatorStatementList.traverseTopDown(visitor);
        if(DesignatorStatementAssignNoSemi!=null) DesignatorStatementAssignNoSemi.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(DesignatorStatementList!=null) DesignatorStatementList.traverseBottomUp(visitor);
        if(DesignatorStatementAssignNoSemi!=null) DesignatorStatementAssignNoSemi.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("MultipleDesignatorStatementListAssign(\n");

        if(DesignatorStatementList!=null)
            buffer.append(DesignatorStatementList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(DesignatorStatementAssignNoSemi!=null)
            buffer.append(DesignatorStatementAssignNoSemi.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [MultipleDesignatorStatementListAssign]");
        return buffer.toString();
    }
}
