// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:25


package rs.ac.bg.etf.pp1.ast;

public class MultipleStaticInitListMul extends NonEmptyStaticInitList {

    private NonEmptyStaticInitList NonEmptyStaticInitList;
    private StaticInitializer StaticInitializer;

    public MultipleStaticInitListMul (NonEmptyStaticInitList NonEmptyStaticInitList, StaticInitializer StaticInitializer) {
        this.NonEmptyStaticInitList=NonEmptyStaticInitList;
        if(NonEmptyStaticInitList!=null) NonEmptyStaticInitList.setParent(this);
        this.StaticInitializer=StaticInitializer;
        if(StaticInitializer!=null) StaticInitializer.setParent(this);
    }

    public NonEmptyStaticInitList getNonEmptyStaticInitList() {
        return NonEmptyStaticInitList;
    }

    public void setNonEmptyStaticInitList(NonEmptyStaticInitList NonEmptyStaticInitList) {
        this.NonEmptyStaticInitList=NonEmptyStaticInitList;
    }

    public StaticInitializer getStaticInitializer() {
        return StaticInitializer;
    }

    public void setStaticInitializer(StaticInitializer StaticInitializer) {
        this.StaticInitializer=StaticInitializer;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(NonEmptyStaticInitList!=null) NonEmptyStaticInitList.accept(visitor);
        if(StaticInitializer!=null) StaticInitializer.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(NonEmptyStaticInitList!=null) NonEmptyStaticInitList.traverseTopDown(visitor);
        if(StaticInitializer!=null) StaticInitializer.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(NonEmptyStaticInitList!=null) NonEmptyStaticInitList.traverseBottomUp(visitor);
        if(StaticInitializer!=null) StaticInitializer.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("MultipleStaticInitListMul(\n");

        if(NonEmptyStaticInitList!=null)
            buffer.append(NonEmptyStaticInitList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(StaticInitializer!=null)
            buffer.append(StaticInitializer.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [MultipleStaticInitListMul]");
        return buffer.toString();
    }
}
