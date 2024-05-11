// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:26


package rs.ac.bg.etf.pp1.ast;

public class NoNamespaceDesignator extends Designator {

    private String identName;
    private DesignatorDetailList DesignatorDetailList;

    public NoNamespaceDesignator (String identName, DesignatorDetailList DesignatorDetailList) {
        this.identName=identName;
        this.DesignatorDetailList=DesignatorDetailList;
        if(DesignatorDetailList!=null) DesignatorDetailList.setParent(this);
    }

    public String getIdentName() {
        return identName;
    }

    public void setIdentName(String identName) {
        this.identName=identName;
    }

    public DesignatorDetailList getDesignatorDetailList() {
        return DesignatorDetailList;
    }

    public void setDesignatorDetailList(DesignatorDetailList DesignatorDetailList) {
        this.DesignatorDetailList=DesignatorDetailList;
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
        buffer.append("NoNamespaceDesignator(\n");

        buffer.append(" "+tab+identName);
        buffer.append("\n");

        if(DesignatorDetailList!=null)
            buffer.append(DesignatorDetailList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [NoNamespaceDesignator]");
        return buffer.toString();
    }
}
