package rs.ac.bg.etf.pp1;

import rs.etf.pp1.mj.runtime.Code;
import rs.etf.pp1.symboltable.Tab;
import rs.etf.pp1.symboltable.concepts.Obj;
import rs.etf.pp1.symboltable.concepts.Struct;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;

import rs.ac.bg.etf.pp1.ast.*;
import rs.ac.bg.etf.pp1.symbol_table.SymbolTable;



public class CodeGenerator extends VisitorAdaptor {

    private int mainPC;
    private SymbolTable symbol_table;
    private boolean error_detected = false;

    private static class CurUnpackingDesignator {
        public Obj designator_obj;

        public CurUnpackingDesignator(Obj designator_obj) {
            this.designator_obj = designator_obj;
        }
    }

    private List<CurUnpackingDesignator> cur_unpacking_desig_list = new ArrayList<>();

    private static class CurJumpToFix {
        int address_to_patch;

        CurJumpToFix(int address_to_patch) {
            this.address_to_patch = address_to_patch;
        }

        public void fix() {
            Code.fixup(this.address_to_patch);
        }
    }

    private Stack<List<CurJumpToFix>>
            cur_term_jumps_to_fix_false_cond_facts = new Stack<>();

    private Stack<List<CurJumpToFix>>
            cur_term_jumps_to_fix_true_cond_facts = new Stack<>();

    private Stack<CurJumpToFix> cur_jumps_to_fix_skip_else = new Stack<>();

    private static class CurForLoop {
        int cond_begin_pc;
        // int cond_end_pc;
        int stmnt_begin_pc;
        // int stmnt_end_pc;
        // int body_begin_pc;
        // int body_end_pc;

        int jump_to_body_start_addr;

        List<Integer> breaks_to_fix_adrs = new ArrayList<>();

        CurForLoop() { }

        int registerToFixJump() {
            Code.putJump(0);
            final int first_byte_of_jmp_address_arg = Code.pc - 2;
            return first_byte_of_jmp_address_arg;
        }

        void fixJump(int address_to_patch) {
            Code.fixup(address_to_patch);
        }

        void registerBreak() {
            this.breaks_to_fix_adrs.add(this.registerToFixJump());
        }

        void fixBreakAdrs() {
            for (Integer break_addr_to_fix : this.breaks_to_fix_adrs) {
                this.fixJump(break_addr_to_fix);
            }
            this.breaks_to_fix_adrs.clear();
        }

        void genContinueJump() {
            Code.putJump(this.stmnt_begin_pc);
        }

    }

    private Stack<CurForLoop> cur_for_loops = new Stack<>();


    CodeGenerator(SymbolTable symbol_table) {
        this.symbol_table = symbol_table;
    }


	// ===========================================================
	// ===========================================================
    // Getters:

    public int getMainPC() {
        return this.mainPC;
    }

    public boolean wasErrorDetected() {
        return this.error_detected || Code.greska;
    }


	// ===========================================================
	// ===========================================================
    // Visiting:


	@Override
	public void visit(final ProgName progName) {
        this.visitInBuiltMethodsDeclarations();
    }



	// ===========================================================
	// ===========================================================
    // In built methods:


    /**
     * Visits inbuilt methods, added to universe before user's src code
     * is processed.
     */
    private void visitInBuiltMethodsDeclarations() {
        this.visitChrMethodDeclaration();
        this.visitOrdMethodDeclaration();
        this.visitLenMethodDeclaration();
    }

    /**
     * chr(ascii_val: int): char
     */
    private void visitChrMethodDeclaration() {
        Obj chr_method = Tab.chrObj;
        chr_method.setAdr(Code.pc);
        final int num_args = chr_method.getLevel();
        final int num_locals = 0;

        Code.put(Code.enter);
        // num formal param (1 param = 1 word = 4B)
        Code.put(num_args);
        // num formal params + local variables (1 param = 1 word = 4B)
        Code.put(num_args + num_locals);
        Code.put(Code.load_n + 0);  // gets converted to byte so no MOD neccessary
        Code.put(Code.exit);
        Code.put(Code.return_);
    }

