#include <iostream>
#include<cstdlib>
#include<ctime>


const char alphanum[] = "0123456789!@#$&abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
int string_length = sizeof(alphanum)-1;

/*
  Genarates password wih length of x,
   using symbols: "0123456789!@#$&abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
*/
std::string PassGenerator_GeneratePassword(size_t x)
{
  srand(time(0));
  std::string password="";
  
  for(int i = 0; i < x; i++)
    password += alphanum[rand() % string_length];
  
  return password;
}
