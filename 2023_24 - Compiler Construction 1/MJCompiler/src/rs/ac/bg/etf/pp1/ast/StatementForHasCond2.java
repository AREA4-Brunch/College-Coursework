// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:26


package rs.ac.bg.etf.pp1.ast;

public class StatementForHasCond2 extends Statement {

    private ForLoopStart ForLoopStart;
    private SafeDesignatorStatementListSemi SafeDesignatorStatementListSemi;
    private ForLoopConditionBegin ForLoopConditionBegin;
    private CondFact CondFact;
    private ForLoopConditionEnd ForLoopConditionEnd;
    private ForLoopPostStmntBegin ForLoopPostStmntBegin;
    private OptDesignatorStatementList OptDesignatorStatementList;
    private ForLoopPostStmntEnd ForLoopPostStmntEnd;
    private ForLoopBodyBegin ForLoopBodyBegin;
    private Statement Statement;
    private ForLoopBodyEnd ForLoopBodyEnd;

    public StatementForHasCond2 (ForLoopStart ForLoopStart, SafeDesignatorStatementListSemi SafeDesignatorStatementListSemi, ForLoopConditionBegin ForLoopConditionBegin, CondFact CondFact, ForLoopConditionEnd ForLoopConditionEnd, ForLoopPostStmntBegin ForLoopPostStmntBegin, OptDesignatorStatementList OptDesignatorStatementList, ForLoopPostStmntEnd ForLoopPostStmntEnd, ForLoopBodyBegin ForLoopBodyBegin, Statement Statement, ForLoopBodyEnd ForLoopBodyEnd) {
        this.ForLoopStart=ForLoopStart;
        if(ForLoopStart!=null) ForLoopStart.setParent(this);
        this.SafeDesignatorStatementListSemi=SafeDesignatorStatementListSemi;
        if(SafeDesignatorStatementListSemi!=null) SafeDesignatorStatementListSemi.setParent(this);
        this.ForLoopConditionBegin=ForLoopConditionBegin;
        if(ForLoopConditionBegin!=null) ForLoopConditionBegin.setParent(this);
        this.CondFact=CondFact;
        if(CondFact!=null) CondFact.setParent(this);
        this.ForLoopConditionEnd=ForLoopConditionEnd;
        if(ForLoopConditionEnd!=null) ForLoopConditionEnd.setParent(this);
        this.ForLoopPostStmntBegin=ForLoopPostStmntBegin;
        if(ForLoopPostStmntBegin!=null) ForLoopPostStmntBegin.setParent(this);
        this.OptDesignatorStatementList=OptDesignatorStatementList;
        if(OptDesignatorStatementList!=null) OptDesignatorStatementList.setParent(this);
        this.ForLoopPostStmntEnd=ForLoopPostStmntEnd;
        if(ForLoopPostStmntEnd!=null) ForLoopPostStmntEnd.setParent(this);
        this.ForLoopBodyBegin=ForLoopBodyBegin;
        if(ForLoopBodyBegin!=null) ForLoopBodyBegin.setParent(this);
        this.Statement=Statement;
        if(Statement!=null) Statement.setParent(this);
        this.ForLoopBodyEnd=ForLoopBodyEnd;
        if(ForLoopBodyEnd!=null) ForLoopBodyEnd.setParent(this);
    }

    public ForLoopStart getForLoopStart() {
        return ForLoopStart;
    }

    public void setForLoopStart(ForLoopStart ForLoopStart) {
        this.ForLoopStart=ForLoopStart;
    }

    public SafeDesignatorStatementListSemi getSafeDesignatorStatementListSemi() {
        return SafeDesignatorStatementListSemi;
    }

    public void setSafeDesignatorStatementListSemi(SafeDesignatorStatementListSemi SafeDesignatorStatementListSemi) {
        this.SafeDesignatorStatementListSemi=SafeDesignatorStatementListSemi;
    }

    public ForLoopConditionBegin getForLoopConditionBegin() {
        return ForLoopConditionBegin;
    }

    public void setForLoopConditionBegin(ForLoopConditionBegin ForLoopConditionBegin) {
        this.ForLoopConditionBegin=ForLoopConditionBegin;
    }

    public CondFact getCondFact() {
        return CondFact;
    }

    public void setCondFact(CondFact CondFact) {
        this.CondFact=CondFact;
    }

    public ForLoopConditionEnd getForLoopConditionEnd() {
        return ForLoopConditionEnd;
    }

    public void setForLoopConditionEnd(ForLoopConditionEnd ForLoopConditionEnd) {
        this.ForLoopConditionEnd=ForLoopConditionEnd;
    }

    public ForLoopPostStmntBegin getForLoopPostStmntBegin() {
        return ForLoopPostStmntBegin;
    }

    public void setForLoopPostStmntBegin(ForLoopPostStmntBegin ForLoopPostStmntBegin) {
        this.ForLoopPostStmntBegin=ForLoopPostStmntBegin;
    }

    public OptDesignatorStatementList getOptDesignatorStatementList() {
        return OptDesignatorStatementList;
    }

    public void setOptDesignatorStatementList(OptDesignatorStatementList OptDesignatorStatementList) {
        this.OptDesignatorStatementList=OptDesignatorStatementList;
    }

