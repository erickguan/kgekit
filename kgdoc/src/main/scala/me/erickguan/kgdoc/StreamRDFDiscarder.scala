package me.erickguan.kgdoc

import org.apache.jena.graph.Triple
import org.apache.jena.rdf.model.Model
import org.apache.jena.riot.system.StreamRDF
import org.apache.jena.sparql.core.Quad
import org.apache.jena.riot.writer.WriterStreamRDFPlain


class StreamRDFDiscarder(other: StreamRDF, predicates: Set[String]) extends StreamRDF {
  def this(other: StreamRDF, predicates: java.util.List[String]) {
    this(other, predicates)
  }

  override def base(baseStr: String): Unit = {
    other.base(baseStr)
  }

  override def finish(): Unit = {
    other.finish()
  }

  override def prefix(prefix: String, iri: String): Unit = {
    other.prefix(prefix, iri)
  }

  override def quad(quadRecord: Quad): Unit = {
    other.quad(quadRecord)
  }

  override def start(): Unit = {
    other.start()
  }

  override def triple(tripleRecord: Triple): Unit = {
    val p = tripleRecord.getPredicate
    if (p.toString.indexOf("http://www.w3.org/2004/02/skos/core#prefLabel") != -1 ||
        p.toString.indexOf("http://www.w3.org/2000/01/rdf-schema#label") != -1) {
      val s = tripleRecord.getSubject
//      print(s.isURI, s.isVariable, s.isLiteral)
//      System.exit(1)
      other.triple(tripleRecord)
    }
  }
}
