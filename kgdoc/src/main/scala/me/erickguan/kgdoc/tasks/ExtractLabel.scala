package me.erickguan.kgdoc.tasks


import com.typesafe.config.Config
import collection.JavaConverters._

object ExtractLabel {
  def run(config: Config): Unit = {
    val prefix = FileResolver.getFilePrefixFromConfig(config)
    val docs = config.getStringList("labelDocs").asScala
    val model = FileResolver.getModelFromFile(docs, prefix)
    print(model)
    // build a T
    // build a C
  }
}
