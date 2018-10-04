package me.erickguan.kgekit.transformer;

import java.io.*;

public class FileTransformer implements Transformer {
    public FileTransformer(String triplePath,
                           String tripleOutputPath,
                           String labelPath,
                           String labelOutputPath,
                           String literalPath,
                           String literalOutputPath,
                           TransformStrategy strategy) throws FileNotFoundException {
        this.tripleInputStream = openInputStream(triplePath);
        this.tripleOutputStream = openOutputStream(tripleOutputPath);
        this.labelInputStream = openInputStream(labelPath);
        this.labelOutputStream = openOutputStream(labelOutputPath);
        this.literalInputStream = openInputStream(literalPath);
        this.literalOutputStream = openOutputStream(literalOutputPath);
        this.strategy = strategy;
    }

    private FileInputStream openInputStream(String path) throws FileNotFoundException {
        return path == null ? null : new FileInputStream(new File(path));
    }

    private FileOutputStream openOutputStream(String path) throws FileNotFoundException {
        return path == null ? null : new FileOutputStream(new File(path));
    }

    @Override
    public void transform() throws IOException {
        strategy.transform(this);
    }

    @Override
    public FileInputStream getTripleInputStream() {
        return tripleInputStream;
    }

    @Override
    public FileOutputStream getTripleOutputStream() {
        return tripleOutputStream;
    }

    @Override
    public FileInputStream getLabelInputStream() {
        return labelInputStream;
    }

    @Override
    public FileOutputStream getLabelOutputStream() {
        return labelOutputStream;
    }

    @Override
    public FileInputStream getLiteralInputStream() {
        return literalInputStream;
    }

    @Override
    public FileOutputStream getLiteralOutputStream() {
        return literalOutputStream;
    }

    private final FileInputStream tripleInputStream;
    private final FileOutputStream tripleOutputStream;
    private final FileInputStream labelInputStream;
    private final FileOutputStream labelOutputStream;
    private final FileInputStream literalInputStream;
    private final FileOutputStream literalOutputStream;
    private TransformStrategy strategy;
}
