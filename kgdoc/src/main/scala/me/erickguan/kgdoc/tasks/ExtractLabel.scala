package me.erickguan.kgdoc.tasks


import com.typesafe.config.Config
import me.erickguan.kgdoc.{StreamRDFDiscarder, FileResolver}

import collection.JavaConverters._

object ExtractLabel {
  def run(config: Config): Unit = {
    val prefix = FileResolver.getFilePrefixFromConfig(config)
    val docs = config.getStringList("labelDocs").asScala
    val model = FileResolver.getModelFromFile(docs, prefix)
    val filters = new StreamRDFDiscarder(model)
    print(filters.onlyLabels)
    // build a T
    // build a C
  }
}
