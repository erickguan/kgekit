package me.erickguan.kgdoc.tasks

import com.typesafe.config.Config

object ExtractLabel {
  def run(config: Config): Unit = {
    print(config)
  }
}
