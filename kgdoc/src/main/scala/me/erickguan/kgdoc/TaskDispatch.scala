package me.erickguan.kgdoc

import me.erickguan.kgdoc.tasks._
import com.typesafe.config.ConfigFactory

object TaskDispatch extends App {
  val conf = ConfigFactory.load().getConfig("kgdoc")
  val task = conf.getString("task")
  val taskConf = conf.getConfig(task)
  task match {
    case "ExtractPredicates" => ExtractPredicates.run(taskConf)
    case "CollectLanguage" => CollectLanguage.run(taskConf)
  }
}

