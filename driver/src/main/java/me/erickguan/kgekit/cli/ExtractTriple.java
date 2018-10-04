package me.erickguan.kgekit.cli;

import me.erickguan.kgekit.transformer.CountingTransformer;
import me.erickguan.kgekit.transformer.FileTransfromer;
import picocli.CommandLine.Option;
import picocli.CommandLine.Command;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

@Command(name="extractTriple", description="Extract triples from files.")
public class ExtractTriple implements Runnable {

    @Option(names = "--triple_file", required = true, description = "triple file location")
    String inFilepath;

    @Option(names = "--output_file", required = true, description = "output file location")
    String outFilepath;

    public void run() {
        Path path = Paths.get(inFilepath);
        if (!Files.exists(path)) {
            System.err.println("Can't find triple file at " + inFilepath);
        }
        FileTransfromer transfromer = new FileTransfromer(inFilepath, outFilepath, new CountingTransformer(10));
        try {
            transfromer.transform();
        } catch (IOException e) {
            System.err.println("Failed to transform.");
        }
    }
}
