//Gokul Caushik

#include <stdio.h>
#include <stdlib.h>



int main (int argc, char *argv[])
{
   FILE *data;
   FILE *output;
   int first_field_seen = 0;	//flag to check if first field (the number to write out) has been seen
   int number_of_bits;		//number of bits to write out the current number 
   int bits_written = 0;	//number of bits written to the buffer for the current byte	
   int bits_to_shift = 0;	
   int bitmask;
   unsigned char shifted;	
   unsigned int number;	        //current number to be written out
   unsigned char buffer[1] = {0};	//buffer to hold the bits of the current byte before writing it out
   if (argc != 3 ) // argc should be 3 
      printf("usage: %s <data_file> <output_filename>\n", argv[0]);
   else {
    // We assume argv[1] and argv[2] are filenames to open
      data = fopen(argv[1],"r");
      output = fopen(argv[2], "wb");
      if ((!data) || (!output))
         printf("Could not open file\n");
      else {
         char c; 		   
         do {
            c = getc(data);	//get a character

	    //If the character is a whitespace or EOF, skip it. If the character is a non-white space character, it means 
	    //it is a number or the number of bits. 	
            if ((c != ' ') && (c != '\t') && (c != '\n') && (c != EOF))
  	    {
	       //In this case, return the character back to the input stream, as now we can read it in formatted as a unsigned
	       //integer. If the first field seen flag is 0, that means that the first field (the number to be written out) has 
	       //not been read yet so you read in the current integer as the number to be written out.
               ungetc(c,data);
               if (first_field_seen == 0) {
	          fscanf(data, "%u", &number);
                  first_field_seen = 1;
               }
	       //If the first field seen flag is 1, that means you already have the number to be written out, so you read in the 
	       //current integer as the number of bits to write out the current number.
               else {
	          first_field_seen = 0;			//reset the flag for the next line
                  fscanf(data, "%d", &number_of_bits);	
	          //We take the one byte buffer and subtract out the number of bytes written to it already, along with the number of
		  //bits to write out for the current number. That tells us how far to left shift the current number to align with the 
		  //space left in the buffer.       
	          bits_to_shift = (8 - bits_written) - number_of_bits;

		  //This is the difficult case where the number of bits required to write out this particular number cannot be held
		  //by the buffer. This means we have to split the number. We have to fill the current byte to full capacity, write it out
		  //and flush the buffer, then use the fresh buffer to accomodate the rest of the bits. 
                  //There is a while loop because it could 
		  //be that even the fresh buffer cannot hold all the bits (this would happen if the number of bits to write 
                  //out is large), so we have to repeat the above process over and over until all the bits are accounted for.
                  while (bits_to_shift < 0) {					//the buffer will overflow 
                     bits_to_shift = (number_of_bits) - (8 - bits_written);	//number of bits that will overflow
                     shifted = (unsigned char) (number >> bits_to_shift);	
                     buffer[0] = buffer[0] | shifted;
		     //We need to keep track of the number of bits still left to write, after we fill the buffer to capacity. Then the 
		     //bits_written variable is reset to 0, to indicate that there will be a fresh buffer to be written to.

                     //This will tell how many bits we still have left to write for this number
	 	     number_of_bits -= (8 - bits_written); 
 		     bits_written = 0;
		     //Write out to output file and then flush the buffer
		     fwrite(buffer,1,sizeof(buffer),output);
                     printf("WROTE OUT %x\n", buffer[0]);
		     buffer[0] = 0x00;
		     //You want to mask the bits of the current number that you just wrote out 
		     bitmask = (0xffffffff >> (32 - bits_to_shift));
                     number = (bitmask & number);			//will expose only the overflow bits
		     bits_to_shift = 8 - number_of_bits;
	          }

		  //Cast the number to an unsigned one byte integer and then shift it by the number of bits specified by bits_to_shift 
		  //which will be stored in a variable called "shifted". 
                  //Now that everything is aligned correctly, all we do is OR the shifted representation with the buffer, that will
                  //concatenate the current bits to the buffer, and then we adjust the bits_written variable to reflect that we just
                  //wrote the specified number of bits to the buffer. This is the simple case where the number of bits to 
                  //write out a specific number fits in the buffer.
                  shifted = ((unsigned char) number) << bits_to_shift;
                  buffer[0] = buffer[0] | shifted;
 	          bits_written += number_of_bits;
                  
               }
	    
            }
         
         } while (c != EOF);
      
         //Pad the remaining bits for current byte and write out
         if (bits_written > 0) {
            fwrite(buffer,1,sizeof(buffer),output);
            printf("WROTE OUT %x\n", buffer[0]);
         }   

      }
   }  
   fclose (data);
   fclose(output); 
   return 0;
}





