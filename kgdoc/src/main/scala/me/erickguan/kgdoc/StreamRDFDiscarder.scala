package me.erickguan.kgdoc

import org.apache.jena.graph.Triple
import org.apache.jena.rdf.model.Model
import org.apache.jena.riot.system.StreamRDF
import org.apache.jena.sparql.core.Quad
import org.apache.jena.riot.writer.WriterStreamRDFPlain


class StreamRDFDiscarder extends StreamRDF {
  override def base(baseStr: String): Unit = {
  }

  override def finish(): Unit = {
  }

  override def prefix(prefix: String, iri: String): Unit = {
  }

  override def quad(quadRecord: Quad): Unit = {
  }

  override def start(): Unit = {
  }

  override def triple(tripleRecord: Triple): Unit = {
    print(tripleRecord)
  }
}
