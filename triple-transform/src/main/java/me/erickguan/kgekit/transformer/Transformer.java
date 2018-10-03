package me.erickguan.kgekit.transformer;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public interface Transformer {
    void transform(InputStream inputStream, OutputStream output) throws IOException;
}
