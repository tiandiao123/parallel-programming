////////////////////////////////////////////////////////////////////////////////
//
//    $Id: SealedDES.java,v 1.1 2008/09/10 20:21:47 randal Exp $
//
//    Randal C. Burns
//    Department of Computer Science
//    Johns Hopkins University
//
//    $Source: /home/randal/repository/public_html/420/src/SealedDES.java,v $
//    $Date: 2008/09/10 20:21:47 $        
//    $Revision: 1.1 $
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//  class: SealedDES
//
//  SealedDES encapsulates the DES encryption and decryption of Strings 
//  into SealedObjects.  It represesnts keys as integers (for simplicity).
//  
//  The main function gives and example of how to:
//    (1) generate a random 24 bit key by starting with a zero valued
//          8 bytes (64 bit key) and then encoding a string with that key
//    (2) perform a brute force search for that key and exame the 
//          resulting output for a known portion of plaintext (in this
//          case "Hopkins".
//
//  Your assignment will be to parallelize this process.
//
////////////////////////////////////////////////////////////////////////////////

import java.util.*;
import java.lang.*;
import javax.crypto.*;
import java.io.File;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.*;
import javax.crypto.spec.*;
import java.util.Random;

import java.io.PrintStream;



class BruteForceDES
{
    // Cipher for the class
    Cipher des_cipher;

    // Key for the class
    SecretKeySpec the_key = null;

    // Byte arrays that hold key block
    byte[] deskeyIN = new byte[8];
    byte[] deskeyOUT = new byte[8];

    // Constructor: initialize the cipher
    public BruteForceDES ()
    {
        try
        {
            des_cipher = Cipher.getInstance("DES");
        }
        catch ( Exception e )
        {
            System.out.println("Failed to create cipher.  Exception: " + e.toString() +
                    " Message: " + e.getMessage()) ;
        }
    }

    // Decrypt the SealedObject
    //
    //   arguments: SealedObject that holds on encrypted String
    //   returns: plaintext String or null if a decryption error
    //     This function will often return null when using an incorrect key.
    //
    public String decrypt ( SealedObject cipherObj )
    {
        try
        {
            return (String)cipherObj.getObject(the_key);
        }
        catch ( Exception e )
        {
            //      System.out.println("Failed to decrypt message. " + ". Exception: " + e.toString()  + ". Message: " + e.getMessage()) ;
        }
        return null;
    }

    // Encrypt the message
    //
    //  arguments: a String to be encrypted
    //  returns: a SealedObject containing the encrypted string
    //
    public SealedObject encrypt ( String plainstr )
    {
        try
        {
            des_cipher.init ( Cipher.ENCRYPT_MODE, the_key );
            return new SealedObject( plainstr, des_cipher );
        }
        catch ( Exception e )
        {
            System.out.println("Failed to encrypt message. " + plainstr +
                    ". Exception: " + e.toString() + ". Message: " + e.getMessage()) ;
        }
        return null;
    }

    //  Build a DES formatted key
    //
    //  Convert an array of 7 bytes into an array of 8 bytes.
    //
    private static void makeDESKey(byte[] in, byte[] out)
    {
        out[0] = (byte) ((in[0] >> 1) & 0xff);
        out[1] = (byte) ((((in[0] & 0x01) << 6) | (((in[1] & 0xff)>>2) & 0xff)) & 0xff);
        out[2] = (byte) ((((in[1] & 0x03) << 5) | (((in[2] & 0xff)>>3) & 0xff)) & 0xff);
        out[3] = (byte) ((((in[2] & 0x07) << 4) | (((in[3] & 0xff)>>4) & 0xff)) & 0xff);
        out[4] = (byte) ((((in[3] & 0x0F) << 3) | (((in[4] & 0xff)>>5) & 0xff)) & 0xff);
        out[5] = (byte) ((((in[4] & 0x1F) << 2) | (((in[5] & 0xff)>>6) & 0xff)) & 0xff);
        out[6] = (byte) ((((in[5] & 0x3F) << 1) | (((in[6] & 0xff)>>7) & 0xff)) & 0xff);
        out[7] = (byte) (   in[6] & 0x7F);

        for (int i = 0; i < 8; i++) {
            out[i] = (byte) (out[i] << 1);
        }
    }

    // Set the key (convert from a long integer)
    public void setKey ( long theKey )
    {
        try
        {
            // convert the integer to the 8 bytes required of keys
            deskeyIN[0] = (byte) (theKey        & 0xFF );
            deskeyIN[1] = (byte)((theKey >>  8) & 0xFF );
            deskeyIN[2] = (byte)((theKey >> 16) & 0xFF );
            deskeyIN[3] = (byte)((theKey >> 24) & 0xFF );
            deskeyIN[4] = (byte)((theKey >> 32) & 0xFF );
            deskeyIN[5] = (byte)((theKey >> 40) & 0xFF );
            deskeyIN[6] = (byte)((theKey >> 48) & 0xFF );

            // theKey should never be larger than 56-bits, so this should always be 0
            deskeyIN[7] = (byte)((theKey >> 56) & 0xFF );

            // turn the 56-bits into a proper 64-bit DES key
            makeDESKey(deskeyIN, deskeyOUT);

            // Create the specific key for DES
            the_key = new SecretKeySpec ( deskeyOUT, "DES" );
        }
        catch ( Exception e )
        {
            System.out.println("Failed to assign key" +  theKey +
                    ". Exception: " + e.toString() + ". Message: " + e.getMessage()) ;
        }
    }


