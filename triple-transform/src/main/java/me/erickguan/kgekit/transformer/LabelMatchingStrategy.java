package me.erickguan.kgekit.transformer;

import org.eclipse.rdf4j.model.Model;
import org.eclipse.rdf4j.model.Statement;
import org.eclipse.rdf4j.model.Value;
import org.eclipse.rdf4j.model.impl.LinkedHashModel;

import java.io.IOException;
import java.util.HashSet;

public class LabelMatchingStrategy extends TransformStrategy {
    @Override
    public void transform(Transformer transformer) throws IOException {
        var countingStrategy = new CountingTripleStrategy(10);
        var entityModel = countingStrategy.getStatements(transformer.getTripleInputStream());
        HashSet<Value> entities = new HashSet<>();
        entities.addAll(entityModel.subjects());
        entities.addAll(entityModel.objects());

        Model labelModel = getModel(transformer.getLabelInputStream());
        Model stmts = gatherStatement(entities, labelModel);
        writeTriples(transformer.getLabelOutputStream(), stmts);
    }

    private Model gatherStatement(HashSet<Value> entities, Model model) {
        Model stmts = new LinkedHashModel();
        for (Statement st : model) {
            if (entities.contains(st.getSubject())) {
                stmts.add(st);
            }
        }
        return stmts;
    }
}

