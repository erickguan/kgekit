package me.erickguan.kgdoc

import com.typesafe.config.ConfigFactory

object TaskDispatch extends App {
  val conf = ConfigFactory.load().getConfig("kgdoc")
  val task = conf.getString("task")
  val taskConf = conf.getConfig(task)
  print(taskConf.getString("foo"))
}

