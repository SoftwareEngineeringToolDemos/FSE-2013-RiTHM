package ca.uwaterloo.esg.rvtool.monitor;

import ca.uwaterloo.esg.rvtool.jni.libconfig.*;

public class MonitorGenerator {
	static {
		try {
			System.loadLibrary("clibconfig");
	        System.loadLibrary("config");
	    }
	    catch( UnsatisfiedLinkError e ) {
	         System.err.println("Native code library failed to load.\n" + e);
	    }
	}
	
	public static void main(String argv[]) {
		config_t cfg = new config_t();
		
		libconfig.config_init(cfg);
		libconfig.config_read_file(cfg, "/home/wallace/git/bitbucket/rvtool/test/Fibo/fibo.cfg");
		
		config_setting_t setting = libconfig.config_lookup(cfg, "program_variables");
		
		if (setting != null) {
			int count = libconfig.config_setting_length(setting);
			String [] var = {""};
			String [] type = {""};
			String [] scope = {""};
			String [] function = {""};
			for (int i =0; i < count; i++) {
				config_setting_t element = libconfig.config_setting_get_elem(setting, i);
				libconfig.config_setting_lookup_string(element, "name", var);
				libconfig.config_setting_lookup_string(element, "type", type);
				libconfig.config_setting_lookup_string(element, "scope", scope);
				if (scope[0].equals("local")) {
					libconfig.config_setting_lookup_string(element, "function", function);
				}
				System.out.println("name=" + var[0] + " type=" + type[0] + " scope=" + scope[0] + " function=" + function[0]);
			}
		}
		
		libconfig.config_destroy(cfg);
		
	}
}
