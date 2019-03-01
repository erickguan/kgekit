package me.erickguan.kgdoc

import org.apache.jena.graph.Triple
import org.apache.jena.riot.system.{StreamRDF, StreamRDFWrapper}
import org.apache.jena.sparql.core.Quad

import scala.collection.JavaConverters._


class StreamRDFPredicatesDiscarder(other: StreamRDF, predicates: Set[String]) extends StreamRDFWrapper(other) {
  def this(other: StreamRDF, predicates: java.util.List[String]) {
    this(other, predicates.asScala.toSet)
  }

  override def triple(tripleRecord: Triple): Unit = {
    val p = tripleRecord.getPredicate
    if (predicates.contains(p.toString)) {
      val s = tripleRecord.getSubject
//      print(s.isURI, s.isVariable, s.isLiteral)
//      System.exit(1)
      super.triple(tripleRecord)
    }
  }
}
