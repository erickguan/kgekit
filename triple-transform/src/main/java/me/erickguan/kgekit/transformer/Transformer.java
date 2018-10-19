package me.erickguan.kgekit.transformer;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;

public interface Transformer {
    void transform() throws IOException;

    InputStream getTripleInputStream() throws IOException;
    OutputStream getTripleOutputStream() throws IOException;
    InputStream getLabelInputStream() throws IOException;
    OutputStream getLabelOutputStream() throws IOException;
    InputStream getLiteralInputStream() throws IOException;
    OutputStream getLiteralOutputStream() throws IOException;
}
