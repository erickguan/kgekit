package me.erickguan.kgekit.transformer;

import org.eclipse.rdf4j.model.*;
import org.eclipse.rdf4j.model.Value;
import org.eclipse.rdf4j.model.impl.LinkedHashModel;
import org.eclipse.rdf4j.rio.*;
import org.eclipse.rdf4j.rio.helpers.StatementCollector;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Array;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;


public class CountingTransformer {
    public CountingTransformer(Path path, int numberOfLink) {
        this.documentPath = path;
        this.numberOfLink = numberOfLink;
    }

    public Model pullSubjectStatements(Resource value) {
        return model.filter(value, null, null);
    }


    public Model pullObjectStatements(Value value) {
        return model.filter(null, null, value);
    }

    public HashSet<Statement> gatherStatement() {
        HashSet<Statement> stmts = new HashSet<>();
        for (HashMap.Entry<Resource, Integer> subjEntry : this.getSubjects().entrySet()) {
            stmts.addAll(pullSubjectStatements(subjEntry.getKey()));
        }
        for (HashMap.Entry<Value, Integer> objEntry : this.getObjects().entrySet()) {
            stmts.addAll(pullObjectStatements(objEntry.getKey()));
        }
        return stmts;
    }

    public HashMap<Resource, Integer> getSubjects() {
        Model model;
        HashMap<Resource, Integer> map = new HashMap<>();
        try {
            model = this.getResults();
        } catch (IOException e) {
            model = new LinkedHashModel();
        }
        for (Resource subject : model.subjects()) {
            Model stmts = model.filter(subject, null, null);
            if (stmts.size() <= numberOfLink) {
                continue;
            }
            for (Statement stmt : stmts) {
                map.merge(stmt.getSubject(), 1, Integer::sum);
            }
        }
        return map;
    }

    public HashMap<Value, Integer> getObjects() {
        Model model;
        HashMap<Value, Integer> map = new HashMap<>();
        try {
            model = this.getResults();
        } catch (IOException e) {
            model = new LinkedHashModel();
        }
        for (Value object : model.objects()) {
            Model stmts = model.filter(null, null, object);
            if (stmts.size() <= numberOfLink) {
                continue;
            }
            for (Statement stmt : stmts) {
                map.merge(stmt.getObject(), 1, Integer::sum);
            }
        }
        return map;
    }

    public Model getResults() throws IOException {
        if (model != null) {
            return model;
        }
        try {
            File file = new File(this.documentPath.toAbsolutePath().toString());
            InputStream inputStream = new FileInputStream(file);
            RDFParser rdfParser = Rio.createParser(RDFFormat.TURTLE);
            model = new LinkedHashModel();
            StatementCollector collector = new StatementCollector(model);
            rdfParser.setRDFHandler(collector);
            rdfParser.parse(inputStream, this.documentPath.toString());
            model = model.unmodifiable();
            return model;
        } catch (IOException e) {
            // handle IO problems (e.g. the file could not be read)
            System.out.println("Error: can't read message.");
            throw e;
        } catch (RDFParseException e) {
            // handle unrecoverable parse error
            System.out.println("Error: RDF parsing error.");
            throw e;
        } catch (RDFHandlerException e) {
            // handle a problem encountered by the RDFHandler
            System.out.println("Error: RDF handling error.");
            throw e;
        }
    }

    private Model model;
    private Path documentPath;
    private int numberOfLink;
}
