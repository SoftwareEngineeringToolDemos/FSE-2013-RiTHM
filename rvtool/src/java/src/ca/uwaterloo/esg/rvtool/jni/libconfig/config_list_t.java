/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.8
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package ca.uwaterloo.esg.rvtool.jni.libconfig;

public class config_list_t {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected config_list_t(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(config_list_t obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        libconfigJNI.delete_config_list_t(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setLength(long value) {
    libconfigJNI.config_list_t_length_set(swigCPtr, this, value);
  }

  public long getLength() {
    return libconfigJNI.config_list_t_length_get(swigCPtr, this);
  }

  public void setElements(SWIGTYPE_p_p_config_setting_t value) {
    libconfigJNI.config_list_t_elements_set(swigCPtr, this, SWIGTYPE_p_p_config_setting_t.getCPtr(value));
  }

  public SWIGTYPE_p_p_config_setting_t getElements() {
    long cPtr = libconfigJNI.config_list_t_elements_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_p_config_setting_t(cPtr, false);
  }

  public config_list_t() {
    this(libconfigJNI.new_config_list_t(), true);
  }

}