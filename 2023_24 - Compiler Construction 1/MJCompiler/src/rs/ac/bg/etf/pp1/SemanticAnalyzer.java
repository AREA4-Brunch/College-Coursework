package rs.ac.bg.etf.pp1;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import org.apache.log4j.Logger;

import rs.ac.bg.etf.pp1.ast.*;
import rs.ac.bg.etf.pp1.symbol_table.SymbolTable;
import rs.etf.pp1.symboltable.Tab;
import rs.etf.pp1.symboltable.concepts.Obj;
import rs.etf.pp1.symboltable.concepts.Struct;
import rs.etf.pp1.symboltable.visitors.SymbolTableVisitor;

public class SemanticAnalyzer extends VisitorAdaptor {

    private boolean error_detected = false;
    private int cnt_global_vars_or_fields = 0;
    private int cnt_global_variables = 0;
    private int cnt_local_variables = 0;
    private final int max_cnt_local_variables = 256 - 1;  // inclusive
    private final int max_cnt_global_variables = 65536 - 1;  // inclusive
    private SymbolTable symbol_table = null;
    private final Logger logger = Logger.getLogger(this.getClass());

    private static class CurConstDecl {
        public ConstAssignmentList constant;
        public String name;
        public int value;
        public Struct val_type;

        public CurConstDecl(
            ConstAssignmentList constant,
            String name,
            int value,
            Struct val_type
        ) {
            this.constant = constant;
            this.name = name;
            this.value = value;
            this.val_type = val_type;
        }
    };

    private List<CurConstDecl> cur_const_decl = new ArrayList<>();

    private static class CurSafeVariableDecl {
        public SafeVarIdentifiersList var;
        public String name;
        public boolean is_array;

        public CurSafeVariableDecl(
            SafeVarIdentifiersList var,
            String name,
            boolean is_array
        ) {
            this.var = var;
            this.name = name;
            this.is_array = is_array;
        }
    };

    private List<CurSafeVariableDecl> cur_safe_var_decl = new ArrayList<>();

    private static class CurVariableDecl {
        public VarIdentifiersList var;
        public String name;
        public boolean is_array;

        public CurVariableDecl(
            VarIdentifiersList var,
            String name,
            boolean is_array
        ) {
            this.var = var;
            this.name = name;
            this.is_array = is_array;
        }
    };

    private List<CurVariableDecl> cur_var_decl = new ArrayList<>();

    private static class CurDesignatorDetailList {
        public int brackets_count = 0;
    }

    private CurDesignatorDetailList cur_designator_detail_list = null;

    private static class CurFormParams {
        int num_params = 0;
    }

    private CurFormParams cur_form_params = null;

    private static class CurFactorParam {
        Struct type;

        public CurFactorParam(Struct type) {
            this.type = type;
        }
    }

    private List<CurFactorParam> cur_factor_params = new ArrayList<>();

    private static class CurDesignatorDetail {
        // state = 1 - INCR
        //         2 - DECR
        public int state = 0;
    }

    private CurDesignatorDetail cur_designator_detail = null;

    private static class CurReturnStmnt {
        Struct expected_type;
        int found = 0;

        public CurReturnStmnt(Struct expected_type) {
            this.expected_type = expected_type;
        }
    }

    private CurReturnStmnt cur_return_stmnt = null;

    private int cur_is_in_for_loop = 0;

    private static class CurUnpackingDesignator {
        public Obj designator;

        public CurUnpackingDesignator(Obj designator) {
            this.designator = designator;
        }
    }

    private List<CurUnpackingDesignator> cur_unpacking_desig_list = new ArrayList<>();


    // ===========================================================
    // Constructors:


    public SemanticAnalyzer(SymbolTable symbol_table) {
        this.symbol_table = symbol_table;
    }


    // ===========================================================
    // Getters:


    public int getNumGlobalVariablesOrFields() {
        return this.cnt_global_vars_or_fields;
    }


    // ===========================================================
    // ===========================================================
    // Errors:


    public void report_error(final String message, final SyntaxNode info) {
        this.error_detected = true;
        final StringBuilder msg = new StringBuilder(message);
        final int line = (info == null) ? 0 : info.getLine();
        if (line != 0) {
            msg.append(" on line ").append(line);
        }
        this.logger.error(msg.toString());
        // System.err.println(msg.toString());
    }

    public void report_info(final String message, final SyntaxNode info) {
        final StringBuilder msg = new StringBuilder(message);
        final int line = (info == null) ? 0 : info.getLine();
        if (line != 0) {
            msg.append(" on line ").append(line);
        }
        this.logger.info(msg.toString());
        // System.out.println(msg.toString());
    }

    public void report_registered_identifier(Obj identifier, SyntaxNode node) {
        SymbolTableVisitor sym_table_visitor = this.symbol_table.createVisitor();
        sym_table_visitor.visitObjNode(identifier);
        String identifier_as_str = sym_table_visitor.getOutput();

        report_info("Registered identifier: " + identifier_as_str, node);
    }

    public void report_referenced_identifier(
        String identifier_reference_used,  // can be name without namespace prefix
        Obj identifier,
        SyntaxNode node
    ) {
        SymbolTableVisitor sym_table_visitor = this.symbol_table.createVisitor();
        sym_table_visitor.visitObjNode(identifier);
        String identifier_as_str = sym_table_visitor.getOutput();

        report_info(
            "Referenced identifier `" + identifier_reference_used + "`: " + identifier_as_str,
            node
        );
    }

    public boolean wasErrorDetected() {
        return this.error_detected;
    }


