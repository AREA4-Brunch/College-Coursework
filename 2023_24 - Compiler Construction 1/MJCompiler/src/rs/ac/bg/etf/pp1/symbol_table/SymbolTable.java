package rs.ac.bg.etf.pp1.symbol_table;

import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import rs.ac.bg.etf.pp1.ast.SyntaxNode;
import rs.ac.bg.etf.pp1.symbol_table.visitors.ExtendedDumpSymbolTableVisitor;
import rs.etf.pp1.symboltable.Tab;
import rs.etf.pp1.symboltable.concepts.Obj;
import rs.etf.pp1.symboltable.concepts.Scope;
import rs.etf.pp1.symboltable.concepts.Struct;
import rs.etf.pp1.symboltable.visitors.SymbolTableVisitor;

public class SymbolTable {

    private static class CurNamespace {
        // public NamespaceName namespace_name;
        public String name;
        public Scope current_scope;
        public CurNamespace(String name, Scope current_scope) {
            this.name = name;
            this.current_scope = current_scope;
        }
    }

    private static SymbolTable instance = null;

    public static SymbolTable getInstance() {
        if (SymbolTable.instance == null) {
            SymbolTable.instance = new SymbolTable();
        }

        return SymbolTable.instance;
    }


    private Set<String> reserved_keywords = null;
    private Map<String, Obj> overloadable_keywords = null;
    private Map<String, Struct> declared_global_types = new HashMap<>();
    private CurNamespace cur_namespace = null;


    public SymbolTable() { }

    public void init() {
        Tab.init();

        // add aditional keywords for types:
        Tab.currentScope().addToLocals(
            new Obj(Obj.Type, "bool", new Struct(Struct.Bool))
        );

        this.initReservedKeywords();
        this.initOverloadableKeywords();
        this.initDeclaredTypes();  // !important must be called after Tab.init()
    }

    public SymbolTableVisitor createVisitor() {
        return new ExtendedDumpSymbolTableVisitor();
    }

    public void openNamespace(String name) {
        this.cur_namespace = new CurNamespace(name, Tab.currentScope());
    }

    public void closeNamespace() {
        this.cur_namespace = null;
    }

    public void openScope() {
        Tab.openScope();
    }

    public void closeScope() {
        Tab.closeScope();
    }

    private void initReservedKeywords() {
        this.reserved_keywords = new HashSet<>(Arrays.asList(
            "program",
            "break",
            "class",
            "else",
            "const",
            "if",
            "new",
            "print",
            "read",
            "return",
            "void",
            "extends",
            "continue",
            "for",
            "static",
            "namespace"


            // Following are allowed to be redeclared by user
            // "int",
            // "char",
            // "eol",
            // "null",
            // "bool",

            // "len",
            // "chr",
            // "ord"
        ));
    }

    private boolean isReservedKeyword(String keyword) {
        return this.reserved_keywords.contains(keyword);
    }

    private void initOverloadableKeywords() {
        String declared_overloadble_keywords[] = {
            "int",
            "char",
            "eol",
            "null",
            "bool",

            "len",
            "chr",
            "ord"
        };

        this.overloadable_keywords = new HashMap<>();
        for (String keyword : declared_overloadble_keywords) {
            Obj node = this.findName(null, keyword);
            this.overloadable_keywords.put(keyword, node);
        }
    }

    /**
     * Returns true if given keyword_node is overloadable under given name,
     * no namespace as keywords originally have no namespace.
     */
    private boolean isOverloadableKeyword(String name, Obj keyword_node) {
        if (keyword_node == null) return false;
        Obj overloadble_keyword = this.overloadable_keywords.getOrDefault(
            name, null
        );
        return overloadble_keyword == keyword_node;
    }

    private void initDeclaredTypes() {
        String declared_global_types[] = { "int", "char", "bool" };

        for (String type_name : declared_global_types) {
            Obj cur = this.findName(null, type_name);
            this.getOrDeclareGType(type_name, cur.getType());
        }
    }

    public Struct getOrDeclareGlobalArrType(Struct elem_type) {
        Struct type = new Struct(Struct.Array);
        type.setElementType(elem_type);
        type = this.getOrDeclareGType(getArrayTypeName(elem_type.getKind()), type);
        return type;
    }

