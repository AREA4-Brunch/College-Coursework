// generated with ast extension for cup
// version 0.8
// 7/1/2024 7:54:25


package rs.ac.bg.etf.pp1.ast;

public class ClassDeclSafeVarDeclOk extends ClassDeclSafeVarDecl {

    private Type Type;
    private ClassFieldVarIdentifiersList ClassFieldVarIdentifiersList;

    public ClassDeclSafeVarDeclOk (Type Type, ClassFieldVarIdentifiersList ClassFieldVarIdentifiersList) {
        this.Type=Type;
        if(Type!=null) Type.setParent(this);
        this.ClassFieldVarIdentifiersList=ClassFieldVarIdentifiersList;
        if(ClassFieldVarIdentifiersList!=null) ClassFieldVarIdentifiersList.setParent(this);
    }

    public Type getType() {
        return Type;
    }

    public void setType(Type Type) {
        this.Type=Type;
    }

    public ClassFieldVarIdentifiersList getClassFieldVarIdentifiersList() {
        return ClassFieldVarIdentifiersList;
    }

    public void setClassFieldVarIdentifiersList(ClassFieldVarIdentifiersList ClassFieldVarIdentifiersList) {
        this.ClassFieldVarIdentifiersList=ClassFieldVarIdentifiersList;
    }

    public void accept(Visitor visitor) {
        visitor.visit(this);
    }

    public void childrenAccept(Visitor visitor) {
        if(Type!=null) Type.accept(visitor);
        if(ClassFieldVarIdentifiersList!=null) ClassFieldVarIdentifiersList.accept(visitor);
    }

    public void traverseTopDown(Visitor visitor) {
        accept(visitor);
        if(Type!=null) Type.traverseTopDown(visitor);
        if(ClassFieldVarIdentifiersList!=null) ClassFieldVarIdentifiersList.traverseTopDown(visitor);
    }

    public void traverseBottomUp(Visitor visitor) {
        if(Type!=null) Type.traverseBottomUp(visitor);
        if(ClassFieldVarIdentifiersList!=null) ClassFieldVarIdentifiersList.traverseBottomUp(visitor);
        accept(visitor);
    }

    public String toString(String tab) {
        StringBuffer buffer=new StringBuffer();
        buffer.append(tab);
        buffer.append("ClassDeclSafeVarDeclOk(\n");

        if(Type!=null)
            buffer.append(Type.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        if(ClassFieldVarIdentifiersList!=null)
            buffer.append(ClassFieldVarIdentifiersList.toString("  "+tab));
        else
            buffer.append(tab+"  null");
        buffer.append("\n");

        buffer.append(tab);
        buffer.append(") [ClassDeclSafeVarDeclOk]");
        return buffer.toString();
    }
}