    // ===========================================================
    // ===========================================================
    // Scopes:


    // ===========================================================
    // nonterminal Program

    @Override
    public void visit(final ProgramNamespace program) {
        super.visit(program);
        this.commonVisit(program, program.getProgName().obj);
    }

    @Override
    public void visit(final ProgramNoNamespace program) {
        super.visit(program);
        this.commonVisit(program, program.getProgName().obj);
    }

    private void commonVisit(Program program, Obj name_obj) {
        if (this.cnt_global_variables > this.max_cnt_global_variables) {
            report_error(
                "Global variables count limit exceeded, max allowed global variables is: "
                + this.max_cnt_global_variables,
            null
            );
        }

        Tab.chainLocalSymbols(name_obj);

        this.cnt_global_vars_or_fields = Tab.currentScope.getnVars();
        // safety check if counted as many global vars as the Tab class:
        if (!this.error_detected && this.cnt_global_vars_or_fields != this.cnt_global_variables) {
            report_error(
                "Semantic analyzer found " + this.cnt_global_variables + " global variables"
                + " but Tab found " + this.cnt_global_vars_or_fields + " global variables or class fields",
                program
            );
        }

        // check if program is valid by checking if void main exists
        Obj main = this.symbol_table.findName(null, "main");
        if ( main == Tab.noObj
          || main.getKind() != Obj.Meth
          || main.getType().getKind() != Struct.None
          || main.getLevel() != 0
        ) {
            report_error("No void main() was declared.", null);
        }

        this.symbol_table.closeScope();
    }

    @Override
    public void visit(final ProgName progName) {
        super.visit(progName);
        progName.obj = this.symbol_table.registerName(
            Obj.Prog,
            null,
            progName.getProgName(),
            Tab.noType
        );
        this.symbol_table.openScope();
    }


    // ===========================================================
    // nonterminal Namespace

    @Override
    public void visit(final Namespace namespace) {
        super.visit(namespace);
        // Tab.chainLocalSymbols(namespace.getNamespaceName().obj);
        this.symbol_table.closeNamespace();
        // this.symbol_table.closeScope();
    }

    @Override
    public void visit(final NamespaceName namespace_name) {
        super.visit(namespace_name);
        String name = namespace_name.getIdentNamespaceName();
        // namespace_name.obj = this.symbol_table.registerName(Obj.NO_VALUE, null, name, new Struct(Struct.None));
        // this.symbol_table.openScope();
        // this.cur_namespace = new CurNamespaceName(namespace_name, Tab.currentScope());
        this.symbol_table.openNamespace(name);
    }


    // ===========================================================
    // nonterminal MethodDecl

    @Override
    public void visit(TypeOrVoidMethodNameIsType method_name) {
        super.visit(method_name);
        this.commonVisit(
            method_name,
            method_name.getMethodName(),
            method_name.getType().struct
        );
    }

    @Override
    public void visit(TypeOrVoidMethodNameIsVoid method_name) {
        super.visit(method_name);
        this.commonVisit(
            method_name,
            method_name.getMethodName(),
            Tab.noType
        );
    }

    private void commonVisit(
        TypeOrVoidMethodName type_method,
        String name,
        Struct ret_type
    ) {
        this.cur_form_params = new CurFormParams();

        type_method.obj = this.symbol_table.registerName(
            Obj.Meth,
            this.symbol_table.getNamespacePrefix(),
            name,
            ret_type
        );

        if (type_method.obj == Tab.noObj) {
            name = this.symbol_table.prependCurrentNamespace(name);
            report_error("Method ret_type_kind: " + ret_type.getKind() + " `" + name + "` declaration failed, identifier already declared.", null);
        }
        else {
            // report_info("Registered `" + ret_type.getKind() + " " + type_method.obj.getName() + "`.", type_method);
            report_registered_identifier(type_method.obj, type_method);
        }

        this.cur_return_stmnt = new CurReturnStmnt(ret_type);
        this.symbol_table.openScope();
    }

    @Override
    public void visit(SingleFormParsArr params) {
        super.visit(params);
        String name = params.getIdentName();
        Struct elem_type = params.getType().struct;
        Struct type = this.symbol_table.getOrDeclareGlobalArrType(elem_type);
        this.commonVisit(params, name, type);
    }

    @Override
    public void visit(SingleFormPars params) {
        super.visit(params);
        String name = params.getIdentName();
        Struct type = params.getType().struct;
        this.commonVisit(params, name, type);
    }

    @Override
    public void visit(MultipleFormParsArr params) {
        super.visit(params);
        String name = params.getIdentName();
        Struct elem_type = params.getType().struct;
        Struct type = this.symbol_table.getOrDeclareGlobalArrType(elem_type);
        this.commonVisit(params, name, type);
    }

    @Override
    public void visit(MultipleFormPars params) {
        super.visit(params);
        String name = params.getIdentName();
        Struct type = params.getType().struct;
        this.commonVisit(params, name, type);
    }

    private void commonVisit(
        FormPars params,
        String name,
        Struct type
    ) {
        params.obj = this.symbol_table.registerName(Obj.Var, null, name, type);
        // ++this.cur_form_params.num_params;

        if (params.obj == Tab.noObj) {
            report_error("Formal parameter: `" + name + "`declaration failed, identifier already declared.", params);
            return;
        }

        params.obj.setFpPos(this.cur_form_params.num_params++);

        // report_info("Registered formal param: " + name + (type.getKind() == Struct.Array ? "[]" : ""), params);
        report_registered_identifier(params.obj, params);
    }

