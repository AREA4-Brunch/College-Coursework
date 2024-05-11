// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:26


package rs.ac.bg.etf.pp1.ast;

public class MultipleDesignatorStatementListSemi extends DesignatorStatementListSemi {

    private DesignatorStatementListSemi DesignatorStatementListSemi;
    private DesignatorStatement DesignatorStatement;

    public MultipleDesignatorStatementListSemi (DesignatorStatementListSemi DesignatorStatementListSemi, DesignatorStatement DesignatorStatement) {
        this.DesignatorStatementListSemi=DesignatorStatementListSemi;
        if(DesignatorStatementListSemi!=null) DesignatorStatementListSemi.setParent(this);
        this.DesignatorStatement=DesignatorStatement;
        if(DesignatorStatement!=null) DesignatorStatement.setParent(this);
    }

    public DesignatorStatementListSemi getDesignatorStatementListSemi() {
        return DesignatorStatementListSemi;
    }

    public void setDesignatorStatementListSemi(DesignatorStatementListSemi DesignatorStatementListSemi) {
        this.DesignatorStatementListSemi=DesignatorStatementListSemi;
    }

    public DesignatorStatement getDesignatorStatement() {
        return DesignatorStatement;
    }

    public void setDesignatorStatement(DesignatorStatement DesignatorStatement) {
        this.DesignatorStatement=DesignatorStatement;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(DesignatorStatementListSemi!=null) DesignatorStatementListSemi.accept(visitor);
        if(DesignatorStatement!=null) DesignatorStatement.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(DesignatorStatementListSemi!=null) DesignatorStatementListSemi.traverseTopDown(visitor);
        if(DesignatorStatement!=null) DesignatorStatement.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(DesignatorStatementListSemi!=null) DesignatorStatementListSemi.traverseBottomUp(visitor);
        if(DesignatorStatement!=null) DesignatorStatement.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("MultipleDesignatorStatementListSemi(\n");

        if(DesignatorStatementListSemi!=null)
            buffer.append(DesignatorStatementListSemi.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(DesignatorStatement!=null)
            buffer.append(DesignatorStatement.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [MultipleDesignatorStatementListSemi]");
        return buffer.toString();
    }
}
