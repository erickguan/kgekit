package me.erickguan.kgekit.transformer;

import org.eclipse.rdf4j.model.impl.LinkedHashModel;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.junitpioneer.jupiter.TempDirectory;
import org.junitpioneer.jupiter.TempDirectory.TempDir;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

class StrategiesTest {
    private static String fixturePrefix = "src/test/resources/fixtures";
    private static String triplePath = fixturePrefix + "/triple.ttl";
    private static String tripleLabelPath = fixturePrefix + "/triple_label.ttl";
    private static String tripleSampleOutputPath = "triple_sample_output.txt";
    private static String tripleLabelSampleOutputPath = "triple_sample_label_output.txt";
    private static String tripleLabelSampleOutput;
    private static String tripleSampleOutput;

    @BeforeAll
    static void createFileSystem() throws IOException {
        tripleSampleOutput = Files.readString(Paths.get(fixturePrefix,tripleSampleOutputPath));
        tripleLabelSampleOutput = Files.readString(Paths.get(fixturePrefix, tripleLabelSampleOutputPath));
    }

    @Test
    @ExtendWith(TempDirectory.class)
    void writeToEmptyFile(@TempDir Path tempDir) {
        var path = tempDir.resolve("empty.txt");
        assertDoesNotThrow(() -> {
            var outputStream = Files.newOutputStream(path);
            var emptyModel = new LinkedHashModel();
            var strategy = new VoidStrategy();
            strategy.writeTriples(outputStream, emptyModel);
            outputStream.close();
            assertEquals(Files.size(path), 0);
        });
    }

    @Test
    @ExtendWith(TempDirectory.class)
    public void testLabelMatchingTransfrom(@TempDir Path tempDir) throws IOException {
        var strategy = new LabelMatchingStrategy();
        var outputPath = tempDir.resolve("tripleLabelOut.txt");
        var transformer = new FileTransformer(
                triplePath,
                null,
                tripleLabelPath,
                outputPath.toString(),
                strategy);
        transformer.transform();
        assertEquals(tripleLabelSampleOutput, Files.readString(outputPath));
    }

    @Test
    @ExtendWith(TempDirectory.class)
    public void testCountingTriplesTransfrom(@TempDir Path tempDir) throws IOException {
        var strategy = new CountingTripleStrategy(10);
        var outputPath = tempDir.resolve("tripleOut.txt");
        var transformer = new FileTransformer(
                triplePath,
                outputPath.toString(),
                strategy);
        transformer.transform();
        assertEquals(tripleSampleOutput, Files.readString(outputPath));
    }
}
