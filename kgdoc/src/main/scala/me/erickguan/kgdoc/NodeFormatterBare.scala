package me.erickguan.kgdoc

import org.apache.jena.atlas.io.AWriter
import org.apache.jena.atlas.lib.CharSpace
import org.apache.jena.graph.Node
import org.apache.jena.riot.out.NodeFormatterBase

class NodeFormatterBare extends NodeFormatterBase {
  override def formatVar(w: AWriter, n: String): Unit = {
    w.write(n)
  }

  def this(charSpace: CharSpace) {
    this()
  }

  override def formatLitString(w: AWriter, lex: String): Unit = w.write(lex)

  override def formatBNode(w: AWriter, n: Node): Unit = {
    try {
      val str = n.toString
      if (str.indexOf(NodeFormatterBare.YAGO_PREFIX) != -1) {
        formatBNode(w, str.substring(NodeFormatterBare.YAGO_PREFIX.length))
      } else {
        formatBNode(w, n.getLocalName)
      }
    } catch {
      case e: Exception => formatBNode(w, n.toString)
    }
  }

  override def formatBNode(w: AWriter, label: String): Unit = w.write(label)

  override def formatURI(w: AWriter, n: Node): Unit = {
    try {
      val str = n.toString
      if (str.indexOf(NodeFormatterBare.YAGO_PREFIX) != -1) {
        formatURI(w, str.substring(NodeFormatterBare.YAGO_PREFIX.length))
      } else {
        formatURI(w, n.getLocalName)
      }
    } catch {
      case e: Exception => formatURI(w, n.toString)
    }
  }
  override def formatURI(w: AWriter, uriStr: String): Unit = w.write(uriStr)

  override def formatLitDT(w: AWriter, lex: String, datatypeURI: String): Unit = {
    w.write(lex)
    w.write(datatypeURI)
  }

  override def formatLitLang(w: AWriter, lex: String, langTag: String): Unit = {
    w.write(lex)
    w.write("@")
    w.write(langTag)
  }
}

object NodeFormatterBare {
  val YAGO_PREFIX = "http://yago-knowledge.org/resource/"
}