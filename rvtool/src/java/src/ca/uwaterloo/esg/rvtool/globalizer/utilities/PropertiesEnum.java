package ca.uwaterloo.esg.rvtool.globalizer.utilities;

public enum PropertiesEnum {
	
	

	/******************************************************************
	 * 						ENUM VALUES								  *
	 ******************************************************************/
	

	 name,
	 type,
	 scope,
	 function;
	 
	
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
	private PropertiesEnum(final String text) 
	{
	    this.text = text;
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
	private PropertiesEnum() 
	{
	    this.text="";
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
