// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:26


package rs.ac.bg.etf.pp1.ast;

public class DesignatorStatementMisc extends DesignatorStatement {

    private Designator Designator;
    private DesignatorDetail DesignatorDetail;

    public DesignatorStatementMisc (Designator Designator, DesignatorDetail DesignatorDetail) {
        this.Designator=Designator;
        if(Designator!=null) Designator.setParent(this);
        this.DesignatorDetail=DesignatorDetail;
        if(DesignatorDetail!=null) DesignatorDetail.setParent(this);
    }

    public Designator getDesignator() {
        return Designator;
    }

    public void setDesignator(Designator Designator) {
        this.Designator=Designator;
    }

    public DesignatorDetail getDesignatorDetail() {
        return DesignatorDetail;
    }

    public void setDesignatorDetail(DesignatorDetail DesignatorDetail) {
        this.DesignatorDetail=DesignatorDetail;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(Designator!=null) Designator.accept(visitor);
        if(DesignatorDetail!=null) DesignatorDetail.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(Designator!=null) Designator.traverseTopDown(visitor);
        if(DesignatorDetail!=null) DesignatorDetail.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(Designator!=null) Designator.traverseBottomUp(visitor);
        if(DesignatorDetail!=null) DesignatorDetail.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("DesignatorStatementMisc(\n");

        if(Designator!=null)
            buffer.append(Designator.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(DesignatorDetail!=null)
            buffer.append(DesignatorDetail.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [DesignatorStatementMisc]");
        return buffer.toString();
    }
}
