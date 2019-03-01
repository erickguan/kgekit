package me.erickguan.kgdoc

import org.apache.jena.graph.Triple
import org.apache.jena.riot.system.{StreamRDF, StreamRDFWrapper}

import scala.collection.JavaConverters._


class StreamRDFEntityPairReverser(other: StreamRDF, predicates: Set[String]) extends StreamRDFWrapper(other) {
  def this(other: StreamRDF, predicates: java.util.List[String]) {
    this(other, predicates.asScala.toSet)
  }

  override def triple(tripleRecord: Triple): Unit = {
    super.triple(Triple.create(tripleRecord.getObject, tripleRecord.getPredicate, tripleRecord.getSubject))
  }
}
