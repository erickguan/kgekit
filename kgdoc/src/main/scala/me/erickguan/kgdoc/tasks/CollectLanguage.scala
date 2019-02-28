package me.erickguan.kgdoc.tasks

import com.typesafe.config.Config
import me.erickguan.kgdoc.{
  FileResolver,
  StreamRDFLanguageCollector,
}
import org.apache.jena.atlas.lib.ProgressMonitor
import org.apache.jena.riot.{RDFLanguages, RDFParser}
import org.apache.jena.riot.system.ProgressStreamRDF
import org.apache.jena.sys.JenaSystem
import org.slf4j.LoggerFactory

object CollectLanguage {
  def run(config: Config): Unit = {
    val prefix = FileResolver.getFilePrefixFromConfig(config)
    val docs = config.getStringList("labelDocs")
    val langStr = config.getString("rdfLang")
    val predicates = config.getStringList("predicates")

    JenaSystem.init()
    val lang = RDFLanguages.nameToLang(langStr)
    val in = FileResolver.getInputStreamFromFiles(docs, prefix)
    val logger = LoggerFactory.getLogger(ExtractPredicates.getClass)
    val progressMonitor =
      ProgressMonitor.create(logger, "LanguageCollector", 100, 1000)
    progressMonitor.start()
    val sink = new ProgressStreamRDF(
      new StreamRDFLanguageCollector(predicates),
      progressMonitor
    )
    RDFParser.source(in).lang(lang).parse(sink)
    progressMonitor.finish()
  }
}