    /**
     * ord(chr: char): int
     */
    private void visitOrdMethodDeclaration() {
        Obj ord_method = Tab.ordObj;
        ord_method.setAdr(Code.pc);
        final int num_args = ord_method.getLevel();
        final int num_locals = 0;

        Code.put(Code.enter);
        // num formal param (1 param = 1 word = 4B)
        Code.put(num_args);
        // num formal params + local variables (1 param = 1 word = 4B)
        Code.put(num_args + num_locals);
        Code.put(Code.load_n + 0);  // gets converted to byte so no MOD neccessary
        Code.put(Code.exit);
        Code.put(Code.return_);
    }

    /**
     * len(arr: any[]): int
     */
    private void visitLenMethodDeclaration() {
        Obj len_method = Tab.lenObj;
        len_method.setAdr(Code.pc);
        final int num_args = len_method.getLevel();
        final int num_locals = 0;

        Code.put(Code.enter);
        Code.put(num_args);
        Code.put(num_args + num_locals);
        Code.put(Code.load_n + 0);
        Code.put(Code.arraylength);
        Code.put(Code.exit);
        Code.put(Code.return_);
    }


	// ===========================================================
	// ===========================================================
    // Generating User Declared Method Bodies:


    @Override
    public void visit(TypeOrVoidMethodNameIsType method_name) {
        super.visit(method_name);
        this.commonVisit(method_name);
    }

    @Override
    public void visit(TypeOrVoidMethodNameIsVoid method_name) {
        super.visit(method_name);
        this.commonVisit(method_name);
    }

    private void commonVisit(TypeOrVoidMethodName type_method) {
        final Obj method_obj = type_method.obj;
        final String name = method_obj.getName();

        if ("main".equals(name)) this.mainPC = Code.pc;

        method_obj.setAdr(Code.pc);

        final int num_args = method_obj.getLevel();
        Code.put(Code.enter);
        Code.put(num_args);
        Code.put(method_obj.getLocalSymbols().size());
    }

    @Override
    public void visit(MethodDeclFormPars method) {
        final Struct ret_type = method.getTypeOrVoidMethodName().obj.getType();
        this.commonVisitMethodDecl(ret_type);
    }

    @Override
    public void visit(MethodDeclNoFormPars method) {
        final Struct ret_type = method.getTypeOrVoidMethodName().obj.getType();
        this.commonVisitMethodDecl(ret_type);
    }

    private void commonVisitMethodDecl(Struct ret_type) {
        if (Tab.noType.equals(ret_type)) {  // function is void
            Code.put(Code.exit);
            Code.put(Code.return_);
        } else {  // not void, so raise runtime error if no return stmnt
            Code.put(Code.trap);
            Code.put(0);
        }
    }

    @Override
    public void visit(StatementReturnExpr expr) {
        Code.put(Code.exit);
        Code.put(Code.return_);
    }

    @Override
    public void visit(StatementReturnNoExpr expr) {
        Code.put(Code.exit);
        Code.put(Code.return_);
    }


	// ===========================================================
	// ===========================================================
    // Implicitly implemented statement/function PRINT:


    /**
     * print(expr: bool|int|char, width: int): void
     */
    @Override
    public void visit(StatementPrintConstArg stmnt) {
        final Struct expr_type = stmnt.getExpr().obj.getType();
        final int width = stmnt.getWidth();
        this.genCodePrint(expr_type, width);
    }

    /**
     *    print(expr: bool|char, width=1): void
     * or print(expr: int, width=5): void
     */
    @Override
    public void visit(StatementPrintNoArg stmnt) {
        final Struct expr_type = stmnt.getExpr().obj.getType();
        // print width 1 for bool or char, 5 for int
        final int width = Tab.intType.equals(expr_type) ? 5 : 1;
        this.genCodePrint(expr_type, width);
    }

    private void genCodePrint(final Struct expr_type, final int width) {
        if (Tab.charType.equals(expr_type)) {  // print char
            Code.loadConst(width);
            Code.put(Code.bprint);
            return;
        }

        // else print bool/int as integer
        Code.loadConst(width);
        Code.put(Code.print);
    }


	// ===========================================================
	// ===========================================================
    // Implicitly implemented statement/function READ:

