package me.erickguan.kgdoc

import java.io._
import java.nio.file.Paths

import com.typesafe.config.Config

object FileResolver {
  val PrefixKey = "basePrefix"

  def getFilePrefixFromConfig(config: Config): String = {
    if (config.hasPath(PrefixKey)) {
      config.getString(PrefixKey)
    } else {
      System.getProperty("user.dir")
    }
  }

  def getOutputStreamFromFilename(filename: String, prefix: String): OutputStream = {
    new FileOutputStream(Paths.get(prefix, filename).toString)
  }

  def getInputStreamFromFiles(docs: java.util.List[String], prefix: String): InputStream = {
    import collection.JavaConverters._

    new SequenceInputStream(docs
      .asScala
      .map(d => new FileInputStream(Paths.get(prefix, d).toString))
      .iterator
      .asJavaEnumeration
    )
  }

}