    @Override
    public void visit(MethodDeclFormPars method) {
        super.visit(method);
        this.commonVisit(method, method.getTypeOrVoidMethodName().obj);
    }

    @Override
    public void visit(MethodDeclNoFormPars method) {
        super.visit(method);
        this.commonVisit(method, method.getTypeOrVoidMethodName().obj);
    }

    private void commonVisit(MethodDecl method, Obj method_name_obj) {
        try {
            // check if correct return keyword was found in case of not void function
            if ( this.cur_return_stmnt.found == 0
              && !Tab.noType.equals(this.cur_return_stmnt.expected_type)
            ) {
                report_error(
                    "Method " + method_name_obj.getName() + " does not contain return stmnt of type "
                    + this.cur_return_stmnt.expected_type.getKind(),
                    method
                );
            }

            this.cnt_local_variables += this.cur_form_params.num_params;
            if (this.cnt_local_variables >= this.max_cnt_local_variables) {
                report_error(
                    "Local variables count limit exceeded, max allowed local variables + formal func args is "
                    + this.max_cnt_local_variables,
                    method
                );
            }

            method_name_obj.setLevel(this.cur_form_params.num_params);

        } finally {
            Tab.chainLocalSymbols(method_name_obj);
            this.symbol_table.closeScope();

            this.cnt_local_variables = 0;
            this.cur_form_params = null;
            this.cur_return_stmnt = null;
        }
    }


    // Scopes
    // ===========================================================
    // ===========================================================


    // ===========================================================
    // nonterminal Type

    @Override
    public void visit(NamespacedType type) {
        super.visit(type);
        this.commonVisit(type, type.getIdentName());
    }

    @Override
    public void visit(RegularType type) {
        super.visit(type);
        this.commonVisit(type, type.getIdentName());
    }

    private void commonVisit(Type type, String name) {
        Obj node = this.symbol_table.findName(null, name);

        if (node == Tab.noObj) {
            report_error("Type `" + name + "` not found.", type);
            type.struct = Tab.noType;
            return;
        }

        if (node.getKind() != Obj.Type) {
            report_error(name + " is not a type.", type);
            type.struct = Tab.noType;
            return;
        }

        // else successfully found type name in symbols table
        type.struct = node.getType();
    }


    // ===========================================================
    // ===========================================================
    // Constants:

    // ===========================================================
    // nonterminal ConstDecl


    @Override
    public void visit(ConstLiteralNumber literal) {
        literal.obj = new Obj(Obj.Con, null, Tab.intType);
        literal.obj.setAdr(literal.getValue());
    }

    @Override
    public void visit(ConstLiteralChar literal) {
        literal.obj = new Obj(Obj.Con, null, Tab.charType);
        literal.obj.setAdr(literal.getValue());
    }

    @Override
    public void visit(ConstLiteralBool literal) {
        literal.obj = new Obj(
            Obj.Con,
            null,
            this.symbol_table.getOrDeclareGType("bool", null)
        );
        literal.obj.setAdr(literal.getValue() ? 1 : 0);
    }

    @Override
    public void visit(MultipleConstAssignments constant) {
        super.visit(constant);
        final int value = constant.getConstLiteral().obj.getAdr();
        Struct val_type = constant.getConstLiteral().obj.getType();
        this.commonVisit(constant, constant.getIdentName(), value, val_type);
    }

    @Override
    public void visit(SingleConstAssignment constant) {
        super.visit(constant);
        final int value = constant.getConstLiteral().obj.getAdr();
        Struct val_type = constant.getConstLiteral().obj.getType();
        this.commonVisit(constant, constant.getIdentName(), value, val_type);
    }

    private void commonVisit(
        ConstAssignmentList constant,
        String name,
        int value,
        Struct val_type
    ) {
        this.cur_const_decl.add(new CurConstDecl(constant, name, value, val_type));
    }

    @Override
    public void visit(ConstDecl constant_decl) {
        try {
            super.visit(constant_decl);
            Struct type = constant_decl.getType().struct;

            for (CurConstDecl const_decl : this.cur_const_decl) {
                ConstAssignmentList constant = const_decl.constant;
                String const_name = const_decl.name;

                // check if declaration and value types are equivalent
                if (!type.assignableTo(const_decl.val_type)) {
                    report_error(
                        "Value of type: " + const_decl.val_type + " is not assignable to const "
                        + const_name + " of type: " + type,
                        null
                    );
                    return;
                }

                constant.obj = this.symbol_table.registerName(
                    Obj.Con,
                    this.symbol_table.getNamespacePrefix(),
                    const_name,
                    type
                );

                if (constant.obj == Tab.noObj) {
                    const_name = this.symbol_table.prependCurrentNamespace(const_name);
                    report_error(
                        "Constant with name `" + const_name + "` declaration failed, identifier already declared.",
                        constant
                    );
                    return;
                }

                // set the actual constant's value
                constant.obj.setAdr(const_decl.value);

                // report_info("Registered const " + const_name, null);
                // report_info("Registered const " + (type != null ? type.toString() : "type? ") + constant.obj.getName(), constant);
                report_registered_identifier(constant.obj, constant);
            }

        } finally {
            this.cur_const_decl.clear();
        }
    }



    // Constants
    // ===========================================================
    // ===========================================================


    // ===========================================================
    // ===========================================================
    // Global Variables:


    // ===========================================================
    // nonterminal SafeVarDecl

    @Override
    public void visit(SafeMultipleArrayIdentifier variable) {
        super.visit(variable);
        this.commonVisit(variable, variable.getIdentName(), true);
    }

