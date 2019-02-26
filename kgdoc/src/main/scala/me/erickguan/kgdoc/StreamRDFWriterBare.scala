package me.erickguan.kgdoc

import org.apache.jena.atlas.io.{AWriter, IO}
import org.apache.jena.atlas.lib.CharSpace
import org.apache.jena.graph.{Node, Triple}
import org.apache.jena.rdf.model.impl.RDFDefaultErrorHandler
import org.apache.jena.rdf.model.{Model, RDFErrorHandler, RDFWriter}
import org.apache.jena.riot.out.{NodeFormatter, NodeFormatterNT}
import org.apache.jena.riot.system.StreamRDF
import org.apache.jena.shared.UnknownPropertyException
import org.apache.jena.sparql.core.Quad

class StreamRDFWriterBare extends StreamRDF {
  // This class is the overall structure - the NodeFormatter controls the
  // appearance of the Nodes themselves.
  protected var out: AWriter = null
  protected var nodeFmt: NodeFormatter = null

  /**
    * Output tuples, choosing ASCII or UTF8 See
    * {@link StreamRDFLib#writer} for ways to create a AWriter object.
    */
  def this(w: AWriter, charSpace: CharSpace) {
    this()
    out = w
    nodeFmt = new NodeFormatterBare(charSpace)
  }
  /**
    * Output tuples, using UTF8 output See {@link StreamRDFLib#writer} for
    * ways to create a AWriter object.
    */
  def this(w: AWriter) {
    this(w, CharSpace.UTF8)
  }


  override def start(): Unit = {
  }

  override def finish(): Unit = {
    IO.flush(out)
  }

  override def triple(triple: Triple): Unit = {
    val s = triple.getSubject
    val p = triple.getPredicate
    val o = triple.getObject
    format(s)
    out.print(" ")
    format(p)
    out.print(" ")
    format(o)
    out.print(" .\n")
  }

  override def quad(quad: Quad): Unit = {
    val s = quad.getSubject
    val p = quad.getPredicate
    val o = quad.getObject
    val g = quad.getGraph
    format(s)
    out.print(" ")
    format(p)
    out.print(" ")
    format(o)
    if (outputGraphSlot(g)) {
      out.print(" ")
      format(g)
    }
    out.print(" .\n")
  }

  private def format(n: Node): Unit = {
    nodeFmt.format(out, n)
  }

  override def base(base: String): Unit = {
  }

  override def prefix(prefix: String, iri: String): Unit = {
  }

  private def outputGraphSlot(g: Node) = g != null && (g ne Quad.tripleInQuad) && !(Quad.isDefaultGraph(g))
}
