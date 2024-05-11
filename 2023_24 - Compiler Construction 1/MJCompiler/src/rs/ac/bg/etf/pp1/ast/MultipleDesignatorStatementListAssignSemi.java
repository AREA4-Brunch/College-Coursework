// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:26


package rs.ac.bg.etf.pp1.ast;

public class MultipleDesignatorStatementListAssignSemi extends DesignatorStatementListSemi {

    private DesignatorStatementListSemi DesignatorStatementListSemi;
    private DesignatorAssign DesignatorAssign;

    public MultipleDesignatorStatementListAssignSemi (DesignatorStatementListSemi DesignatorStatementListSemi, DesignatorAssign DesignatorAssign) {
        this.DesignatorStatementListSemi=DesignatorStatementListSemi;
        if(DesignatorStatementListSemi!=null) DesignatorStatementListSemi.setParent(this);
        this.DesignatorAssign=DesignatorAssign;
        if(DesignatorAssign!=null) DesignatorAssign.setParent(this);
    }

    public DesignatorStatementListSemi getDesignatorStatementListSemi() {
        return DesignatorStatementListSemi;
    }

    public void setDesignatorStatementListSemi(DesignatorStatementListSemi DesignatorStatementListSemi) {
        this.DesignatorStatementListSemi=DesignatorStatementListSemi;
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
        if(DesignatorStatementListSemi!=null) DesignatorStatementListSemi.accept(visitor);
        if(DesignatorAssign!=null) DesignatorAssign.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(DesignatorStatementListSemi!=null) DesignatorStatementListSemi.traverseTopDown(visitor);
        if(DesignatorAssign!=null) DesignatorAssign.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(DesignatorStatementListSemi!=null) DesignatorStatementListSemi.traverseBottomUp(visitor);
        if(DesignatorAssign!=null) DesignatorAssign.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("MultipleDesignatorStatementListAssignSemi(\n");

        if(DesignatorStatementListSemi!=null)
            buffer.append(DesignatorStatementListSemi.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(DesignatorAssign!=null)
            buffer.append(DesignatorAssign.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [MultipleDesignatorStatementListAssignSemi]");
        return buffer.toString();
    }
}
