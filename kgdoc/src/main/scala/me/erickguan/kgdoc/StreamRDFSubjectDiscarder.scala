package me.erickguan.kgdoc

import org.apache.jena.graph.Triple
import org.apache.jena.riot.system.{StreamRDF, StreamRDFWrapper}

import scala.collection.JavaConverters._


class StreamRDFSubjectDiscarder(other: StreamRDF, functions: Function[Node, Node] String]) extends StreamRDFWrapper(other) {
  def this(other: StreamRDF, predicates: java.util.List[String]) {
    this(other, predicates.asScala.toSet)
  }

  override def triple(tripleRecord: Triple): Unit = {
    val s = tripleRecord.getSubject
    if (predicates.contains(p.toString)) {
      val s = tripleRecord.getSubject
//      print(s.isURI, s.isVariable, s.isLiteral)
//      System.exit(1)
      super.triple(tripleRecord)
    }
  }
}

private val = null

def this (other: StreamRDF, function: Function[Node, Node] ) {
  this ()
  super (other)
  this.function = function
}

  override def triple (triple: Triple): Unit = {
  val obj: Node = triple.getObject
  val obj2: Node = function.apply (obj)
  if (obj ne obj2) {
  triple = Triple.create (triple.getSubject, triple.getPredicate, obj2)
l}
  super.triple (triple)
}

  override def quad (quad: Quad): Unit = {
  val obj: Node = quad.getObject
  val obj2: Node = function.apply (obj)
  if (obj ne obj2) {
  quad = Quad.create (quad.getGraph, quad.getSubject, quad.getPredicate, obj2)
}
  super.quad (quad)
}
}