    /**
     *    read(dst: int): void
     * or read(dst: char): void
     */
    @Override
    public void visit(StatementRead stmnt) {
        final Obj designator_obj = stmnt.getDesignator().obj;
        final Struct expr_type = designator_obj.getType();

        if (Tab.charType.equals(expr_type)) {  // read byte/char
            Code.put(Code.bread);
        } else {  // else read integer
            Code.put(Code.read);
        }

        Code.store(designator_obj);
    }


	// ===========================================================
	// ===========================================================
    // Constants Assignment:

    // Done in semantic analysis


	// ===========================================================
	// ===========================================================
    // Variables Assignment:


    @Override
    public void visit(NamespacedDesignator designator) {
        this.loadDesignatorAddress(designator);
    }

    @Override
    public void visit(NoNamespaceDesignator designator) {
        this.loadDesignatorAddress(designator);
    }

    @Override
    public void visit(DesignatorAssignNormal designator_assign) {
        final Designator designator = designator_assign.getDesignator();
        this.storeDesignator(designator.obj);
        // its value neve gets used for further assignment
        // this.loadDesignator(designator);
    }

    @Override
    public void visit(HasOptDesignatorList desig_assign) {
        Obj desig_obj = desig_assign.getOptDesignator().obj;

        // if designator was provided add it, else add null
        if (desig_obj != null) {
            this.cur_unpacking_desig_list.add(
                new CurUnpackingDesignator(desig_obj)
            );
        } else {
            this.cur_unpacking_desig_list.add(null);
        }
    }

    @Override
    public void visit(DesignatorAssignArrUnpack designator_assign) {
        try {
            final Obj src_desig = designator_assign.getDesignator1().obj;
            final Obj dst_desig_arr = designator_assign.getDesignator().obj;
            final Obj src_desig_el = this.getElementFromArray(src_desig);
            final Obj dst_desig_arr_el = this.getElementFromArray(dst_desig_arr);

            // check if there are enough elements to unpack
            // +1 is for dst_desig_arr as thats the min
            final int min_num_els_to_unpack = 1 + this.cur_unpacking_desig_list.size();
            Code.loadConst(min_num_els_to_unpack);
            Code.load(src_desig);
            Code.put(Code.arraylength);
            // if req_len > N raise error, else all OK
            Code.putFalseJump(Code.gt, 0);  // if lte jump to OK
            int fixup_addr = Code.pc - 2;
            Code.put(Code.trap);  // not enough els to unpack
            Code.put(1);
            Code.fixup(fixup_addr);
            // else OK, there are enough els to unpack

            // assign el by el to all designators but dst_desig_arr
            // load first array value
            int src_desig_el_idx = 0;
            for (CurUnpackingDesignator cur_desig : this.cur_unpacking_desig_list) {
                if (cur_desig != null) {  // load cur array value
                    Code.load(src_desig);
                    Code.loadConst(src_desig_el_idx);
                    Code.load(src_desig_el);
                    Code.store(cur_desig.designator_obj);
                }

                src_desig_el_idx++;
            }
            // src_desig_el_idx is on the stack and represents
            // next idx of dst arr el value to load,
            // guaranteed there is more to load

            // assign the rest of the elements to the dst_desig_arr

            // base = src_desig_el_idx
            // l = leftover_els - 1 = min(
            //     src_desig.len - src_desig_el_idx - 1,
            //     dst_desig_arr.len - 1
            // );

            Code.loadConst(src_desig_el_idx);  // load base
            // load l:
            Code.load(src_desig);  // e-stack: b, arr,
            Code.put(Code.arraylength);  // e-stack: b, N
            Code.loadConst(src_desig_el_idx);  // e-stack: b, N, src_desig_el_idx
            Code.put(Code.sub);  // b, N - src_desig_el_idx
            Code.loadConst(1);
            Code.put(Code.sub);  // b, N - src_desig_el_idx - 1 = l1
            Code.load(dst_desig_arr);
            Code.put(Code.arraylength);
            Code.loadConst(1);
            Code.put(Code.sub);  // b, l1, l2
            Code.put(Code.dup2);  // b, l1, l2, l1, l2
            Code.putFalseJump(Code.gt, 0);  // b, l1, l2
            fixup_addr = Code.pc - 2;
            // then l1 > l2  // so remove l1 from b, l1, l2
            Code.put(Code.dup_x1);  // b, l2, l1, l2
            Code.put(Code.pop);  // b, l2, l1
            Code.fixup(fixup_addr);
            Code.put(Code.pop);  // if l1 > l2 then `b, l2` else `b, l1`

            // currently e-stack: b, l
            final int do_while_begin = Code.pc;
            Code.put(Code.dup2);     // b, l, b, l
            Code.load(dst_desig_arr);  // b, l, b, l, arr
            Code.put(Code.dup_x2);  // b, l, arr, b, l, arr
            Code.put(Code.pop);     // b, l, arr, b, l
            Code.put(Code.dup);     // b, l, arr, b, l, l
            Code.put(Code.dup_x2);  // b, l, arr, l, b, l, l
            Code.put(Code.pop);     // b, l, arr, l, b, l
            Code.put(Code.add);     // b, l, arr, l, b + l
            Code.load(src_desig);   // b, l, arr, l, b + l, arr_src
            Code.put(Code.dup_x1);  // b, l, arr, l, arr_src, b + l, arr_src
            Code.put(Code.pop);     // b, l, arr, l, arr_src, b + l
            // copy from 1 array to another
            Code.load(src_desig_el);
            Code.store(dst_desig_arr_el);  // b, l
            // update the counter l = l - 1
            Code.loadConst(1);  // b, l, 1
            Code.put(Code.sub);  // b, l = l - 1
            Code.put(Code.dup);  // b, l, l
            Code.loadConst(-1);  // b, l, l, -1
            // if l <= -1 then it is the end of the assignment
            Code.putFalseJump(Code.gt, 0);
            fixup_addr = Code.pc - 2;
            // l > -1 => more els to copy
            Code.putJump(do_while_begin);
            Code.fixup(fixup_addr);
            // else: currently e-stack: b, l where l <= -1
            Code.put(Code.pop);  // b
            Code.put(Code.pop);  // empty

        } finally {
            this.cur_unpacking_desig_list.clear();
        }
    }

