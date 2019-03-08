package me.erickguan.kgdoc

import org.apache.jena.graph.Triple
import org.apache.jena.graph.Node
import org.apache.jena.riot.system.{StreamRDF, StreamRDFWrapper}

import scala.collection.JavaConverters._


class StreamRDFSubjectDiscarder(other: StreamRDF, function: Function[Node, Boolean])
  extends StreamRDFWrapper(other) {
  override def triple(triple: Triple): Unit = {
    val obj: Node = triple.getObject
    if (!function.apply(obj)) {
      super.triple(triple)
    }
  }
}
