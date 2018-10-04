package me.erickguan.kgekit.transformer;

import me.erickguan.kgekit.writer.SimpleRDFWriter;
import org.eclipse.rdf4j.model.Model;
import org.eclipse.rdf4j.model.Statement;
import org.eclipse.rdf4j.model.impl.LinkedHashModel;
import org.eclipse.rdf4j.rio.*;
import org.eclipse.rdf4j.rio.helpers.StatementCollector;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public abstract class TransformStrategy {
    abstract void transform(Transformer transformer) throws IOException;

    protected Model getModel(InputStream in) throws IOException {
        try {
            RDFParser rdfParser = Rio.createParser(RDFFormat.TURTLE);
            Model model = new LinkedHashModel();
            StatementCollector collector = new StatementCollector(model);
            rdfParser.setRDFHandler(collector);
            rdfParser.parse(in, ".");
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

    protected void writeTriples(OutputStream outputStream, Model model) throws RDFHandlerException {
        SimpleRDFWriter writer = new SimpleRDFWriter(outputStream);
        writer.setPrefixToBeRemoved("http://yago-knowledge.org/resource/");
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
}
