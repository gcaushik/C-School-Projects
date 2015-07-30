#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>  

#define BIAS 1023     

double my_frexp(double, int *);
int endian_check();

union data {
   double x;
   long int y;
   char z[8];
}u;

double my_frexp(double x, int *exp) 
{
   int i;
   unsigned char *num = (unsigned char *) &x;
   unsigned char adjusted_number[8];
   int endianness;

   //First of all if the number is zero, then you just return 0 right away
   if (x == 0) {
      *exp = 0;
      return 0;
   }
   
   //First do endian check
   endianness = endian_check();

   if (endianness) {
      int j;
      //Change to big endian
      for (j=sizeof(double)-1; j>=0; j--) {
         adjusted_number[sizeof(double)-j-1] = num[j];
      }
   }


   else {
      int j;
      for (j=0; j<sizeof(double); j++) {
         adjusted_number[j] = num[j];
      }
   }

   
   unsigned char sign = ((0x80) & (adjusted_number[0])) >> 7;
   
   //Get the exponent (last 7 bits from byte 1 and first 4 bits from byte 2)
   double exponent = 0;
   unsigned char current;
   int counter = 0;
   int j;
   int value;
   for (i=1; i>=0; i--) {
      current = adjusted_number[i];
      switch(i) {
         case 0:
            current = current & 0x7f;
            for (j=0; j<7; j++) {
               value = current & 0x1;
               if (value)
                  exponent += exp2(counter);
               counter++;
               current = current >> 1;
            }
            break;
         default:
            current = current >> 4;
            for (j=0; j<4; j++) {
               value = current & 0x1;
               if (value)
                  exponent += exp2(counter);
               counter++;
               current = current >> 1;
            }
            break;        
            
      }
               
           
   }


   if ((int) exponent == 0) {
      *exp = -(BIAS - 1);
      printf("DENORMALIZED NUMBER\n");
      
      //Normalize the fraction (no implicit leading bit so need to traverse to get it)
      double fraction = 0.5;
      int index = 2;
      int shift = 0;
      int leading = 0;

      for (i=1; i<8; i++) {
      current = adjusted_number[i];
      switch(i) {
         case 1:
            current = current << 4;
            for (j=0; j<4; j++) {
               value = current & 0x80;
               if ((leading == 0) && (value == 0)) 
                  shift++;
               else if ((leading == 0) && (value)) 
                  leading = 1;
	       else if (leading == 1) {
                  if (value) 
                     fraction += (1.0/(exp2(index)));
                  index++;
               }
               current = current << 1;
            }
            break;

         case 7:
            for (j=0; j<7; j++) {
               value = current & 0x80;
               if ((leading == 0) && (value == 0)) 
                  shift++;
               else if ((leading == 0) && (value)) 
                  leading = 1;
	       else if (leading == 1) {
                  if (value) 
                     fraction += (1.0/(exp2(index)));
                  index++;
               }
               current = current << 1;
            }
            break;
         default:
            for (j=0; j<8; j++) {
               value = current & 0x80;
               if ((leading == 0) && (value == 0)) 
                  shift++;
               else if ((leading == 0) && (value)) 
                  leading = 1;
	       else if (leading == 1) {
                  if (value) 
                     fraction += (1.0/(exp2(index)));
                  index++;
               }
               current = current << 1;
            }
            break;        
            
      }
               
           
   }

      *exp = *exp - shift;
      return fraction;
   }


   //In the case that the number is not denormalized...
   //Get the fraction (the implicit leading bit + last 4 bits of byte 2 and all bytes 3-8, except the last bit)
   double fraction = 0.5;
   int index = 2;
 
   for (i=1; i<8; i++) {
      current = adjusted_number[i];
      switch(i) {
         case 1:
            current = current << 4;
            for (j=0; j<4; j++) {
               value = current & 0x80;
               if (value) 
                  fraction += (1.0/(exp2(index)));
               index++;
               current = current << 1;
            }
            break;

         case 7:
            for (j=0; j<7; j++) {
               value = current & 0x80;
               if (value)
                  fraction += (1.0/(exp2(index)));
               index++;
               current = current << 1;
            }
            break;
         default:
            for (j=0; j<8; j++) {
               value = current & 0x80;
               if (value)
                  fraction += (1.0/(exp2(index)));
               index++;
               current = current << 1;
            }
            break;        
            
      }
               
           
   }

   
   if ((int) exponent == 2047) {
      *exp = 0;
      if (fraction > 0.5) 
         printf("NaN\n");      
      else 
         printf("INFINITY\n");
      return x;
   }

   *exp = (((int) exponent) - BIAS) + 1;

   return fraction;
}

//Return 1 if it is little endian, 0 if it is big endian.
int endian_check() 
{
   int little_endian;
   union check {
      int y;
      char test[4];
   }endian;
   
   endian.y = 1;
   if (endian.test[0] == 1) 
      little_endian = 1;
   

   else 
      little_endian = 0;		 

   return little_endian;
}


int main(int argc, char *argv[])
{
   double x, r;
   int exp;
   int my_exp;
   
   if (argc < 2) {
      printf("Usage: ./a.out <int>\n");
      exit(2);
   }
   x = strtod(argv[1], NULL);
   r = frexp(x, &exp);

   printf("frexp(%g, &e) = %g: %g * %d^%d = %g\n",
                  x, r, r, FLT_RADIX, exp, x);

   r = my_frexp(x, &my_exp);
   printf("my_frexp(%g, &e) = %g: %g * %d^%d = %g\n",
                  x, r, r, FLT_RADIX, my_exp, x);

   exit(EXIT_SUCCESS);
}       



