package me.erickguan.kgekit.cli;

import me.erickguan.kgekit.transformer.Transformer;
import org.eclipse.rdf4j.model.*;

import java.io.IOException;
import java.nio.file.Path;
import java.nio.file.Paths;

public class Driver {
    public static void main(String args[]) {
        try {
            String filename = args[args.length-1];
            Path path = Paths.get(filename);
            Transformer transformer = new Transformer(path);
            Model model = transformer.getResults();
            for (Statement statement: model) {
                Resource subject = statement.getSubject();
                IRI relation = statement.getPredicate();
                Value object = statement.getObject();
                System.out.println(subject);
                System.out.println(relation);
                System.out.println(object);
                System.out.println("--------------------");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