    // Program demonstrating how to create a random key and then search for the key value.
    public static void main ( String[] args ) throws IOException
    {
        if ( 3 != args.length )
        {
            System.out.println ("Usage: java SealedDES key_size_in_bits filename");
            return;
        }

        for(String str:args){
        	System.out.print(str+" ");
        }
        int numOfThreads = Integer.parseInt(args[0]);
        // create object to printf to the console
        PrintStream p = new PrintStream(System.out);

        // Get the argument
        long keybits = Long.parseLong ( args[1] );

        long maxkey = ~(0L);
        maxkey = maxkey >>> (64 - keybits);

        // Create a simple cipher
        BruteForceDES enccipher = new BruteForceDES ();

        // Get a number between 0 and 2^64 - 1
        Random generator = new Random ();
        long key =  generator.nextLong();

        // Mask off the high bits so we get a short key
        key = key & maxkey;

        // Set up a key
        enccipher.setKey ( key );

        // Get the filename
        String filename = args[2];
        // Read in the file to encrypt
        File inputFile = new File(filename);

        // Turn it into a string
        if (!inputFile.exists()) {
            System.err.println("error: Input file not found.");
            System.exit(1);
        }
        byte[] encoded = Files.readAllBytes(Paths.get(filename));

        String plainstr = new String(encoded, StandardCharsets.US_ASCII);

        // Encrypt
        SealedObject sldObj = enccipher.encrypt ( plainstr );

        // Here ends the set-up.  Pretending like we know nothing except sldObj,
        // discover what key was used to encrypt the message.

        // Get and store the current time -- for timing
        long runstart;
        runstart = System.currentTimeMillis();

        List<Thread> listOfThreads = new ArrayList<>();
        for(int i=0;i<numOfThreads;i++){
            //SealedObject sldObj = enccipher.encrypt ( plainstr );
            Thread add_thread = new Thread(new myThread("Thread-"+(i+1),key,sldObj,plainstr,runstart,maxkey,numOfThreads));
            listOfThreads.add(add_thread);
        }
        
        for(Thread th:listOfThreads){
               th.start();
        }

        for(Thread th:listOfThreads){
              try {
                  th.join();
              }catch(InterruptedException ie){
                  ie.printStackTrace();
              }
        }

        // Output search time
        long elapsed = System.currentTimeMillis() - runstart;
        long keys = maxkey + 1;
        System.out.println ( "Completed search of " + keys + " keys at " + elapsed + " milliseconds.");
    }


    static class myThread implements Runnable{

         private String name;
         private long maxkey;
         private String plainstr;
         long runstart;
         long key;

         //SealedDES enccipher;
         SealedObject sldObj;
         BruteForceDES deccipher;
         //BruteForceDES enccipher;

         int numOfThreads;
         long bucket_size;

         int thread_id = 0;


         myThread(String name,long key,SealedObject sldObj,String plainstr,long runstart,long maxkey,int numOfThreads){
             //deccipher = new BruteForceDES();
             this.name = name;
             this.key = key;
             // enccipher = new BruteForceDES();
             // enccipher.setKey(key);



             this.plainstr = plainstr;
             this.sldObj = sldObj;
             this.runstart = runstart;
             
             this.sldObj = sldObj;
             
             this.maxkey = maxkey;
             this.numOfThreads = numOfThreads;
             this.thread_id = Integer.parseInt(name.split("-")[1]);

             if(thread_id==numOfThreads){
                this.bucket_size = maxkey - (maxkey/numOfThreads)*(numOfThreads-1);
             }else{
                this.bucket_size = maxkey/numOfThreads;
             }
             

             deccipher = new BruteForceDES ();
         
         }

         @Override
         public void run(){
             System.out.println(name + " begin run() function");
             long start_index = (maxkey/numOfThreads)*(thread_id-1);
             
             for(long i= start_index;i < start_index+bucket_size;i++){
                  deccipher.setKey ( i );
                  String decryptstr = deccipher.decrypt ( sldObj );

                if (( decryptstr != null ) && ( decryptstr.contains(plainstr))){
                     //  Remote printlns if running for time.
                     //p.printf("Found decrypt key %016x producing message: %s\n", i , decryptstr);
                	 long elapsed = System.currentTimeMillis() - runstart;
                     System.out.println (name + " Found decrypt key " + i + " in " + elapsed + " milliseconds, and " 
                     	+ " producing message: " + decryptstr);
                }

                if ( i % 100000 == 0 ){
                     long elapsed = System.currentTimeMillis() - runstart;
                     System.out.println ( name + " Searched key number " + i + " at " + elapsed + " milliseconds.");
                }

             }

         }


    }
}

////////////////////////////////////////////////////////////////////////////////
//
//  Revsion History 
//    
//  $Log: SealedDES.java,v $
//  Revision 1.1  2008/09/10 20:21:47  randal
//  Initial checkin.
//
//  Revision 1.1  2008/09/05 20:31:11  randal
//  Checkin.
//
////////////////////////////////////////////////////////////////////////////////