    @Override
    public void visit(SafeMultipleVarIdentifier variable) {
        super.visit(variable);
        this.commonVisit(variable, variable.getIdentName(), false);
    }

    @Override
    public void visit(SafeSingleArrayIdentifier variable) {
        super.visit(variable);
        this.commonVisit(variable, variable.getIdentName(), true);
    }

    @Override
    public void visit(SafeSingleVarIdentifier variable) {
        super.visit(variable);
        this.commonVisit(variable, variable.getIdentName(), false);
    }

    @Override
    public void visit(SafeVarDeclSuccess declaration) {
        super.visit(declaration);
        try {
            Struct type = declaration.getType().struct;
            Struct array_type = this.symbol_table.getOrDeclareGlobalArrType(type);

            this.cnt_global_variables += this.cur_safe_var_decl.size();

            for (CurSafeVariableDecl var_decl : this.cur_safe_var_decl) {
                SafeVarIdentifiersList var = var_decl.var;
                String var_name = var_decl.name;
                boolean is_array = var_decl.is_array;

                var.obj = this.symbol_table.registerName(
                    Obj.Var,
                    this.symbol_table.getNamespacePrefix(),
                    var_name,
                    is_array ? array_type : type
                );

                if (var.obj == Tab.noObj) {
                    var_name = this.symbol_table.prependCurrentNamespace(var_name);
                    report_error(
                        "Variable with name `" + var_name + "` declaration failed, identifier already declared in this scope",
                        var
                    );
                    return;
                }

                // report_info("Registered variable " + var_name, null);
                // report_info("Registered safe variable " + (type != null ? type.toString() : "type? ") + (is_array ? "[] " : " ") + var.obj.getName(), var);
                report_registered_identifier(var.obj, var);
            }

        } finally {
            this.cur_safe_var_decl.clear();
        }
    }

    private void commonVisit(
        SafeVarIdentifiersList variable,
        String name,
        boolean is_array
    ) {
        this.cur_safe_var_decl.add(new CurSafeVariableDecl(variable, name, is_array));
    }


    // ===========================================================
    // nonterminal VarDecl - local variables


    @Override
    public void visit(MultipleArrayIdentifier variable) {
        super.visit(variable);
        this.commonVisit(variable, variable.getIdentName(), true);
    }

    @Override
    public void visit(MultipleVarIdentifier variable) {
        super.visit(variable);
        this.commonVisit(variable, variable.getIdentName(), false);
    }

    @Override
    public void visit(SingleArrayIdentifier variable) {
        super.visit(variable);
        this.commonVisit(variable, variable.getIdentName(), true);
    }

    @Override
    public void visit(SingleVarIdentifier variable) {
        super.visit(variable);
        this.commonVisit(variable, variable.getIdentName(), false);
    }

    private void commonVisit(
        VarIdentifiersList variable,
        String name,
        boolean is_array
    ) {
        this.cur_var_decl.add(new CurVariableDecl(variable, name, is_array));
    }

    @Override
    public void visit(VarDecl declaration) {
        super.visit(declaration);
        try{
            Struct type = declaration.getType().struct;
            Struct array_type = this.symbol_table.getOrDeclareGlobalArrType(type);

            this.cnt_local_variables += this.cur_var_decl.size();

            for (CurVariableDecl var_decl : this.cur_var_decl) {
                VarIdentifiersList var = var_decl.var;
                String var_name = var_decl.name;
                boolean is_array = var_decl.is_array;

                var.obj = this.symbol_table.registerName(
                    Obj.Var,
                    this.symbol_table.getNamespacePrefix(),
                    var_name,
                    is_array ? array_type : type
                );

                if (var.obj == Tab.noObj) {
                    var_name = this.symbol_table.prependCurrentNamespace(var_name);
                    report_error(
                        "Variable with name `" + var_name + "` declaration failed, identifier already declared in this scope",
                        var
                    );
                    return;
                }

                // report_info("Registered variable " + var_name, null);
                // report_info("Registered  variable " + (type != null ? type.toString() : "type? ") + (is_array ? "[] " : " ") + var.obj.getName(), var);
                report_registered_identifier(var.obj, var);
            }

        } finally {
            this.cur_var_decl.clear();
        }
    }


    // Global Variables
    // ===========================================================
    // ===========================================================


    // ===========================================================
    // ===========================================================
    // Accessing Registered Names:


    @Override
    public void visit(NamespacedDesignator designator) {
        super.visit(designator);
        String name = designator.getIdentName();
        String namespace_name = designator.getNamespaceName();
        this.commonVisit(designator, namespace_name, name);
    }

    @Override
    public void visit(NoNamespaceDesignator designator) {
        super.visit(designator);
        String name = designator.getIdentName();
        this.commonVisit(designator, null, name);
    }

    @Override
    public void visit(SecondDesignatorDetailList detail_list) {
        super.visit(detail_list);
        if (this.cur_designator_detail_list == null) {
            this.cur_designator_detail_list = new CurDesignatorDetailList();
        }

        if (!Tab.intType.equals(detail_list.getExpr().obj.getType())) {
            report_error("Array element index must be of type int", detail_list.getExpr());
        }

        ++this.cur_designator_detail_list.brackets_count;
    }

