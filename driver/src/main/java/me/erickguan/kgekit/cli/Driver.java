package me.erickguan.kgekit.cli;

import me.erickguan.kgekit.transformer.CountingTransformer;
import org.eclipse.rdf4j.model.*;

import java.io.IOException;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.HashSet;

public class Driver {
    public static void main(String args[]) {
        String filename = args[args.length-1];
        Path path = Paths.get(filename);
        CountingTransformer transformer = new CountingTransformer(path, 10);
        for (Statement stmt: transformer.gatherStatement()) {
            System.out.print(stmt.getSubject() + "\n");
            System.out.println("--------------------");
        }
    }
}
