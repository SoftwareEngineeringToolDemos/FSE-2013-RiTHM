
/*
 * Filename: UserInput.java
 * Package: utilities
 * Project: Globalizer
 * Created By: Johnson J. Thomas
 * Date: 19.07.2012
 * Description: 
 */



// packages
package ca.uwaterloo.esg.rvtool.globalizer.utilities;

//imports
import java.util.ArrayList;
import java.util.HashMap;







/*
 * class Name: FileIO
 * Description: 
 */
public class UserInput 
{
	
	
	
	/******************************************************************
	 * 						PUBLIC VARIABLES						  *
	 ******************************************************************/

	public HashMap<String, ArrayList<String>> map;
	
	

	
	
	
	
	
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
	public UserInput()
	{
		map = new HashMap<String, ArrayList<String>>();
	}
		
	
	
	/*
	 * Method Name:
	 * Parameters:
	 * Parameter Description:
	 * Method Description: Adds a variable to the map given a function name to which the variable belongs
	 * Return: 
	 *
	 * @name: 
	 * @params: 
	 * @desc:Adds a variable to the map given a function name to which the variable belongs
	 * @return:
     * @see java.lang.Enum#toString()
     * 
     */
	public void addVariable(String functionName, String variableName)
	{
		ArrayList<String> variables;
		
		if(!map.containsKey(functionName))
		{
			variables = new ArrayList<String>();
		}
		else 
		{
			variables = map.get(functionName);
		}
		
		//Adding the variableName to the arraylist variables
		variables.add(variableName);

		//Adding to the HashMap
		if(!map.containsKey(functionName))
		{
			map.put(functionName, variables);
		}
	}
	
	
	
	/*
	 * Method Name:getFunctionVariables
	 * Parameters:String functionName
	 * Parameter Description: contains the name of the function
	 * Method Description: This function returns the variables associated with a particular function
	 * 					   whose variables are returned.
	 * Return: ArrayList<String>
	 *
	 * @name: getFunctionVariables
	 * @params: String functionName
	 * @params desc: contains the name of the function
	 * @desc: This function returns the variables associated with a particular function
	 * 					   whose variables are returned.
	 * @return: ArrayList<String>
     * @see java.lang.Enum#toString()
     * 
     */
	public ArrayList<String> getFunctionVariables(String functionName) 
	{
		if(!map.containsKey(functionName)) 
		{
			return null;
		}
		
		return(map.get(functionName));
	}
	
	/*
	 * Description: This function prints the key:value pairs in the HashMap in the format ""
	 */
	
	
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
	public void printMap()
	{
		for(String functionName: map.keySet()) 
		{
			for(String variableName: map.get(functionName)) 
			{
				System.out.println(variableName+" "+functionName);
			}
		}
	}
}
