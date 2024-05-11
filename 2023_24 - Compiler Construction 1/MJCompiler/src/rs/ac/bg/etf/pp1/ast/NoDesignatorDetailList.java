// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:26


package rs.ac.bg.etf.pp1.ast;

public class NoDesignatorDetailList extends DesignatorDetailList {

    public NoDesignatorDetailList () {
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("NoDesignatorDetailList(\n");

        buffer.append(tab);
        buffer.append(") [NoDesignatorDetailList]");
        return buffer.toString();
    }
}
