
/*
 * Filename: Globalize.java
 * Package: globalize
 * Project: Globalizer
 * Created By: Johnson J. Thomas
 * Date: 19.07.2012
 * Description: 
 */



// packages
package ca.uwaterloo.esg.rvtool.globalizer.globalize;

//imports
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import ca.uwaterloo.esg.rvtool.globalizer.utilities.*;







/*
 * class Name: FileIO
 * Description: 
 */
public class Globalize 
{
	/******************************************************************
	 * 						FINAL VARIABLES			 				  *
	 ******************************************************************/
	
	final String VARIABLES = "VARIABLE_TO_MONITOR_";
	
	
	
	
	
	
	/******************************************************************
	 * 						PUBLIC VARIABLES						  *
	 ******************************************************************/
	public String allFileContents = ""; 
	public String functionTitle = ""; 
	public boolean isEndOfFunction = false; 
	public boolean foundAFunction = false; 
	
	
	
	
	
	
	/******************************************************************
	 * 						PRIVATE VARIABLES						  *
	 ******************************************************************/
	private UserInput input = new UserInput(); 
	
	private boolean isFirstOpenBrace = true; 
	private boolean isFirstCloseBrace = true; 

	private int allOpenBraces = -1; 
	private int allCloseBraces = -1; 
 
	
	
	
	
	
	
	
	
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
	public String FindFunction(String line, UserInput info)
	{
		input = info; 
		Pattern pattern = null;
		Matcher matcher = null; 
		
		Pattern pattern1 = null;
		Matcher matcher1 = null; 
		
		
		line = line.trim().replaceAll(" +", " ");
		
		
		// we will use regex to see if the line we are looking at is the line which marks the start
		// of a function of interest
		for(String functionName: info.map.keySet()) 
		{
			for(@SuppressWarnings("unused") String variableName: info.map.get(functionName)) 
			{

				pattern = Pattern.compile ( "(.*?)"+ functionName + "\\((.*?)\\)(.*?)", Pattern.CASE_INSENSITIVE );
				matcher = pattern.matcher ( line );
				
		        
		        if(matcher.matches())
		        {
		        	pattern1 = Pattern.compile (functionName + "\\((.*?)\\)(.*?)", Pattern.CASE_INSENSITIVE );
					matcher1 = pattern1.matcher ( line );
					
					
					
					if(matcher1.matches())
					{
					   if(!line.contains(SymbolsEnum.SEMICOLON.toString()))
					   {
				        	allOpenBraces = -1;
				        	allCloseBraces = -1; 
				        	isEndOfFunction = false;
				        	isFirstOpenBrace = true;
				        	isFirstCloseBrace = true; 
				        	functionTitle = functionName; 
				        	foundAFunction = true;  
				        	
				        	if(line.contains(SymbolsEnum.CURLY_BRACE_OPEN.toString()))
				        	{
				        		GetBracketCount(line);
				        	}
					   }
					}
					else
					{
						
						for (TypesEnum key : TypesEnum.values()) 
						{
							  String k = ""; 
							  
							  k = key.toString(); 
							  if(line.contains(k))
							  {
								  line = line.replaceFirst(key.toString(), ""); 
								  line = line.trim(); 
								  line = line.replaceFirst(" ", " "); 
								  
								  pattern1 = null; 
								  matcher1 = null; 
								  
								  pattern1 = Pattern.compile (functionName + "\\((.*?)\\)(.*?)", Pattern.CASE_INSENSITIVE );
								  matcher1 = pattern1.matcher ( line );
								  
								   if(matcher1.matches())
									{
									   if(!line.contains(SymbolsEnum.SEMICOLON.toString()))
									   {
								        	allOpenBraces = -1;
								        	allCloseBraces = -1; 
								        	isEndOfFunction = false;
								        	isFirstOpenBrace = true;
								        	isFirstCloseBrace = true; 
								        	functionTitle = functionName; 
								        	foundAFunction = true;  
								        	
								        	if(line.contains(SymbolsEnum.CURLY_BRACE_OPEN.toString()))
								        	{
								        		GetBracketCount(line);
								        	}
									   }
									}
									
							  }
						}
						
					}
		        }

			}
		}
        
         return ""; 
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
	public String ReplaceUserInput(String strLine)
	{
		GetBracketCount(strLine); 
		
		if(allOpenBraces - allCloseBraces == 0 && allOpenBraces != -1 && allCloseBraces != -1)
		{
			isEndOfFunction = true; 
			foundAFunction = false; 
		}
		else if(isEndOfFunction == false)
		{
			for(String functionName: input.map.keySet()) 
			{
				for(String variableName: input.map.get(functionName)) 
				{
					if(functionName.matches(functionTitle))
					{
						String variableN[] = null; 

						if(variableName.contains(TypesEnum.INT.toString()))
						{
							
							variableN = variableName.split(TypesEnum.INT.toString());
							variableN[1] = variableN[1].trim(); 
						}
						else if(variableName.contains(TypesEnum.FLOAT.toString()))
						{
							variableN = variableName.split(TypesEnum.FLOAT.toString());
							variableN[1] = variableN[1].trim(); 
						}
						else if(variableName.contains(TypesEnum.STRING.toString()))
						{
							variableN = variableName.split(TypesEnum.STRING.toString());
							variableN[1] = variableN[1].trim(); 
						}
						else if(variableName.contains(TypesEnum.DOUBLE.toString()))
						{
							variableN = variableName.split(TypesEnum.DOUBLE.toString());
							variableN[1] = variableN[1].trim(); 
						}
						else if(variableName.contains(TypesEnum.SHORT.toString()))
						{
							variableN = variableName.split(TypesEnum.SHORT.toString());
							variableN[1] = variableN[1].trim(); 
						}
						else if(variableName.contains(TypesEnum.LONG.toString()))
						{
							variableN = variableName.split(TypesEnum.LONG.toString());
							variableN[1] = variableN[1].trim(); 
						}
						else if(variableName.contains(TypesEnum.BYTE.toString()))
						{
							variableN = variableName.split(TypesEnum.BYTE.toString());
							variableN[1] = variableN[1].trim(); 
						}
						
						
						
						if(strLine.contains(variableN[1]))
						{
							
				        	if(strLine.contains(TypesEnum.BOOLEAN.toString()) || strLine.contains(TypesEnum.BYTE.toString()) 
				        	   || strLine.contains(TypesEnum.CHAR.toString()) || strLine.contains(TypesEnum.DOUBLE.toString()) 
				        	   || strLine.contains(TypesEnum.FLOAT.toString())|| strLine.contains(TypesEnum.INT.toString()) 
				        	   || strLine.contains(TypesEnum.LONG.toString()) || strLine.contains(TypesEnum.SHORT.toString()) 
				        	   || strLine.contains(TypesEnum.STRING.toString()))
				        	  
				        	{
				        		// replace key types with nothing
				        		strLine = strLine.replaceAll(TypesEnum.BOOLEAN.toString(), ""); 
				        		strLine = strLine.replaceAll(TypesEnum.BYTE.toString(), ""); 
				        		strLine = strLine.replaceAll(TypesEnum.CHAR.toString(), ""); 
				        		strLine = strLine.replaceAll(TypesEnum.DOUBLE.toString() , ""); 
				        		strLine = strLine.replaceAll(TypesEnum.FLOAT.toString(), ""); 
				        		strLine = strLine.replaceAll(TypesEnum.INT.toString() , ""); 
				        		strLine = strLine.replaceAll(TypesEnum.LONG.toString(), ""); 
				        		strLine = strLine.replaceAll(TypesEnum.SHORT.toString(), ""); 
				        		strLine = strLine.replaceAll(TypesEnum.STRING.toString(), ""); 
				        		
				        		// replace variable name 
				        		variableN[1] = variableN[1].trim(); 
				        		Pattern pattern = Pattern.compile ( "(\\t.*)" + variableN[1] + "(.*?)", Pattern.CASE_INSENSITIVE );
						        Matcher matcher = pattern.matcher ( strLine );

						        
						        if(matcher.matches())
						        {
						        	
						        	if (strLine.matches("^.*(?<![a-zA-Z])" + variableN[1] + "(?![a-zA-Z]).*$"))
						        	{
						        		strLine = strLine.replaceAll("\\b" +variableN[1] + "\\b", functionTitle +"_" + variableN[1]);
						        	}
						        	
						        }
						        
				        		
				        	}
				        	else
				        	{
				        		// in the case that there are no types (ie. it is not a local declaration)
				        		// we will simply need to change the variable name and leave everything else as is 

				        		variableN[1] = variableN[1].trim(); 

				        		Pattern pattern = Pattern.compile ("(.*?)" + variableN[1] + "(.*?)", Pattern.CASE_INSENSITIVE );
						        Matcher matcher = pattern.matcher ( strLine );

						        
						        if(matcher.matches())
						        {
						        
						        	if (strLine.matches("^.*(?<![a-zA-Z])" + variableN[1] + "(?![a-zA-Z]).*$")) 
						        	{
						        		strLine = strLine.replaceAll("\\b" +variableN[1] + "\\b", functionTitle +"_" + variableN[1]);
						        	}
						        	
						        }
						        
				        	}
		
						}
						
					}
					
					
				}
				
			}
			
		}
		
		return strLine + "\n"; 
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
	public void GetBracketCount( String str) 
	{ 
		// variables
		int openBracketCount = 0; 
		int closeBracketCount = 0; 
		
    	
    	
    	
    	// switch of the string characters. If there is an opening brace,
    	// increment the counters, to keep track of the number of each type
    	// of brace. Later, we can balasnce it out and see if the count is 0. 
    	// if it is 0, then we have closed all the curly braces which we have opened
		for (final char ch : str.toCharArray()) 
		{ 
			
			// If the current character is a bracket, increase bracketCount. 
			switch (ch) 
			{ 
				case '{': 
				{
					openBracketCount++;
					break; 
				}
				case '}': 
				{
					closeBracketCount++; 
					break; 
				}
					
			}
			
			
		} 
		
		if(isFirstOpenBrace == true && openBracketCount > 0)
		{
			isFirstOpenBrace = false; 
		}
		
		if(isFirstCloseBrace == true && closeBracketCount > 0)
		{
			isFirstCloseBrace = false; 
		}

		if(isFirstCloseBrace == false || isFirstOpenBrace == false)
		{
			if(isFirstOpenBrace == false)
			{
				allOpenBraces = openBracketCount + allOpenBraces;
			}
			
			if(isFirstCloseBrace == false)
			{
				allCloseBraces = closeBracketCount + allCloseBraces;
			}
		}
		
	} 
	
}
