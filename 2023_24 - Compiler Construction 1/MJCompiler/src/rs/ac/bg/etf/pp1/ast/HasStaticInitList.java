// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:25


package rs.ac.bg.etf.pp1.ast;

public class HasStaticInitList extends StaticInitList {

    private NonEmptyStaticInitList NonEmptyStaticInitList;

    public HasStaticInitList (NonEmptyStaticInitList NonEmptyStaticInitList) {
        this.NonEmptyStaticInitList=NonEmptyStaticInitList;
        if(NonEmptyStaticInitList!=null) NonEmptyStaticInitList.setParent(this);
    }

    public NonEmptyStaticInitList getNonEmptyStaticInitList() {
        return NonEmptyStaticInitList;
    }

    public void setNonEmptyStaticInitList(NonEmptyStaticInitList NonEmptyStaticInitList) {
        this.NonEmptyStaticInitList=NonEmptyStaticInitList;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(NonEmptyStaticInitList!=null) NonEmptyStaticInitList.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(NonEmptyStaticInitList!=null) NonEmptyStaticInitList.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(NonEmptyStaticInitList!=null) NonEmptyStaticInitList.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("HasStaticInitList(\n");

        if(NonEmptyStaticInitList!=null)
            buffer.append(NonEmptyStaticInitList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [HasStaticInitList]");
        return buffer.toString();
    }
}