    private void commonVisit(
        Designator designator,
        String namespace,
        String name
    ) {
        try {
            int brackets_count = this.cur_designator_detail_list != null ?
                                 this.cur_designator_detail_list.brackets_count
                               : 0;

            if (brackets_count > 1) {
                designator.obj = Tab.noObj;
                report_error("Invalid array element access", designator);
                return;
            }

            designator.obj = this.symbol_table.findName(namespace, name);

            if (designator.obj == Tab.noObj) {  // miss
                if (namespace != null) {  // update to full name for logging purposes
                    name = this.symbol_table.prependNamespace(namespace, name);
                }
                report_error("Referenced undeclared identifier: " + name, designator);
                return;
            }
            // else hit

            // check if found was treated as array and is an array
            final boolean is_array = designator.obj.getType().getKind() == Struct.Array;

            if (brackets_count > 0 && !is_array) {
                report_error("Referenced identifier: " + name + " is not an array", designator);
                return;
            }

            if (is_array && brackets_count > 0) {
                designator.obj = new Obj(
                    Obj.Elem,
                    designator.obj.getName(),
                    designator.obj.getType().getElemType(),
                    designator.obj.getAdr(),
                    designator.obj.getLevel()
                );
            }

            // get the log of the found referenced identifier
            this.report_referenced_identifier(name, designator.obj, designator);

        } finally {
            this.onDesignatorProcessed();
        }
    }

    private void onDesignatorProcessed() {
        this.cur_designator_detail_list = null;
        // --this.cur_designator_detail_list.brackets_count;
    }


    // Accessing Registered Names
    // ===========================================================
    // ===========================================================
    

    // ===========================================================
    // ===========================================================
    // Veryfying Context Rules/Conditions



    // ===========================================================
    // Verify Function Calls:


    @Override
    public void visit(FactorDesignatorFactorPars factor) {
        super.visit(factor);
        this.commonVisit(factor, factor.getFuncCall().obj.getType());
    }

    @Override
    public void visit(FactorDesignatorNoFactorPars factor) {
        super.visit(factor);
        this.commonVisit(factor, factor.getDesignator().obj.getType());
    }

    @Override
    public void visit(FactorNumber factor) {
        super.visit(factor);
        this.commonVisit(factor, this.symbol_table.getOrDeclareGType("int", null));
    }

    @Override
    public void visit(FactorChar factor) {
        super.visit(factor);
        this.commonVisit(factor, this.symbol_table.getOrDeclareGType("char", null));
    }

    @Override
    public void visit(FactorBool factor) {
        super.visit(factor);
        this.commonVisit(factor, this.symbol_table.getOrDeclareGType("bool", null));
    }

    @Override
    public void visit(FactorNewExpr factor) {
        super.visit(factor);
        if (factor.getExpr().obj.getType().getKind() != Struct.Int) {
            report_error("In `new [expr]`, `expr` must be of type int", factor);
        }

        Struct array_type = this.symbol_table.getOrDeclareGlobalArrType(factor.getType().struct);
        this.commonVisit(factor, array_type);
    }

    @Override
    public void visit(FactorNewFactorPars factor) {
        super.visit(factor);
        // skipping context rules since no classes
        this.commonVisit(factor, factor.getType().struct);
    }

    @Override
    public void visit(FactorLparen factor) {
        super.visit(factor);
        this.commonVisit(factor, factor.getExpr().obj.getType());
    }

    private void commonVisit(Factor factor, Struct type) {
        factor.obj = new Obj(Obj.NO_VALUE, null, type);
    }

    @Override
    public void visit(HasMulopFactorList mf_list) {
        super.visit(mf_list);
        this.commonVisit(mf_list, mf_list.getFactor().obj.getType());
    }

    @Override
    public void visit(SingleMulopFactor mf_list) {
        super.visit(mf_list);
        this.commonVisit(mf_list, mf_list.getFactor().obj.getType());
    }

    private void commonVisit(MulopFactorList mf_list, Struct type) {
        mf_list.obj = new Obj(Obj.NO_VALUE, null, type);
    }

    @Override
    public void visit(TermFactor term) {
        super.visit(term);
        this.commonVisit(term, term.getFactor().obj.getType());
    }

    @Override
    public void visit(TermMulopFactorList term) {
        super.visit(term);
        Struct factor_type = term.getFactor().obj.getType();
        Struct mulop_factor_list_type = term.getMulopFactorList().obj.getType();

        if ( factor_type.getKind() != Struct.Int
          || !mulop_factor_list_type.equals(factor_type)
        ) {
            report_error("Factors must be of type int", term);
        }

        this.commonVisit(term, factor_type);
    }

    private void commonVisit(Term term, Struct type) {
        term.obj = new Obj(Obj.NO_VALUE, null, type);
    }

    @Override
    public void visit(HasAddopTermList term_list) {
        super.visit(term_list);
        this.commonVisit(term_list, term_list.getTerm().obj.getType());
    }

    @Override
    public void visit(NoAddopTerm term_list) {
        super.visit(term_list);
        this.commonVisit(term_list, Tab.noType);
    }

    private void commonVisit(AddopTermList term_list, Struct type) {
        term_list.obj = new Obj(Obj.NO_VALUE, null, type);
    }

    @Override
    public void visit(ExprMinus expr) {
        super.visit(expr);
        Struct term_type = expr.getExprNegation().getTerm().obj.getType();
        Struct add_op_type = expr.getAddopTermList().obj.getType();

        if (term_type.getKind() != Struct.Int) {  // Term must be of type int
            report_error(
                "Expression starting with minus must be entirely of type int",
                expr
            );
        }

        this.commonVisit(expr, term_type, add_op_type);
    }

