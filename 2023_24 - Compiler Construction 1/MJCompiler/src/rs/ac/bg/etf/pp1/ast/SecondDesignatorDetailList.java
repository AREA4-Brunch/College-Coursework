// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:26


package rs.ac.bg.etf.pp1.ast;

public class SecondDesignatorDetailList extends DesignatorDetailList {

    private DesignatorDetailList DesignatorDetailList;
    private Expr Expr;

    public SecondDesignatorDetailList (DesignatorDetailList DesignatorDetailList, Expr Expr) {
        this.DesignatorDetailList=DesignatorDetailList;
        if(DesignatorDetailList!=null) DesignatorDetailList.setParent(this);
        this.Expr=Expr;
        if(Expr!=null) Expr.setParent(this);
    }

    public DesignatorDetailList getDesignatorDetailList() {
        return DesignatorDetailList;
    }

    public void setDesignatorDetailList(DesignatorDetailList DesignatorDetailList) {
        this.DesignatorDetailList=DesignatorDetailList;
    }

    public Expr getExpr() {
        return Expr;
    }

    public void setExpr(Expr Expr) {
        this.Expr=Expr;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(DesignatorDetailList!=null) DesignatorDetailList.accept(visitor);
        if(Expr!=null) Expr.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(DesignatorDetailList!=null) DesignatorDetailList.traverseTopDown(visitor);
        if(Expr!=null) Expr.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(DesignatorDetailList!=null) DesignatorDetailList.traverseBottomUp(visitor);
        if(Expr!=null) Expr.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("SecondDesignatorDetailList(\n");

        if(DesignatorDetailList!=null)
            buffer.append(DesignatorDetailList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(Expr!=null)
            buffer.append(Expr.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [SecondDesignatorDetailList]");
        return buffer.toString();
    }
}
