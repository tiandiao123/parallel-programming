import java.io.IOException;
import java.util.*;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapreduce.*;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;


public class FoF {
	public static class Map extends Mapper<LongWritable, Text, Text, IntWritable> {
		public void map(LongWritable key,Text value,Context context) 
		throws IOException, InterruptedException{
                Text wordOut = new Text();
                IntWritable one = new IntWritable(1);
                String line = value.toString();
                String[] strs = line.split(" ");
                String first_word = strs[0];
                for(int i=1;i<strs.length;i++){
                	for(int j=i+1;j<strs.length;j++){
                		String[] array = new String[]{first_word,strs[i],strs[j]};
                		Arrays.sort(array);
                		String temp = array[0]+"_"+array[1]+"_"+array[2];
                		wordOut.set(temp);
                		context.write(wordOut,one);
                	}
                }
		}
	}
	
	public static class Reduce extends Reducer<Text, IntWritable, Text, IntWritable>{

		public void reduce(Text key, Iterable<IntWritable> ones,Context context) 
		throws IOException, InterruptedException{
               int count = 0;
               Iterator<IntWritable> it = ones.iterator();
               while(it.hasNext()){
               	   count++;
               	   it.next();
               }

               if(count>=3){
               	   String line = key.toString();
               	   String[] strs = line.split("_");
               	   for(int i=0;i<3;i++){
               		   IntWritable one = new IntWritable(1);
               		   String[] temp = new String[]{strs[(i+1)%3],strs[(i+2)%3]};
               	       Arrays.sort(temp);
               	       String keyOut = strs[i] + " " + temp[0] + " " + temp[1];
               	       Text wordOut = new Text(keyOut);
               	       context.write(wordOut,null);
               	   }
               	   
               }
		}


	}

	public static void main(String[] args) throws IOException, ClassNotFoundException, InterruptedException{
		// TODO Auto-generated method stub
		Configuration conf = new Configuration();
		
		@SuppressWarnings({ "deprecation", "unused" })
		Job job = new Job(conf,"FoF");
		
		job.setJarByClass(FoF.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(IntWritable.class);
		
		job.setMapperClass(Map.class);
		job.setReducerClass(Reduce.class);
		
		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);
		
		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileOutputFormat.setOutputPath(job, new Path(args[1]));
		
		job.waitForCompletion(true);
		
	}

}
