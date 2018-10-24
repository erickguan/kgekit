package me.erickguan.kgekit.transformer;

import org.eclipse.rdf4j.model.*;
import org.eclipse.rdf4j.model.Value;
import org.eclipse.rdf4j.model.impl.LinkedHashModel;
import org.eclipse.rdf4j.model.impl.SimpleValueFactory;
import org.eclipse.rdf4j.model.util.Models;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Array;
import java.util.*;
import java.util.stream.Collectors;
import java.util.stream.Stream;


public class CountingTripleStrategy extends TransformStrategy {
    /*
     * Graph is directed without loop. We need to calculate the sum of indegree and outdegree of
     * all vertices.
     */
    class DirectedGraph {
        class Node {
            Node() {
                incomingRels = new LinkedList<>();
                outgoingRels = new LinkedList<>();
            }
            LinkedList<AbstractMap.SimpleImmutableEntry<Integer, IRI>> incomingRels;
            LinkedList<AbstractMap.SimpleImmutableEntry<Integer, IRI>> outgoingRels;
        }
        /*
         * A mapping system from Id to Resource+Value.
         */
        private Node[] graph;
        private Resource[] resourceMap;
        private Value[] valueMap;
        private HashMap<String, Integer> valueIds;
        private boolean searched = false;
        private boolean plucked[];

        DirectedGraph(Model model) {
            var objects = model.objects();
            var subjects = model.subjects();
            var size = Stream.concat(objects.stream(), subjects.stream())
                    .map(Object::toString)
                    .collect(Collectors.toUnmodifiableSet()).size();
            valueIds = new HashMap<>(size);
            resourceMap = new Resource[size];
            valueMap = new Value[size];
            var i = 0;
            for (var e : objects) {
                var str = e.toString();
                if (valueIds.get(str) == null) {
                    valueMap[i] = e;
                    valueIds.put(str, i++);
                } else {
                    valueMap[valueIds.get(str)] = e;
                }
            }
            for (var e : subjects) {
                var str = e.toString();
                if (valueIds.get(str) == null) {
                    resourceMap[i] = e;
                    valueIds.put(str, i++);
                } else {
                    resourceMap[valueIds.get(str)] = e;
                }
            }
            graph = new Node[size];
            for (var j = 0; j < graph.length; ++j) {
                graph[j] = new Node();
            }
            plucked = new boolean[size];

            for (var stmt : model) {
                var outNode = graph[valueIds.get(stmt.getSubject().toString())];
                outNode.outgoingRels.add(new AbstractMap.SimpleImmutableEntry<>(valueIds.get(stmt.getObject().toString()), stmt.getPredicate()));
                var inNode = graph[valueIds.get(stmt.getObject().toString())];
                inNode.incomingRels.add(new AbstractMap.SimpleImmutableEntry<>(valueIds.get(stmt.getSubject().toString()), stmt.getPredicate())) ;
            }
        }

        /*
         * BFS search
         */
        void countValue(int numCount) {
            int[] weight = new int[graph.length];
            boolean[] visited = new boolean[graph.length];
            Queue<Integer> queue = new LinkedList<>();
            for (int i = 0; i < graph.length; ++i) {
                weight[i] = graph[i].incomingRels.size() + graph[i].outgoingRels.size();
                if (weight[i] < numCount) {
                    queue.add(i);
                }
            }

            while (queue.size() != 0) {
                var t = queue.poll();
                if (visited[t]) {
                    continue;
                }
                visited[t] = true;

                for (var v: graph[t].outgoingRels) {
                    var obj = v.getKey();
                    if (!visited[obj]) {
                        queue.add(obj);
                    }

                    if (weight[t] < numCount) {
                        weight[obj]--;
                    }
                }
                for (var v: graph[t].incomingRels) {
                    var obj = v.getKey();
                    if (!visited[obj]) {
                        queue.add(obj);
                    }

                    if (weight[t] < numCount) {
                        weight[obj]--;
                    }
                }
            }
            for (var i = 0; i < graph.length; ++i) {
                assert visited[i];
                if (weight[i] >= numCount) {
                    plucked[i] = true;
                }
            }
        }

        HashSet<Resource> getResources() {
            if (!searched) {
                countValue(numberOfLink);
            }
            var res = new HashSet<Resource>();
            for (var i = 0; i < graph.length; ++i) {
                if (plucked[i] && resourceMap[i] != null) {
                    res.add(resourceMap[i]);
                }
            }
            return res;
        }

        HashSet<Value> getValues() {
            if (!searched) {
                countValue(numberOfLink);
            }
            var res = new HashSet<Value>();
            for (var i = 0; i < graph.length; ++i) {
                if (plucked[i] && valueMap[i] != null) {
                    res.add(valueMap[i]);
                }
            }
            return res;
        }
    }

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
        return gatherStatement(model);
    }

    private Model gatherStatement(Model model) {
        var g = new DirectedGraph(model);
        return getCountStatements(model, g.getResources(), g.getValues());
    }

    private Model getCountStatements(Model model, Set<Resource> resources, Set<Value> values) {
        var res = new LinkedHashModel();
        for (var subj : resources) {
            Model entities = model.filter(subj, null, null);
            for (var ent : entities) {
                if (values.contains(ent.getObject())) {
                    res.add(ent);
                }
            }
        }
        for (var obj : values) {
            Model entities = model.filter(null, null, obj);
            for (var ent : entities) {
                if (resources.contains(ent.getSubject())) {
                    res.add(ent);
                }
            }
        }
        return res;
    }

    private int numberOfLink;
}
