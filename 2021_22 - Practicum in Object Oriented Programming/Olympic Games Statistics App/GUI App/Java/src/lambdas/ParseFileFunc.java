package lambdas;


import java.io.File;


@FunctionalInterface
public interface ParseFileFunc {
    int call(File file);
}
