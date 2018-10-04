package me.erickguan.kgekit.transformer;

import org.eclipse.rdf4j.model.*;
import org.eclipse.rdf4j.model.Value;
import org.eclipse.rdf4j.model.impl.LinkedHashModel;

import java.io.IOException;
import java.util.HashMap;


public class CountingTripleStrategy extends TransformStrategy {
    public CountingTripleStrategy(int numberOfLink) {
        this.numberOfLink = numberOfLink;
    }

    @Override
    public void transform(Transformer transformer) throws IOException {
        Model model = getModel(transformer.getTripleInputStream());
        Model stmts = gatherStatement(model);
        writeTriples(transformer.getTripleOutputStream(), stmts);
    }

    private Model pullSubjectStatements(Model model, Resource value) {
        return model.filter(value, null, null);
    }

    private Model pullObjectStatements(Model model, Value value) {
        return model.filter(null, null, value);
    }

    private Model gatherStatement(Model model) {
        Model stmts = new LinkedHashModel();
        for (HashMap.Entry<Resource, Integer> subjEntry : this.getSubjects(model).entrySet()) {
            stmts.addAll(pullSubjectStatements(model, subjEntry.getKey()));
        }
        for (HashMap.Entry<Value, Integer> objEntry : this.getObjects(model).entrySet()) {
            stmts.addAll(pullObjectStatements(model, objEntry.getKey()));
        }
        return stmts;
    }

    private HashMap<Resource, Integer> getSubjects(Model model) {
        HashMap<Resource, Integer> map = new HashMap<>();
        for (Resource subject : model.subjects()) {
            Model stmts = model.filter(subject, null, null);
            if (stmts.predicates().size() <= numberOfLink) {
                continue;
            }
            for (Statement stmt : stmts) {
                map.merge(stmt.getSubject(), 1, Integer::sum);
            }
        }
        return map;
    }

    private HashMap<Value, Integer> getObjects(Model model) {
        HashMap<Value, Integer> map = new HashMap<>();
        for (Value object : model.objects()) {
            Model stmts = model.filter(null, null, object);
            if (stmts.predicates().size() <= numberOfLink) {
                continue;
            }
            for (Statement stmt : stmts) {
                map.merge(stmt.getObject(), 1, Integer::sum);
            }
        }
        return map;
    }

    private int numberOfLink;
}
