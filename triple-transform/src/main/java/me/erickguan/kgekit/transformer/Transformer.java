package me.erickguan.kgekit.transformer;

import org.eclipse.rdf4j.model.Model;
import org.eclipse.rdf4j.model.impl.LinkedHashModel;
import org.eclipse.rdf4j.rio.*;
import org.eclipse.rdf4j.rio.helpers.StatementCollector;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Path;


public class Transformer {
    public Transformer(Path path) {
        this.documentPath = path;
    }

    public Model getResults() throws IOException {
        try {
            File file = new File(this.documentPath.toAbsolutePath().toString());
            InputStream inputStream = new FileInputStream(file);
            RDFParser rdfParser = Rio.createParser(RDFFormat.TURTLE);
            Model model = new LinkedHashModel();
            StatementCollector collector = new StatementCollector(model);
            rdfParser.setRDFHandler(collector);
            rdfParser.parse(inputStream, this.documentPath.toString());
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

    private Path documentPath;
}
