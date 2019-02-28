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
    w.write(n.getLocalName)
  }
  override def formatBNode(w: AWriter, label: String): Unit = w.write(label)

  override def formatURI(w: AWriter, n: Node): Unit = {
    formatURI(w, n.getLocalName)
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
