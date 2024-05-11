// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:26


package rs.ac.bg.etf.pp1.ast;

public class DesignatorStatementFuncCall extends DesignatorStatement {

    private FuncCall FuncCall;

    public DesignatorStatementFuncCall (FuncCall FuncCall) {
        this.FuncCall=FuncCall;
        if(FuncCall!=null) FuncCall.setParent(this);
    }

    public FuncCall getFuncCall() {
        return FuncCall;
    }

    public void setFuncCall(FuncCall FuncCall) {
        this.FuncCall=FuncCall;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(FuncCall!=null) FuncCall.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(FuncCall!=null) FuncCall.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(FuncCall!=null) FuncCall.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("DesignatorStatementFuncCall(\n");

        if(FuncCall!=null)
            buffer.append(FuncCall.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [DesignatorStatementFuncCall]");
        return buffer.toString();
    }
}
