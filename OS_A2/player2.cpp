#include<iostream>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>

using namespace std;

const int SIZE=5;
const int NUM=3;
const int SHIPS[NUM]={2,3,4};
void initializeGrid(int grid[SIZE][SIZE])
{
  for(int i=0;i<SIZE;i++)
  {
     for(int j=0;j<SIZE;j++)
     {
      grid[i][j]=0;
     }
  }
}

void printGrid(int grid[SIZE][SIZE])
{
  for(int i=0;i<SIZE;i++)
  {
     for(int j=0;j<SIZE;j++)
     {
      cout<<grid[i][j]<<" ";
     }
     cout<<"\n";
  }
}

bool placeShip(int grid[SIZE][SIZE],int size,int row,int col,char align)
{
  if(align=='H')
  {
   if(col+size-1>=SIZE)
   {
     cout<<"Out of bounds\n";
     return false;
   }
   for(int i=0;i<size;i++)
   {
     if(grid[row][col+i]==1)
     {
     cout<<"Overlap\n";
     return false;
     }
   }
   for(int i=0;i<size;i++)
   {
     grid[row][col+i]=1;
   }
  }
  else if(align=='V')
  {
    if(row+size-1>=SIZE)
   {
     cout<<"Out of bounds\n";
     return false;
   }
   for(int i=0;i<size;i++)
   {
     if(grid[row+i][col]==0)
     {
     cout<<"Overlap\n";
     return false;
     }
   }
   for(int i=0;i<size;i++)
   {
     grid[row+i][col]=1;
   }
  }
  return true;
}


void getShipPosition(int grid[SIZE][SIZE],int size)
{
    int row, col;
    char align;
    do{
        cout << "Enter index of the ship: (row column)\n";
        cin >> row >> col;
        while (row < 0 || row >= SIZE || col < 0 || col >= SIZE) 
        {
            cout << "Invalid index! Enter a valid index:\n";
            cin >> row >> col;
        }
        cout << "Enter 'H' for Horizontal 'V' for Vertical\n";
        cin >> align;
        while (align != 'H' && align != 'V') 
        {
            cout << "Invalid choice! Enter a valid choice:\n";
            cin >> align;
        }
        }while (!placeShip(grid,size ,row, col, align));
}


void getValidGuess(int &row,int &col)
{
 do{
  cout << "Enter the guess index of the ship: (row column)\n";
  cin >> row >> col;
  } while (row < 0 || row >= SIZE || col < 0 || col >= SIZE);

}

bool shipsSink(int grid[SIZE][SIZE])
{
  for(int i=0;i<SIZE;i++)
  {
   for(int j=0;j<SIZE;j++)
   {
     if(grid[i][j]==1)
     {
      return false;
     }
   }
  }
  return true;
}


int main()
{
  int rv1=mkfifo("player1",0666);
  int rv2=mkfifo("player2",0666);
  int r=open("player1",O_RDONLY);
  int w=open("player2",O_WRONLY);
  int grid1[SIZE][SIZE];
  
  int grid2[SIZE][SIZE];
  initializeGrid(grid2);
  for(int i=0;i<NUM;i++) 
  {
    int size=SHIPS[i];
    getShipPosition(grid2,size);
  }
  cout<<"Player 2 Grid:\n";
  printGrid(grid2);
  cout<<"----------GAME STARTS----------\n";
  while(true)
  {
     int row,col;
     char response;
     read(r,&row,sizeof(row));
     read(r,&col,sizeof(col));
     if(grid2[row][col]==1)
     {
       grid2[row][col]=-1;
       response='H';  
       cout<<"Player 2 Hit!\n";
     }
     else
     {
       response='M';
       cout<<"Player 2 Miss!\n";
     }
     write(w,&response,sizeof(response));
     if(shipsSink(grid2))
     {    
      cout<<"-----------------------------\n";
      cout<<"Player 2 Grid:\n";
      printGrid(grid2);
      cout<<"-----------------------------\n";
       cout<<"Player 1 wins!\n";
      cout<<"----------GAME ENDS----------\n";
       return 0;
     }
     getValidGuess(row,col);
     write(w,&row,sizeof(row));
     write(w,&col,sizeof(col));
     read(r,&response,sizeof(response));
  }
  close(r);
  close(w);
  unlink("player1");
  unlink("player2");
}