    @Override
    public void visit(ExprNoMinus expr) {
        super.visit(expr);
        Struct term_type = expr.getTerm().obj.getType();
        Struct add_op_type = expr.getAddopTermList().obj.getType();
        this.commonVisit(expr, term_type, add_op_type);
    }

    private void commonVisit(Expr expr, Struct type, Struct add_op_type) {
        if (add_op_type.getKind() != Struct.None) {  // there were add ops terms provided
            if ( add_op_type.getKind() != Struct.Int
              || !add_op_type.compatibleWith(type)
            ) {  // invalid add ops terms
                report_error("Expression must be entirely of type int", expr);
            }
        }

        expr.obj = new Obj(Obj.NO_VALUE, null, type);
    }

    @Override
    public void visit(MultipleActPars act_pars) {
        super.visit(act_pars);
        this.commonVisit(act_pars, act_pars.getExpr().obj.getType());
    }

    @Override
    public void visit(SingleActPar act_pars) {
        super.visit(act_pars);
        this.commonVisit(act_pars, act_pars.getExpr().obj.getType());
    }

    private void commonVisit(ActPars act_pars, Struct type) {
        this.cur_factor_params.add(new CurFactorParam(type));
    }

    @Override
    public void visit(FuncCall func_call) {
        super.visit(func_call);
        try {
            Designator designator = func_call.getDesignator();
            func_call.obj = designator.obj;
            if (designator.obj == Tab.noObj) return;

            if (designator.obj.getKind() != Obj.Meth) {
                report_error(designator.obj.getName() + " is not a function.", designator);
                return;
            }

            // Check the number of function args provided
            final int num_expected_params = designator.obj.getLevel();
            Collection<Obj> expected_arguments = designator.obj.getLocalSymbols();

            if (expected_arguments.size() < num_expected_params) {
                report_error(
                    "Func: " + designator.obj.getName()
                    + " expects num args: " + num_expected_params
                    + ", but only registered: " + expected_arguments.size(),
                    designator
                );
                return;
            }

            if (this.cur_factor_params.size() != num_expected_params) {
                report_error(
                    "Wrong number of function arguments provided for func: " + designator.obj.getName()
                    + ", got: " + this.cur_factor_params.size()
                    + ", expected: " + num_expected_params,
                    designator
                );
                return;
            }

            // Check the type of function args provided
            java.util.Iterator<CurFactorParam> param_iter = this.cur_factor_params.iterator();
            java.util.Iterator<Obj> expected_arg_iter = expected_arguments.iterator();
            for (int i = 0; i < num_expected_params; ++i) {
                Obj expected_arg = expected_arg_iter.next();
                Struct expected_type = expected_arg.getType();
                CurFactorParam param = param_iter.next();

                // if function has type arg None then accept any
                if (expected_type.getKind() == Struct.None) continue;

                // if function expects arr of els of type None then accept any array
                if ( expected_type.getKind() == Struct.Array
                  && param.type.getKind() == Struct.Array
                  && expected_type.getElemType().getKind() == Struct.None
                ) {
                    continue;
                }

                if (!param.type.compatibleWith(expected_type)) {
                    report_error(
                        "Function arg number " + i + " of " + designator.obj.getName() + " is of wrong type"
                        + ", got type: " + param.type.getKind()
                        + ", expected type: " + expected_type.getKind(),
                        designator
                    );
                }
            }

        } finally {
            this.cur_factor_params.clear();
        }
    }

    @Override
    public void visit(DesignatorDetailFactorIncr detail) {
        super.visit(detail);
        if (this.cur_designator_detail == null) {
            this.cur_designator_detail = new CurDesignatorDetail();
        }

        this.cur_designator_detail.state = 1;
    }

    @Override
    public void visit(DesignatorDetailFactorDecr detail) {
        super.visit(detail);
        if (this.cur_designator_detail == null) {
            this.cur_designator_detail = new CurDesignatorDetail();
        }

        this.cur_designator_detail.state = 2;
    }

    @Override
    public void visit(DesignatorStatementMisc stmnt) {
        super.visit(stmnt);
        try {
            Designator designator = stmnt.getDesignator();
            if (designator.obj == Tab.noObj) return;

            // check if the detail is valid in the context of designator
            switch (this.cur_designator_detail.state) {
                case 1: case 2: {  // INCR or DECR
                    if ( designator.obj.getKind() != Obj.Var
                      && designator.obj.getKind() != Obj.Elem
                    ) {
                        report_error(
                            designator.obj.getName() + " is not a variable nor an array element which is neccessary for increment or decrement.",
                            designator
                        );
                    } else if (designator.obj.getType().getKind() != Struct.Int) {
                        report_error("Increment and decrement can be used only for int types.", designator);
                    }
                    return;
                }

                default:
                    throw new RuntimeException("Unknown state: " + this.cur_designator_detail.state);
            }

        } finally {
            this.onDesignatorStatementProcessed();
        }
    }

    private void onDesignatorStatementProcessed() {
        this.cur_designator_detail = null;
    }


    // Verify Function Calls
    // ===========================================================


    // ===========================================================
    // Verify Return in method:


    @Override
    public void visit(StatementReturnExpr stmnt) {
        super.visit(stmnt);
        this.commonReturnStmntProcessing(stmnt, stmnt.getExpr().obj.getType());
    }

    @Override
    public void visit(StatementReturnNoExpr stmnt) {
        super.visit(stmnt);
        this.commonReturnStmntProcessing(stmnt, Tab.noType);
    }

