package me.erickguan.kgdoc.tasks

import java.io.OutputStreamWriter

import com.typesafe.config.Config
import me.erickguan.kgdoc._
import org.apache.jena.atlas.io.{BufferingWriter, Writer2}
import org.apache.jena.atlas.lib.ProgressMonitor
import org.apache.jena.riot.system.ProgressStreamRDF
import org.apache.jena.riot.{RDFLanguages, RDFParser}
import org.apache.jena.sys.JenaSystem
import org.slf4j.LoggerFactory

object ExtractPredicates {
  def run(config: Config): Unit = {
    val prefix = FileResolver.getFilePrefixFromConfig(config)
    val docs = config.getStringList("rdfDocs")
    val langStr = config.getString("rdfLang")
    val outFilename = config.getString("outFilename")
    val literalLangAllowed = config.getStringList("literalLangAllowed")
    val predicates = config.getStringList("predicates")

    JenaSystem.init()
    val lang = RDFLanguages.nameToLang(langStr)
    val in = FileResolver.getInputStreamFromFiles(docs, prefix)
    val out = FileResolver.getOutputStreamFromFilename(outFilename, prefix)
    val logger = LoggerFactory.getLogger(ExtractPredicates.getClass)
    val progressMonitor = ProgressMonitor.create(logger, "Discarder", 100, 1000)
    progressMonitor.start()
    val sink = new ProgressStreamRDF(
      new StreamRDFPredicatesDiscarder(
        new StreamRDFWriterBareLabel(
          StreamRDFWriterBareLabel.getLiteralLangAllowedSet(literalLangAllowed),
          Writer2.wrap(new BufferingWriter(new OutputStreamWriter(out))),
        ),
        predicates,
      ),
      progressMonitor
    )
    RDFParser.source(in).lang(lang).parse(sink)
    progressMonitor.finish()
  }
}