    /**
     * Assumes the designator address in case of arr el was already set
     * on the expression stack appropriately and the value to store is
     * right after it.
     */
    private void storeDesignator(Obj designator_obj) {
        Code.store(designator_obj);
    }

    /**
     * Assumes the designator address in case of arr el was already set
     * on the expression stack appropriately.
     */
    private void loadDesignator(Obj designator_obj) {
        Code.load(designator_obj);
    }

    /**
     * In case the given designator is an array element,
     * it sets the top of stack from: idx to arr, idx.
     */
    private void loadDesignatorAddress(Designator designator) {
        // when using non arr elements address should be provide on
        // procedure stack, nothing on expressin stack
        if (designator.obj.getKind() != Obj.Elem) return;

        // expr stack: idx
        // desired expr stack: arr, idx
        Obj arr_obj = this.getArrayFromElement(designator.obj);
        Code.load(arr_obj);  // e-stack: idx, arr
        Code.put(Code.dup_x1);  // e-stack: arr, idx, arr
        Code.put(Code.pop);  // e-stack: arr, idx
    }

    private Obj getArrayFromElement(Obj arr_el_obj) {
        Struct arr_type = this.symbol_table.getOrDeclareGlobalArrType(
            arr_el_obj.getType()
        );
        return new Obj(
            Obj.Var,
            arr_el_obj.getName(),
            arr_type,
            arr_el_obj.getAdr(),
            arr_el_obj.getLevel()
        );
    }

    private Obj getElementFromArray(Obj arr_obj) {
        return new Obj(
            Obj.Elem,
            arr_obj.getName(),
            arr_obj.getType().getElemType(),
            arr_obj.getAdr(),
            arr_obj.getLevel()
        );
    }


	// ===========================================================
	// ===========================================================
    // Popping stack leftovers:


    /**
     * Only FuncCall that does not leave return value on expression stack.
     */
    @Override
    public void visit(DesignatorStatementFuncCall stmnt) {
        // only this statement just calls the function and should clear
        // the expression stack in case the function was not void
        Obj func_obj = stmnt.getFuncCall().obj;
        this.genCodeFuncCall(func_obj);

        // ingore the case where designator/function is an array element

        if (Tab.noType.equals(func_obj.getType())) return;
        Code.put(Code.pop);
    }

