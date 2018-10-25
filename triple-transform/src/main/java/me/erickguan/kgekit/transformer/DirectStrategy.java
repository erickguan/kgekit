package me.erickguan.kgekit.transformer;

import org.eclipse.rdf4j.model.Model;
import org.eclipse.rdf4j.model.Statement;
import org.eclipse.rdf4j.model.Value;
import org.eclipse.rdf4j.model.impl.LinkedHashModel;

import java.io.IOException;
import java.util.HashSet;

public class DirectStrategy extends TransformStrategy {
    @Override
    public void transform(Transformer transformer) throws IOException {
        Model labelModel = getModel(transformer.getLabelInputStream());
        writeTriples(transformer.getLabelOutputStream(), labelModel);
    }
}

