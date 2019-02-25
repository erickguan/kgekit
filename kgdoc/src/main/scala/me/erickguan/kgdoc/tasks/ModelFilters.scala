package me.erickguan.kgdoc.tasks

import org.apache.jena.rdf.model.Model

class ModelFilters(model: Model) {
  def onlyLabels: Model = {
    val iter = model.listStatements

    // print out the predicate, subject and object of each statement
    while ( {
      iter.hasNext
    }) {
      val stmt = iter.nextStatement
      // get next statement
      val subject = stmt.getSubject
      // get the subject
      val predicate = stmt.getPredicate
      // get the predicate
      val `object` = stmt.getObject // get the object
      System.out.print(subject.toString)
      System.out.print(" " + predicate.toString + " ")
      System.out.print(" \"" + `object`.toString + "\"")
      System.out.println(" .")
    }
    model
  }

  def triples: Model = {
    model
  }
}
