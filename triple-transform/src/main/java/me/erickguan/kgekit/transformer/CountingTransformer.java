package me.erickguan.kgekit.transformer;

import me.erickguan.kgekit.writer.SimpleRDFWriter;
import org.eclipse.rdf4j.model.*;
import org.eclipse.rdf4j.model.Value;
import org.eclipse.rdf4j.model.impl.LinkedHashModel;
import org.eclipse.rdf4j.rio.*;
import org.eclipse.rdf4j.rio.helpers.StatementCollector;

import java.io.*;
import java.lang.reflect.Array;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;


public class CountingTransformer implements Transformer {
    public CountingTransformer(int numberOfLink) {
        this.numberOfLink = numberOfLink;
    }

    public void transform(InputStream in, OutputStream out) throws IOException {
        Model model = getModel(in);
        Model stmts = gatherStatement(model);
        writeTriples(out, stmts);
    }

    private void writeTriples(OutputStream outputStream, Model model) throws RDFHandlerException {
        RDFWriter writer = new SimpleRDFWriter(outputStream);

        try {
            writer.startRDF();
            for (Statement st: model) {
                writer.handleStatement(st);
            }
            writer.endRDF();
        } catch (RDFHandlerException e) {
            System.err.println("RDF can't be processed. " + e.getMessage());
            throw e;
        }
    }

    private Model getModel(InputStream in) throws IOException {
        try {
            RDFParser rdfParser = Rio.createParser(RDFFormat.TURTLE);
            Model model = new LinkedHashModel();
            StatementCollector collector = new StatementCollector(model);
            rdfParser.setRDFHandler(collector);
            rdfParser.parse(in, null);
            return model.unmodifiable();
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

    private Model pullSubjectStatements(Model model, Resource value) {
        return model.filter(value, null, null);
    }


    private Model pullObjectStatements(Model model, Value value) {
        return model.filter(null, null, value);
    }

    private Model gatherStatement(Model model) {
        Model stmts = new LinkedHashModel();
        for (HashMap.Entry<Resource, Integer> subjEntry : this.getSubjects(model).entrySet()) {
            stmts.addAll(pullSubjectStatements(model, subjEntry.getKey()));
        }
        for (HashMap.Entry<Value, Integer> objEntry : this.getObjects(model).entrySet()) {
            stmts.addAll(pullObjectStatements(model, objEntry.getKey()));
        }
        return stmts;
    }

    private HashMap<Resource, Integer> getSubjects(Model model) {
        HashMap<Resource, Integer> map = new HashMap<>();
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

    private HashMap<Value, Integer> getObjects(Model model) {
        HashMap<Value, Integer> map = new HashMap<>();
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

    private int numberOfLink;
}
