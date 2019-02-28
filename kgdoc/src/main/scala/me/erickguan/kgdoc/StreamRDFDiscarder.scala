package me.erickguan.kgdoc

import org.apache.jena.graph.Triple
import org.apache.jena.rdf.model.Model
import org.apache.jena.riot.system.StreamRDF
import org.apache.jena.sparql.core.Quad
import org.apache.jena.riot.writer.WriterStreamRDFPlain


class StreamRDFDiscarder(other: StreamRDF) extends StreamRDF {
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
    if (tripleRecord.getPredicate.toString.indexOf("http://www.w3.org/2004/02/skos/core#prefLabel") != -1) {
      val s = tripleRecord.getSubject
//      print(s.isURI, s.isVariable, s.isLiteral)
//      System.exit(1)
      other.triple(tripleRecord)
    }
  }
}
