// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:25


package rs.ac.bg.etf.pp1.ast;

public class ClassMultipleArrayIdentifier extends ClassFieldVarIdentifiersList {

    private ClassFieldVarIdentifiersList ClassFieldVarIdentifiersList;
    private String identName;

    public ClassMultipleArrayIdentifier (ClassFieldVarIdentifiersList ClassFieldVarIdentifiersList, String identName) {
        this.ClassFieldVarIdentifiersList=ClassFieldVarIdentifiersList;
        if(ClassFieldVarIdentifiersList!=null) ClassFieldVarIdentifiersList.setParent(this);
        this.identName=identName;
    }

    public ClassFieldVarIdentifiersList getClassFieldVarIdentifiersList() {
        return ClassFieldVarIdentifiersList;
    }

    public void setClassFieldVarIdentifiersList(ClassFieldVarIdentifiersList ClassFieldVarIdentifiersList) {
        this.ClassFieldVarIdentifiersList=ClassFieldVarIdentifiersList;
    }

    public String getIdentName() {
        return identName;
    }

    public void setIdentName(String identName) {
        this.identName=identName;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(ClassFieldVarIdentifiersList!=null) ClassFieldVarIdentifiersList.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(ClassFieldVarIdentifiersList!=null) ClassFieldVarIdentifiersList.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(ClassFieldVarIdentifiersList!=null) ClassFieldVarIdentifiersList.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("ClassMultipleArrayIdentifier(\n");

        if(ClassFieldVarIdentifiersList!=null)
            buffer.append(ClassFieldVarIdentifiersList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(" "+tab+identName);
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [ClassMultipleArrayIdentifier]");
        return buffer.toString();
    }
}
