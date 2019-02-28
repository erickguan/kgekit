package me.erickguan.kgdoc

import org.apache.jena.atlas.io.{AWriter, IO}
import org.apache.jena.atlas.lib.CharSpace
import org.apache.jena.graph.{Node, Triple}
import org.apache.jena.riot.out.NodeFormatter
import org.apache.jena.riot.system.StreamRDF
import org.apache.jena.sparql.core.Quad

import scala.collection.mutable
import scala.collection.JavaConverters._

class StreamRDFLanguageCollector(predicates: Set[String]) extends StreamRDF {
  var literalLanguageSet: mutable.Set[String] = mutable.Set()

  def this(p: java.util.List[String]) {

    this(p.asScala.toSet)
  }

  override def start(): Unit = {
    literalLanguageSet = mutable.Set()
  }

  override def finish(): Unit = {
    print("=====language===== ")
    literalLanguageSet.toList.sorted.foreach(f => print(s"$f "))
    print("\n")
  }

  override def triple(triple: Triple): Unit = {
    val p = triple.getPredicate
    val o = triple.getObject
    if (!predicates.contains(p.toString)) {
      return
    }
    literalLanguageSet += o.getLiteralLanguage
  }

  override def quad(quad: Quad): Unit = {
    val s = quad.getSubject
    val p = quad.getPredicate
    val o = quad.getObject
    val g = quad.getGraph
  }

  override def base(base: String): Unit = {
  }

  override def prefix(prefix: String, iri: String): Unit = {
  }
}


