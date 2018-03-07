import java.util.*;
import java.lang.*;

class CoinFlip{
     public static int numOfThreads = 0;
     public static int numIterations = 0;
     //public static int[] heads;

     public static void main(String[] args) {
     	   int heads = 0;
     	   long startTime = System.currentTimeMillis();

     	   numOfThreads = Integer.parseInt(args[0]);
     	   numIterations = Integer.parseInt(args[1]);

           List<myThreads> listThreads = new ArrayList<>();
           Thread[] threads = new Thread[numOfThreads];
           for(int i=0;i<numOfThreads;i++){
           	    myThreads createThreads = new myThreads("thread" + i);
           	    listThreads.add(createThreads);
                threads[i] = new Thread(createThreads);
           }

           for(int i=0;i<threads.length;i++){
           	   threads[i].start();
           }

           
          for(int i=0;i<threads.length;i++){
		         try {
		             threads[i].join();
		         }catch (InterruptedException ie) {
		              ie.printStackTrace();
		         }
          }
           
          for(myThreads th:listThreads){
           	   heads += th.getNumHeads();
          }

         System.out.println(heads + " heads in "+ numIterations +" coin tosses");
     	   long endTime = System.currentTimeMillis();
     	   System.out.println("Elaspsed time: "+(endTime-startTime));

     }



  static class myThreads implements Runnable{
          private int myhead = 0;
          private String name;
          Random random;

          myThreads(String name){
               this.name = name;
               random = new Random();
          }
  
		  @Override
		  public void run(){
		        int bucketSize = numIterations/numOfThreads;
		        
		        for(int i=0;i<bucketSize;i++){
		            int headOrTails = random.nextInt(2);
		            if(headOrTails==1){
		            	myhead++;
		            }
		        }
		  }


		  public int getNumHeads(){
		  	return myhead;
		  }

   }

}

