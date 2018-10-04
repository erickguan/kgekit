package me.erickguan.kgekit.writer;

import org.eclipse.rdf4j.common.io.IndentingWriter;
import org.eclipse.rdf4j.model.*;
import org.eclipse.rdf4j.model.datatypes.XMLDatatypeUtil;
import org.eclipse.rdf4j.model.util.Literals;
import org.eclipse.rdf4j.model.vocabulary.RDF;
import org.eclipse.rdf4j.model.vocabulary.XMLSchema;
import org.eclipse.rdf4j.rio.*;
import org.eclipse.rdf4j.rio.helpers.AbstractRDFWriter;
import org.eclipse.rdf4j.rio.turtle.TurtleUtil;

import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.nio.charset.StandardCharsets;
import java.util.*;

public class SimpleRDFWriter extends AbstractRDFWriter implements RDFWriter {
    private static final RDFFormat SIMPLE = new RDFFormat("Simple",
            Arrays.asList("text/plain", "text/plain"), StandardCharsets.UTF_8,
            Arrays.asList("txt"),
            null,
//            SimpleValueFactory.getInstance().createIRI("http://www.w3.org/ns/formats/Turtle"),
            RDFFormat.NO_NAMESPACES, RDFFormat.NO_CONTEXTS);

    /*-----------*
     * Variables *
     *-----------*/


    protected IndentingWriter writer;

    protected boolean writingStarted;

    /**
     * Flag indicating whether the last written statement has been closed.
     */
    protected boolean statementClosed;

    protected Resource lastWrittenSubject;

    protected IRI lastWrittenPredicate;

    public SimpleRDFWriter(OutputStream out) {
        this(new OutputStreamWriter(out, StandardCharsets.UTF_8));
    }

    public SimpleRDFWriter(Writer writer) {
        this.writer = new IndentingWriter(writer);
        namespaceTable = new LinkedHashMap<String, String>();
        writingStarted = false;
        statementClosed = true;
        lastWrittenSubject = null;
        lastWrittenPredicate = null;
    }

    private int prefixToBeRemoved;

    public void setPrefixToBeRemoved(String prefix) {
        prefixToBeRemoved = prefix.length();
    }

    @Override
    public RDFFormat getRDFFormat() {
        return SIMPLE;
    }

    @Override
    public void startRDF()
            throws RDFHandlerException
    {
        if (writingStarted) {
            throw new RuntimeException("Document writing has already started");
        }

        writingStarted = true;
    }

    @Override
    public void endRDF()
            throws RDFHandlerException
    {
        if (!writingStarted) {
            throw new RuntimeException("Document writing has not yet started");
        }

        try {
            writer.flush();
        }
        catch (IOException e) {
            throw new RDFHandlerException(e);
        }
        finally {
            writingStarted = false;
        }
    }

    @Override
    public void handleNamespace(String prefix, String name)
            throws RDFHandlerException
    {
        return;
    }

    @Override
    public void handleStatement(Statement st)
            throws RDFHandlerException {
        if (!writingStarted) {
            throw new RuntimeException("Document writing has not yet been started");
        }

        Resource subj = st.getSubject();
        IRI pred = st.getPredicate();
        Value obj = st.getObject();

        try {
            writeResource(subj);
            wrapLine(true);
            writer.increaseIndentation();

            // Write new predicate
            writePredicate(pred);
            wrapLine(true);

            writeValue(obj);
            writer.writeEOL();
        } catch (IOException e) {
            throw new RDFHandlerException(e);
        }
    }

    private void wrapLine(boolean space) throws IOException {
        if (space) {
            writer.write(" ");
        }
    }

    @Override
    public void handleComment(String comment)
            throws RDFHandlerException
    {
        return;
    }

    protected void writePredicate(IRI predicate)
            throws IOException
    {
        if (predicate.equals(RDF.TYPE)) {
            // Write short-cut for rdf:type
            writer.write("a");
        }
        else {
            writeURI(predicate);
        }
    }

    /**
     * Writes a value, optionally shortening it if it is an {@link IRI} and has a namespace definition that is
     * suitable for use in this context for shortening or a {@link BNode} that has been confirmed to be able
     * to be shortened in this context.
     *
     * @param val
     *        The {@link Value} to write.
     * @param canShorten
     *        True if, in the current context, we can shorten this value if it is an instance of {@link BNode}
     *        .
     * @throws IOException
     */
    protected void writeValue(Value val)
            throws IOException
    {
        if (val instanceof Resource) {
            writeResource((Resource)val);
        }
        else {
            writeLiteral((Literal)val);
        }
    }

    protected void writeResource(Resource res)
            throws IOException
    {
        writeURI((IRI)res);
    }

    protected void writeURI(IRI uri)
            throws IOException
    {
        String uriString = uri.toString();
        if (uriString.length() > prefixToBeRemoved) {
            uriString = uriString.substring(prefixToBeRemoved);
        }

        // Write full URI
        writer.write(TurtleUtil.encodeURIString(uriString));
    }

    protected void writeLiteral(Literal lit)
            throws IOException
    {
        String label = lit.getLabel();
        IRI datatype = lit.getDatatype();

        if (label.indexOf('\n') != -1 || label.indexOf('\r') != -1 || label.indexOf('\t') != -1) {
            // Write label as long string
            writer.write("\"\"\"");
            writer.write(TurtleUtil.encodeLongString(label));
            writer.write("\"\"\"");
        }
        else {
            // Write label as normal string
            writer.write("\"");
            writer.write(TurtleUtil.encodeString(label));
            writer.write("\"");
        }

        if (Literals.isLanguageLiteral(lit)) {
            // Append the literal's language
            writer.write("@");
            writer.write(lit.getLanguage().get());
        }
        else if (!XMLSchema.STRING.equals(datatype)) {
            // Append the literal's datatype (possibly written as an abbreviated
            // URI)
            writer.write("^^");
            writeURI(datatype);
        }
    }
}