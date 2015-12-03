package ca.uwaterloo.esg.rvtool.TTRV.Instrumentor;

import java.io.IOException;
import java.util.Vector;



public class GoomfInstrumentor {

	public static void main(String[] args) {
		
		Parser myParser = new Parser();
		Vector<PropVar> propVarVector = new Vector<PropVar>();
		Vector<ProgVar> progVarVector = new Vector<ProgVar>();
		
		if (args.length != 11) {
			System.err.println("Usage: ca.uwaterloo.esg.rvtool.TTRV.Instrumentor.GoomfInstrumentor " +
					"<output header filepath> <spec (cfg) filepath> <critical variable list filepath> <context> <trigger type> " +
					"<algorithm type> <invocation type> <buffer size> <instrumentation spec output path> <line mappings file>");
			System.exit(1);
		}
		
		String outputHeaderFilepath = args[0]; 	//output path for GooMFInstrumentor.h
		String specFilepath = args[1]; 			//filepath to the spec (cfg) file
		String varFile = args[2]; 				//filepath to list of critical variables
		String context = args[3]; 
		String triggerType = args[4];
		String algType = args[5];
		String invocationType= args[6];
		String bufferSize = args[7];
		String outputInstInfo = args[8];		//filepath to list of instrumentation locations
		String lineMappingFilepath = args[9];	//filepath to variable to line map (.../Tool/ILP/instLines.txt)
		String isstatic = args[10];
		
		
		
		try{
			myParser.headerCreator(outputHeaderFilepath, context, triggerType, algType, invocationType, bufferSize);
		}
		catch(IOException e)
		{
			System.err.println("SOOMF: Can not write file GOOMFInstrumentor.h");
			e.printStackTrace();
			System.exit(1);
		}
		
		
		try{
			myParser.extractPropVar(specFilepath, propVarVector);
		}
		catch(IOException e)
		{
			System.err.println("SOOMF: Can not read .cfg file from GOOMF");
			e.printStackTrace();
			System.exit(1);
		}
		
		
		try{
			myParser.extractInstVariables(varFile, progVarVector, propVarVector);
		}
		catch(IOException e)
		{
			System.err.println("SOOMF: Can not read file crit_vars.txt");
			e.printStackTrace();
			System.exit(1);
		}
		
		
		//--------- Instrument the critical lines
		try{
			myParser.instrument(lineMappingFilepath, outputInstInfo, progVarVector, context,isstatic);
		}
		catch(IOException e)
		{
			System.err.println("SOOMF: Can not read file crit_vars.txt");
			e.printStackTrace();
			System.exit(1);
		}
		
		


	}

}