    private void commonReturnStmntProcessing(SyntaxNode stmnt, Struct ret_expr_type) {
        if (this.cur_return_stmnt == null) {
            report_error("Return keyword must not be outside of a method.", stmnt);
            return;
        }

        if (!ret_expr_type.equals(this.cur_return_stmnt.expected_type)) {
            report_error(
                "Expression type of return keyword does not match the return type of the method it is in.",
                stmnt
            );
            return;
        }

        ++this.cur_return_stmnt.found;
    }


    // Verify Return in method
    // ===========================================================

    // ===========================================================
    // Verify Designator Assignment:


    @Override
    public void visit(DesignatorAssignNormal desig_assign) {
        super.visit(desig_assign);
        Designator designator = desig_assign.getDesignator();

        if ( designator.obj.getKind() != Obj.Var
          && designator.obj.getKind() != Obj.Elem
        ) {
            report_error(
                "Assignment requires left side of equals operator to be a variable or an array element",
                designator
            );
            return;
        }

        Struct expr_type = desig_assign.getExpr().obj.getType();
        if (!expr_type.assignableTo(designator.obj.getType())) {
            report_error(
                "Types on the left and right side of assignment operator are not compatible for assignment"
                + " got left type kind: " + designator.obj.getType().getKind()
                + " got right type kind: " + expr_type.getKind()

                + " got left type: " + designator.obj.getType()
                + " got right type: " + expr_type
                ,
                desig_assign
            );
            return;
        }
    }

    @Override
    public void visit(DesignatorAssignArrUnpack desig_assign) {
        super.visit(desig_assign);
        try {
            Designator designator_right = desig_assign.getDesignator1();
            if (designator_right.obj.getType().getKind() != Struct.Array) {
                report_error(
                    "Unpacking assignment requires a type of array on the right side of assignment operator",
                    designator_right
                );
                return;
            }

            Struct elem_type = designator_right.obj.getType().getElemType();

            // check the rest of designator types
            for (CurUnpackingDesignator cur_desig : this.cur_unpacking_desig_list) {
                Obj designator = cur_desig.designator;

                if (designator.getKind() != Obj.Var && designator.getKind() != Obj.Elem) {
                    report_error(
                        "Unpacking assignment requires all but last designator before assignment operator to be variables or array elements",
                        desig_assign.getOptDesignatorList()
                    );
                    return;
                }

                if (!elem_type.assignableTo(designator.getType())) {
                    report_error(
                          "Unpacking assignment requires all but last designator before assignment operator to be of "
                        + "compatible type with designator elem type on the right side of the assignment operator",
                        desig_assign.getOptDesignatorList()
                    );
                    return;
                }
            }

            // last designator before the assignment operator must be an array
            Struct last_designator_type = desig_assign.getDesignator().obj.getType();
            if ( last_designator_type.getKind() != Struct.Array
              || !elem_type.assignableTo(last_designator_type.getElemType())
            ) {
                report_error(
                    "Unpacking assignment requires the last designator before assignment operator to be array of "
                  + "compatiable element type with the designator elem type on the right side of the assignment operator",
                  desig_assign.getDesignator()
                );
                return;
            }

        } finally {
            this.cur_unpacking_desig_list.clear();
        }
    }

    @Override
    public void visit(HasOptDesignatorList desig_assign) {
        super.visit(desig_assign);
        Obj desig_obj = desig_assign.getOptDesignator().obj;

        // if designator was provided add it
        if (desig_obj != null) {
            this.cur_unpacking_desig_list.add(
                new CurUnpackingDesignator(desig_obj)
            );
        }
    }

    @Override
    public void visit(NoDesignatorList desig_assign) {
        super.visit(desig_assign);
        // desig_assign.obj = null;
    }

    @Override
    public void visit(HasOptDesignator opt_designator) {
        super.visit(opt_designator);
        opt_designator.obj = opt_designator.getDesignator().obj;
    }

    @Override
    public void visit(NoDesignator opt_designator) {
        super.visit(opt_designator);
        opt_designator.obj = null;  // !important to propagate null
    }


    // Verify Designator Assignment
    // ===========================================================


    // ===========================================================
    // Verify READ, PRINT:


    @Override
    public void visit(StatementRead stmnt) {
        super.visit(stmnt);
        Designator designator = stmnt.getDesignator();

        if ( designator.obj.getKind() != Obj.Var
          && designator.obj.getKind() != Obj.Elem
        ) {
            report_error(
                "`read` requires argument to be a variable or an array element",
                designator
            );
            return;
        }

        Struct designator_type = designator.obj.getType();
        if ( !designator_type.equals(Tab.intType)
          && !designator_type.equals(Tab.charType)
          && !designator_type.equals(this.symbol_table.getOrDeclareGType("bool", null))
        ) {
            report_error(
                "`read` requires argument to be of type `int` or `char` or `bool`",
                designator
            );
            return;
        }
    }


    @Override
    public void visit(StatementPrintConstArg stmnt) {
        super.visit(stmnt);
        this.commonPrintStmntProcessing(stmnt, stmnt.getExpr().obj.getType());
    }

    @Override
    public void visit(StatementPrintNoArg stmnt) {
        super.visit(stmnt);
        this.commonPrintStmntProcessing(stmnt, stmnt.getExpr().obj.getType());
    }

    private void commonPrintStmntProcessing(SyntaxNode stmnt, Struct designator_type) {
        if ( !designator_type.equals(Tab.intType)
          && !designator_type.equals(Tab.charType)
          && !designator_type.equals(this.symbol_table.getOrDeclareGType("bool", null))
        ) {
            report_error(
                "`print` requires argument to be of type `int` or `char` or `bool`",
                stmnt
            );
            return;
        }
    }


