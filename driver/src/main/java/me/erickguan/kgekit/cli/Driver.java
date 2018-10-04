package me.erickguan.kgekit.cli;

import me.erickguan.kgekit.transformer.CountingTransformer;
import me.erickguan.kgekit.transformer.FileTransfromer;
import org.apache.commons.cli.*;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class Driver {
    public static void main(String args[]) {
        CommandLineParser parser = new DefaultParser();
        Options options = new Options();
        options.addOption("action", true, "describe action");
        options.addOption("triple_file", true, "path to ttl triple file");
        options.addOption("triple_output", true, "path to triple output file");
        HelpFormatter formatter = new HelpFormatter();
        try {
            CommandLine cmd = parser.parse(options, args);
            String action = cmd.getOptionValue("action");
            if (action != null) {
                if (action.equals("extractTriple")) {
                    extractTriple(cmd.getOptionValue("triple_file"),
                                  cmd.getOptionValue("triple_output"));
                } else {
                    formatter.printHelp("kgekit", options);
                }
            } else {
                formatter.printHelp("kgekit", options);
            }
        } catch (ParseException e) {
            System.err.println("Parsing failed. Reason: " + e.getMessage());
        }
    }

    static private void extractTriple(String inFilepath, String outFilepath) {
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
