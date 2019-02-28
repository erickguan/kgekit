package me.erickguan.kgdoc.tasks


import java.io.{FileWriter, OutputStreamWriter, StringWriter}

import com.typesafe.config.Config
import me.erickguan.kgdoc._
import org.apache.jena.atlas.io.{BufferingWriter, Writer2}
import org.apache.jena.atlas.lib.ProgressMonitor
import org.apache.jena.riot.process.StreamRDFApplyObject
import org.apache.jena.riot.system.{ProgressStreamRDF, StreamRDF2}
import org.apache.jena.riot.{RDFDataMgr, RDFLanguages, RDFParser}
import org.slf4j.{Logger, LoggerFactory}

import scala.collection.mutable.ListBuffer


object ExtractLabel {
  def run(config: Config): Unit = {
    val prefix = FileResolver.getFilePrefixFromConfig(config)
    val docs = config.getStringList("labelDocs")
    val langStr = config.getString("rdfLang")
    val outFilename = config.getString("outFilename")
    val lang = RDFLanguages.nameToLang(langStr)
    val in = FileResolver.getInputStreamFromFiles(docs, prefix)
    val out = FileResolver.getOutputStreamFromFilename(outFilename, prefix)
    val logger = LoggerFactory.getLogger(ExtractLabel.getClass)
    val progressMonitor = ProgressMonitor.create(logger, "Discarder", 100, 1000)
    progressMonitor.start()
    val sink = new ProgressStreamRDF(
      new StreamRDFDiscarder(
        new StreamRDFWriterBareLabel(
          Writer2.wrap(new BufferingWriter(new OutputStreamWriter(out))),
        )),
      progressMonitor
    )
    RDFParser.source(in).lang(lang).parse(sink)
    progressMonitor.finish()
  }
}
