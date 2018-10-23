package me.erickguan.kgekit.transformer;

import org.eclipse.rdf4j.model.*;
import org.eclipse.rdf4j.model.Value;
import org.eclipse.rdf4j.model.impl.LinkedHashModel;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.HashMap;


public class CountingTripleStrategy extends TransformStrategy {
    public CountingTripleStrategy(int numberOfLink) {
        this.numberOfLink = numberOfLink;
    }

    @Override
    public void transform(Transformer transformer) throws IOException {
        var stmts = getStatements(transformer.getTripleInputStream());
        writeTriples(transformer.getTripleOutputStream(), stmts);
    }

    public Model getStatements(InputStream stream) throws IOException {
        Model model = getModel(stream);
        var size = model.size();
        do {
            model = gatherStatement(model);
        } while (size != model.size());
        return model;
    }

    private Model pullSubjectStatements(Model model, Resource value) {
        return model.filter(value, null, null);
    }

    private Model pullObjectStatements(Model model, Value value) {
        return model.filter(null, null, value);
    }

    private Model gatherStatement(Model model) {
        Model stmts = new LinkedHashModel();
        for (Resource subjEntry : this.getSubjects(model)) {
            stmts.addAll(pullSubjectStatements(model, subjEntry));
        }
        for (Value objEntry : this.getObjects(model)) {
            stmts.addAll(pullObjectStatements(model, objEntry));
        }
        return stmts;
    }

    /*
     * Make sure a subject has 10 relations
     */
    private ArrayList<Resource> getSubjects(Model model) {
        var subjects = new ArrayList<Resource>();
        for (Resource subject : model.subjects()) {
            Model stmts = model.filter(subject, null, null);
            if (stmts.predicates().size() >= numberOfLink) {
                subjects.add(subject);
            }
        }
        return subjects;
    }

    /*
     * Make sure a object has 10 relations
     */
    private ArrayList<Value> getObjects(Model model) {
        var objects = new ArrayList<Value>();
        for (Value object : model.objects()) {
            Model stmts = model.filter(null, null, object);
            if (stmts.predicates().size() >= numberOfLink) {
                objects.add(object);
            }
        }
        return objects;
    }

    private int numberOfLink;
}
