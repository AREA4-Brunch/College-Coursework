// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:26


package rs.ac.bg.etf.pp1.ast;

public class FactorNewFactorPars extends Factor {

    private Type Type;
    private FactorPars FactorPars;

    public FactorNewFactorPars (Type Type, FactorPars FactorPars) {
        this.Type=Type;
        if(Type!=null) Type.setParent(this);
        this.FactorPars=FactorPars;
        if(FactorPars!=null) FactorPars.setParent(this);
    }

    public Type getType() {
        return Type;
    }

    public void setType(Type Type) {
        this.Type=Type;
    }

    public FactorPars getFactorPars() {
        return FactorPars;
    }

    public void setFactorPars(FactorPars FactorPars) {
        this.FactorPars=FactorPars;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(Type!=null) Type.accept(visitor);
        if(FactorPars!=null) FactorPars.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(Type!=null) Type.traverseTopDown(visitor);
        if(FactorPars!=null) FactorPars.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(Type!=null) Type.traverseBottomUp(visitor);
        if(FactorPars!=null) FactorPars.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("FactorNewFactorPars(\n");

        if(Type!=null)
            buffer.append(Type.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(FactorPars!=null)
            buffer.append(FactorPars.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [FactorNewFactorPars]");
        return buffer.toString();
    }
}