    private void genCodeFuncCall(Obj func_obj) {
        final int offset = func_obj.getAdr() - Code.pc;
        Code.put(Code.call);
        Code.put2(offset);
    }

    /**
     * INCR or DECR. No load of value on expression stack upon completion.
     */
    @Override
    public void visit(DesignatorStatementMisc stmnt) {
        final Designator designator = stmnt.getDesignator();
        this.loadDesignator(designator.obj);

        if (stmnt.getDesignatorDetail() instanceof DesignatorDetailFactorIncr) {
            Code.loadConst(1);
        } else {  // decrement
            Code.loadConst(-1);
        }

        Code.put(Code.add);
        this.storeDesignator(designator.obj);

        // return value is never used so just dont load
        // into the expression stack
        // pop the designator whose value was loaded
        // Code.put(Code.pop);
    }

    /**
     * DesignatorAssign never loads its value onto expression stack
     * so no need to do anything.
     */
    @Override
    public void visit(StatementDesigAssign stmnt) {
        // pop the designator whose value was loaded
        // Code.put(Code.pop);
    }


	// ===========================================================
	// ===========================================================
    // Expressions and Terms:


    /**
     * Must be called bottom up.
     */
    // private class PostFixExprExecutor extends VisitorAdaptor {

        // ===========================================================
        // Factor:

        @Override
        public void visit(FactorNumber factor) {
            Code.loadConst(factor.getValue());
        }
    
        @Override
        public void visit(FactorChar factor) {
            Code.loadConst(factor.getValue());
        }
    
        @Override
        public void visit(FactorBool factor) {
            Code.loadConst(factor.getValue() ? 1 : 0);
        }
    
        /**
         * Leaves the allocated space adr on the expression stack.
         */
        @Override
        public void visit(FactorNewExpr factor) {
            final Struct elem_type = factor.getType().struct;
            // expr has set the num elements on top of stack

            // byte array only if elem type is char
            Code.put(Code.newarray);
            // for chars store array of 1B els, else 4B els
            Code.put(Tab.charType.equals(elem_type) ? 0 : 1);
        }

        /**
         * Load the designator onto the expression stack.
         */
        @Override
        public void visit(FactorDesignatorNoFactorPars factor) {
            CodeGenerator.this.loadDesignator(factor.getDesignator().obj);
        }

        /**
         * Should leave the FuncCall return value on the epression stack.
         */
        @Override
        public void visit(FactorDesignatorFactorPars factor) {
            this.genCodeFuncCall(factor.getFuncCall().obj);
        }


        // ===========================================================
        // Operations over factors:

        @Override
        public void visit(HasMulopFactorList factor) {
            this.genMulopCode(factor.getMulop());
        }

        @Override
        public void visit(SingleMulopFactor factor) {
            this.genMulopCode(factor.getMulop());
        }

        @Override
        public void visit(HasAddopTermList has_addop) {
            this.genAddopCode(has_addop.getAddop());
        }

        @Override
        public void visit(ExprNegation expr) {
            Code.put(Code.neg);
        }

        private void genMulopCode(Mulop mulop) {
            if (mulop instanceof MulopMul) {
                Code.put(Code.mul);
                return;
            }

            if (mulop instanceof MulopDiv) {
                Code.put(Code.div);
                return;
            }

            if (mulop instanceof MulopMod) {
                Code.put(Code.rem);
                return;
            }
        }

        private void genAddopCode(Addop addop) {
            if (addop instanceof AddopPlus) {
                Code.put(Code.add);
                return;
            }

            if (addop instanceof AddopMinus) {
                Code.put(Code.sub);
                return;
            }
        }

    // }


    // ===========================================================
    // Logic Conditions:
    

    @Override
    public void visit(IfBegin if_begin) {
        this.onIfKeywordBegin();
    }

    @Override
    public void visit(SafeIfElseConditionOk cond) {
        this.onIfBodyBegin();
    }

    @Override
    public void visit(ElseBegin else_begin) {
        this.onIfBodyEnd(true);
    }

    @Override
    public void visit(StatementIfElse if_else) {
        this.onElseBodyEnd();
    }

    @Override
    public void visit(StatementIfNoElse if_no_else) {
        this.onIfBodyEnd(false);
    }

