// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:26


package rs.ac.bg.etf.pp1.ast;

public class FirstDesignatorDetailList extends DesignatorDetailList {

    private DesignatorDetailList DesignatorDetailList;
    private String I2;

    public FirstDesignatorDetailList (DesignatorDetailList DesignatorDetailList, String I2) {
        this.DesignatorDetailList=DesignatorDetailList;
        if(DesignatorDetailList!=null) DesignatorDetailList.setParent(this);
        this.I2=I2;
    }

    public DesignatorDetailList getDesignatorDetailList() {
        return DesignatorDetailList;
    }

    public void setDesignatorDetailList(DesignatorDetailList DesignatorDetailList) {
        this.DesignatorDetailList=DesignatorDetailList;
    }

    public String getI2() {
        return I2;
    }

    public void setI2(String I2) {
        this.I2=I2;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(DesignatorDetailList!=null) DesignatorDetailList.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(DesignatorDetailList!=null) DesignatorDetailList.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(DesignatorDetailList!=null) DesignatorDetailList.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("FirstDesignatorDetailList(\n");

        if(DesignatorDetailList!=null)
            buffer.append(DesignatorDetailList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(" "+tab+I2);
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [FirstDesignatorDetailList]");
        return buffer.toString();
    }
}
