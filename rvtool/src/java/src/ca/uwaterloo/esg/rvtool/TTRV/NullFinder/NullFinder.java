package ca.uwaterloo.esg.rvtool.TTRV.NullFinder;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.util.*;








public class NullFinder {
	
	public static void main(String[] args) {
		
		System.out.println(">>>>>>>>>>> Starting the processing of Nulls");
		
		Hashtable hashtable = new Hashtable();
		
		// For processing structs in the future versions
		
		//Vector<StructVar> structVars = new Vector<StructVar>();
		

		try
		{
			  StringTokenizer tok;
  
			  String dir = args[0];
			  FileInputStream fstream = new FileInputStream(dir+"/LLVM/"+"critInst.txt");
			  DataInputStream in = new DataInputStream(fstream);
			  BufferedReader br = new BufferedReader(new InputStreamReader(in));


			  
			  FileInputStream fstream_null = new FileInputStream(dir+"/Nulls/"+"Nulls.txt");
			  DataInputStream in_null = new DataInputStream(fstream_null);
			  BufferedReader br_null = new BufferedReader(new InputStreamReader(in_null));
			  
			  // For processing structs in the future versions
			  /*
			  FileInputStream fstream_struct = new FileInputStream(dir+"/"+"structVars.txt");
			  DataInputStream in_struct = new DataInputStream(fstream_struct);
			  BufferedReader br_struct = new BufferedReader(new InputStreamReader(in_struct));
			  */

			  BufferedWriter out = new BufferedWriter(new FileWriter(dir+"/Pass"+"/"+"Nulls.txt")); 
			  BufferedWriter out_var = new BufferedWriter(new FileWriter(dir+"/Pass"+"/"+"critInst.txt")); 
			  
			  // writing out structs
			  // BufferedWriter out_struct = new BufferedWriter(new FileWriter(dir+"/Pass"+"/"+"critStructs.txt")); 



			  String str_line;
			  int index = 0;
			  int count = 0;
			  
			  // Extracting the structs
			  /*
			  while ((str_line = br_struct.readLine()) != null)   {
				  tok = new StringTokenizer(str_line,":");
				  
				  StructVar sVar = new StructVar();

				  
				  String varString = tok.nextToken().trim();
				  
				  StringTokenizer tokVar = new StringTokenizer(varString,",");
				  
				  
				  sVar.func = tokVar.nextToken();
				  
				  
				  sVar.name = tokVar.nextToken().trim();
				  
				  
				  
				  
				  
				  String typeString = tok.nextToken().trim();
				  
				  StringTokenizer tokType = new StringTokenizer(typeString,";");
				  
				  
				  while(tokType.hasMoreTokens())
				  {
					  String type = tokType.nextToken();
					  
					  int index1 = type.indexOf("_");
					  int index2 = type.indexOf("=");
					  
					  if(type.contains("type"))
					  {
						  Struct newStruct = new Struct();
						  newStruct.name = type.substring(index1+1,index2);
						  newStruct.type = type.substring(index2+1);
						  
						  sVar.var.add(newStruct);
					  }
					  
					  if(type.contains("prime"))
					  {
						  String name = type.substring(index1+1,index2);
						  
						  for(int i =0; i < sVar.var.size();i++)
						  {
							  if(sVar.var.get(i).name.contentEquals(name))
							  {
								  sVar.var.get(i).prime = type.substring(index2+1);
								  break;
							  }
						  }
					  }
					  
					  if(type.contains("order"))
					  {
						  String name = type.substring(index1+1,index2);
						  
						  for(int i =0; i < sVar.var.size();i++)
						  {
							  if(sVar.var.get(i).name.contentEquals(name))
							  {
								  sVar.var.get(i).order = Integer.parseInt(type.substring(index2+1).trim());
								  break;
							  }
						  }
					  }
				  }
				  				  
				  
				  
				 String varName = sVar.name;
				 
				 StringTokenizer tokName;
				 
				 if(varName.contains("."))
				 {
					 tokName = new StringTokenizer(varName,".");
				 }
				 else
				 {
					 // contains "->"
					 tokName = new StringTokenizer(varName,"->");
					 
				 }
				  
				 
				  
				  while(tokName.hasMoreTokens())
				  {
					 sVar.components.add(tokName.nextToken().trim()); 
				  }
				  
				  structVars.add(sVar);

				  
			  }
			  */
			  

			  
			  while ((str_line = br.readLine()) != null)   {
				  tok = new StringTokenizer(str_line,",");
				  String t1 = tok.nextToken().trim();
				  String t2  = tok.nextToken().trim();
				  hashtable.put(t2, t1);

			  }
			  

			  while ((str_line = br_null.readLine()) != null)   {

				  if(str_line.contains("="))
				  {
					  index = str_line.indexOf("=");
					  String var_int = str_line.substring(0,index-1);
					  if(var_int.contains("%"))
					  {
						  index = var_int.indexOf("%");
						  var_int = var_int.substring(index+1);
					  }
					  var_int.trim();
					  str_line = str_line.substring(index+1);
					  tok = new StringTokenizer(str_line,",");
					  
					  while(tok.hasMoreTokens())
					  {
						  String param = tok.nextToken().trim();
						  if(param.contains("@"))
						  {
							  index = param.indexOf("@");
							  param = param.substring(index+1);
							  if(param.contains("."))
							  {
								  index = param.indexOf(".");
								  param = param.substring(0,index);
							  }

							  if(hashtable.containsKey(param))
							  {
								  String tmp = (String) hashtable.get(param);
								  hashtable.put(var_int, tmp);
								  out_var.write(tmp+","+var_int+">"+param+"\n");
								  
							  }

						  }
					  }
					  
				  }
				  else
				  {
					  if(str_line.contains("store"))
					  {
						  boolean find = false;
						  tok = new StringTokenizer(str_line,",");
						  count = 0;
						  String var = "";
						  
						  // struct variables
						  // String structName = "";
						  // String structVar = "";
						  //int order = -1;
						 // boolean struct = false;
						 // boolean parseStruct = false;
						 // String funcStruct = "";
						 // String finalStruct = "";
						  
						  while(tok.hasMoreTokens())
						  {
							  count++;
							  String param = tok.nextToken().trim();
							  if(param.contains("@") && count > 1)
							  {
								  if(param.contains("struct"))
								  {
									  // processing the structs
									  
									 /* int indexStruct = param.indexOf("struct");
									  int indexSname = param.indexOf(".", indexStruct);
									  int indexEnd = param.indexOf("*",indexSname);
									  structName = param.substring(indexSname+1, indexEnd);
									  index = param.indexOf("@");
									  structVar = param.substring(index+1);
									  struct = true;*/
									  
								  }
								  else
								  {
									  index = param.indexOf("@");
									  param = param.substring(index+1);
									  if(param.contains("."))
									  {
										  index = param.indexOf(".");
										  param = param.substring(0,index);
									  }

									  if(hashtable.containsKey(param))
									  {
										  find = true;
										  var = param;
									  }
								  }

							  }
							  if(find == true)
							  {

								  if(param.contains(">>"))
								  {
									  index = param.indexOf(">>");
									  param = param.substring(index+2);

									  out.write(var+","+param+"\n");
								  }
							  }
							  
							  //processing the structs
							 /* if(struct == true)
							  {
								  if(count == 4)
								  {
									  int index1 = param.indexOf(" ");
									  int index2 = param.indexOf(")");
									  order = Integer.parseInt(param.substring(index1+1, index2).trim());
									  
									  for(int i = 0 ;i < structVars.size(); i++)
									  {
										  if(structVars.get(i).components.get(structVars.get(i).components.size() - 2).contentEquals(structVar))
										  {
											  String operand;
											  
											  for(int j =0; j < structVars.get(i).var.size(); j++)
											  {
												  if(structVars.get(i).var.get(j).name.contentEquals(structVar) && structVars.get(i).var.get(j).prime.contentEquals(structName))
												  {
													   operand = structVars.get(i).components.get(structVars.get(i).components.size() - 1);
													   
													   for(int k = j+1; k< structVars.get(i).var.size(); k++)
													   {
														   if(structVars.get(i).var.get(k).name.contentEquals(operand) && structVars.get(i).var.get(k).order == order)
														   {
															   funcStruct = structVars.get(i).func;
															   finalStruct = structVars.get(i).name;
															   parseStruct = true;
														   }
													   }
												  }
											  }
										  }
									  }
									  
									  
								  }
								  
								  if(parseStruct == true)
								  {
									  index = param.indexOf(">>");
									  param = param.substring(index+2);

									  out_struct.write(funcStruct+","+finalStruct+","+param+"\n");
								  }
							  }*/
						  }
						  
					  }
				  }
				  
			  }
			  
			  Enumeration e = hashtable.keys();
			  while(e.hasMoreElements()) { 
				  String str = (String) e.nextElement(); 
				  System.out.println(str);
				  if(!str.contains("rvtool"))
				  {
					  out_var.write(hashtable.get(str)+","+str+"\n");
				  }

			  } 
			  
			  out.close();
			  out_var.close();
			  // closing struct file
			 // out_struct.close();
			  
				System.out.println(">>>>>>>>>>> Finished the processing of Nulls");


		}
		catch(Exception e)
		{
			System.err.println("cant not open file for processing nulls");
			System.exit(0);
		}

	}

}