    /**
     * On keyword IF reached, Condition not yet processed.
     */
    private void onIfKeywordBegin() {
        this.cur_term_jumps_to_fix_false_cond_facts.add(new ArrayList<>());
        this.cur_term_jumps_to_fix_true_cond_facts.add(new ArrayList<>());
    }

    private void onIfBodyBegin() {
        this.fixCurCondTermJumpsTrueCondFacts();
        this.cur_term_jumps_to_fix_true_cond_facts.pop();
    }

    private void onIfBodyEnd(boolean has_else) {
        if (has_else) this.genCodeSkipElseBody();

        // fix the last CondTerm leftover on false to skip if body
        this.fixCurCondTermJumpsFalseCondFacts();
        this.cur_term_jumps_to_fix_false_cond_facts.pop();
    }

    private void genCodeSkipElseBody() {
        // at end of IF body set to skip ELSE whose end is not yet known
        Code.putJump(0);   // set dummy jmp address for now

        // store the code address to replace the dummy jump address
        // with accurate address later
        final int first_byte_of_jmp_address_arg = Code.pc - 2;
        this.cur_jumps_to_fix_skip_else.add(
            new CurJumpToFix(first_byte_of_jmp_address_arg)
        );
    }

    private void onElseBodyEnd() {
        // patch jump from end of IF THEN body over the ELSE
        CurJumpToFix cur = this.cur_jumps_to_fix_skip_else.pop();
        cur.fix();
    }

    /**
     * Fixes all conditional facts of current CondTerm when they
     * evaluate to true.
     */
    private void fixCurCondTermJumpsTrueCondFacts() {
        // patch jump addresses when if evaluates to false
        for (CurJumpToFix cur : this.cur_term_jumps_to_fix_true_cond_facts.peek()) {
            cur.fix();
        }
        this.cur_term_jumps_to_fix_true_cond_facts.peek().clear();
    }

    /**
     * Fixes all false conditional facts jumps of current CondTerm when they
     * evaluate to false.
     */
    private void fixCurCondTermJumpsFalseCondFacts() {
        // patch jump addresses when if evaluates to false
        for (CurJumpToFix cur : this.cur_term_jumps_to_fix_false_cond_facts.peek()) {
            cur.fix();
        }
        this.cur_term_jumps_to_fix_false_cond_facts.peek().clear();
    }

    /**
     * Check if expr does not equal to zero.
     */
    @Override
    public void visit(CondFactExpr cond_relop) {
        final int oc = Code.ne;
        // expr is already on stack, add 0 for: expr != 0
        Code.loadConst(0);
        this.registerToFixFalseCondFact(oc);
    }

    /**
     * Compare 2 expressions to get a logical condiational factor.
     */
    @Override
    public void visit(CondFactRelop cond_relop) {
        final int oc = this.getRelopOperationCode(cond_relop.getRelop());
        this.registerToFixFalseCondFact(oc);
    }

    private void registerToFixTrueCondFact() {
        // set dummy jmp address for now
        Code.putJump(0);

        // store the code address to replace the dummy jump address
        // with accurate address later
        final int first_byte_of_jmp_address_arg = Code.pc - 2;
        this.cur_term_jumps_to_fix_true_cond_facts.peek().add(
            new CurJumpToFix(first_byte_of_jmp_address_arg)
        );
    }

    private void registerToFixFalseCondFact(int operation_code) {
        // set dummy jmp address for now
        Code.putFalseJump(operation_code, 0);

        // store the code address to replace the dummy jump address
        // with accurate address later
        final int first_byte_of_jmp_address_arg = Code.pc - 2;
        this.cur_term_jumps_to_fix_false_cond_facts.peek().add(
            new CurJumpToFix(first_byte_of_jmp_address_arg)
        );
    }

    private int getRelopOperationCode(Relop relop) {
        if (relop instanceof RelopEq) {
            return Code.eq;
        }

        if (relop instanceof RelopIneq) {
            return Code.ne;
        }

        if (relop instanceof RelopGt) {
            return Code.gt;
        }

        if (relop instanceof RelopGte) {
            return Code.ge;
        }

        if (relop instanceof RelopLt) {
            return Code.lt;
        }

        if (relop instanceof RelopLte) {
            return Code.le;
        }

        return -1;
    }