    public Struct getOrDeclareGType(String type_name, Struct type) {
        if (this.declared_global_types.containsKey(type_name)) {
            return this.declared_global_types.get(type_name);
        }

        this.declared_global_types.put(type_name, type);
        return type;
    }

    private String getArrayTypeName(Integer elem_type_kind) {
        switch (elem_type_kind) {
            case Struct.Int: return "int[]";
            case Struct.Char: return "char[]";
            case Struct.Bool: return "bool[]";
            default: return null;
        }
    }

    public Obj registerName(
        int kind,
        String namespace,
        String name,
        Struct type
    ) {
        if (this.isReservedKeyword(name)) {
            report_error(
                name + " is a reserved keyword in " + SymbolTable.class.getName(),
                null
            );
            return Tab.noObj;
        }

        // register only if it has not been declared already in same scope
        if (this.findNameInSameScope(namespace, name) != Tab.noObj) {
            return Tab.noObj;
        }

        if (namespace != null) name = this.prependNamespace(namespace, name);
        return Tab.insert(kind, name, type);
    }

    public Obj findName(String namespace, String name) {
        if (namespace != null) {
            name = this.prependNamespace(namespace, name);
            return Tab.find(name);
        }

        if (this.cur_namespace == null) {
            return Tab.find(name);
        }

        // since no namespace is provided check if it is a local
        // all the way down to the namespace scope
        for (Scope scope = Tab.currentScope();
             scope != this.cur_namespace.current_scope;
             scope = scope.getOuter()
        ) {
            Obj node = scope.findSymbol(name);
            if (node != null) return node;
        }

        // check the namespace scope:
        Obj node = Tab.find(this.prependCurrentNamespace(name));
        if (node != Tab.noObj) return node;  // found in this namespace

        // else look in outer scopes to the namespace scope
        for (Scope scope = this.cur_namespace.current_scope.getOuter();
             scope != null;
             scope = scope.getOuter()
        ) {
            node = scope.findSymbol(name);
            if (node != null) return node;
        }

        return Tab.noObj;
    }

    private Obj findNameInSameScope(String namespace, String name) {
        Scope current_scope = Tab.currentScope();

        if (namespace != null) {
            name = this.prependNamespace(namespace, name);
        } else if (this.cur_namespace != null) {
            name = this.prependCurrentNamespace(name);
        }

        Obj found = current_scope.findSymbol(name);
        return found != null ? found : Tab.noObj;
    }

    public String prependNamespace(String namespace, String name) {
        StringBuilder sb = new StringBuilder();
        sb.append(namespace)
          .append("::")
          .append(name);
        return sb.toString();
    }

    public String prependCurrentNamespace(String name) {
        if (this.cur_namespace == null) return name;

        return this.prependNamespace(
            this.cur_namespace.name,
            name
        );
    }

    public String getNamespacePrefix() {
        if ( this.cur_namespace == null
          || this.cur_namespace.current_scope != Tab.currentScope()
        ) {
            return null;
        }

        return this.cur_namespace.name;
    }

	private void report_error(final String message, final SyntaxNode info) {
		// this.error_detected = true;
		final StringBuilder msg = new StringBuilder(message);
		final int line = (info == null) ? 0 : info.getLine();
		if (line != 0) {
			msg.append(" on line ").append(line);
        }
		// this.logger.error(msg.toString());
        System.err.println(msg.toString());
	}

    public boolean areCompatibleTypes(Struct type1, Struct type2) {
        // if both arr also checks elem types
        if (type1.equals(type2)) return true;

        if ( (type1.getKind() == Struct.Array)
           ^ (type2.getKind() == Struct.Array)
        ) {  // only one of them is array
            Struct arr_type = type1.getKind() == Struct.Array ?
                            type1 : type2;

            Struct not_arr_type = type1.getKind() == Struct.Array ?
                                type2 : type1;

            return this.areCompatibleTypes(
                arr_type.getElemType(),
                not_arr_type
            );
        }

        return false;
    }

}
