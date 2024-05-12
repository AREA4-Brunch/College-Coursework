package rs.ac.bg.etf.pp1;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.PrintStream;

import org.apache.log4j.Logger;
import org.apache.log4j.xml.DOMConfigurator;

import java_cup.runtime.Symbol;
import rs.ac.bg.etf.pp1.ast.Program;
import rs.ac.bg.etf.pp1.symbol_table.SymbolTable;
import rs.ac.bg.etf.pp1.util.Log4JUtils;
import rs.etf.pp1.mj.runtime.Code;
import rs.etf.pp1.symboltable.Tab;

public class Compiler {

    private static Compiler instance = null;
    private SymbolTable symbol_table = SymbolTable.getInstance();

    static {
        DOMConfigurator.configure(Log4JUtils.instance().findLoggerConfigFile());
        Log4JUtils.instance().prepareLogFile(Logger.getRootLogger());
    }

    public static Compiler getInstance() {
        if (Compiler.instance == null) {
            Compiler.instance = new Compiler();
        }
        return Compiler.instance;
    }

    public static void main(String[] args) {
        final Logger logger = Logger.getLogger(Compiler.class);

        /// called with args: test/program.mj test/program.obj
        if (args.length < 2) {
            logger.info("Not enough arguments provided on program call!");
            return;
        }

        final File sourceCode = new File(args[0]);

        if (!sourceCode.exists()) {
            // logger.error("Source file [" + sourceCode.getAbsolutePath() + "] not found!");
            logger.error("Source file [" + sourceCode.getAbsolutePath() + "] not found!");
            return;
        }

        final int max_src_file_size = 8 * 1024;  // 8KB
        if (sourceCode.length() > max_src_file_size) {
            logger.error("Maximum source file size is " + max_src_file_size);
            return;
        }

        // redirect the sys out and err to file
        String file_name_no_ext = args[0].substring(0, args[0].length() - ".obj".length() + 1);
        try {
            File stdout_file = new File(file_name_no_ext + ".out");
            PrintStream stdout = new PrintStream(new FileOutputStream(stdout_file, false));
            System.setOut(stdout);

        } catch(Exception err) {
            logger.error("Failed to redirect sys out stream to file.");
        }

        try {
            File stderr_file = new File(file_name_no_ext + ".err");
            PrintStream stderr = new PrintStream(new FileOutputStream(stderr_file, false));
            System.setErr(stderr);

        } catch(Exception err) {
            logger.error("Failed to redirect sys err stream to file.");
        }

        logger.info("Compiling source file: " + sourceCode.getAbsolutePath());

        try (BufferedReader br = new BufferedReader(new FileReader(sourceCode))) {
            final Yylex lexer = new Yylex(br);  // for lexic analysis
            final MJParser parser = new MJParser(lexer);  // for syntax analysis
            // Lexic and Syntax analysis executed on parser.parse()
            final Symbol prog_token = parser.parse();

            if (prog_token == null) {
                throw new RuntimeException("Failed to parse, token == null, nonterminal Program was never recognized.");
            }

            // Semantic analysis
            Compiler compiler = Compiler.getInstance();
            compiler.symbol_table.init();  // init the universe
            final SemanticAnalyzer semantic_analyzer = new SemanticAnalyzer(
                compiler.symbol_table
            );
            final Program program = (Program) prog_token.value;
            program.traverseBottomUp(semantic_analyzer);

            // log semantic analysis and symbol table, then syntax tree
            compiler.tsdump();
            logger.info("\nSyntax tree (token: " + prog_token.toString() + "):");
            logger.info(prog_token.value.toString());

            // notify and exit if parsing failed
            if ( lexer.wasErrorDetected()
              || parser.wasErrorDetected()
              || semantic_analyzer.wasErrorDetected()
            ) {
                logger.error(
                    "\n\nParsing Failed. Errors Detected by:"
                  +  (lexer.wasErrorDetected() ? " LEXER," : "")
                  +  (parser.wasErrorDetected() ? " PARSER," : "")
                  +  (semantic_analyzer.wasErrorDetected() ? " SEMANTIC ANALYZER" : "")
                );
                return;
            }
            // else successful parsing and semantic analysis

            // Compile the program:
            
            // create output file
            File obj_file = new File(args[1]);
            logger.info("Generating bytecode file: " + obj_file.getAbsolutePath());
            if (obj_file.exists()) {
                logger.info("Deleting already existing bytecode file: " + obj_file.getAbsolutePath());
                obj_file.delete();
            }

            CodeGenerator code_generator = new CodeGenerator(compiler.symbol_table);
            program.traverseBottomUp(code_generator);

            if (code_generator.wasErrorDetected()) {
                logger.error("Compilation failed :(");
                return;
            }

            Code.dataSize = semantic_analyzer.getNumGlobalVariablesOrFields();
            Code.mainPc = code_generator.getMainPC();
            Code.write(new FileOutputStream(obj_file));

            logger.info("\n\nSuccessfuly compiled the source file:\n" + sourceCode.getAbsolutePath() + "\n\n");

        } catch (Exception e) {
            // logger.error("An exception occured: " + e.toString());
            logger.error("An exception occured: " + e.toString());
            return;
        }

    }

    public void tsdump() {
        Tab.dump(this.symbol_table.createVisitor());
    }

}