    @Override
    public void visit(NoCondTermList or_cond) {
        // if no additional OR then previous true cond term
        // then naturally cond term falls through to first stmnt of IF
    }

    /**
     * Only last CondTerm jumps to start of else, others jump
     * to next CondTerm if they failed.
     */
    @Override
    public void visit(ConditionalOR or_cond) {
        // if previous CondTerm is true then just wait to set
        // address of first statement of the IF body
        this.registerToFixTrueCondFact();

        // if one CondFact of previous CondTerm is false then
        // that CondTerm is entirely false so set all their
        // jumps next condition which is to be loaded by next CondTerm/Fact
        this.fixCurCondTermJumpsFalseCondFacts();
    }


    // ===========================================================
    // ===========================================================
    // For Loop:


    @Override
    public void visit(ForLoopStart loop) {
        this.cur_for_loops.add(new CurForLoop());
    }

    @Override
    public void visit(ForLoopConditionBegin loop) {
        this.cur_term_jumps_to_fix_false_cond_facts.add(new ArrayList<>());
        this.cur_term_jumps_to_fix_true_cond_facts.add(new ArrayList<>());

        CurForLoop for_loop = this.cur_for_loops.peek();
        for_loop.cond_begin_pc = Code.pc;
    }

    @Override
    public void visit(ForLoopConditionEnd loop) {
        CurForLoop for_loop = this.cur_for_loops.peek();
        // for_loop.cond_end_pc = Code.pc;
        for_loop.jump_to_body_start_addr = for_loop.registerToFixJump();
    }

    @Override
    public void visit(ForLoopPostStmntBegin loop) {
        CurForLoop for_loop = this.cur_for_loops.peek();
        for_loop.stmnt_begin_pc = Code.pc;
    }

    @Override
    public void visit(ForLoopPostStmntEnd loop) {
        CurForLoop for_loop = this.cur_for_loops.peek();
        // for_loop.stmnt_end_pc = Code.pc;

        // after post stmnt check the condition again
        Code.putJump(for_loop.cond_begin_pc);
    }

    @Override
    public void visit(ForLoopBodyBegin loop) {
        CurForLoop for_loop = this.cur_for_loops.peek();
        // for_loop.body_begin_pc = Code.pc;

        // resolve single CondTerm when true which just
        // fell through to embedded jump_to_body_start_addr
        for_loop.fixJump(for_loop.jump_to_body_start_addr);

        // resolve condition true jump to jump here
        this.fixCurCondTermJumpsTrueCondFacts();
        this.cur_term_jumps_to_fix_true_cond_facts.pop();
    }

    @Override
    public void visit(ForLoopBodyEnd loop) {
        CurForLoop for_loop = this.cur_for_loops.peek();

        // after the body execute post stmnt
        Code.putJump(for_loop.stmnt_begin_pc);
        // for_loop.body_end_pc = Code.pc;

        // Link everything to end of body, 1st stmnt not in for:

        // !important must be after putJump(stmnt_begin_pc)
        this.fixCurCondTermJumpsFalseCondFacts();
        this.cur_term_jumps_to_fix_false_cond_facts.pop();

        for_loop.fixBreakAdrs();
    }

    @Override
    public void visit(StatementForHasCond2 loop) {
        this.commonVisitForLoop();
    }

    @Override
    public void visit(StatementForNoCond1 loop) {
        this.commonVisitForLoop();
    }

    @Override
    public void visit(StatementForHasCond4 loop) {
        this.commonVisitForLoop();
    }

    @Override
    public void visit(StatementForNoCond3 loop) {
        this.commonVisitForLoop();
    }

    private void commonVisitForLoop() {
        this.onForLoopEnd();
    }

    private void onForLoopEnd() {
        this.cur_for_loops.pop();
    }


    // ===========================================================
    // Break and Continue:


    @Override
    public void visit(StatementBreak stmnt_break) {
        CurForLoop for_loop = this.cur_for_loops.peek();
        for_loop.registerBreak();
    }

    /**
     * Must be called after ForLoopPostStmntBegin.
     */
    @Override
    public void visit(StatementContinue stmnt_continue) {
        CurForLoop for_loop = this.cur_for_loops.peek();
        for_loop.genContinueJump();
    }

}
