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

class StreamRDFWriterBareLabel(literalLangAllowed: Set[String], out: AWriter, nodeFmt: NodeFormatter) extends StreamRDF {
  def this(literalLangAllowed: Set[String], w: AWriter, charSpace: CharSpace) {
    this(literalLangAllowed, w, new NodeFormatterBare(charSpace))
  }
  /**
    * Output tuples, using UTF8 output See {@link StreamRDFLib#writer} for
    * ways to create a AWriter object.
    */
  def this(literalLangAllowed: Set[String], w: AWriter) {
    this(literalLangAllowed, w, CharSpace.UTF8)
  }


  override def start(): Unit = {
  }

  override def finish(): Unit = {
    IO.flush(out)
  }

  override def triple(triple: Triple): Unit = {
    val s = triple.getSubject
    val o = triple.getObject
    if (!literalLangAllowed.contains(o.getLiteralLanguage)) {
      return
    }
    try {
      format(s)
      out.print("\t")

      format(o)
      out.print("\n")
    } catch {
      // bad line, we can filter this later
      case e: Exception => out.print(s"=====$e=====\n\n")
    }
  }

  override def quad(quad: Quad): Unit = {
    val s = quad.getSubject
    val p = quad.getPredicate
    val o = quad.getObject
    val g = quad.getGraph
    format(s)
    out.print("\t")
    format(p)
    out.print("\t")
    format(o)
    if (outputGraphSlot(g)) {
      out.print("\t")
      format(g)
    }
    out.print("\n")
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

object StreamRDFWriterBareLabel {
  def getLiteralLangAllowedSet(literalLangAllowed: java.util.List[String]): Set[String] = {
    import scala.collection.JavaConverters._

    literalLangAllowed.asScala.toSet
  }
}
