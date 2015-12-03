package ca.uwaterloo.esg.rvtool.monitor;

import java.io.BufferedReader;
import java.io.PrintWriter;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Hashtable;
import java.util.StringTokenizer;
import java.util.Vector;

import ca.uwaterloo.esg.rvtool.jni.libconfig.config_setting_t;
import ca.uwaterloo.esg.rvtool.jni.libconfig.config_t;
import ca.uwaterloo.esg.rvtool.jni.libconfig.libconfig;

import au.com.bytecode.opencsv.CSVReader;
import java.io.PrintWriter;
public class MonitorCreator {
	
	static {
		try {
			System.loadLibrary("clibconfig");
	        System.loadLibrary("config");
	    }
	    catch( UnsatisfiedLinkError e ) {
	         System.err.println("Native code library failed to load.\n" + e);
	    }
	}

	static private void LSPConverter() {

	}

	/**
	 * � * This Function extracts the variables used by GOOMF �
	 * @param : � 
	 * @return: void
	 */
	static private void extractPropVar(String specFilePath,
			Vector<PropVar> propVarVector) throws NumberFormatException,
			IOException {

		config_t cfg = new config_t();
		
		libconfig.config_init(cfg);
		libconfig.config_read_file(cfg, specFilePath);
		
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
				propVarVector.add(new PropVar(var[0],type[0],scope[0],function[0]));
				System.err.println("name=" + var[0] + " type=" + type[0] + " scope=" + scope[0] + " function=" + function[0]);
			}
		}
		
		libconfig.config_destroy(cfg);

	}
	static private int extractPropListLength(String specFilePath){

		config_t cfg = new config_t();
		
		libconfig.config_init(cfg);
		libconfig.config_read_file(cfg, specFilePath);
		
		config_setting_t setting = libconfig.config_lookup(cfg, "properties");

		int length = libconfig.config_setting_length(setting);
		libconfig.config_destroy(cfg);
		return length;
	}

	/**
	 * 
	 * @param args: <filepath of monitor template> <libconfig spec filepath> <output filepath> <sampling period> <info about main>
	 */
	public static void main(String[] args) {
		String monitorTemplatePath="";
		String libconfigSpecFilepath="";
		String outputMonitorPath;
		String lsp ;
		String mainFnInfo;
		String algType;
		String invocationType;
		String bufferSize;
		String context;
		String triggerType;
		PrintWriter appender;
		int no_of_props;
		System.out.println(">>>>>>>>>>>>>>> Start Creating Monitor.c Code");
		
		if (args.length != 8 &&
				args.length != 2) {
			System.err.println("ERROR: Invalid number of arguments\nUsage: ca.uwaterloo.esg.rvtool.monitor.MonitorCreator"
					+ " <filepath of monitor template> <libconfig spec filepath> <output filepath> <sampling period> <info about main> <GooMF algorithm type> <GooMF invocation type> <GooMF buffer size>");
			System.exit(1);
		}
		if (args.length == 8)
		{
			monitorTemplatePath = args[0];
			libconfigSpecFilepath = args[1];
			outputMonitorPath = args[2];
			lsp = args[3];
			mainFnInfo = args[4];
			algType = args[5];
			invocationType= args[6];
			bufferSize = args[7];
			
			context = "context"; 
			triggerType = "_GOOMF_enum_no_trigger";
		}
		else
		{
			monitorTemplatePath = args[0];
			libconfigSpecFilepath = args[1];
			try {
				appender = new PrintWriter(new BufferedWriter(new FileWriter(monitorTemplatePath, true)));
				appender.write("\n");
				no_of_props = extractPropListLength(libconfigSpecFilepath);
				appender.write("void report(){ \n");
				appender.write("	_GOOMF_enum_verdict_type v["+ no_of_props +"];\n");
				appender.write("	char str[100]; \n");
				appender.write("	register int i; \n");
				appender.write("	_GOOMF_getCurrentStatus(context, v); \n");
				for(int i = 0;i< no_of_props ;i++)
				{
					appender.write("	_GOOMF_typeToString(&v["+i+"], str); \n");
					appender.write("	fprintf(logFile,	\"property"+ i + ": %s\\n\", str); \n\n");
				}
				appender.write("}; \n");
				appender.close();
			} catch (IOException e) {
				System.err.println("ERROR: Monitor generation failed");
				e.printStackTrace();
				System.exit(1);
			}
			return;
		}
		


		

		Vector<PropVar> propVarVector = new Vector<PropVar>();
		
		boolean endOfKillMonitor = false;
		boolean addedDestroy = false;
		
		// TODO: rewrite this very hackish code to read information about the main function of the program
		Hashtable<String,String> mainInfo = new Hashtable<String,String>();

		FileInputStream iStream = null;
		try {
			iStream = new FileInputStream(mainFnInfo);
		} catch (FileNotFoundException e1) {
			System.err.println("Could not open main function info file: " + mainFnInfo);
			e1.printStackTrace();
			System.exit(1);
		}
		DataInputStream dInStream = new DataInputStream(iStream);
	    BufferedReader bReader = new BufferedReader(new InputStreamReader(dInStream));
	    String line;
	    try {
			while ((line = bReader.readLine()) != null) {
				String[] tuple = line.split(",");
				if (tuple.length == 2) {
					mainInfo.put(tuple[0],tuple[1]);
				}
			}
		} catch (IOException e1) {
			System.err.println("Read error for main function info file: " + mainFnInfo);
			e1.printStackTrace();
			System.exit(1);
		}
	    
	    if(!mainInfo.containsKey("args") || !mainInfo.containsKey("return")) {
	    	System.err.println("Information about main function of program missing");
	    	System.exit(1);
	    }
	    
		// Transforming the LSP to nanoseconds

		LSPConverter();
		no_of_props = extractPropListLength(libconfigSpecFilepath);
		try {
			extractPropVar(libconfigSpecFilepath, propVarVector);
			
		} catch (Exception e) {
			System.err.println("Cant read properties from GooMF meta file");
			e.printStackTrace();
			System.exit(2);
		}

		try {
			FileInputStream fstream = new FileInputStream(monitorTemplatePath);
			DataInputStream in = new DataInputStream(fstream);
			BufferedReader br = new BufferedReader(new InputStreamReader(in));

			BufferedWriter out = new BufferedWriter(new FileWriter(outputMonitorPath));

			String strLine = "";
			boolean addLine = false;

			while ((strLine = br.readLine()) != null) {

				if (strLine.contains("void monitor_thread")) {
					addLine = true;
				}
				if (strLine.contains("void checktimers")) {
					addLine = false;
				}

				out.write(strLine + "\n");

				// Adding the header file for the GOOMF data structures.

				if (strLine.contains("#include \"GooMFInstrumentor.h\"")) {
					out.write("#include \"ProgramState.h\" \n \n");
					out.write("#define LSP_SEC 0\n#define LSP_NSEC " + lsp + "\n\n");
					out.write("/* Global Variables of Interest of Monitored Prgoram must be here */ \n#include \"rithmic_globals.h\"\n");
					
					// defining context
					out.write("_GOOMF_context "+context+" = NULL; \n");
					
					// defining initContext
					out.write("int initContext(){ \n");
					out.write("return _GOOMF_initContext(&"+context+","+triggerType+","+algType+","+invocationType+","+bufferSize+"); \n");
					out.write("}; \n");
					
					out.write("\n\n");
					
					//defining Report
					out.write("void report(){ \n");
					out.write("	_GOOMF_enum_verdict_type v["+ no_of_props +"];\n");
					out.write("	char str[100]; \n");
					out.write("	register int i; \n");
					out.write("	_GOOMF_getCurrentStatus(context, v); \n");
					for(int i = 0;i< no_of_props ;i++)
					{
						out.write("	_GOOMF_typeToString(&v["+i+"], str); \n");
						out.write("	fprintf(logFile,	\"property"+ i + ": %s\\n\", str); \n\n");
					}
					out.write("}; \n");
					out.write("\n\n");
					
					//defining destroyContext
					out.write("int  destroyContext(){ \n");
					out.write("return _GOOMF_destroyContext(&"+context+"); \n");
					out.write("}; \n");
					
					out.write("\n\n");
					
				}
				
				if (strLine.contains("sleep_time.tv_nsec = LSP_NSEC;"))
				{
					out.write("\n	if (initContext() == _GOOMF_SUCCESS) \n\t\t printf(\"Context initialized..\\n\"); \n");
					out.write("	else \n\t\tprintf(\"Problem with context initialization..\\n\"); \n");
				}

				if (strLine
						.contains("clock_nanosleep(CLOCK_REALTIME, 0, &sleep_time, NULL);")
						&& addLine == true) {
					out.write("\n		/*---------------------------------------------------------------------------------------*/ \n");
					out.write("		/*----------- The extraction of the variables of interest and calling of GOOMF ----------*/ \n \n");

					for (int i = 0; i < propVarVector.size(); i++) {
						out.write("		current_program_state."
								+ propVarVector.get(i).name + " = "
								+ ((propVarVector.get(i).scope.equals("local")) ? propVarVector.get(i).function + "_" : "")
								+ propVarVector.get(i).name + "; \n");
					}

					out.write("		_GOOMF_nextState(context, (void*)(&current_program_state)); \n");
					out.write("      logevent(\"monitor\",0, current_program_state);\n");
					out.write("		_GOOMF_flush(context); \n");

					out.write("\n		/*---------------------------------------------------------------------------------------*/ \n \n");

				}
				
				if(addLine == true && strLine.contains("/* exit */"))
				{
					addedDestroy = true;
					endOfKillMonitor = false;
					out.write("\n	report(); \n");
					out.write("	destroyContext(); \n\n\n");
				}
				

				if (strLine.contains("// Main thread Body")) {
					if (mainInfo.get("return").compareTo("void") != 0) {
						out.write("    if (!args) { *retVal = 1; pthread_exit(retVal); }\n");
						out.write("    *retVal = ");
					}
					out.write("    program_main(");
					if (mainInfo.get("args").compareTo("std") == 0) {
						out.write("args->argc, args->argv");
					}
					out.write(");\n\n    kill_monitor = 1;\n    pthread_exit((void *)retVal);\n");					
				}
			}
			out.close();

			System.out.println(">>>>>>>>>>>>>>> Finished Creating Monitor.c Code");

		} catch (Exception e) {
			System.err.println("ERROR: Monitor generation failed");
			e.printStackTrace();
			System.exit(1);
		}

	}

}
