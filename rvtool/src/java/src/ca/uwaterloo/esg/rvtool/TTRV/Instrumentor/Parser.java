package ca.uwaterloo.esg.rvtool.TTRV.Instrumentor;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.StringTokenizer;
import java.util.Vector;


public class Parser {
	
	

	/**	
	 � * This Function creates the header file needed to instrument with GOOMF	
	 � * @param	: dir (directory of monitored program), context (the structure to be allocated),triggerType (triggers the program trace flush), algType (algorithm type that performs the verification), invocationType (whether the buffer flush is blocking call or not), bufferSize (if trigger is _GOOMF_enum_buffer_trigger, specify the maximal size of the buffer)
	 � * @return : void	
	*/	
	public void headerCreator(String headerOutputPath, String context, String triggerType,String algType,String invocationType,String bufferSize) throws  NumberFormatException, IOException
	{
		
		System.out.println(">>>>>>>>>>>>>>> Creating GOOMFInstrumentor.h");
		
		BufferedWriter out = new BufferedWriter(new FileWriter(headerOutputPath));
		
		out.write("#ifndef _GOOMF_INSTRUMENTOR_ \n");
		out.write("#define _GOOMF_INSTRUMENTOR_ \n");
		
		out.write("\n\n");
		
		out.write("#include \"GooMFOnlineAPI.h\" \n");
		out.write("#include \"ProgramState.h\" \n");
		
		out.write("\n\n");
		
		out.write("extern _GOOMF_context "+context+"; \n");
		out.write("struct _GOOMF_host_program_state_struct current_program_state; \n");
		
		out.write("\n\n");

		
		out.write("extern int initContext(); \n");
		
		out.write("\n");
		
		out.write("extern void report(); \n");
		
		out.write("\n");
		
		out.write("extern int destroyContext(); \n\n");


		//out.write("void initContext(){ \n");
		//out.write("_GOOMF_initContext(&"+context+","+triggerType+","+algType+","+invocationType+","+bufferSize+"); \n");
		//out.write("}; \n");
		
		//out.write("\n\n");

		//out.write("void destroyContext(){ \n");
		//out.write("_GOOMF_destroyContext(&"+context+"); \n");
		//out.write("}; \n");
		
		//out.write("\n\n");

		
		out.write("#endif \n");
		
		out.close();
		
		System.out.println(">>>>>>>>>>>>>>> Finished Creating GOOMFInstrumentor.h");

		
	}

	
	
	/**	
	 � * This Function extracts the variables used by GOOMF	
	 � * @param	: 
	 � * @return : void	
	*/	
	public void extractPropVar(String specFilepath, Vector<PropVar> propVarVector) throws  NumberFormatException, IOException
	{
		
		System.out.println(">>>>>>>>>>>>>>> Starting Extraction of Property Variables");

		StringTokenizer tok;
		FileInputStream fstream = new FileInputStream(specFilepath);
		DataInputStream in = new DataInputStream(fstream);
		BufferedReader br = new BufferedReader(new InputStreamReader(in));
		
		String strLine;
		boolean goOn = false;
		boolean startingLine = false;
		
		int varNum = 0;
		  
		while ((strLine = br.readLine()) != null)   {
			
			if(strLine.contains("program_variables"))
			{
				goOn = true;
				startingLine = true;
			}
			
			if(startingLine == true)
			{
				tok = new StringTokenizer(strLine,"=");
				int turns = 0;
				PropVar pVar = new PropVar();
				pVar.num = varNum;
				varNum++;
				
				while(tok.hasMoreTokens())
				{
					turns++;
					String strProcess = tok.nextToken().trim();
					
					if(turns == 3)
					{
						
						int index1 = strProcess.indexOf("\"");
						int index2 = strProcess.indexOf("\"", index1+1);

						
						strProcess = strProcess.substring(index1+1, index2);
						pVar.name = strProcess;
					}
					
					if(turns == 4)
					{
						int index1 = strProcess.indexOf("\"");
						int index2 = strProcess.indexOf("\"", index1+1);
						
						strProcess = strProcess.substring(index1+1, index2);
						pVar.type = strProcess;

					}
					
					if(turns == 6) {
						int index1 = strProcess.indexOf("\"");
						int index2 = strProcess.indexOf("\"", index1+1);
						
						strProcess = strProcess.substring(index1+1, index2);
						pVar.func = strProcess;
					}
				}
				
				propVarVector.add(pVar);
				startingLine = false;

			}
			else
			{
				if(goOn == true)
				{
					tok = new StringTokenizer(strLine,"=");
					int turns = 0;
					PropVar pVar = new PropVar();
					pVar.num = varNum;
					varNum++;
					
					while(tok.hasMoreTokens())
					{
						turns++;
						String strProcess = tok.nextToken().trim();
						
						if(turns == 2)
						{
							
							int index1 = strProcess.indexOf("\"");
							int index2 = strProcess.indexOf("\"", index1+1);

							
							strProcess = strProcess.substring(index1+1, index2);
							pVar.name = strProcess;
						}
						
						if(turns == 3)
						{
							int index1 = strProcess.indexOf("\"");
							int index2 = strProcess.indexOf("\"", index1+1);
							
							strProcess = strProcess.substring(index1+1, index2);
							pVar.type = strProcess;

						}
						
						if(turns == 5)
						{
							int index1 = strProcess.indexOf("\"");
							int index2 = strProcess.indexOf("\"", index1+1);
							
							strProcess = strProcess.substring(index1+1, index2);
							pVar.func = strProcess;

						}
					}
					
					propVarVector.add(pVar);

				}
			}
			
		}
				
		System.out.println(">>>>>>>>>>>>>>> Finished Extraction of Property Variables");

		
	}


