/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.8
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package ca.uwaterloo.esg.rvtool.jni.libconfig;

public class config_t {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected config_t(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(config_t obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        libconfigJNI.delete_config_t(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setRoot(config_setting_t value) {
    libconfigJNI.config_t_root_set(swigCPtr, this, config_setting_t.getCPtr(value), value);
  }

  public config_setting_t getRoot() {
    long cPtr = libconfigJNI.config_t_root_get(swigCPtr, this);
    return (cPtr == 0) ? null : new config_setting_t(cPtr, false);
  }

  public void setDestructor(SWIGTYPE_p_f_p_void__void value) {
    libconfigJNI.config_t_destructor_set(swigCPtr, this, SWIGTYPE_p_f_p_void__void.getCPtr(value));
  }

  public SWIGTYPE_p_f_p_void__void getDestructor() {
    long cPtr = libconfigJNI.config_t_destructor_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_f_p_void__void(cPtr, false);
  }

  public void setFlags(int value) {
    libconfigJNI.config_t_flags_set(swigCPtr, this, value);
  }

  public int getFlags() {
    return libconfigJNI.config_t_flags_get(swigCPtr, this);
  }

  public void setTab_width(int value) {
    libconfigJNI.config_t_tab_width_set(swigCPtr, this, value);
  }

  public int getTab_width() {
    return libconfigJNI.config_t_tab_width_get(swigCPtr, this);
  }

  public void setDefault_format(short value) {
    libconfigJNI.config_t_default_format_set(swigCPtr, this, value);
  }

  public short getDefault_format() {
    return libconfigJNI.config_t_default_format_get(swigCPtr, this);
  }

  public void setInclude_dir(String value) {
    libconfigJNI.config_t_include_dir_set(swigCPtr, this, value);
  }

  public String getInclude_dir() {
    return libconfigJNI.config_t_include_dir_get(swigCPtr, this);
  }

  public void setError_text(String value) {
    libconfigJNI.config_t_error_text_set(swigCPtr, this, value);
  }

  public String getError_text() {
    return libconfigJNI.config_t_error_text_get(swigCPtr, this);
  }

  public void setError_file(String value) {
    libconfigJNI.config_t_error_file_set(swigCPtr, this, value);
  }

  public String getError_file() {
    return libconfigJNI.config_t_error_file_get(swigCPtr, this);
  }

  public void setError_line(int value) {
    libconfigJNI.config_t_error_line_set(swigCPtr, this, value);
  }

  public int getError_line() {
    return libconfigJNI.config_t_error_line_get(swigCPtr, this);
  }

  public void setError_type(config_error_t value) {
    libconfigJNI.config_t_error_type_set(swigCPtr, this, value.swigValue());
  }

  public config_error_t getError_type() {
    return config_error_t.swigToEnum(libconfigJNI.config_t_error_type_get(swigCPtr, this));
  }

  public void setFilenames(String[] value) {
    libconfigJNI.config_t_filenames_set(swigCPtr, this, value);
  }

  public String[] getFilenames() {
//    long cPtr = libconfigJNI.config_t_filenames_get(swigCPtr, this);
//    return (cPtr == 0) ? null : new SWIGTYPE_p_p_char(cPtr, false);
	  return null;
  }

  public void setNum_filenames(long value) {
    libconfigJNI.config_t_num_filenames_set(swigCPtr, this, value);
  }

  public long getNum_filenames() {
    return libconfigJNI.config_t_num_filenames_get(swigCPtr, this);
  }

  public config_t() {
    this(libconfigJNI.new_config_t(), true);
  }

}