package ca.uwaterloo.esg.rvtool.TTRV.CfgParser;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.util.*;

public class ILParser {
	
	
	private static void SortLoops(Vector<DFSLoop> dfsloop)
	{
		boolean swapped = false;
		do
		{
			swapped = false;
			for(int i =1; i < dfsloop.size(); i++)
			{
				if(dfsloop.get(i).loop.size() > dfsloop.get(i-1).loop.size())
				{
					DFSLoop temp1 = dfsloop.get(i-1);
					dfsloop.remove(i-1);
					dfsloop.insertElementAt(temp1, i);
					swapped = true;	
				}
			}
			
		}while(swapped == true);			
	}
	
	
	/**	
	 � * This Function writes out the ILP model for solving the optimization problem
	 � * @param	: dir (Directory of monitored program),graph (Structure containing the CFG),fileOut (output file), limit (the minimum intended LSP), dfsLoops(Structure containing loop structures),dummies (Structure containing the dummy nodes)
	 � * @return : void	
	*/
	public void ILP_lpsolve(String dir,Vector<Node> graph, String fileOut, int limit, Vector<DFSLoop> dfsLoop, Vector<Dummy> dummies)
	{
		System.out.println(">>>>>>>>>>>>>> Creating ILP Model");

		Vector<YTracker> yVector = new Vector<YTracker>();
		int ytracker = 0;
		int[] degreeArray;
		degreeArray = new int[dummies.size()*2+graph.size()];
		int[] backArray;
		backArray = new int[dummies.size()*2+graph.size()];
		

		
		try
		{			
			BufferedWriter out = new BufferedWriter(new FileWriter(dir+"/ILP/"+fileOut+".lp")); 
			BufferedWriter out1 = new BufferedWriter(new FileWriter(dir+"/Graph/vertices")); 
			
			BufferedWriter outtest1 = new BufferedWriter(new FileWriter(dir+"/ILP/"+"test1.txt")); 
			BufferedWriter outtest2 = new BufferedWriter(new FileWriter(dir+"/ILP/"+"test2.txt")); 




			
			out.write("min: ");

			
			for(int i = 0; i < dummies.size(); i++)
			{
				out.write("x"+Integer.toString(i*2));
				out.write(" + ");
				
				out1.write("v,"+dummies.get(i).head.label+",x"+Integer.toString(i*2)+"\n");

				out.write("x"+Integer.toString(i*2+1));
				if (graph.size() > 0) {
					out.write(" + ");
				}
				
				out1.write("v,"+dummies.get(i).tail.label+",x"+Integer.toString(i*2+1)+"\n");
				
			}
			
			for(int i = 0; i < graph.size(); i++)
			{
				out.write("x"+Integer.toString(i+dummies.size()*2));
				if(i != graph.size() -1)
				{
					out.write(" + ");
				}
				
				out1.write("v,"+graph.get(i).label+",x"+Integer.toString(i+dummies.size()*2)+"\n");

				
			}
			out.write(";"+"\n");
			
			//---------------- finding the start node
			for(int i = 0; i < dummies.size(); i++)
			{				
				if(dummies.get(i).head.parents.size() == 0)
				{
					out.write("x"+Integer.toString(2*i)+ " = 0; \n");
					
					for(int j = 0; j < dummies.get(i).head.edges.size();j++)
					{
						out.write("e"+Integer.toString(2*i)+"_"+Integer.toString(j)+"_0 = "+Integer.toString(dummies.get(i).head.edges.get(j).weight) + ";\n");
						outtest1.write("e"+Integer.toString(2*i)+"_"+Integer.toString(j)+"_0 \n");
					}
					
					//break;
				}
				
			}
			
			//-------------------- adding constraints for each node
			
			for(int i = 0; i < dummies.size(); i++)
			{
				for(int j = 0; j < dummies.get(i).head.edges.size(); j++)
				{
					boolean backEdge = false;
					if(dummies.get(i).head.loop == true)
					{
						for(int hh = 0; hh < dfsLoop.size();hh++)
						{
							if(dfsLoop.get(hh).source.label == dummies.get(i).head.label && dfsLoop.get(hh).dest.label == dummies.get(i).head.edges.get(j).label)
							{
								backEdge = true;
								break;
							}
						}
					}
					
					if(backEdge == false)
					{
						int index =-1;
						Node child = null;
						
						
						for(int k = 0; k < graph.size(); k++)
						{
							if(graph.get(k).label == dummies.get(i).head.edges.get(j).label)
							{
								index = dummies.size()*2+k;
								child = graph.get(k);
								break;
							}
						}
						
						if(index == -1)
						{
							for(int k = 0; k < dummies.size(); k++)
							{
								if(dummies.get(k).head.label == dummies.get(i).head.edges.get(j).label)
								{
									index = k*2;
									child = dummies.get(k).head;
									break;
								}
								if(dummies.get(k).tail.label == dummies.get(i).head.edges.get(j).label)
								{
									index = k*2+1;
									child = dummies.get(k).tail;
									break;
								}
							}
						}
						
						if(dummies.get(i).head.bfsIndegree == 0)
						{

							int k = 0;
							
							out.write("e"+Integer.toString(i*2)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+" + "+Integer.toString(limit)+" x"+Integer.toString(index)+" >= "+Integer.toString(limit)+";\n");
							outtest1.write("e"+Integer.toString(i*2)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+" \n ");
							
							out.write("y"+Integer.toString(ytracker)+" + "+"y"+Integer.toString(ytracker+1)+" = "+"e"+Integer.toString(i*2)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("y"+Integer.toString(ytracker)+" <= "+"e"+Integer.toString(i*2)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(i*2)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("x"+Integer.toString(index)+" + "+"y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(i*2)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("x"+Integer.toString(index)+" + "+"y"+Integer.toString(ytracker+1)+" >= 1"+";\n");
							//---------- creating the new values
							int edgeCopies = degreeArray[index];

							for(int kk = 0; kk < child.edges.size();kk++)
							{
								
								backEdge = false;
								if(child.loop == true)
								{
									for(int hh = 0; hh < dfsLoop.size();hh++)
									{
										if(dfsLoop.get(hh).source.label == child.label && dfsLoop.get(hh).dest.label ==  child.edges.get(kk).label)
										{
											backEdge = true;
											break;
										}
									}
								}
								
								if(backEdge == false)
								{
									out.write("-y"+Integer.toString(ytracker)+" + "+"e"+Integer.toString(index)+"_"+Integer.toString(kk)+"_"+Integer.toString(edgeCopies)+" - "+Integer.toString(child.edges.get(kk).weight)+" = 0 ;\n");
									outtest2.write("e"+Integer.toString(index)+"_"+Integer.toString(kk)+"_"+Integer.toString(edgeCopies)+"\n");
								}
								else
								{

									
									boolean yFind = false;
									
									for(int ll = 0; ll < yVector.size();ll++)
									{
										if(yVector.get(ll).source == child.label && yVector.get(ll).dest == child.edges.get(kk).label)
										{
											yFind = true;
											yVector.get(ll).yList.add("-y"+Integer.toString(ytracker));
										}
									}
									
									if(yFind == false)
									{
										YTracker yTrack = new YTracker();
										yTrack.yList.add("-y"+Integer.toString(ytracker));
										yTrack.dest = child.edges.get(kk).label;
										yTrack.source = child.label;
										yVector.add(yTrack);
									}
									
								}

							}
							
							edgeCopies++;
							degreeArray[index] = edgeCopies;
							
							ytracker = ytracker +2;
						
						}
						
						for(int k = 0; k < dummies.get(i).head.bfsIndegree;k++)
						{
							out.write("e"+Integer.toString(i*2)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+" + "+Integer.toString(limit)+" x"+Integer.toString(index)+" >= "+Integer.toString(limit)+";\n");
							outtest1.write("e"+Integer.toString(i*2)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+" \n ");
							
							out.write("y"+Integer.toString(ytracker)+" + "+"y"+Integer.toString(ytracker+1)+" = "+"e"+Integer.toString(i*2)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("y"+Integer.toString(ytracker)+" <= "+"e"+Integer.toString(i*2)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(i*2)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("x"+Integer.toString(index)+" + "+"y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(i*2)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("x"+Integer.toString(index)+" + "+"y"+Integer.toString(ytracker+1)+" >= 1"+";\n");
							//---------- creating the new values
							int edgeCopies = degreeArray[index];

							for(int kk = 0; kk < child.edges.size();kk++)
							{
								
								backEdge = false;
								if(child.loop == true)
								{
									for(int hh = 0; hh < dfsLoop.size();hh++)
									{
										if(dfsLoop.get(hh).source.label == child.label && dfsLoop.get(hh).dest.label ==  child.edges.get(kk).label)
										{
											backEdge = true;
											break;
										}
									}
								}
								
								if(backEdge == false)
								{
									out.write("-y"+Integer.toString(ytracker)+" + "+"e"+Integer.toString(index)+"_"+Integer.toString(kk)+"_"+Integer.toString(edgeCopies)+" - "+Integer.toString(child.edges.get(kk).weight)+" = 0 ;\n");
									outtest2.write("e"+Integer.toString(index)+"_"+Integer.toString(kk)+"_"+Integer.toString(edgeCopies)+"\n");
								}
								else
								{

									
									boolean yFind = false;
									
									for(int ll = 0; ll < yVector.size();ll++)
									{
										if(yVector.get(ll).source == child.label && yVector.get(ll).dest == child.edges.get(kk).label)
										{
											yFind = true;
											yVector.get(ll).yList.add("-y"+Integer.toString(ytracker));
										}
									}
									
									if(yFind == false)
									{
										YTracker yTrack = new YTracker();
										yTrack.yList.add("-y"+Integer.toString(ytracker));
										yTrack.dest = child.edges.get(kk).label;
										yTrack.source = child.label;
										yVector.add(yTrack);
									}
									
								}

							}
							
							edgeCopies++;
							degreeArray[index] = edgeCopies;
							
							ytracker = ytracker +2;
						}
					}
				}
				
				//---------- for the tail
				
				for(int j = 0; j < dummies.get(i).tail.edges.size(); j++)
				{
					int index =-1;
					Node child = null;
					
					boolean backEdge = false;
					if(dummies.get(i).tail.loop == true)
					{
						for(int hh = 0; hh < dfsLoop.size();hh++)
						{
							if(dfsLoop.get(hh).source.label == dummies.get(i).tail.label && dfsLoop.get(hh).dest.label == dummies.get(i).tail.edges.get(j).label)
							{
								backEdge = true;
								break;
							}
						}
					}
					
					if(backEdge == false)
					{

						
						for(int k = 0; k < graph.size(); k++)
						{
							if(graph.get(k).label == dummies.get(i).tail.edges.get(j).label)
							{
								index = dummies.size()*2+k;
								child = graph.get(k);
								break;
							}
						}
						
						if(index == -1)
						{
							for(int k = 0; k < dummies.size(); k++)
							{
								if(dummies.get(k).head.label == dummies.get(i).tail.edges.get(j).label)
								{
									index = k*2;
									child = dummies.get(k).head;
									break;
								}
								if(dummies.get(k).tail.label == dummies.get(i).tail.edges.get(j).label)
								{
									index = k*2+1;
									child = dummies.get(k).tail;
									break;
								}
							}
						}
						
						
						if(dummies.get(i).tail.bfsIndegree == 0)
						{
							int k = 0;

							out.write("e"+Integer.toString(2*i+1)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+" + "+Integer.toString(limit)+" x"+Integer.toString(index)+" >= "+Integer.toString(limit)+";\n");
							outtest1.write("e"+Integer.toString(2*i+1)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+" \n");
							
							out.write("y"+Integer.toString(ytracker)+" + "+"y"+Integer.toString(ytracker+1)+" = "+"e"+Integer.toString(2*i+1)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("y"+Integer.toString(ytracker)+" <= "+"e"+Integer.toString(2*i+1)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(2*i+1)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("x"+Integer.toString(index)+" + "+"y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(2*i+1)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("x"+Integer.toString(index)+" + "+"y"+Integer.toString(ytracker+1)+" >= 1"+";\n");
							
							//---------- creating the new values
							int edgeCopies = degreeArray[index];

							for(int kk = 0; kk < child.edges.size();kk++)
							{
								backEdge = false;
								if(child.loop == true)
								{
									for(int hh = 0; hh < dfsLoop.size();hh++)
									{
										if(dfsLoop.get(hh).source.label == child.label && dfsLoop.get(hh).dest.label ==  child.edges.get(kk).label)
										{
											backEdge = true;
											break;
										}
									}
								}
								
								if(backEdge == false)
								{
									out.write("-y"+Integer.toString(ytracker)+" + "+"e"+Integer.toString(index)+"_"+Integer.toString(kk)+"_"+Integer.toString(edgeCopies)+" - "+Integer.toString(child.edges.get(kk).weight)+" = 0 ;\n");
									outtest2.write("e"+Integer.toString(index)+"_"+Integer.toString(kk)+"_"+Integer.toString(edgeCopies)+"\n");
								}
								else
								{
									
									boolean yFind = false;
									
									for(int ll = 0; ll < yVector.size();ll++)
									{
										if(yVector.get(ll).source == child.label && yVector.get(ll).dest == child.edges.get(kk).label)
										{
											yFind = true;
											yVector.get(ll).yList.add("-y"+Integer.toString(ytracker));
										}
									}
									
									if(yFind == false)
									{
										YTracker yTrack = new YTracker();
										yTrack.yList.add("-y"+Integer.toString(ytracker));
										yTrack.dest = child.edges.get(kk).label;
										yTrack.source = child.label;
										yVector.add(yTrack);
									}

									
									
								}

							}
							
							edgeCopies++;
							degreeArray[index] = edgeCopies;
							
							ytracker = ytracker +2;
						
						}
						
						for(int k = 0; k < dummies.get(i).tail.bfsIndegree;k++)
						{
							out.write("e"+Integer.toString(2*i+1)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+" + "+Integer.toString(limit)+" x"+Integer.toString(index)+" >= "+Integer.toString(limit)+";\n");
							outtest1.write("e"+Integer.toString(2*i+1)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+" \n");
							
							out.write("y"+Integer.toString(ytracker)+" + "+"y"+Integer.toString(ytracker+1)+" = "+"e"+Integer.toString(2*i+1)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("y"+Integer.toString(ytracker)+" <= "+"e"+Integer.toString(2*i+1)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(2*i+1)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("x"+Integer.toString(index)+" + "+"y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(2*i+1)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("x"+Integer.toString(index)+" + "+"y"+Integer.toString(ytracker+1)+" >= 1"+";\n");
							
							//---------- creating the new values
							int edgeCopies = degreeArray[index];

							for(int kk = 0; kk < child.edges.size();kk++)
							{
								backEdge = false;
								if(child.loop == true)
								{
									for(int hh = 0; hh < dfsLoop.size();hh++)
									{
										if(dfsLoop.get(hh).source.label == child.label && dfsLoop.get(hh).dest.label ==  child.edges.get(kk).label)
										{
											backEdge = true;
											break;
										}
									}
								}
								
								if(backEdge == false)
								{
									out.write("-y"+Integer.toString(ytracker)+" + "+"e"+Integer.toString(index)+"_"+Integer.toString(kk)+"_"+Integer.toString(edgeCopies)+" - "+Integer.toString(child.edges.get(kk).weight)+" = 0 ;\n");
									outtest2.write("e"+Integer.toString(index)+"_"+Integer.toString(kk)+"_"+Integer.toString(edgeCopies)+"\n");
								}
								else
								{
									
									boolean yFind = false;
									
									for(int ll = 0; ll < yVector.size();ll++)
									{
										if(yVector.get(ll).source == child.label && yVector.get(ll).dest == child.edges.get(kk).label)
										{
											yFind = true;
											yVector.get(ll).yList.add("-y"+Integer.toString(ytracker));
										}
									}
									
									if(yFind == false)
									{
										YTracker yTrack = new YTracker();
										yTrack.yList.add("-y"+Integer.toString(ytracker));
										yTrack.dest = child.edges.get(kk).label;
										yTrack.source = child.label;
										yVector.add(yTrack);
									}

									
									
								}

							}
							
							edgeCopies++;
							degreeArray[index] = edgeCopies;
							
							ytracker = ytracker +2;
						}
					}

				}
				
			}
			
			
			//-------- adding constraints for the graph nodes
			
			for(int i = 0; i < graph.size(); i++)
			{
				for(int j = 0; j < graph.get(i).edges.size(); j++)
				{
					int index =-1;
					Node child = null;
					
					boolean backEdge = false;
					if(graph.get(i).loop == true)
					{
						for(int hh = 0; hh < dfsLoop.size();hh++)
						{
							if(dfsLoop.get(hh).source.label == graph.get(i).label && dfsLoop.get(hh).dest.label == graph.get(i).edges.get(j).label)
							{
								backEdge = true;
								break;
							}
						}
					}
					
					if(backEdge == false)
					{
						for(int k = 0; k < graph.size(); k++)
						{
							if(graph.get(k).label == graph.get(i).edges.get(j).label)
							{
								index = dummies.size()*2+k;
								child = graph.get(k);
								break;
							}
						}
						
						if(index == -1)
						{
							for(int k = 0; k < dummies.size(); k++)
							{
								if(dummies.get(k).head.label == graph.get(i).edges.get(j).label)
								{
									index = k*2;
									child = dummies.get(k).head;
									break;
								}
								if(dummies.get(k).tail.label == graph.get(i).edges.get(j).label)
								{
									index = k*2+1;
									child = dummies.get(k).tail;
									break;
								}
							}
						}
						
						if(graph.get(i).bfsIndegree == 0)
						{

							int k =0;
							out.write("e"+Integer.toString(dummies.size()*2+i)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+" + "+Integer.toString(limit)+" x"+Integer.toString(index)+" >= "+Integer.toString(limit)+";\n");
							outtest1.write("e"+Integer.toString(dummies.size()*2+i)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+" \n ");
							
							out.write("y"+Integer.toString(ytracker)+" + "+"y"+Integer.toString(ytracker+1)+" = "+"e"+Integer.toString(dummies.size()*2+i)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("y"+Integer.toString(ytracker)+" <= "+"e"+Integer.toString(dummies.size()*2+i)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(dummies.size()*2+i)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("x"+Integer.toString(index)+" + "+"y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(dummies.size()*2+i)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("x"+Integer.toString(index)+" + "+"y"+Integer.toString(ytracker+1)+" >= 1"+";\n");
							
							
							//---------- creating the new values
							int edgeCopies = degreeArray[index];

							for(int kk = 0; kk < child.edges.size();kk++)
							{
								backEdge = false;
								if(child.loop == true)
								{
									for(int hh = 0; hh < dfsLoop.size();hh++)
									{
										if(dfsLoop.get(hh).source.label == child.label && dfsLoop.get(hh).dest.label ==  child.edges.get(kk).label)
										{
											backEdge = true;
											break;
										}
									}
								}
								
								if(backEdge == false)
								{
									out.write("-y"+Integer.toString(ytracker)+" + "+"e"+Integer.toString(index)+"_"+Integer.toString(kk)+"_"+Integer.toString(edgeCopies)+" - "+Integer.toString(child.edges.get(kk).weight)+" = 0 ;\n");
									outtest2.write("e"+Integer.toString(index)+"_"+Integer.toString(kk)+"_"+Integer.toString(edgeCopies)+"\n");
								}
								else
								{
									boolean yFind = false;
									
									for(int ll = 0; ll < yVector.size();ll++)
									{
										if(yVector.get(ll).source == child.label && yVector.get(ll).dest == child.edges.get(kk).label)
										{
											yFind = true;
											yVector.get(ll).yList.add("-y"+Integer.toString(ytracker));
										}
									}
									
									if(yFind == false)
									{
										YTracker yTrack = new YTracker();
										yTrack.yList.add("-y"+Integer.toString(ytracker));
										yTrack.dest = child.edges.get(kk).label;
										yTrack.source = child.label;
										yVector.add(yTrack);
									}
									
								}
							}
							
							edgeCopies++;
							degreeArray[index] = edgeCopies;
							
							ytracker = ytracker +2;
						
						}
						
						for(int k = 0; k < graph.get(i).bfsIndegree;k++)
						{
							out.write("e"+Integer.toString(dummies.size()*2+i)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+" + "+Integer.toString(limit)+" x"+Integer.toString(index)+" >= "+Integer.toString(limit)+";\n");
							outtest1.write("e"+Integer.toString(dummies.size()*2+i)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+" \n ");
							
							out.write("y"+Integer.toString(ytracker)+" + "+"y"+Integer.toString(ytracker+1)+" = "+"e"+Integer.toString(dummies.size()*2+i)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("y"+Integer.toString(ytracker)+" <= "+"e"+Integer.toString(dummies.size()*2+i)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(dummies.size()*2+i)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("x"+Integer.toString(index)+" + "+"y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(dummies.size()*2+i)+"_"+Integer.toString(j)+"_"+Integer.toString(k)+";\n");
							out.write("x"+Integer.toString(index)+" + "+"y"+Integer.toString(ytracker+1)+" >= 1"+";\n");
							
							
							//---------- creating the new values
							int edgeCopies = degreeArray[index];

							for(int kk = 0; kk < child.edges.size();kk++)
							{
								backEdge = false;
								if(child.loop == true)
								{
									for(int hh = 0; hh < dfsLoop.size();hh++)
									{
										if(dfsLoop.get(hh).source.label == child.label && dfsLoop.get(hh).dest.label ==  child.edges.get(kk).label)
										{
											backEdge = true;
											break;
										}
									}
								}
								
								if(backEdge == false)
								{
									out.write("-y"+Integer.toString(ytracker)+" + "+"e"+Integer.toString(index)+"_"+Integer.toString(kk)+"_"+Integer.toString(edgeCopies)+" - "+Integer.toString(child.edges.get(kk).weight)+" = 0 ;\n");
									outtest2.write("e"+Integer.toString(index)+"_"+Integer.toString(kk)+"_"+Integer.toString(edgeCopies)+"\n");
								}
								else
								{
									boolean yFind = false;
									
									for(int ll = 0; ll < yVector.size();ll++)
									{
										if(yVector.get(ll).source == child.label && yVector.get(ll).dest == child.edges.get(kk).label)
										{
											yFind = true;
											yVector.get(ll).yList.add("-y"+Integer.toString(ytracker));
										}
									}
									
									if(yFind == false)
									{
										YTracker yTrack = new YTracker();
										yTrack.yList.add("-y"+Integer.toString(ytracker));
										yTrack.dest = child.edges.get(kk).label;
										yTrack.source = child.label;
										yVector.add(yTrack);
									}
									
								}
							}
							
							edgeCopies++;
							degreeArray[index] = edgeCopies;
							
							ytracker = ytracker +2;
						}
					}
					
				}
				
			}
			
			///------------------- considering the back edges and loops
			
			SortLoops(dfsLoop);
			
			
			for(int kk = 0; kk < dfsLoop.size(); kk++)
			{
				DFSLoop dfsL = dfsLoop.get(kk);
				int numBE = 1;
				
								
				for(int j = 0; j < dfsL.loop.size(); j++)
				{
					Node nodeDFS = dfsL.loop.get(j);
					
					if(nodeDFS.label == dfsL.source.label)
					{
						numBE = nodeDFS.bfsIndegree;


						int index1 = -1;
						int index2 = -1;
						Node child = null;
						
						for(int k = 0; k < graph.size(); k++)
						{
							if(graph.get(k).label == nodeDFS.label)
							{
								index1 = dummies.size()*2+k;
								break;
							}
						}
						
						if(index1 == -1)
						{
							for(int k = 0; k < dummies.size(); k++)
							{
								if(dummies.get(k).head.label == nodeDFS.label)
								{
									index1 = k*2;
									break;
								}
								if(dummies.get(k).tail.label == nodeDFS.label)
								{
									index1 = k*2+1;
									break;
								}
							}
						}
						
						
						int backCopies = backArray[index1];
						
						
				
						for(int jj = 0; jj < nodeDFS.edges.size(); jj++)
						{
							if(nodeDFS.edges.get(jj).label == dfsL.dest.label)
							{
								

								int nodeLable = dfsL.dest.label;

								for(int k = 0; k < graph.size(); k++)
								{
									if(graph.get(k).label == nodeLable)
									{
										index2 = dummies.size()*2+k;
										child = graph.get(k);
										break;
									}
								}
								if(index2 == -1)
								{
									for(int k = 0; k < dummies.size(); k++)
									{
										if(dummies.get(k).head.label == nodeLable)
										{
											index2 = k*2;
											child = dummies.get(k).head;

											break;
										}
										if(dummies.get(k).tail.label == nodeLable)
										{
											index2 = k*2+1;
											child = dummies.get(k).tail;

											break;
										}
									}
								}
								
								
								
								
								boolean backEdge = false;
								
								if(backEdge == false)
								{
									YTracker yTrack = null;
									
									for(int ll =0 ; ll < yVector.size(); ll++)
									{
										if(yVector.get(ll).source == dfsL.source.label && yVector.get(ll).dest == dfsL.dest.label)
										{
											yTrack = yVector.get(ll);
											break;
										}
									}
									
									if(yTrack.yList.size() < numBE)
									{
										System.err.println("SOOMF: ERROR IN THE HOUSE ");
										System.exit(-1);
									}

									for(int ll = 0; ll < yTrack.yList.size(); ll++)
									{

										//---------- creating the new values

										out.write(yTrack.yList.get(ll)+" + "+"e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+" - "+Integer.toString(nodeDFS.edges.get(jj).weight)+" = 0 ;\n");

										//----------------------------------------------------
										
										
										
										out.write("e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+" + "+Integer.toString(limit)+" x"+Integer.toString(index2)+" >= "+Integer.toString(limit)+";\n");
										outtest1.write("e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+" \n ");
										
										out.write("y"+Integer.toString(ytracker)+" + "+"y"+Integer.toString(ytracker+1)+" = "+"e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+";\n");
										out.write("y"+Integer.toString(ytracker)+" <= "+"e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+";\n");
										out.write("y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+";\n");
										out.write("x"+Integer.toString(index2)+" + "+"y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+";\n");
										out.write("x"+Integer.toString(index2)+" + "+"y"+Integer.toString(ytracker+1)+" >= 1"+";\n");
										
										
										//---------- creating the new values
										int backCopies2 = backArray[index2];
										
										
										

										for(int k2 = 0; k2 < child.edges.size();k2++)
										{
											

											boolean backE = false;
											if(child.loop == true)
											{
												for(int hh = 0; hh < dfsLoop.size();hh++)
												{
													if(dfsLoop.get(hh).source.label == child.label && dfsLoop.get(hh).dest.label ==  child.edges.get(k2).label)
													{
														backE = true;
														break;
													}
												}
											}
											
											if(backE == false)
											{
												boolean find2 = false;
												int indexN = 0;
												
												for(int f = 0; f < dfsL.loop.size(); f++)
												{
													if(dfsL.loop.get(f).label == child.label)
													{
														indexN = f;
														break;
													}
												}
												for(int f = 0; f < dfsL.loop.size(); f++)
												{

													
													if(child.edges.get(k2).label == dfsL.loop.get(f).label)
													{
														
														if(f > indexN+1)
														{
															find2 = false;
														}
														else
														{
															find2 = true;
														}
														break;
													}
												}
												
												if(find2 == true)
												{
													out.write("-y"+Integer.toString(ytracker)+" + "+"e"+Integer.toString(index2)+"_"+Integer.toString(k2)+"_B_"+Integer.toString(backCopies2)+"_"+Integer.toString(ll)+" - "+Integer.toString(child.edges.get(k2).weight)+" = 0 ;\n");
													outtest2.write("e"+Integer.toString(index2)+"_"+Integer.toString(k2)+"_B_"+Integer.toString(backCopies2)+"_"+Integer.toString(ll)+"\n");
												}
											}
											else
											{
												//System.out.println("--- Adding a tracker to "+ child.label + " for "+child.edges.get(k2).label + " y is "+"-y"+Integer.toString(ytracker));
												
											}

										}
										
										
										ytracker = ytracker +2;
										
									
									}
									
								}
							}
							
							

						}
						
						backCopies++;
						backArray[index1] = backCopies;
								
						

					}
				}
								
				for(int j = 0; j < dfsL.loop.size(); j++)
				{
					Node nodeDFS = dfsL.loop.get(j);
					
					if(nodeDFS.label != dfsL.source.label)
					{
						int index1 = -1;
						int index2 = -1;
						Node child = null;
						
						for(int k = 0; k < graph.size(); k++)
						{
							if(graph.get(k).label == nodeDFS.label)
							{
								index1 = dummies.size()*2+k;
								break;
							}
						}
						if(index1 == -1)
						{
							for(int k = 0; k < dummies.size(); k++)
							{
								if(dummies.get(k).head.label == nodeDFS.label)
								{
									index1 = k*2;
									break;
								}
								if(dummies.get(k).tail.label == nodeDFS.label)
								{
									index1 = k*2+1;
									break;
								}
							}
						}
						
						
						int backCopies = backArray[index1]; 

						
						for(int jj = 0; jj < nodeDFS.edges.size(); jj++)
						{
							int nodeLabel = nodeDFS.edges.get(jj).label;

							
							boolean find = false;
							for(int f = 0; f < dfsL.loop.size(); f++)
							{
								if(nodeLabel == dfsL.loop.get(f).label)
								{
									
									if(f == j+1)
									{
										find = true;
									}
									else
									{
										find = false;
									}
									break;
								}
							}
							
							if(find == true)
							{

								for(int k = 0; k < graph.size(); k++)
								{
									if(graph.get(k).label == nodeLabel)
									{
										index2 = dummies.size()*2+k;
										child = graph.get(k);
										break;
									}
								}
								if(index2 == -1)
								{
									for(int k = 0; k < dummies.size(); k++)
									{
										if(dummies.get(k).head.label == nodeLabel)
										{
											index2 = k*2;
											child = dummies.get(k).head;

											break;
										}
										if(dummies.get(k).tail.label == nodeLabel)
										{
											index2 = k*2+1;
											child = dummies.get(k).tail;

											break;
										}
									}
								}
								
								
								boolean BEdge = false;
								
								if(child.loop == true)
								{
									for(int hh = 0; hh < dfsLoop.size();hh++)
									{
										if(dfsLoop.get(hh).source.label == nodeDFS.label && dfsLoop.get(hh).dest.label ==  child.label)
										{
											BEdge = true;
											break;
										}
									}
								}
								
								if(BEdge == false)
								{
									
									/*if(numBE == 0)
									{


										int ll = 0;
										
										//---------- creating the new values
										
										out.write("e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+" + "+Integer.toString(limit)+" x"+Integer.toString(index2)+" >= "+Integer.toString(limit)+";\n");
										outtest1.write("e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+" \n ");
										
										out.write("y"+Integer.toString(ytracker)+" + "+"y"+Integer.toString(ytracker+1)+" = "+"e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+";\n");
										out.write("y"+Integer.toString(ytracker)+" <= "+"e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+";\n");
										out.write("y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+";\n");
										out.write("x"+Integer.toString(index2)+" + "+"y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+";\n");
										out.write("x"+Integer.toString(index2)+" + "+"y"+Integer.toString(ytracker+1)+" >= 1"+";\n");
										
										
										//---------- creating the new values
										
										if(child.label != dfsL.source.label)
										{
											int backCopies2 = backArray[index2];

											for(int k2 = 0; k2 < child.edges.size();k2++)
											{
												boolean backE = false;
												if(child.loop == true)
												{
													for(int hh = 0; hh < dfsLoop.size();hh++)
													{
														if(dfsLoop.get(hh).source.label == child.label && dfsLoop.get(hh).dest.label ==  child.edges.get(k2).label)
														{
															backE = true;
															break;
														}
													}
												}
												
												if(backE == false)
												{
													
													int indexN = 0;
													
													for(int f = 0; f < dfsL.loop.size(); f++)
													{
														if(dfsL.loop.get(f).label == child.label)
														{
															indexN = f;
															break;
														}
													}
													
													boolean find2 = false;
													for(int f = 0; f < dfsL.loop.size(); f++)
													{
														if(child.edges.get(k2).label == dfsL.loop.get(f).label)
														{
															if(f > indexN+1)
															{
																find2 = false;
															}
															else
															{
																find2 = true;
															}
															break;
														}
													}
													
													if(find2 == true)
													{
														out.write("-y"+Integer.toString(ytracker)+" + "+"e"+Integer.toString(index2)+"_"+Integer.toString(k2)+"_B_"+Integer.toString(backCopies2)+"_"+Integer.toString(ll)+" - "+Integer.toString(child.edges.get(k2).weight)+" = 0 ;\n");
														outtest2.write("e"+Integer.toString(index2)+"_"+Integer.toString(k2)+"_B_"+Integer.toString(backCopies2)+"_"+Integer.toString(ll)+"\n");
													}
												}
												else
												{
													//System.out.println("---(FAlse) Adding a tracker to "+ child.label + " for "+child.edges.get(k2).label + " y is "+"-y"+Integer.toString(ytracker));
													
												}
												
											}
										}

										
						
										
										ytracker = ytracker +2;
										
									
									
									}*/
									
									for(int ll = 0; ll < numBE; ll++)
									{

										
										//---------- creating the new values
										
										out.write("e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+" + "+Integer.toString(limit)+" x"+Integer.toString(index2)+" >= "+Integer.toString(limit)+";\n");
										outtest1.write("e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+" \n ");
										
										out.write("y"+Integer.toString(ytracker)+" + "+"y"+Integer.toString(ytracker+1)+" = "+"e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+";\n");
										out.write("y"+Integer.toString(ytracker)+" <= "+"e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+";\n");
										out.write("y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+";\n");
										out.write("x"+Integer.toString(index2)+" + "+"y"+Integer.toString(ytracker+1)+" <= "+"e"+Integer.toString(index1)+"_"+Integer.toString(jj)+"_B_"+Integer.toString(backCopies)+"_"+Integer.toString(ll)+";\n");
										out.write("x"+Integer.toString(index2)+" + "+"y"+Integer.toString(ytracker+1)+" >= 1"+";\n");
										
										
										//---------- creating the new values
										
										if(child.label != dfsL.source.label)
										{
											int backCopies2 = backArray[index2];

											for(int k2 = 0; k2 < child.edges.size();k2++)
											{
												boolean backE = false;
												if(child.loop == true)
												{
													for(int hh = 0; hh < dfsLoop.size();hh++)
													{
														if(dfsLoop.get(hh).source.label == child.label && dfsLoop.get(hh).dest.label ==  child.edges.get(k2).label)
														{
															backE = true;
															break;
														}
													}
												}
												
												if(backE == false)
												{
													
													int indexN = 0;
													
													for(int f = 0; f < dfsL.loop.size(); f++)
													{
														if(dfsL.loop.get(f).label == child.label)
														{
															indexN = f;
															break;
														}
													}
													
													boolean find2 = false;
													for(int f = 0; f < dfsL.loop.size(); f++)
													{
														if(child.edges.get(k2).label == dfsL.loop.get(f).label)
														{
															if(f > indexN+1)
															{
																find2 = false;
															}
															else
															{
																find2 = true;
															}
															break;
														}
													}
													
													if(find2 == true)
													{
														out.write("-y"+Integer.toString(ytracker)+" + "+"e"+Integer.toString(index2)+"_"+Integer.toString(k2)+"_B_"+Integer.toString(backCopies2)+"_"+Integer.toString(ll)+" - "+Integer.toString(child.edges.get(k2).weight)+" = 0 ;\n");
														outtest2.write("e"+Integer.toString(index2)+"_"+Integer.toString(k2)+"_B_"+Integer.toString(backCopies2)+"_"+Integer.toString(ll)+"\n");
													}
												}
												else
												{
													//System.out.println("---(FAlse) Adding a tracker to "+ child.label + " for "+child.edges.get(k2).label + " y is "+"-y"+Integer.toString(ytracker));
													
												}
												
											}
										}

										
						
										
										ytracker = ytracker +2;
										
									
									}
								
								}
								
								
								
							}
							
							
							}
						backCopies++;
						backArray[index1] = backCopies;

					}

				}
								
			}
			
			
			//--------------------- writing out the integer and binary types
			out.write("bin ");

			for(int i = 0; i < dummies.size()*2; i++)
			{
				out.write("x"+Integer.toString(i));
				out.write(",");

				out.write("x"+Integer.toString(i+1));
				out.write(",");
				
			}
			
			for(int i = 0; i < graph.size(); i++)
			{
				out.write("x"+Integer.toString(i+dummies.size()*2));
				if(i != graph.size() -1)
				{
					out.write(",");
				}
				
			}
			out.write(";"+"\n");
			
			out.write("int ");

			
			for(int i = 0; i < ytracker; i++)
			{
				out.write("y"+Integer.toString(i));
				if(i != ytracker -1)
				{
					out.write(",");
				}
			}
			out.write(";"+"\n");
			
			//------------------------ writing out the SOS
			
			out.write("sos \n ");

			int tracker=0;
			int counter = 0;
			
			while(true)
			{
				if(tracker >= ytracker)
				{
					out.write("\n");
					break;
				}
				out.write("SOS"+Integer.toString(counter)+": "+"y"+Integer.toString(tracker)+","+"y"+Integer.toString(++tracker)+" <= 1 ;\n");
				counter++;
				tracker++;
				
				
			}
			

			out.close();
			out1.close();
			outtest1.close();
			outtest2.close();
			System.out.println(">>>>>>>>>>>>>> ILP Model Complete");

			
			
		}catch(Exception e)
		{
			System.err.println("--- error in ilp");
			e.printStackTrace();
			System.exit(0);
		}
		
	}
}