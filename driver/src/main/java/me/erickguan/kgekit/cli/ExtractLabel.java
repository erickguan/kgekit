package me.erickguan.kgekit.cli;

import me.erickguan.kgekit.transformer.FileTransformer;
import me.erickguan.kgekit.transformer.MatchingStrategy;
import picocli.CommandLine.Command;
import picocli.CommandLine.Option;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

@Command(name="extractLabel", description="Extract labels from files.")
public class ExtractLabel implements Runnable {

    @Option(names = "--triple_file", required = true, description = "triple file location")
    String tripleFilePath;

    @Option(names = "--label_file", required = true, description = "label file location")
    String labelFilePath;

    @Option(names = "--output_file", required = true, description = "output file location")
    String labelOutputFilePath;

    public void run() {
        Path path = Paths.get(tripleFilePath);
        if (!Files.exists(path)) {
            System.err.println("Can't find triple file at " + tripleFilePath);
        }
        path = Paths.get(labelFilePath);
        if (!Files.exists(path)) {
            System.err.println("Can't find label file at " + labelFilePath);
        }
        try {
            FileTransformer transfromer = new FileTransformer(tripleFilePath,
                                                              null,
                                                              labelFilePath,
                                                              labelOutputFilePath,
                                                              null,
                                                              null,
                                                              new MatchingStrategy());
            transfromer.transform();
        } catch (IOException e) {
            System.err.println("Failed to transform." + e.getMessage());
        }
    }
}
