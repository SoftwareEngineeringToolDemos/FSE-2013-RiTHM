
/*
 * Filename: UIManager.java
 * Package: management
 * Project: Globalizer
 * Created By: Johnson J. Thomas
 * Date: 19.07.2012
 * Description: 
 */



// packages
package ca.uwaterloo.esg.rvtool.globalizer.management;

//imports
import ca.uwaterloo.esg.rvtool.globalizer.utilities.*;







/*
 * class Name: FileIO
 * Description: 
 */
public class UIManager 
{
	/******************************************************************
	 * 						STATIC VARIABLES						   *
	 ******************************************************************/
	static FileIO file = new FileIO(); 
	
	
	
	
	
	
	/******************************************************************
	 * 						PUBLIC STATIC MAIN						  *
	 ******************************************************************/
	
	
	
	/*
	 * Method Name:main
	 * Parameters: String[] args
	 * Parameter Description: main arguments
	 * Method Description: main method which calls on loading in the main .c file
	 * Return: void
	 *
	 * @name: main
	 * @params: String[] args
	 * @desc: main method which calls on loading in the main .c file
	 * @return: void
	 * @see java.lang.Enum#toString()
	 * 
	 */
	public static void main(String[] args)
	{		
		if (args.length != 4) {
			System.err.println("Usage: ca.uwaterloo.esg.rvtool.globalizer.management.UIManager <c file to globalize> <processed spec file> <output comma-separated file of critical variables> <GooMF instrumentor metadata output>");
			System.exit(1);
		}
		
		file.LoadMainFile(args[1], args[0]); 
		file.SaveOutputAsInstLines(args[2]);
		file.SaveOutputAsGooMFInst(args[3]);
	}
	
}
