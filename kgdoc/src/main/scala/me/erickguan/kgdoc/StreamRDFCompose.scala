package me.erickguan.kgdoc

import org.apache.jena.riot.system.StreamRDF
import org.apache.jena.sparql.core.Quad
import org.apache.jena.graph.Triple

import scala.collection.mutable.ListBuffer

class StreamRDFCompose(streamRDFs: ListBuffer[StreamRDF]) extends StreamRDF {
//  private val streamRDFs = ListBuffer[StreamRDF]()

  def addStreamRDF(streamRDF: StreamRDF): Unit = {
    streamRDFs.append(streamRDF)
  }

  override def base(baseStr: String): Unit = {
    streamRDFs.head.base(baseStr)
  }

  override def finish(): Unit = {
    streamRDFs.foreach(s => s.finish())
  }

  override def prefix(prefix: String, iri: String): Unit = {
    streamRDFs.head.prefix(prefix, iri)
  }

  override def quad(quadRecord: Quad): Unit = {
    streamRDFs.foreach(s => s.quad(quadRecord))
  }

  override def start(): Unit = {
    streamRDFs.foreach(s => s.start())
  }

  override def triple(tripleRecord: Triple): Unit = {
    streamRDFs.foreach(s => s.triple(tripleRecord))
  }
}