    // Verify READ, PRINT
    // ===========================================================


    // ===========================================================
    // CondFact:


    @Override
    public void visit(RelopEq relop) {
        super.visit(relop);
        this.commonVisit(relop, "EQUALITY");
    }

    @Override
    public void visit(RelopIneq relop) {
        super.visit(relop);
        this.commonVisit(relop, "INEQUALITY");
    }

    @Override
    public void visit(RelopGt relop) {
        super.visit(relop);
        this.commonVisit(relop, "GT");
    }

    @Override
    public void visit(RelopGte relop) {
        super.visit(relop);
        this.commonVisit(relop, "GTE");
    }

    @Override
    public void visit(RelopLt relop) {
        super.visit(relop);
        this.commonVisit(relop, "LT");
    }

    @Override
    public void visit(RelopLte relop) {
        super.visit(relop);
        this.commonVisit(relop, "LTE");
    }

    private void commonVisit(Relop relop, String name) {
        relop.obj = new Obj(Obj.NO_VALUE, name, Tab.noType);
    }

    @Override
    public void visit(CondFactRelop cond_fact) {
        super.visit(cond_fact);
        Struct expr1_type = cond_fact.getExpr().obj.getType();
        Struct expr2_type = cond_fact.getExpr1().obj.getType();

        if (!expr1_type.compatibleWith(expr2_type)) {
            report_error(
                "Types on the left and right side of a relop operator are not compatible",
                cond_fact
            );
            return;
        }

        // for arrays only relop operators that can be used are !=, ==
        if ( expr1_type.getKind() == Struct.Array
          || expr2_type.getKind() == Struct.Array
        ) {
            String relop_name = cond_fact.getRelop().obj.getName();

            if (!"EQUALITY".equals(relop_name) && !"INEQUALITY".equals(relop_name)) {
                report_error(
                    "For arrays only equality and inequality operators are allowed",
                    cond_fact
                );
                return;
            }
        }

        this.commonVisit(cond_fact, this.symbol_table.getOrDeclareGType("bool", null));
    }

    @Override
    public void visit(CondFactExpr cond_fact) {
        super.visit(cond_fact);
        Struct type = cond_fact.getExpr().obj.getType();
        this.commonVisit(cond_fact, type);
    }

    private void commonVisit(CondFact cond_fact, Struct type) {
        cond_fact.obj = new Obj(Obj.NO_VALUE, null, type);
    }


    // CondFact
    // ===========================================================


    // ===========================================================
    // For loop:


    @Override
    public void visit(StatementBreak stmnt) {
        super.visit(stmnt);
        if (this.cur_is_in_for_loop <= 0) {
            report_error(
                "Break statement can appear only inside a for loop",
                stmnt
            );
        }
    }

    @Override
    public void visit(StatementContinue stmnt) {
        super.visit(stmnt);
        if (this.cur_is_in_for_loop <= 0) {
            report_error(
                "Continue statement can appear only inside a for loop",
                stmnt
            );
        }
    }

    @Override
    public void visit(ForLoopBodyBegin for_loop) {
        super.visit(for_loop);
        ++this.cur_is_in_for_loop;
    }

    @Override
    public void visit(StatementForHasCond2 stmnt) {
        super.visit(stmnt);
        this.commonForLoopProcessing();
    }

    @Override
    public void visit(StatementForNoCond1 stmnt) {
        super.visit(stmnt);
        this.commonForLoopProcessing();
    }

    @Override
    public void visit(StatementForHasCond4 stmnt) {
        super.visit(stmnt);
        this.commonForLoopProcessing();
    }

    @Override
    public void visit(StatementForNoCond3 stmnt) {
        super.visit(stmnt);
        this.commonForLoopProcessing();
    }

    private void commonForLoopProcessing() {
        --this.cur_is_in_for_loop;
    }


    // For loop
    // ===========================================================

    // ===========================================================
    // If Statement:


    @Override
    public void visit(CondTerm cond_term) {
        super.visit(cond_term);
        Struct condition_type = cond_term.getCondFact().obj.getType();
        cond_term.obj = new Obj(Obj.NO_VALUE, null, condition_type);
    }

    @Override
    public void visit(Condition condition) {
        super.visit(condition);
        Struct condition_type = condition.getCondTerm().obj.getType();
        condition.obj = new Obj(Obj.NO_VALUE, null, condition_type);
    }

    @Override
    public void visit(SafeIfElseConditionOk stmnt) {
        super.visit(stmnt);
        Struct condition_type = stmnt.getCondition().obj.getType();
        stmnt.obj = new Obj(Obj.NO_VALUE, null, condition_type);
    }

    @Override
    public void visit(StatementIfElse stmnt) {
        super.visit(stmnt);
        Struct condition_type = stmnt.getSafeIfElseCondition().obj.getType();
        this.commonIfStatementProcessing(stmnt, condition_type);
    }

    @Override
    public void visit(StatementIfNoElse stmnt) {
        super.visit(stmnt);
        Struct condition_type = stmnt.getSafeIfElseCondition().obj.getType();
        this.commonIfStatementProcessing(stmnt, condition_type);
    }

    private void commonIfStatementProcessing(SyntaxNode stmnt, Struct condition_type) {
        if (!condition_type.equals(this.symbol_table.getOrDeclareGType("bool", null))) {
            report_error(
                "If condition must be of type bool",
                stmnt
            );
        }
    }


    // If Statement
    // ===========================================================


    // Veryfying Context Rules/Conditions
    // ===========================================================
    // ===========================================================

}
