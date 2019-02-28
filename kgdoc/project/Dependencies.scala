import sbt._

object Dependencies {
  lazy val jenaLibs = "org.apache.jena" % "apache-jena-libs" % "3.10.0"
  lazy val confLib = "com.typesafe" % "config" % "1.3.2"
  lazy val loggerLib = "org.slf4j" % "slf4j-log4j12" % "1.7.26"
  lazy val scalaTest = "org.scalatest" %% "scalatest" % "3.0.5"
}
