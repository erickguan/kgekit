package me.erickguan.kgekit.cli;

import picocli.CommandLine;
import picocli.CommandLine.RunLast;
import picocli.CommandLine.RunLast;
import picocli.CommandLine.Command;
import picocli.CommandLine.Option;

import java.util.List;

@Command(description = "Operates kgekit CLI.",
        name = "kgekit",
        mixinStandardHelpOptions = true,
        version = "kgekit 0.1",
        subcommands = {ExtractTriple.class})
public class Driver implements Runnable {
    public static void main(String args[]) {
        CommandLine cli = new CommandLine(new Driver());
        cli.setUnmatchedArgumentsAllowed(true);
        cli.setUnmatchedOptionsArePositionalParams(true);
        List<Object> result = cli.parseWithHandler(new RunLast(), args);
    }

    public void run() {
        System.out.println("Specify a command.");
    }

}
