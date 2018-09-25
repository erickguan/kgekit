import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import org.eclipse.rdf4j.rio.Rio;

class Transformer {
    public static void main(String args[]) {
        System.out.println("Hello world");
    }

    Transformer(String path) {
        this.documentUrl = new URL(path);
    }

    Model getResults() {
        InputStream inputStream = this.documentUrl.openStream();
        RDFParser rdfParser = Rio.createParser(RDFFormat.TURTLE);
        Model model = new LinkedHashModel();
        rdfParser.setRDFHandler(new StatementCollector(model));
        try {
            rdfParser.parse(inputStream, this.documentURL.toString());
        } catch (IOException e) {
            // handle IO problems (e.g. the file could not be read)
        } catch (RDFParseException e) {
            // handle unrecoverable parse error
        } catch (RDFHandlerException e) {
            // handle a problem encountered by the RDFHandler
        } finally {
            inputStream.close();
        }
    }

    java.net.URL documentUrl;
}
