
/*
 * Filename: FileIO.java
 * Package: utilities
 * Project: Globalizer
 * Created By: Johnson J. Thomas
 * Date: 19.07.2012
 * Description: 
 */



// packages
package ca.uwaterloo.esg.rvtool.globalizer.utilities;

//imports
import ca.uwaterloo.esg.rvtool.globalizer.globalize.*;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.StringTokenizer;

import org.apache.commons.io.FilenameUtils;







/*
 * class Name: FileIO
 * Description: 
 */
public class FileIO 
{
	
	/******************************************************************
	 * 						PRIVATE VARIABLES						  *
	 ******************************************************************/
	
	private Globalize globalize = new Globalize(); 
	private UserInput input = new UserInput(); 
	
	private String finalOutput = "";
	private String globalHeaderOutput = "";
	private String returnValue = ""; 
	private String fileName;
	
	private static String getHeaderPreamble() {
		return  "#ifndef __GLOBAL_HEADER_H__\n" +
				"#define __GLOBAL_HEADER_H__\n";
	}
	
	private static String getHeaderClose() {
		return "#endif /* __GLOBAL_HEADER_H__ */\n";
	}
		
	
	/******************************************************************
	 * 						PUBLIC METHODS							  *
	 ******************************************************************/
	
	/*
	 * Method Name:
	 * Parameters:
	 * Parameter Description:
	 * Method Description:
	 * Return: 
	 *
	 * @name: 
	 * @params: 
	 * @desc:
	 * @return:
     * @see java.lang.Enum#toString()
     * 
     */
	public void LoadMainFile(String specFilepath, String fileName)
	{
		 FileInputStream fstream = null;
		 
		 //LoadUserInputFile();
		 LoadUserInputFilePsuedoJNI(specFilepath);
		 String variableN[] = null; 
		 String variableType = null;
		 globalHeaderOutput = getHeaderPreamble();
		 finalOutput = "#include \"rithmic_globals.h\"\n";
		 
		 for(String functionName: input.map.keySet()) 
		 {
			for(String variableName: input.map.get(functionName)) 
			{
				if(variableName.contains(TypesEnum.INT.toString()))
				{
					variableN = variableName.split(TypesEnum.INT.toString());
					variableN[1] = variableN[1].trim(); 
					variableType = TypesEnum.INT.toString(); 
					
				}
				else if(variableName.contains(TypesEnum.FLOAT.toString()))
				{
					variableN = variableName.split(TypesEnum.FLOAT.toString());
					variableN[1] = variableN[1].trim(); 
					variableType = TypesEnum.FLOAT.toString(); 
					
				}
				else if(variableName.contains(TypesEnum.STRING.toString()))
				{
					variableN = variableName.split(TypesEnum.STRING.toString());
					variableN[1] = variableN[1].trim(); 
					variableType = TypesEnum.STRING.toString(); 
				}
				else if(variableName.contains(TypesEnum.DOUBLE.toString()))
				{
					variableN = variableName.split(TypesEnum.DOUBLE.toString());
					variableN[1] = variableN[1].trim(); 
					variableType = TypesEnum.DOUBLE.toString(); 
				}
				else if(variableName.contains(TypesEnum.SHORT.toString()))
				{
					variableN = variableName.split(TypesEnum.SHORT.toString());
					variableN[1] = variableN[1].trim(); 
					variableType = TypesEnum.SHORT.toString(); 
				}
				else if(variableName.contains(TypesEnum.LONG.toString()))
				{
					variableN = variableName.split(TypesEnum.LONG.toString());
					variableN[1] = variableN[1].trim(); 
					variableType = TypesEnum.LONG.toString(); 
				}
				else if(variableName.contains(TypesEnum.BYTE.toString()))
				{
					variableN = variableName.split(TypesEnum.BYTE.toString());
					variableN[1] = variableN[1].trim(); 
					variableType = TypesEnum.BYTE.toString(); 
					
				}
				 String varDecl = variableType + " "+ functionName+"_" + variableN[1] + ";\n";
//				 finalOutput = finalOutput + "extern " + varDecl; 
				 globalHeaderOutput = globalHeaderOutput + varDecl;
			}
		 }
		 
		 globalHeaderOutput += getHeaderClose();
		
		try 
		{			
			fstream = new FileInputStream(fileName);
			
		} 
		catch (FileNotFoundException e1) 
		{
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		  
		  DataInputStream in = new DataInputStream(fstream);
		  BufferedReader br = new BufferedReader(new InputStreamReader(in));
		  String strLine;
		  
		  try 
		  {
			while ((strLine = br.readLine()) != null)   
			  {
				  globalize.FindFunction(strLine, input);  
				  
				  if(globalize.foundAFunction == true)
				  {
					  
					  returnValue = globalize.ReplaceUserInput(strLine);
					  
					  if(globalize.isEndOfFunction == true)
					  {
						  globalize.foundAFunction = false; 
						  globalize.isEndOfFunction = false; 
					  }
					  
					  finalOutput = finalOutput + returnValue; 
				  }
				  else
				  {
					  finalOutput = finalOutput + strLine + "\n";
				  }
			  }
		  } 
		  catch (IOException e1) 
		  {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		  }
		  
		 
		  
		  try 
		  {
			String globalizedFilepath = FilenameUtils.getFullPath(fileName) + FilenameUtils.getBaseName(fileName) + "_globalized.c";
			System.err.println("Globalizing: " + fileName + " -> " + globalizedFilepath);
			FileWriter outputFile = new FileWriter( globalizedFilepath );
			outputFile.write(finalOutput);
			outputFile.close(); 
			in.close();
		  } 
		  catch (IOException e) 
		  {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		  
		  try {
			  String globalizedFilepath = FilenameUtils.getFullPath(fileName) + "rithmic_globals.h";
			  System.err.println("Generating global header: " + globalizedFilepath);
			  FileWriter outputFile = new FileWriter( globalizedFilepath );
			  outputFile.write(globalHeaderOutput);
			  outputFile.close();
		  } catch (IOException e) {
			  e.printStackTrace();
		  }
		  
		
	}
	
	
	
	/*
	 * Method Name:
	 * Parameters:
	 * Parameter Description:
	 * Method Description:
	 * Return: 
	 *
	 * @name: 
	 * @params: 
	 * @desc:
	 * @return:
     * @see java.lang.Enum#toString()
     * 
     */
	public void LoadUserInputFile()
	{
		 int lengthOfLine = 0; 
		 UserInput info = new UserInput();
		 
		 try
		 {			 
			 FileInputStream fstream = new FileInputStream("test/input_spec.cfg");
			 DataInputStream in = new DataInputStream(fstream);
			 BufferedReader br = new BufferedReader(new InputStreamReader(in));
			 String strLine;
		  
			 while ((strLine = br.readLine()) != null)   
			 {
				  strLine = strLine.trim(); 
				  String[] str = strLine.split("~"); 
				  
				  String[] varsToAdd = str[0].split(",");
				  lengthOfLine = str.length; 
				  
				  if(lengthOfLine < 2)
				  {
					  throw new IOException("Local variable and/or function name cannot be found!"); 
					  
				  }
				 
				  for(int z = 0; z < varsToAdd.length; z++)
				  {
					  info.addVariable(str[1], varsToAdd[z]);
				  }
				  
			 }
		 }
		 catch (Exception e)
     	 {
     		  System.err.println("Error: " + e.getMessage());
         }

		 input = info; 
	}
	
	// TODO: remove all the hard-coded badness
	public void SaveOutputAsInstLines(String outputPath) {
		try {
			FileWriter writer = new FileWriter(outputPath);
			for (String s : input.map.keySet()) {
				for (int i = 0; i < input.map.get(s).size(); i++) {
					String []var = input.map.get(s).get(i).split("\\s+");
					writer.write("globalFUNC," + s + "_" + var[var.length-1] + "\n");
				}
			}
			writer.close();
		} catch (IOException e) {
			System.err.println("Error: could not open file for write - " + outputPath);
			e.printStackTrace();
		}
	}
	
	public void SaveOutputAsGooMFInst(String outputPath) {
		try {
			FileWriter writer = new FileWriter(outputPath);
			for (String s : input.map.keySet()) {
				for (int i = 0; i < input.map.get(s).size(); i++) {
					String []var = input.map.get(s).get(i).split("\\s+");
					writer.write(s + "_" + var[var.length-1] + "," + var[var.length-2] + ",globalFUNC\n");
				}
			}
			writer.close();
		} catch (IOException e) {
			System.err.println("Error: could not open file for write - " + outputPath);
			e.printStackTrace();
		}
	}
	
	/*
	 * Method Name:
	 * Parameters:
	 * Parameter Description:
	 * Method Description:
	 * Return: 
	 *
	 * @name: 
	 * @params: 
	 * @desc:
	 * @return:
     * @see java.lang.Enum#toString()
     * 
     */
	public void LoadUserInputFilePsuedoJNI(String specFileName)
	{
		 UserInput info = new UserInput();
		 StringTokenizer disect;
		 
		 try
		 {			 
			 FileInputStream fstream = new FileInputStream(specFileName);
			 DataInputStream in = new DataInputStream(fstream);
			 BufferedReader br = new BufferedReader(new InputStreamReader(in));
			 String strLine;
		  
			 while ((strLine = br.readLine()) != null)   
			 {
				 String[] tokens = strLine.split(",");
				 String name = null,type=null,scope = null,function=null;
				 for(int i=0;i<tokens.length;i++) {				
					 disect = new StringTokenizer(tokens[i],"=");
					 String var = disect.nextToken();
					 String value = disect.nextToken();
					 PropertiesEnum prop = PropertiesEnum.valueOf(var);
					 
					 switch(prop) {
					 case name:
						 name = value;
						 break;
					 case type:
						 type = value;
						 break;
					 case scope:
						 scope = value;
						 break;
					 case function:
						 function = value;
						 break;
					 default:
						System.err.println("Unknown name, value pair");
						break;
					 }
				 }
				 
				 if(scope.equalsIgnoreCase("local")) {					 
					 String var_name = type + " " + name;
					 info.addVariable(function, var_name);
					 System.out.println("Added: "+var_name+"~"+function);
				 }
			 }
		 }
		 catch (Exception e)
     	 {
     		  System.err.println("Error: " + e.getMessage());
         }

		 input = info; 
	}

}
