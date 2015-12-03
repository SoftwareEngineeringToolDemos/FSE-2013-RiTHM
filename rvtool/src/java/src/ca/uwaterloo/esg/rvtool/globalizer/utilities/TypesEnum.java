
/*
 * Filename: TypesEnum.java
 * Package: utilities
 * Project: Globalizer
 * Created By: Johnson J. Thomas
 * Date: 19.07.2012
 * Description: 
 */



// packages
package ca.uwaterloo.esg.rvtool.globalizer.utilities;







/*
 * class Name: FileIO
 * Description: 
 */
public enum TypesEnum 
{
	
	

	/******************************************************************
	 * 						ENUM VALUES								  *
	 ******************************************************************/
	

	INT("int"),
	BOOLEAN("boolean"),
	STRING("String"),
	FLOAT("float"),
	DOUBLE("double"),
	BYTE("byte"),
	CHAR("char"),
	LONG("long"),
	SHORT("short"); 
	
	
	/******************************************************************
	 * 						PRIVATE VARIABLES						  *
	 ******************************************************************/	
	
	 private final String text;
	
	
	
	 
	 
	 
	 
	
	/******************************************************************
	 * 						PRIVATE METHODS							  *
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
	private TypesEnum(final String text) 
	{
	    this.text = text;
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
    @Override
    public String toString() 
    {
        // TODO Auto-generated method stub
        return text;
    }
}
