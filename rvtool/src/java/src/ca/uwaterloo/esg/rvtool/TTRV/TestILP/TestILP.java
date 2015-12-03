package ca.uwaterloo.esg.rvtool.TTRV.TestILP;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.util.StringTokenizer;
import java.util.Vector;


public class TestILP {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try
		{
			  

  
			  String dir = args[0];
			  FileInputStream fstream = new FileInputStream(dir+"/ILP/test2.txt");
			  DataInputStream in = new DataInputStream(fstream);
			  BufferedReader br = new BufferedReader(new InputStreamReader(in));
			  
			  FileInputStream fstream3 = new FileInputStream(dir+"/ILP/test1.txt");
			  DataInputStream in3 = new DataInputStream(fstream3);
			  BufferedReader br3 = new BufferedReader(new InputStreamReader(in3));
			  

			  
			  BufferedWriter out = new BufferedWriter(new FileWriter(dir+"/ILP/outtest2.txt")); 
			  BufferedWriter out2 = new BufferedWriter(new FileWriter(dir+"/ILP/outtest1.txt")); 



			  
			  String str_line;

			  
			  
			  while ((str_line = br.readLine()) != null)   {
				  String temp = str_line;
				  temp = temp.trim();
				  
				  FileInputStream fstream2 = new FileInputStream(dir+"/ILP/copy.txt");
				  DataInputStream in2 = new DataInputStream(fstream2);
				  BufferedReader br2 = new BufferedReader(new InputStreamReader(in2));
				  
				  String copy;
				  int counter =0;
				  
				  while ((str_line = br2.readLine()) != null)   {
					  copy = str_line;
					  copy = copy.trim();
					  
					  if(copy.contentEquals(temp))
					  {
						  counter++;
					  }
					  
				  }
				  
				  br2.close();
				  
				  if(counter > 1)
				  {
					  out.write(temp+"\n");
				  }
				  
			  }
			  
			  br.close();
			  
			  while ((str_line = br3.readLine()) != null)   {
				  
				  String temp = str_line;
				  temp = temp.trim();
				  
				  FileInputStream fstream2 = new FileInputStream(dir+"/ILP/test2.txt");
				  DataInputStream in2 = new DataInputStream(fstream2);
				  BufferedReader br2 = new BufferedReader(new InputStreamReader(in2));
				  
				  String copy;
				  int counter =0;
				  
				  while ((str_line = br2.readLine()) != null)   {
					  copy = str_line;
					  copy = copy.trim();
					  
					  if(copy.contentEquals(temp))
					  {
						  counter++;
					  }
					  
				  }
				  
				  br2.close();
				  
				  if(counter == 0)
				  {
					  out2.write(temp+"\n");
				  }
				  
			  }
			  
			
			  out.close();
			  out2.close();
			  
				System.out.println(">>>>>>>>>>>>>>> Processing User Variables");

		}
		catch(Exception e)
		{
			System.err.println("SOOMF: cant not open file for crit_vars.txt");
			System.exit(0);
		}

	}

}