    public ForLoopPostStmntEnd getForLoopPostStmntEnd() {
        return ForLoopPostStmntEnd;
    }

    public void setForLoopPostStmntEnd(ForLoopPostStmntEnd ForLoopPostStmntEnd) {
        this.ForLoopPostStmntEnd=ForLoopPostStmntEnd;
    }

    public ForLoopBodyBegin getForLoopBodyBegin() {
        return ForLoopBodyBegin;
    }

    public void setForLoopBodyBegin(ForLoopBodyBegin ForLoopBodyBegin) {
        this.ForLoopBodyBegin=ForLoopBodyBegin;
    }

    public Statement getStatement() {
        return Statement;
    }

    public void setStatement(Statement Statement) {
        this.Statement=Statement;
    }

    public ForLoopBodyEnd getForLoopBodyEnd() {
        return ForLoopBodyEnd;
    }

    public void setForLoopBodyEnd(ForLoopBodyEnd ForLoopBodyEnd) {
        this.ForLoopBodyEnd=ForLoopBodyEnd;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(ForLoopStart!=null) ForLoopStart.accept(visitor);
        if(SafeDesignatorStatementListSemi!=null) SafeDesignatorStatementListSemi.accept(visitor);
        if(ForLoopConditionBegin!=null) ForLoopConditionBegin.accept(visitor);
        if(CondFact!=null) CondFact.accept(visitor);
        if(ForLoopConditionEnd!=null) ForLoopConditionEnd.accept(visitor);
        if(ForLoopPostStmntBegin!=null) ForLoopPostStmntBegin.accept(visitor);
        if(OptDesignatorStatementList!=null) OptDesignatorStatementList.accept(visitor);
        if(ForLoopPostStmntEnd!=null) ForLoopPostStmntEnd.accept(visitor);
        if(ForLoopBodyBegin!=null) ForLoopBodyBegin.accept(visitor);
        if(Statement!=null) Statement.accept(visitor);
        if(ForLoopBodyEnd!=null) ForLoopBodyEnd.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(ForLoopStart!=null) ForLoopStart.traverseTopDown(visitor);
        if(SafeDesignatorStatementListSemi!=null) SafeDesignatorStatementListSemi.traverseTopDown(visitor);
        if(ForLoopConditionBegin!=null) ForLoopConditionBegin.traverseTopDown(visitor);
        if(CondFact!=null) CondFact.traverseTopDown(visitor);
        if(ForLoopConditionEnd!=null) ForLoopConditionEnd.traverseTopDown(visitor);
        if(ForLoopPostStmntBegin!=null) ForLoopPostStmntBegin.traverseTopDown(visitor);
        if(OptDesignatorStatementList!=null) OptDesignatorStatementList.traverseTopDown(visitor);
        if(ForLoopPostStmntEnd!=null) ForLoopPostStmntEnd.traverseTopDown(visitor);
        if(ForLoopBodyBegin!=null) ForLoopBodyBegin.traverseTopDown(visitor);
        if(Statement!=null) Statement.traverseTopDown(visitor);
        if(ForLoopBodyEnd!=null) ForLoopBodyEnd.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(ForLoopStart!=null) ForLoopStart.traverseBottomUp(visitor);
        if(SafeDesignatorStatementListSemi!=null) SafeDesignatorStatementListSemi.traverseBottomUp(visitor);
        if(ForLoopConditionBegin!=null) ForLoopConditionBegin.traverseBottomUp(visitor);
        if(CondFact!=null) CondFact.traverseBottomUp(visitor);
        if(ForLoopConditionEnd!=null) ForLoopConditionEnd.traverseBottomUp(visitor);
        if(ForLoopPostStmntBegin!=null) ForLoopPostStmntBegin.traverseBottomUp(visitor);
        if(OptDesignatorStatementList!=null) OptDesignatorStatementList.traverseBottomUp(visitor);
        if(ForLoopPostStmntEnd!=null) ForLoopPostStmntEnd.traverseBottomUp(visitor);
        if(ForLoopBodyBegin!=null) ForLoopBodyBegin.traverseBottomUp(visitor);
        if(Statement!=null) Statement.traverseBottomUp(visitor);
        if(ForLoopBodyEnd!=null) ForLoopBodyEnd.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("StatementForHasCond2(\n");

        if(ForLoopStart!=null)
            buffer.append(ForLoopStart.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(SafeDesignatorStatementListSemi!=null)
            buffer.append(SafeDesignatorStatementListSemi.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(ForLoopConditionBegin!=null)
            buffer.append(ForLoopConditionBegin.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(CondFact!=null)
            buffer.append(CondFact.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(ForLoopConditionEnd!=null)
            buffer.append(ForLoopConditionEnd.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(ForLoopPostStmntBegin!=null)
            buffer.append(ForLoopPostStmntBegin.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(OptDesignatorStatementList!=null)
            buffer.append(OptDesignatorStatementList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(ForLoopPostStmntEnd!=null)
            buffer.append(ForLoopPostStmntEnd.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(ForLoopBodyBegin!=null)
            buffer.append(ForLoopBodyBegin.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(Statement!=null)
            buffer.append(Statement.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(ForLoopBodyEnd!=null)
            buffer.append(ForLoopBodyEnd.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [StatementForHasCond2]");
        return buffer.toString();
    }
}
