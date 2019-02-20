package me.erickguan.kgdoc.tasks

import java.nio.file.Paths

import com.typesafe.config.Config
import org.apache.jena.rdf.model.{Model, ModelFactory}

import scala.collection.mutable

object FileResolver {
  val PrefixKey = "basePrefix"

  def getFilePrefixFromConfig(config: Config): String = {
    if (config.hasPath(PrefixKey)) {
      config.getString(PrefixKey)
    } else {
      System.getProperty("user.dir")
    }
  }

  def getModelFromFile(docs: mutable.Buffer[String], prefix: String): Model = {
    val model = ModelFactory.createDefaultModel()
    for (docName <- docs) {
      val path = Paths.get(prefix, docName)
      val newModel = ModelFactory.createDefaultModel()

      newModel.read(path.toString)
      model.add(newModel)
    }
    model
  }

}
