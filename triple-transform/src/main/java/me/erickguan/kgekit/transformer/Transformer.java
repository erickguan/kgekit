package me.erickguan.kgekit.transformer;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public interface Transformer {
    void transform() throws IOException;

    FileInputStream getTripleInputStream();
    FileOutputStream getTripleOutputStream();
    FileInputStream getLabelInputStream();
    FileOutputStream getLabelOutputStream();
    FileInputStream getLiteralInputStream();
    FileOutputStream getLiteralOutputStream();
}