	/**	
	 � * This Function extracts the program variables to instrument	
	 � * @param	: 
	 � * @return : void	
	*/	
	public void extractInstVariables(String varFile, Vector<ProgVar> progVarVector, Vector<PropVar> propVarVector) throws  NumberFormatException, IOException
	{
		System.out.println(">>>>>>>>>>>>>>> Starting Extracting Variables of Interest");
		
		StringTokenizer tok;
  
		FileInputStream fstream = new FileInputStream(varFile);
		DataInputStream in = new DataInputStream(fstream);
		BufferedReader br = new BufferedReader(new InputStreamReader(in));
			  

		String str_line;
		String scope;
		String name;
		String type;
		String func="";
			  
			  
		while ((str_line = br.readLine()) != null)   {
			ProgVar pVar = new ProgVar();
			
			tok = new StringTokenizer(str_line,",");
					  
					  
			name = tok.nextToken().trim();
			type = tok.nextToken().trim();
			scope = tok.nextToken().trim();
			
			if(scope.contains("local"))
			{
				pVar.func = func;
				pVar.name = name;
				pVar.type = type;
			}
			else
			{
				pVar.func = "globalFUNC";
				pVar.name = name;
				pVar.type = type;	  
			}
			
			for(int i= 0; i < propVarVector.size(); i++)
			{
				if(pVar.name.compareTo(propVarVector.get(i).func + "_" + propVarVector.get(i).name) == 0)
				{
					pVar.pVar = propVarVector.get(i);
					break;
				}
			}
			
			progVarVector.add(pVar);
		}
		

		System.out.println(">>>>>>>>>>>>>>> Finished Extracting Variables of Interest");

	}






	public void instrument(String criticalVarFilepath, String outputInstFilepath, Vector<ProgVar> progVarVector, String context,String isstatic) throws  NumberFormatException, IOException
	{
		
		System.out.println(">>>>>>>>>>>>>>> Starting Mapping Global Variables to Local Variables");

		StringTokenizer tok;
		FileInputStream fstream = new FileInputStream(criticalVarFilepath);
		DataInputStream in = new DataInputStream(fstream);
		BufferedReader br = new BufferedReader(new InputStreamReader(in));
		
		BufferedWriter out = new BufferedWriter(new FileWriter(outputInstFilepath)); 
		
		
		//-------- init context
		out.write("GOOMF_Header,"+"<#include \"GooMFInstrumentor.h\"> \n");
		//out.write("GOOMF_Init,"+"<initContext();> \n");
		//out.write("GOOMF_Dest,"+"<destroyContext();> \n");

		
		

		
		String strLine;
		
		String func;
		String name;
		  
		while ((strLine = br.readLine()) != null)   {
			
			tok = new StringTokenizer(strLine,",");
			
			func = tok.nextToken().trim();
			name = tok.nextToken().trim();
			
			for(int i = 0; i < progVarVector.size(); i++)
			{
				if((progVarVector.get(i).func.contentEquals(func) || progVarVector.get(i).func.contentEquals("globalFUNC") ) && progVarVector.get(i).name.contentEquals(name))
				{						
					// can only handle primary types at the moment
					if(isstatic.contentEquals("0"))
						out.write(strLine+","+"< current_program_state."+progVarVector.get(i).pVar.name+"="+name + "; queueEvent(current_program_state, __LINE__); > \n");
					else
						out.write(strLine+","+"< current_program_state."+progVarVector.get(i).pVar.name+"="+name + ";_GOOMF_nextEvent("+context+","+progVarVector.get(i).pVar.num+",(void*)(&"+name+"));logevent(\"main\",__LINE__,current_program_state);> \n");

				}
			}
			
		}
		
		out.close();
		
		System.out.println(">>>>>>>>>>>>>>> Finished Mapping Global Variables to Local Variables");

		
		
	}

}
