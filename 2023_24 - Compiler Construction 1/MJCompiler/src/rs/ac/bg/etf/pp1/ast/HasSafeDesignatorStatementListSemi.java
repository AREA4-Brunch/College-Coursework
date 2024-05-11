// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:26


package rs.ac.bg.etf.pp1.ast;

public class HasSafeDesignatorStatementListSemi extends SafeDesignatorStatementListSemi {

    private DesignatorStatementListSemi DesignatorStatementListSemi;

    public HasSafeDesignatorStatementListSemi (DesignatorStatementListSemi DesignatorStatementListSemi) {
        this.DesignatorStatementListSemi=DesignatorStatementListSemi;
        if(DesignatorStatementListSemi!=null) DesignatorStatementListSemi.setParent(this);
    }

    public DesignatorStatementListSemi getDesignatorStatementListSemi() {
        return DesignatorStatementListSemi;
    }

    public void setDesignatorStatementListSemi(DesignatorStatementListSemi DesignatorStatementListSemi) {
        this.DesignatorStatementListSemi=DesignatorStatementListSemi;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(DesignatorStatementListSemi!=null) DesignatorStatementListSemi.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(DesignatorStatementListSemi!=null) DesignatorStatementListSemi.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(DesignatorStatementListSemi!=null) DesignatorStatementListSemi.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("HasSafeDesignatorStatementListSemi(\n");

        if(DesignatorStatementListSemi!=null)
            buffer.append(DesignatorStatementListSemi.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [HasSafeDesignatorStatementListSemi]");
        return buffer.toString();
    }
}
