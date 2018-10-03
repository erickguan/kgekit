package me.erickguan.kgekit.transformer;

import java.io.*;

public class FileTransfromer {
    public FileTransfromer(String inPath, String outPath, Transformer transfromer) {
        this.inPath = inPath;
        this.outPath = outPath;
        this.transformer = transfromer;
    }

    public void transform() throws IOException {
        FileInputStream in = new FileInputStream(new File(inPath));
        FileOutputStream out = new FileOutputStream(new File(outPath));
        transformer.transform(in, out);
    }

    private String inPath;
    private String outPath;
    final private Transformer transformer;
}
