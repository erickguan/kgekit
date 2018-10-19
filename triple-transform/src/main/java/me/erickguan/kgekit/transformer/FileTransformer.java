package me.erickguan.kgekit.transformer;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.InvalidPathException;
import java.nio.file.Paths;

public class FileTransformer implements Transformer {
    public FileTransformer(String triplePath,
                           String tripleOutputPath,
                           TransformStrategy strategy) throws InvalidPathException {
        this(triplePath, tripleOutputPath, null, null, null, null, strategy);
    }

    public FileTransformer(String triplePath,
                           String tripleOutputPath,
                           String labelPath,
                           String labelOutputPath,
                           TransformStrategy strategy) throws InvalidPathException {
        this(triplePath, tripleOutputPath, labelPath, labelOutputPath, null, null, strategy);
    }

    public FileTransformer(String triplePath,
                           String tripleOutputPath,
                           String labelPath,
                           String labelOutputPath,
                           String literalPath,
                           String literalOutputPath,
                           TransformStrategy strategy) throws InvalidPathException {
        this.tripleInputPath = triplePath;
        this.tripleOutputPath = tripleOutputPath;
        this.labelInputPath = labelPath;
        this.labelOutputPath = labelOutputPath;
        this.literalInputPath = literalPath;
        this.literalOutputPath = literalOutputPath;
        this.strategy = strategy;
    }

    private InputStream openInputStream(String pathStr) throws IOException {
        if (pathStr == null) {
            throw new InvalidPathException("", "Path is empty");
        }
        var path = Paths.get(pathStr);
        if (!Files.exists(path)) {
            throw new FileNotFoundException();
        }
        return Files.newInputStream(path);
    }

    private OutputStream openOutputStream(String pathStr) throws IOException {
        if (pathStr == null) {
            throw new InvalidPathException("", "Path is empty");
        }
        var path = Paths.get(pathStr);
        return Files.newOutputStream(path);
    }

    @Override
    public void transform() throws IOException {
        strategy.transform(this);
    }

    @Override
    public InputStream getTripleInputStream() throws IOException {
        return openInputStream(tripleInputPath);
    }

    @Override
    public OutputStream getTripleOutputStream() throws IOException {
        return openOutputStream(tripleOutputPath);
    }

    @Override
    public InputStream getLabelInputStream() throws IOException {
        return openInputStream(labelInputPath);
    }

    @Override
    public OutputStream getLabelOutputStream() throws IOException {
        return openOutputStream(labelOutputPath);
    }

    @Override
    public InputStream getLiteralInputStream() throws IOException {
        return openInputStream(literalInputPath);
    }

    @Override
    public OutputStream getLiteralOutputStream()  throws IOException {
        return openOutputStream(literalOutputPath);
    }

    private final String tripleInputPath;
    private final String tripleOutputPath;
    private final String labelInputPath;
    private final String labelOutputPath;
    private final String literalInputPath;
    private final String literalOutputPath;
    private TransformStrategy strategy;
}
