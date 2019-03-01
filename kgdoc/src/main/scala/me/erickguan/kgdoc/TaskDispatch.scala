package me.erickguan.kgdoc

import me.erickguan.kgdoc.tasks._
import com.typesafe.config.ConfigFactory

object TaskDispatch extends App {
  val conf = ConfigFactory.load().getConfig("kgdoc")
  val task = conf.getString("task")
  val taskConf = conf.getConfig(task)
  task match {
    case "ExtractEntityPairByPredicates" => ExtractEntityPairByPredicates.run(taskConf)
    case "ExtractEntityPairByPredicatesYAGOLabel" => ExtractEntityPairByPredicates.run(taskConf)
    case "ExtractEntityPairByPredicatesWikidataLabel" => ExtractEntityPairByPredicates.run(taskConf)
    case "ExtractEntityPairByPredicatesWikidataArticleUrl" => ExtractEntityPairByPredicates.run(taskConf)
    case "CollectLanguage" => CollectLanguage.run(taskConf)
  }
}

