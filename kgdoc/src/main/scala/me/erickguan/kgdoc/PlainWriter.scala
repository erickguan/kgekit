package me.erickguan.kgdoc

import java.io.{OutputStream, Writer}

import org.apache.jena.rdf.model.impl.RDFDefaultErrorHandler
import org.apache.jena.rdf.model.{Model, RDFErrorHandler, RDFWriter}
import org.apache.jena.shared.UnknownPropertyException

class PlainWriter extends RDFWriter {
  private var errorHandler: RDFErrorHandler = new RDFDefaultErrorHandler
  def setErrorHandler(errHandler: RDFErrorHandler): RDFErrorHandler = {
    errorHandler = errHandler
    errorHandler
  }

  def setProperty(propName: String, propValue: Object): Object = throw new UnknownPropertyException(propName)

  def write(model: Model, out: OutputStream, base: String) = {

  }

  def write(model: Model, out: Writer, base: String) = {

  }
}
