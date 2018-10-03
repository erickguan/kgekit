package me.erickguan.kgekit.cli;

import me.erickguan.kgekit.transformer.CountingTransformer;
import org.eclipse.rdf4j.model.*;

import java.io.IOException;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.HashMap;

public class Driver {
    public static void main(String args[]) {
        String filename = args[args.length-1];
        Path path = Paths.get(filename);
        CountingTransformer transformer = new CountingTransformer(path, 10);
        HashMap<Value, Integer> subjectStmts = transformer.getSubjects();
        HashMap<Value, Integer> predicateStmts = transformer.getPredicates();
        for (HashMap.Entry<Value, Integer> entry: subjectStmts.entrySet()) {
            Value subject = entry.getKey();
            System.out.print(subject+":"+entry.getValue()+"\n");
            Integer predicate = predicateStmts.get(subject);
            if (predicate != null) {
                System.out.println(predicate);
            }
            System.out.println("--------------------");
        }
    }
}
